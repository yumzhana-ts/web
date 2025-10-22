#include "include/Client.class.hpp"
#include "Logger.class.hpp"

Client::Client(Socket &webserv) : serverSocket(&webserv)
{
	_fd = webserv.acceptClient();
	if (_fd < 0)
	{
		throw std::runtime_error("Failed to accept client connection");
	}
}

Client::~Client()
{
	if (_fd < -1)
	{
		std::cout << "closing " << _fd << '\n';
		close(_fd);
	}
}

int Client::getClientFd() const
{
	return _fd;
}

std::string Client::getRaw_request()
{
	return this->raw_request;
}

void Client::getRequest() 
{
    usleep(1000000);
    this->raw_request.clear();
    std::string headers;
    std::string body;
    size_t headers_end = 0;
    size_t expected_length = 0;
    bool partial = false;


    while (safeRead(this->_fd, headers) > 0) 
    {
        size_t pos = headers.find("\r\n\r\n");
        if (pos != std::string::npos) {
            headers_end = pos + 4;
            break;
        }
    }

    if (headers_end == 0)
    {
        Logger::debug(" 💩 [Client] Incomplete request headers");
        return;
    }

    std::string headers_only = headers.substr(0, headers_end);

    if (headers_only.find("Transfer-Encoding: chunked") != std::string::npos) 
    {
        std::string initial_body = headers.substr(headers_end);
        body = readChunked(this->_fd, initial_body);
        expected_length = headers_end + body.size();
        if (body.empty())
            partial = true;
    } 
    else 
    {
        size_t content_length = parseContentLength(headers_only);
        std::string initial_body = headers.substr(headers_end);
        body = readContentLength(this->_fd, initial_body, content_length);
        expected_length = headers_end + content_length;
        if (body.size() < content_length)
            partial = true;
    }
    this->raw_request = headers_only + body;
    bool complete = !partial && (raw_request.size() == expected_length);
    std::string mark = complete ? "✉️ " : "❌";

    Logger::info(mark + " [Client][FD "+ toString(this->_fd) +  
                    "][PORT " + toString(this->serverSocket->getServerPort()) + 
                    "] Request received " + toString(raw_request.size()) + 
                  "/" + toString(expected_length) + " bytes ");
}


ssize_t Client::safeRead(int fd, std::string &buffer)
{
    char temp[4096];
    ssize_t n = read(fd, temp, sizeof(temp));
    if (n > 0)
        buffer.append(temp, n);
    return n;
}

size_t Client::parseContentLength(const std::string &headers)
{
    size_t cl_pos = headers.find("Content-Length:");
    if (cl_pos == std::string::npos)
        return 0;
    size_t line_end = headers.find("\r\n", cl_pos);
    std::string value = headers.substr(cl_pos + 15, line_end - (cl_pos + 15));
    return std::atoi(value.c_str());
}

std::string Client::readContentLength(int fd, std::string &initial, size_t content_length)
{
    std::string result = initial;
    while (result.size() < content_length)
    {
        ssize_t n = safeRead(fd, result);
        if (n <= 0)
            break;
    }
    if (result.size() > content_length)
        result.resize(content_length);
    return result;
}

std::string Client::readChunked(int fd, std::string &body)
{
    std::string result;
    ssize_t n;

    while (true)
    {
        while (body.find("\r\n") == std::string::npos)
        {
            n = safeRead(fd, body);
            if (n <= 0) 
            {
                Logger::debug(" 💩 [Client] Partial chunked read detected");
                return result;
            }
        }
        size_t line_end = body.find("\r\n");
        std::string hex_size = body.substr(0, line_end);
        size_t chunk_size = std::strtoul(hex_size.c_str(), 0, 16);

        if (chunk_size == 0)
            break;

        size_t needed = line_end + 2 + chunk_size + 2; // +\r\n после чанка
        while (body.size() < needed)
        {
            n = safeRead(fd, body);
            if (n <= 0) {
                Logger::debug(" 💩 [Client] Partial chunked read detected");
                return result;
            }
        }

        result.append(body.substr(line_end + 2, chunk_size));
        body.erase(0, needed);
    }
    return result;
}


