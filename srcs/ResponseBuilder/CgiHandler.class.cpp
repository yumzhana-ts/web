/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.class.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/10/01 01:35:42 by ytsyrend         ###   ########.fr       */
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
    if (DBG){std::cout << GREEN << "[CgiHandler] Destructor called" << RESET_COLOR << std::endl;}
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
    if(request.method == "POST")
        this->upload();
    std::string input = request.bodyrawline;
    this->launch(input);
}

void CgiHandler::closePipeEnds(bool child)
{
    if (child)
    {
        // In child:
        // We're using pipe_in[0] for reading (stdin)
        // and pipe_out[1] for writing (stdout)

        // Close unused ends:
        close(pipe_in[1]);   // child doesn't write to stdin
        close(pipe_out[0]);  // child doesn't read from stdout
    }
    else
    {
        // In parent:
        // We're using pipe_in[1] to write to child
        // and pipe_out[0] to read from child

        // Close unused ends:
        close(pipe_in[0]);   // parent doesn't read from stdin
        close(pipe_out[1]);  // parent doesn't write to stdout
    }
}

/*void CgiHandler::runChild(const char* scriptPath, char** envp)
{
    dup2(pipe_in[0], STDIN_FILENO);
    dup2(pipe_out[1], STDOUT_FILENO);
    closePipeEnds(true);

    char *cgi_argv[] = { const_cast<char*>(interpreter_path.c_str()),
        const_cast<char*>(scriptPath),NULL };
    execve(interpreter_path.c_str(), cgi_argv, envp);
    //this->setError(INTERNALERROR);
    //perror("execve");
    //_exit(1);
}*/

void CgiHandler::runChild(const char* scriptPath, char** envp)
{
    std::cerr << "[child] PID: " << getpid() << " - Setting up CGI process\n";

    // Pipe redirection
    if (dup2(pipe_in[0], STDIN_FILENO) == -1) {
        perror("[child] dup2 pipe_in[0] -> STDIN failed");
        _exit(1);
    }
    if (dup2(pipe_out[1], STDOUT_FILENO) == -1) {
        perror("[child] dup2 pipe_out[1] -> STDOUT failed");
        _exit(1);
    }

    std::cerr << "[child] STDIN_FILENO now = pipe_in[0] = " << pipe_in[0] << "\n";
    std::cerr << "[child] STDOUT_FILENO now = pipe_out[1] = " << pipe_out[1] << "\n";

    // Close unnecessary pipe ends
    closePipeEnds(true);
    std::cerr << "[child] Unused pipe ends closed.\n";

    // Determine what to execute
    char* cgi_argv[3]; // Max 2 args + NULL
    const char* exec_path = NULL;

    if (!interpreter_path.empty()) {
        // Script with interpreter
        cgi_argv[0] = const_cast<char*>(interpreter_path.c_str());
        cgi_argv[1] = const_cast<char*>(scriptPath);
        cgi_argv[2] = NULL;
        exec_path = interpreter_path.c_str();

        std::cerr << "[child] Executing script using interpreter:\n";
        std::cerr << "        interpreter = " << interpreter_path << "\n";
        std::cerr << "        script path = " << scriptPath << "\n";
    } else {
        // Direct binary
        cgi_argv[0] = const_cast<char*>(scriptPath);
        cgi_argv[1] = NULL;
        exec_path = scriptPath;

        std::cerr << "[child] Executing binary CGI directly:\n";
        std::cerr << "        binary path = " << scriptPath << "\n";
    }

    // Print command being executed
    std::cerr << "[child] execve(" << exec_path << ", argv = [";
    for (int i = 0; cgi_argv[i] != NULL; ++i) {
        std::cerr << "\"" << cgi_argv[i] << "\", ";
    }
    std::cerr << "NULL], envp)\n";

    // (Optional) Print a few env vars for debugging
    if (envp) {
        std::cerr << "[child] Environment variables:\n";
        for (int i = 0; envp[i]; ++i) {
            std::cerr << "        " << envp[i] << "\n";
        }
    }

    // Execute CGI process


    // Confirm the file is there
    if (access(exec_path, F_OK) == -1) {
        std::cerr << "[child] File does not exist.\n";
    } else if (access(exec_path, X_OK) == -1) {
        std::cerr << "[child] File exists but is not executable.\n";
    } else {
        std::cerr << "[child] File exists and is executable — likely exec format error.\n";
    }

    execve(exec_path, cgi_argv, envp);


    /*{
        int err = errno;
        std::cerr << "execve failed with errno: " << err
                << " (" << std::strerror(err) << ")\n";
        _exit(1);  // Exit child so parent can detect failure
    }*/
    std::cerr <<"[child] i am failed" << std::endl;
    //_exit(1);
}


