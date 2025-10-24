#include "include/Socket.class.hpp"
#include "Logger.class.hpp"
#include "Client.class.hpp"
#include "Socket.class.hpp"
#include "EventDispatcher.class.hpp"

void Socket::shutdown() 
{
	close(this->_server_fd);
	Logger::info("Shutdown server.\n");
}

Socket::Socket(int port) 
: _server_fd(-1), _port(port), _addrlen(sizeof(_address))
{
	std::memset(&_address, 0, sizeof(_address));
}

Socket::~Socket()
{

}

bool Socket::setup()
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd < 0)
	{
		throw std::runtime_error("Socket creation failed");
	}
	int opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		throw std::runtime_error("Setsockopt failed");
	}
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = ServerConfigDataSet::getInstance().new_host;
	if (_address.sin_addr.s_addr == INADDR_NONE)
		throw std::runtime_error("Invalid ip address");
	_address.sin_port = htons(_port);
	if (bind(_server_fd, (sockaddr *)&_address, sizeof(_address)) == -1)
		throw std::runtime_error("Bind failed");
	if (listen(_server_fd, 10) == -1)
		throw std::runtime_error("Listen failed");
	Logger::info("✅ [Socket] Server listening on port " + toString(_port) + " — setup complete");
	return true;
}

int Socket::acceptClient()
{
	int fd = accept(_server_fd, (sockaddr *)&_address, &_addrlen); 
	return fd;
}

int Socket::response(const Client& client)
{
    int bytesSent = send(client.getClientFd(), this->res.c_str(), this->res.length(), 0);

    if (bytesSent < 0)
        throw std::runtime_error("❌ Failed to send to FD " + toString(client.getClientFd()) + ": " + std::string(strerror(errno)));
    else 
		Logger::debug(" ✅ [Socket] [FD " + toString(client.getClientFd()) + "] Sent " + toString(bytesSent) + " bytes");
    return bytesSent;
}


int Socket::getServerFd() const
{
	return _server_fd;
}

void Socket::setResponse(std::string res)
{
	this->res = res;
}

