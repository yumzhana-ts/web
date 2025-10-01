/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:41:01 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/09/30 18:02:24 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EventDispatcher.class.hpp"
#include "Logger.class.hpp"
#include "lib.hpp"

//TODO: stupid cookies are not working again as it should
//TODO: handle validation schema + {}
//TODO: test cgi with game

//TODO: create folder for location if none
//TODO: localhost name support
//TODO: /database выводит пустую страницу
//TODO: still dont know how cgi is tested

int main(int argc, char **argv) 
{
    if(argc == 2)
    {
        Logger::info("🔧 Server starting");
        EventDispatcher server;
        EventDispatcher::g_server = &server;
        server.setupSignals();
        try
        {
            //server.setupConfig("config/default.conf");
            server.setupConfig(argv[1]);
            server.setupSockets();
            server.run();
        }
        catch(const std::exception &e)
        {
            server.shutdown();
            Logger::error(toString(e.what()));
            return (1);
        }
        server.shutdown();
        Logger::info("🛑 Server stopped gracefully");
        return (0);
    }
    Logger::error("🛑 Provide configuration file");
    return (0);
}