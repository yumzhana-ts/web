/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticHandleandler.class.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/21 17:07:40 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseBuilder/SendResponseHandler.class.hpp"
#include "Logger.class.hpp"

/****************************************************/
/*                    Constructor                   */
/****************************************************/

SendResponseHandler::SendResponseHandler(Socket &socket, Client &client, std::string raw_response) 
{
    nextHandler = NULL;
    socket.setResponse(raw_response);
    socket.response(client);
    if (DBG){ std::cout << GREEN << "[SendResponseHandler] Default Constructor called" << RESET_COLOR << std::endl;}
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

SendResponseHandler::~SendResponseHandler(void) 
{
    if (DBG){std::cout << GREEN << "[SendResponseHandler] Destructor called" << RESET_COLOR << std::endl;}
}

/****************************************************
*                 Memeber Functions                *
****************************************************/

void SendResponseHandler::handle()
{
    Logger::info("Handling static response");
}
