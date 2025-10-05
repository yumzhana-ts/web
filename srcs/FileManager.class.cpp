/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChainOfResponsibility.class.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/17 00:39:41 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileManager.class.hpp"
#include "Logger.class.hpp"

FileManager* FileManager::instance = NULL;

/****************************************************/
/*                    Constructor                   */
/****************************************************/

FileManager::FileManager() : nextFileId(1) // начинаем ID с 1
{
    if (DBG){ std::cout << GREEN << "[FileManager] Default Constructor called" << RESET_COLOR << std::endl; }
}

/****************************************************/
/*                    Destructor                    */
/****************************************************/

FileManager::~FileManager() 
{
    if (DBG){ std::cout << GREEN << "[FileManager] Destructor called" << RESET_COLOR << std::endl; }
    file.clear();
}

/****************************************************
*                 Singleton Handling                *
****************************************************/

FileManager* FileManager::getInstance() 
{
    if (!instance)
        instance = new FileManager();
    return instance;
}

void FileManager::destroyInstance() 
{
    delete instance;
    instance = NULL;
}

/****************************************************
*                 File Handling                     *
****************************************************/

int FileManager::uploadFile(const std::string& filename) 
{
    int id = nextFileId++;
    file[id] = filename;

    if (DBG){ 
        std::cout << GREEN << "[FileManager] File uploaded: " << filename << " with ID " << id << RESET_COLOR << std::endl; 
    }
    return id;
}

int FileManager::uploadFile(const std::string& directory, const std::string& filename, bool text, std::string& body)
{
    std::string filefullpath = directory + filename;
    if(text == false)
    {
        if (!saveBinaryFile(body, filefullpath))
            throw(std::logic_error(""));
        
    }
    else
    {
        std::ofstream file(filefullpath.c_str());
        if (!file.is_open())
            throw(std::logic_error(""));
        file << body;
        file.close();
    }
    int id = nextFileId++;
    file[id] = filename;

    if (DBG){ std::cout << GREEN << "[FileManager] File uploaded: " << filename << " with ID " << id << RESET_COLOR << std::endl; }
    return id;
}

void FileManager::deleteFile(int id, const std::string& directory) 
{
    // Ищем файл по ID
    std::map<int, std::string>::iterator it = file.find(id);
    if (it == file.end()) {
        throw std::logic_error("[FileManager] File with given ID not found");
    }

    std::string filepath = directory + it->second;

    // Удаляем файл с диска
    if (std::remove(filepath.c_str()) != 0) {
        throw std::logic_error("[FileManager] Failed to delete file");
    }

    if (DBG) {
        std::cout << RED << "[FileManager] File deleted: "
                  << it->second << " with ID " << id
                  << RESET_COLOR << std::endl;
    }

    // Удаляем запись из map
    file.erase(it);
}

/*bool FileManager::deleteFile(int id) 
{
    std::map<int, std::string>::iterator it = file.find(id);
    if (it != file.end()) {
        std::string filepath = "www/database/" + it->second;
        if (std::remove(filepath.c_str()) != 0) {
            if (DBG){
                std::cerr << "\033[31m[FileManager] Failed to delete file on disk: " 
                          << filepath << "\033[0m" << std::endl;
            }
        } else {
            if (DBG){
                std::cout << GREEN << "[FileManager] File deleted on disk: " 
                          << filepath << RESET_COLOR << std::endl;
            }
        }
        if (DBG){ 
            std::cout << GREEN << "[FileManager] File removed from map: " 
                      << it->second << " with ID " << id << RESET_COLOR << std::endl; 
        }
        file.erase(it);
        return true;
    }
    return false;
}*/

std::string FileManager::getFile(int id) const 
{
    std::map<int, std::string>::const_iterator it = file.find(id);
    if (it != file.end()) {
        return it->second;
    }
    return "";
}

void FileManager::printAllFiles() const 
{
    if (file.empty()) {
        std::cout << RED << "[FileManager] No files stored." << RESET_COLOR << std::endl;
        return;
    }

    std::cout << GREEN << "[FileManager] Current files:" << RESET_COLOR << std::endl;
    for (std::map<int, std::string>::const_iterator it = file.begin(); it != file.end(); ++it) {
        std::cout << "  ID: " << it->first << " -> " << it->second << std::endl;
    }
}

std::vector<std::pair<int, std::string> > FileManager::getAllFiles() const 
{
    std::vector<std::pair<int, std::string> > result;
    for (std::map<int, std::string>::const_iterator it = file.begin(); it != file.end(); ++it) {
        result.push_back(*it);
    }
    return result;
}
