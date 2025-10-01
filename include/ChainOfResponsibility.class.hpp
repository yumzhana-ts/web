/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChainOfResponsibility.class.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:01 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/19 13:08:35 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <vector>
#include "Socket.class.hpp"
#include "Client.class.hpp"
#include <arpa/inet.h>

class RequestDataSet;

class IHandler
{
    protected:
        IHandler* nextHandler;
    public:
        IHandler();
        virtual ~IHandler(void);
        virtual void handle() = 0;
        IHandler* setNext(IHandler* handler);
};


class ChainOfResponsibility
{
    private:
        RequestDataSet *request;
    public:
        ChainOfResponsibility(std::string &buf, Client& client, Socket& socket);
        ~ChainOfResponsibility(void);
};

std::string openFile(const std::string& filename);
int randomInRange(int min, int max);
std::string urlDecode(const std::string &str);
std::string getTimestampForFilename();
std::ofstream createFile(const std::string &filename);
std::string trimQuotes(const std::string &s);
bool saveBinaryFile(const std::string &data, const std::string &filename);
bool isEmptyLine(const std::string &line);
std::string buildFullPath(const std::string& pagePath);
in_addr_t ipStringToHostIP(const std::string &ip_str);

#define DBG 0
#define RESET_COLOR "\033[0m"
#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define WHITE "\033[0;37m"
#define BOLD_BLACK "\033[1;30m"
#define BG_WHITE "\033[0;47m"
