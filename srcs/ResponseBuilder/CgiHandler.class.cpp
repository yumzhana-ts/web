/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.class.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/10/18 16:24:26 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseBuilder/CgiHandler.class.hpp"
#include "Logger.class.hpp"
#include "FileManager.class.hpp"
#include "DataSetFactory/ServerConfigDataSet.class.hpp"

/****************************************************/
/*                    Constructor                   */
/****************************************************/

CgiHandler::CgiHandler(RequestDataSet &req, std::string path, int session_number)
    : AResponse(req), script_path(path), raw_cgi_response("")
{
    serve_cookie = true;
    if (path.find(".py") != std::string::npos)
        interpreter_path = "/usr/bin/python3";
    else if (path.find(".php") != std::string::npos)
        interpreter_path = "/usr/bin/php-cgi";
    else if (path.find(".pl") != std::string::npos)
        interpreter_path = "/usr/bin/perl";
    else if (path.find(".sh") != std::string::npos || path.find(".bash") != std::string::npos)
        interpreter_path = "/usr/bin/bash";
    else if (path.find(".bla") != std::string::npos)
    {
        script_path = "/home/ytsyrend/Desktop/Core/web/ubuntu_cgi_tester";
        interpreter_path = "";
    }
    else
        interpreter_path = "";
    session_id = session_number;
    target_directory = "";
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

CgiHandler::~CgiHandler(void)
{
    if (DBG)
    {
        std::cout << GREEN << "[CgiHandler] Destructor called" << RESET_COLOR << std::endl;
    }
}

/****************************************************
 *                 Memeber Functions                *
 ****************************************************/

void CgiHandler::handle()
{
    this->handleSession();
    this->build();
    this->parseCgiResponse(this->raw_cgi_response);
    this->raw_response = this->serialize();
}

void CgiHandler::build()
{
    if (request.method == "POST")
        this->upload();
    std::string input = request.bodyrawline;
    this->launch(input);
}

int CgiHandler::setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void CgiHandler::closePipeEnds(bool child)
{
    if (child)
    {
        close(pipe_in[1]);
        close(pipe_out[0]);
    }
    else
    {
        close(pipe_in[0]);
        close(pipe_out[1]);
    }
}

void CgiHandler::runChild(const char *scriptPath, char **envp)
{
    dup2(pipe_in[0], STDIN_FILENO);
    dup2(pipe_out[1], STDOUT_FILENO);
    closePipeEnds(true);

    char *cgi_argv[] = {const_cast<char *>(interpreter_path.c_str()),
                        const_cast<char *>(scriptPath), NULL};
    execve(interpreter_path.c_str(), cgi_argv, envp);

    perror("execve");
    _exit(1);
}

void CgiHandler::setupEpoll()
{
    epoll_fd = epoll_create(1);
    struct epoll_event evIn, evOut;
    evIn.events = EPOLLIN;
    evIn.data.fd = pipe_out[0];
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_out[0], &evIn);

    evOut.events = EPOLLOUT;
    evOut.data.fd = pipe_in[1];
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_in[1], &evOut);
}

void CgiHandler::runParent(const char *input)
{
    closePipeEnds(false);
    setNonBlocking(pipe_in[1]);
    setNonBlocking(pipe_out[0]);
    setupEpoll();

    size_t written = 0;
    size_t len = strlen(input);
    bool stdinClosed = false;

    std::stringstream cgi_output;
    char buffer[1024];

    while (true)
    {
        struct epoll_event events[2];
        int n = epoll_wait(epoll_fd, events, 2, -1);
        if (n < 0)
            continue;

        for (int i = 0; i < n; ++i)
        {
            int fd = events[i].data.fd;

            if ((events[i].events & EPOLLOUT) && !stdinClosed)
            {
                ssize_t w = write(fd, input + written, len - written);
                if (w > 0)
                    written += w;
                if (written >= len)
                {
                    close(fd);
                    stdinClosed = true;
                }
            }

            if (events[i].events & EPOLLIN)
            {
                ssize_t r = read(fd, buffer, sizeof(buffer));
                if (r > 0)
                    cgi_output.write(buffer, r);
                else if (r == 0)
                {
                    close(fd);
                    raw_cgi_response = cgi_output.str();
                    return;
                }
            }
        }
    }
}

int CgiHandler::launch(std::string input)
{
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
    {
        perror("pipe");
        return 1;
    }

    for (size_t i = 0; i < envVars.size(); ++i)
        envp.push_back(const_cast<char *>(envVars[i].c_str()));
    envp.push_back(NULL);

    pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }
    if (pid == 0)
        runChild(script_path.c_str(), envp.data());
    else
    {
        runParent(input.c_str());
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
            int exitCode = WEXITSTATUS(status);
            if (exitCode == 1)
            {
                Logger::debug("i am not executed");
                this->setError(INTERNALERROR);
            }
        }
    }
    return 0;
}

void CgiHandler::parseCgiResponse(const std::string &raw)
{
    size_t pos = raw.find("\r\n\r\n");
    size_t header_end_len = 0;

    if (pos != std::string::npos)
    {
        header_end_len = 4;
    }
    else
    {
        pos = raw.find("\n\n");
        if (pos != std::string::npos)
        {
            header_end_len = 2;
        }
    }

    std::string headers_str;
    std::string body_str;

    if (pos != std::string::npos)
    {
        headers_str = raw.substr(0, pos);
        if (pos + header_end_len <= raw.size())
            body_str = raw.substr(pos + header_end_len);
    }
    else
    {
        // нет разделителя — значит всё тело без заголовков
        body_str = raw;
    }

    this->body = body_str;
    // --- заголовки ---
    std::istringstream header_stream(headers_str);
    std::string line;
    while (std::getline(header_stream, line))
    {
        if (line.empty())
            continue;

        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        if (line.compare(0, 7, "Status:") == 0)
        {
            std::istringstream ss(line.substr(7));
            ss >> this->status_code;
            std::getline(ss, this->status_text);
            if (!this->status_text.empty() && this->status_text[0] == ' ')
                this->status_text.erase(0, 1);
        }
        else
        {
            size_t sep = line.find(':');
            if (sep != std::string::npos)
            {
                std::string key = line.substr(0, sep);
                std::string value = line.substr(sep + 1);
                if (!value.empty() && value[0] == ' ')
                    value.erase(0, 1);
                this->headers[key] = value;
            }
        }
    }

    // дефолтный статус
    if (this->status_code == 0)
    {
        this->status_code = 200;
        this->status_text = "OK";
    }

    // Content-Length, если не был передан
    if (this->headers.find("Content-Length") == this->headers.end())
    {
        std::ostringstream len_stream;
        len_stream << this->body.length();
        this->headers["Content-Length"] = len_stream.str();
    }
}

void CgiHandler::upload()
{
    if (request.body.size() > ServerConfigDataSet::getInstance().client_max_body_size)
    {
        this->setError(PAYLOADTOOLARGE);
        return;
    }
    this->directory = "www/database/";
    std::string filename = "upload_" + getTimestampForFilename();
    std::string filefullpath = this->directory + filename;
    try
    {
        if (SessionManager::getInstance()->getSession(this->session_id) != NULL)
            SessionManager::getInstance()->getSession(this->session_id)->setName(request.body);
        FileManager::getInstance()->uploadFile(this->directory, filename, request.text, request.body);
    }
    catch (const std::exception &e)
    {
        this->setError(INTERNALERROR);
    }
}