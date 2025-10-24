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
#include "lib.hpp"
#include "Socket.class.hpp"
#include "Client.class.hpp"
#include "ChainOfResponsibility.class.hpp"
#include "ChainBuilder.class.hpp"
#include "DataSetFactory/ServerConfigDataSet.class.hpp"


class EventDispatcher {
private:
	int epoll_fd;
	std::map<int, Client*> clients;
	std::map<int, Socket*> sockets;
    static void signalHandler(int signum);
public:
	static EventDispatcher* g_server;
	static volatile sig_atomic_t g_running;
	void setupSignals();
	Socket* getSocket(int fd);
	EventDispatcher();
	~EventDispatcher();
	void shutdown();
	void setupSocket(int port);
    void run();
	void setupClient(Socket &socket);
	void handleClient(Client& client);
	void addServerToEpoll(int server_fd);
	void addClientToEpoll(int client_fd);
	std::map<int, Socket*> getSockets(){return (this->sockets);}
	void setupSockets();
	void setupConfig(std::string configFile);
};

void set_non_blocking(int fd);