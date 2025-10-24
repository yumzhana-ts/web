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

#include "Socket.class.hpp"
#include "lib.hpp"

class Socket;

class Client
{
private:
	int _fd;
	Socket* serverSocket;
public:
	std::string raw_request;
	Client() {} ;
	Client(Socket& webserv); // Calls acceptClient() from the passed-in Socket object.
	~Client(); // Automatically close the socket when the client is destroyed.

	int getClientFd() const; // Simple getter for the socket file descriptor.
	std::string getRaw_request(); // Returns the full raw request string received from the client.
	
	ssize_t response(std::string response);
	void getRequest(); // Reads data sent by the client over the socket.
	Socket* getServerSocket() const { return serverSocket; }
	std::string readChunked(int fd, std::string &body);
	std::string readContentLength(int fd, std::string &initial, size_t content_length);
	size_t parseContentLength(const std::string &headers);
	ssize_t safeRead(int fd, std::string &buffer);
	void debugPeekFdStream();
	bool waitForData(int timeout_sec);
};
