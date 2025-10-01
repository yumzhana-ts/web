#include <fstream>
#include <sstream>
#include <iostream>
#include "lib.hpp"

std::string openFile(const std::string &filename)
{
    std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Failed to open file: " + filename);

    file.seekg(0, std::ios::end);
    std::string data(file.tellg(), '\0');
    file.seekg(0, std::ios::beg);
    file.read(&data[0], data.size());

    return data;
}


int randomInRange(int min, int max)
{
    return min + (std::rand() % (max - min + 1));
}

std::string urlDecode(const std::string &str)
{
    std::string result;
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (str[i] == '+')
        {
            result += ' ';
        }
        else if (str[i] == '%' && i + 2 < str.length())
        {
            std::istringstream iss(str.substr(i + 1, 2));
            int hex;
            iss >> std::hex >> hex;
            result += static_cast<char>(hex);
            i += 2;
        }
        else
        {
            result += str[i];
        }
    }
    return result;
}

std::string getTimestampForFilename() 
{
    time_t now = time(0);
    tm* t = localtime(&now);

    char buf[16];
    strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", t); // compact format
    return std::string(buf);
}

std::string trimQuotes(const std::string &s)
{
    if (s.length() >= 2 && s[0] == '"' && s[s.length() - 1] == '"')
        return s.substr(1, s.length() - 2);
    return s;
}

#include <fstream>
#include <string>

bool saveBinaryFile(const std::string &data, const std::string &filename) 
{
    std::ofstream out(filename.c_str(), std::ios::binary);
    if (!out.is_open())
        return false;

    out.write(data.data(), data.size());
    out.close();
    return true;
}

#include <cstdlib>
#include <string>

#define PROJECT_ROOT "Desktop/Core/web"

#include <cstdlib>
#include <string>

std::string buildFullPath(const std::string& pagePath) 
{
    const char* env_home = std::getenv("HOME");
    std::string home = (env_home != NULL) ? env_home : "/";

    std::string fullPath = home;

    // склеиваем с PROJECT_ROOT
    if (!fullPath.empty()) {
        if (fullPath[fullPath.size() - 1] != '/')
            fullPath += '/';
        fullPath += PROJECT_ROOT;
    }

    // склеиваем с pagePath
    if (!pagePath.empty()) {
        if (fullPath[fullPath.size() - 1] != '/')
            fullPath += '/';
        if (pagePath[0] == '/')
            fullPath += pagePath.substr(1);
        else
            fullPath += pagePath;
    }

    return fullPath;
}
 // inet_addr
#include <arpa/inet.h>

in_addr_t ipStringToHostIP(const std::string &ip_str)
{
    if (ip_str.empty())
        return INADDR_NONE;
    if (ip_str == "localhost")
        return inet_addr("127.0.0.1");
    return inet_addr(ip_str.c_str()); 
}