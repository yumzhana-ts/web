/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SessionData.class.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/16 21:52:51 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.class.hpp"
#include "ResponseBuilder/SessionData.class.hpp"

/****************************************************/
/*                    Constructor                   */
/****************************************************/

SessionData::SessionData()
{
    session_id = randomInRange(10000, 20000);
    nextHandler = NULL;
    if (DBG){ std::cout << GREEN << "[SessionData] Default Constructor called" << RESET_COLOR << std::endl;}
}

SessionData::SessionData(int session): session_id(session)
{
    nextHandler = NULL;
    name = "";
    if (DBG){ std::cout << GREEN << "[SessionData] Default Constructor called" << RESET_COLOR << std::endl;}
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

SessionData::~SessionData(void) 
{
    if (DBG){std::cout << GREEN << "[SessionData] Destructor called" << RESET_COLOR << std::endl;}
}

/****************************************************
*                 Memeber Functions                *
****************************************************/

void SessionData::handle()
{
    this->build();
    if(nextHandler)
        nextHandler->handle();
}

void SessionData::build()
{
    Logger::debug("Cookie session id: " + toString(session_id));
}