/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:41:01 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/10/23 23:17:10 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EventDispatcher.class.hpp"
#include "Logger.class.hpp"
#include "lib.hpp"


//TODO: fix config verification
//TODO: CGI to take interpereter from config

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