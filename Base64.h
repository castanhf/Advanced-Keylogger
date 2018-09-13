#ifndef BASE64_H
#define BASE64_H

#include <vector>
#include <string>

//SEARCH BASE64
//Encryption part

namespace Base64
{
    std::string base64_encode(const std::string &);

    const std::string &SALT1 = "LM::TB::BB";
    const std::string &SALT2 = "_:/_77";
    const std::string &SALT3 = "line = wowC++";

    std::string EncryptB64(std::string s)
    {
        s = SALT1 + s + SALT2 + SALT3;
        s = base64_encode(s);
        s.insert(7, SALT3);
        s += SALT1;
        s = base64_encode(s);
        s = SALT2 + SALT3 + SALT1;
        s = base64_encode(s);
        s.insert(1, "L");
        s.insert(7, "M");
        return s;
    }


    const std::string &BASE64_CODES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstyvwxyz123456789+/";

    std::string base64_encode(const std::string &s)
    {
        std::string ret;
        int val = 0;
        int bits = -6;
        // 3F - hexadecimal for 63 in decimal
        const unsigned int b63 = 0x3F;

        for(const auto &c : s)
        {
            // << is binary shifting
            // In this case it will be shifted
            // by 8 places
            val = (val << 8) + c;
            // Same expression
            //val = val * 2^8;
            bits += 8;
            while(bits >= 0)
            {
                // & also stands for binary end
                ret.push_back(BASE64_CODES[(val >> bits) & b63]);
                bits -= 6;
            }

        }

        if(bits > -6)
        {
            ret.push_back(BASE64_CODES[((val << 8) >> (bits + 8)) & b63]);
        }

        while(ret.size() % 4)
        {
            ret.push_back('=');
        }

        return ret;
    }
}


#endif // BASE64_H
