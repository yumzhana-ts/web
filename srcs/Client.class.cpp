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
// Calls acceptClient() from the passed-in Socket object.
// This waits for and accepts a new client connection.
// If it fails, throws exception

Client::~Client()
{
	if (_fd < -1) // (_fd >= 0)
	{
		std::cout << "closing " << _fd << '\n';
		close(_fd);
	}
}

int Client::getClientFd() const
{
	return _fd;
}
// Getter for the socket FD.

std::string Client::getRaw_request()
{
    //std::cout << this->raw_request << std::endl;
	return this->raw_request;
}

void Client::getRequest() 
{
    debugPeekFdStream();
    this->raw_request.clear();
    std::string headers;
    std::string body;
    size_t headers_end = 0;
    size_t expected_length = 0;
    bool partial = false;

    // 1. Read headers fully
    while (true) {
        if (!waitForData(5)) { // wait max 5 seconds for data
            Logger::debug(" 💩 [Client] Timeout waiting for headers");
            partial = true;
            break;
        }

        ssize_t n = safeRead(this->_fd, headers);
        if (n <= 0) {
            Logger::debug(" 💩 [Client] Connection closed while reading headers");
            partial = true;
            break;
        }

        size_t pos = headers.find("\r\n\r\n");
        if (pos != std::string::npos) {
            headers_end = pos + 4;
            break;
        }
    }

    if (headers_end == 0) {
        Logger::debug(" 💩 [Client] Incomplete request headers");
        return;
    }

    std::string headers_only = headers.substr(0, headers_end);

    // 2. Read body
    if (headers_only.find("Transfer-Encoding: chunked") != std::string::npos) {
        std::string initial_body = headers.substr(headers_end);
        body = readChunked(this->_fd, initial_body);
        expected_length = headers_end + body.size();
        if (body.empty())
            partial = true;
    } else {
        size_t content_length = parseContentLength(headers_only);
        std::string initial_body = headers.substr(headers_end);
        body = readContentLength(this->_fd, initial_body, content_length);
        expected_length = headers_end + content_length;
        if (body.size() < content_length)
            partial = true;
    }

    // 3. Combine headers + body
    this->raw_request = headers_only + body;

    // 4. Check completeness
    bool complete = !partial && (raw_request.size() == expected_length);
    std::string mark = complete ? "✉️ " : "❌";

    Logger::info(mark + " [Client][FD "+ toString(this->_fd) +  "][PORT " +
                 toString(this->serverSocket->getServerPort()) +
                 "] Request received " + toString(raw_request.size()) + "/" +
                 toString(expected_length) + " bytes");
}

// ----- Helper functions -----

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
    return atoi(value.c_str());
}

std::string Client::readContentLength(int fd, std::string &initial, size_t content_length)
{
    std::string result = initial;
    while (result.size() < content_length) {
        if (!waitForData(5)) {
            Logger::debug(" 💩 [Client] Timeout waiting for content-length body");
            break;
        }
        ssize_t n = safeRead(fd, result);
        if (n <= 0) {
            Logger::debug(" 💩 [Client] Connection closed during content-length read");
            break;
        }
    }
    if (result.size() > content_length)
        result.resize(content_length);
    return result;
}

std::string Client::readChunked(int fd, std::string &body)
{
    std::string result;
    while (true) {
        // 1. Read chunk size line
        size_t line_end = body.find("\r\n");
        while (line_end == std::string::npos) {
            if (!waitForData(5)) {
                Logger::debug(" 💩 [Client] Timeout waiting for chunk size");
                return result;
            }
            ssize_t n = safeRead(fd, body);
            if (n <= 0) {
                Logger::debug(" 💩 [Client] Connection closed during chunk size read");
                return result;
            }
            line_end = body.find("\r\n");
        }

        std::string hex_size = body.substr(0, line_end);
        unsigned long chunk_size = strtoul(hex_size.c_str(), 0, 16);
        body.erase(0, line_end + 2);

        if (chunk_size == 0) {
            // consume trailing \r\n
            while (body.size() < 2) {
                if (!waitForData(5)) break;
                ssize_t n = safeRead(fd, body);
                if (n <= 0) break;
            }
            if (body.size() >= 2)
                body.erase(0, 2);
            break; // done
        }

        // 2. Read chunk data
        while (body.size() < chunk_size + 2) { // +2 for \r\n
            if (!waitForData(5)) {
                Logger::debug(" 💩 [Client] Timeout waiting for chunk data");
                return result;
            }
            ssize_t n = safeRead(fd, body);
            if (n <= 0) {
                Logger::debug(" 💩 [Client] Connection closed during chunk read");
                return result;
            }
        }

        result.append(body.substr(0, chunk_size));
        body.erase(0, chunk_size + 2); // remove chunk data + \r\n
    }

    return result;
}

#include <sys/select.h>
#include <unistd.h>

bool Client::waitForData(int timeout_sec) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(this->_fd, &readfds);

    struct timeval timeout;
    timeout.tv_sec = timeout_sec;
    timeout.tv_usec = 0;

    int result = select(this->_fd + 1, &readfds, NULL, NULL, &timeout);
    return result > 0 && FD_ISSET(this->_fd, &readfds);
}


#include <sys/socket.h> // for recv
#include <unistd.h>     // for close
#include <errno.h>      // for errno
#include <cstring>      // for strerror

void Client::debugPeekFdStream()
{
    std::string accumulated;
    int attempts = 10;

    while (attempts-- > 0)
    {
        if (!waitForData(1)) {
            //Logger::debug("⏳ [Client] No data available after waiting.");
            continue;
        }

        char buffer[1025];
        ssize_t bytes_read = recv(this->_fd, buffer, 1024, MSG_PEEK);

        if (bytes_read < 0) {
            //Logger::debug("❌ [Client] recv(MSG_PEEK) failed: " + std::string(strerror(errno)));
            return;
        }

        buffer[bytes_read] = '\0';
        accumulated = std::string(buffer);

        // Check for full HTTP headers
        if (accumulated.find("\r\n\r\n") != std::string::npos) {
            //Logger::debug("📦 [Client] Full headers peeked:\n" + accumulated);
            return;
        }

        // Wait and try again
        usleep(1000000); // 100ms
    }

    //Logger::debug("⚠️ [Client] Gave up after partial data:\n" + accumulated);
}

