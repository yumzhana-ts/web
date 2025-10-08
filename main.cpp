/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:41:01 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/10/01 21:39:56 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EventDispatcher.class.hpp"
#include "Logger.class.hpp"
#include "lib.hpp"


//TODO: handle validation schema + {}
//TODO: hostname
//TODO: still dont know how cgi is tested

//TODO: PUT + locationhandler: logic for saving and checking directory
//TODO: POST + script: cgi runner
//TODO: POST + 


//TODO: put has to be passed to location check
//TODO: make cleint max  body size manadatory
//TODO: check if there is index in vector of saved indexes
//

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