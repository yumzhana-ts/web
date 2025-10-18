/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.class.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:01 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/10/18 16:22:59 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "lib.hpp"
#include "DataSetFactory/RequestDataSet.class.hpp"
#include "../ChainOfResponsibility.class.hpp"
#include "AResponse.class.hpp"

class CgiHandler: public IHandler, public AResponse
{
private:
    int pipe_in[2];
    int pipe_out[2];
    pid_t pid;
    int epoll_fd;
    std::string interpreter_path;
    std::string script_path;
    void closePipeEnds(bool child);
    void runChild(const char* scriptPath, char** envp);
    void parseCgiResponse(const std::string &raw);
    void runParent(const char* input);
    int setNonBlocking(int fd);
    void setupEpoll();
public:
    std::vector<std::string> envVars;
    std::vector<char*> envp;
    std::string raw_cgi_response;
    CgiHandler(RequestDataSet &req, std::string path, int session_number);
    ~CgiHandler();
    int launch(std::string input);
    void handle();
    void build();
    void upload();
    std::string target_directory;
};


#define DBG 0
// Define ANSI escape sequences for colors
#define RESET_COLOR "\033[0m"
#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define WHITE "\033[0;37m"
#define BOLD_BLACK "\033[1;30m"
#define BG_WHITE "\033[0;47m"