/*void CgiHandler::runParent(const char* input)
{
    
    closePipeEnds(false);

    // отправляем данные в stdin CGI
    write(pipe_in[1], input, strlen(input));
    close(pipe_in[1]);

    std::cout << "input:" << input << std::endl;
    // читаем stdout CGI
    std::stringstream cgi_output;
    char buffer[1024];
    ssize_t n;
    while ((n = read(pipe_out[0], buffer, sizeof(buffer))) > 0)
    {
        // записываем ровно n байт, не полагаясь на '\0'
        cgi_output.write(buffer, n);
        //usleep(1000000); 
    }

    this->raw_cgi_response = cgi_output.str();
    //usleep(1000000); 
    close(pipe_out[0]);
}*/

/*void CgiHandler::runParent(const char* input)
{
    closePipeEnds(false);

    // Send data into CGI stdin using write loop
    ssize_t total_written = 0;
    ssize_t to_write = strlen(input);
    const char* input_ptr = input;

    while (total_written < to_write) {
        ssize_t written = write(pipe_in[1], input_ptr + total_written, to_write - total_written);
        if (written == -1) {
            perror("write");
            break;
        }
        total_written += written;
    }
    std::cout << "Written to CGI: " << total_written << " bytes\n";

    close(pipe_in[1]); // Close after writing

    // Read from CGI stdout
    std::stringstream cgi_output;
    char buffer[1024];
    ssize_t n;
    while ((n = read(pipe_out[0], buffer, sizeof(buffer))) > 0) {
        cgi_output.write(buffer, n);
    }

    this->raw_cgi_response = cgi_output.str();
    close(pipe_out[0]);
}*/

void CgiHandler::runParent(const char* input)
{
    std::cerr << "[DEBUG] Running parent process handler...\n";

    closePipeEnds(false);

    ssize_t total_written = 0;
    ssize_t to_write = strlen(input);
    const char* input_ptr = input;

    while (total_written < to_write) {
        ssize_t written = write(pipe_in[1], input_ptr + total_written, to_write - total_written);
        if (written == -1) {
            std::cerr << "[ERROR] write() failed: " << std::strerror(errno) << std::endl;
            break;
        }
        std::cerr << "[DEBUG] Wrote " << written << " bytes to pipe_in[1]\n";
        total_written += written;
    }
    std::cerr << "[DEBUG] Total bytes written to child stdin: " << total_written << "\n";

    close(pipe_in[1]);
    std::cerr << "[DEBUG] Closed pipe_in[1] after writing\n";

    std::stringstream cgi_output;
    char buffer[1024];
    ssize_t n;
    while ((n = read(pipe_out[0], buffer, sizeof(buffer))) > 0) {
        std::cerr << "[DEBUG] Read " << n << " bytes from pipe_out[0]\n";
        cgi_output.write(buffer, n);
    }

    if (n == -1) {
        std::cerr << "[ERROR] read() failed: " << std::strerror(errno) << std::endl;
    } else {
        std::cerr << "[DEBUG] Finished reading from pipe_out[0]\n";
    }

    this->raw_cgi_response = cgi_output.str();
    std::cerr << "[DEBUG] Total CGI response size: " << this->raw_cgi_response.size() << " bytes\n";

    close(pipe_out[0]);
    std::cerr << "[DEBUG] Closed pipe_out[0]\n";
}


/*int CgiHandler::launch(std::string input)
{
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) 
    {
        std::cerr << "pipe error: " << std::strerror(errno) << std::endl;
        this->setError(INTERNALERROR);
        return 1;
    }

    
    for (size_t i = 0; i < envVars.size(); ++i)
        envp.push_back(const_cast<char*>(envVars[i].c_str()));
    envp.push_back(NULL);

    pid = fork();
    if (pid < 0)
    {
        //perror("fork");
        this->setError(INTERNALERROR);
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
}*/

