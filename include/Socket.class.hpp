#pragma once

#include "Client.class.hpp"
#include "lib.hpp"

class Client;

class Socket
{
	private:
		int _server_fd;
		int _port;
		struct sockaddr_in _address; // holds the server IP, port, and family(AF_INET).
		socklen_t _addrlen; // is needed for accept() - it stores the size of the sockaddr_in.
		std::string res;

	public:
		Socket() {};
		Socket(int port);
		~Socket();
		bool setup(); // Sets up the entire listening socket. socket(), setsockopt(), bind(), listen().
		int acceptClient(); // Waits for a client to connect. Returns a new file desctiptor - used to communicate with that specific client.
		int response(const Client& client); // Sends the current response string - res, to a specific client.
		int getServerFd() const;
		void setResponse(std::string res);
		void closeServerSocket();
		void shutdown();
		int getServerPort(){return (this->_port);}
};