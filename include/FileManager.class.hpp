#pragma once
#include "lib.hpp"
#include "ResponseBuilder/SessionData.class.hpp"

class FileManager
{
public:
    static FileManager *getInstance();
    int uploadFile(const std::string &filename);
    int uploadFile(const std::string& directory, const std::string& filename, bool text, std::string& body);
   /// void deleteFile(int id, const std::string& directory);
    std::string getFile(int id) const;
    void printAllFiles() const;
    std::vector<std::pair<int, std::string> > getAllFiles() const;
    //void deleteFile(int id, const std::string& directory);
    bool deleteFile(int id);
    void destroyInstance();
    void deleteFile(int id, const std::string& directory);
    bool hasFileName(const std::string& name);
private:
    FileManager();
    ~FileManager();

    static FileManager *instance;
    std::map<int, std::string> file;
    int nextFileId; // счётчик для автоинкремента
};


#define DBG 0
#define RESET_COLOR "\033[0m"
#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define WHITE "\033[0;37m"
#define BOLD_BLACK "\033[1;30m"
#define BG_WHITE "\033[0;47m"