int CgiHandler::launch(std::string input)
{
    std::cerr << "[DEBUG] Starting launch()\n";

    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) 
    {
        std::cerr << "[ERROR] pipe() failed: " << std::strerror(errno) << std::endl;
        this->setError(INTERNALERROR);
        return 1;
    }

    std::cerr << "[DEBUG] Pipes created successfully.\n";
    std::cerr << "[DEBUG] pipe_in:  read = " << pipe_in[0] << ", write = " << pipe_in[1] << "\n";
    std::cerr << "[DEBUG] pipe_out: read = " << pipe_out[0] << ", write = " << pipe_out[1] << "\n";

    for (size_t i = 0; i < envVars.size(); ++i)
        envp.push_back(const_cast<char*>(envVars[i].c_str()));
    envp.push_back(NULL);

    std::cerr << "[DEBUG] Environment prepared. Forking...\n";
    pid = fork();
    if (pid < 0)
    {
        std::cerr << "[ERROR] fork() failed: " << std::strerror(errno) << std::endl;
        this->setError(INTERNALERROR);
        return 1;
    }
    if (pid == 0)
    {
        std::cerr << "[DEBUG] In child process (PID: " << getpid() << ")\n";
        std::cerr << "Its wrong script: " << script_path << std::endl;
        runChild(script_path.c_str(), envp.data());
    }
    else
    {
        std::cerr << "[DEBUG] In parent process. PID of child: " << pid << "\n";
        std::cerr << "[DEBUG] Input size: " << input.size() << " bytes\n";

        runParent(input.c_str());

        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) 
        {
            int exitCode = WEXITSTATUS(status);
            std::cerr << "[DEBUG] Child exited with code: " << exitCode << "\n";
            if (exitCode == 1)
            {
                Logger::debug("i am not executed");
                this->setError(INTERNALERROR);
            }
        }
        else if (WIFSIGNALED(status))
        {
            std::cerr << "[DEBUG] Child terminated by signal: " << WTERMSIG(status) << "\n";
        }
        else
        {
            std::cerr << "[DEBUG] Child exited abnormally.\n";
        }
    }

    return 0;
}

void CgiHandler::parseCgiResponse(const std::string &raw)
{
    size_t pos = raw.find("\r\n\r\n");
    size_t header_end_len = 0;

    if (pos != std::string::npos) {
        header_end_len = 4;
    } else {
        pos = raw.find("\n\n");
        if (pos != std::string::npos) {
            header_end_len = 2;
        }
    }

    std::string headers_str;
    std::string body_str;

    if (pos != std::string::npos) {
        headers_str = raw.substr(0, pos);
        if (pos + header_end_len <= raw.size())
            body_str = raw.substr(pos + header_end_len);
    } else {
        // нет разделителя — значит всё тело без заголовков
        body_str = raw;
    }

    this->body = body_str;
    // --- заголовки ---
    std::istringstream header_stream(headers_str);
    std::string line;
    while (std::getline(header_stream, line)) {
        if (line.empty()) continue;

        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        if (line.compare(0, 7, "Status:") == 0) {
            std::istringstream ss(line.substr(7));
            ss >> this->status_code;
            std::getline(ss, this->status_text);
            if (!this->status_text.empty() && this->status_text[0] == ' ')
                this->status_text.erase(0, 1);
        } else {
            size_t sep = line.find(':');
            if (sep != std::string::npos) {
                std::string key = line.substr(0, sep);
                std::string value = line.substr(sep + 1);
                if (!value.empty() && value[0] == ' ')
                    value.erase(0, 1);
                this->headers[key] = value;
            }
        }
    }

    // дефолтный статус
    if (this->status_code == 0) {
        this->status_code = 200;
        this->status_text = "OK";
    }

    // Content-Length, если не был передан
    if (this->headers.find("Content-Length") == this->headers.end()) {
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
        if(SessionManager::getInstance()->getSession(this->session_id) != NULL)
            SessionManager::getInstance()->getSession(this->session_id)->setName(request.body);
		FileManager::getInstance()->uploadFile(this->directory, filename, request.text, request.body);
	}
	catch(const std::exception &e)
	{
		this->setError(INTERNALERROR);
	}
}