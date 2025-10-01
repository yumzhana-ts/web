/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChainOfResponsibility.class.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/17 00:39:41 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Sessions.class.hpp"
#include "Logger.class.hpp"

SessionManager* SessionManager::instance = NULL;
/****************************************************/
/*                    Constructor                   */
/****************************************************/

SessionManager::SessionManager() 
{
    if (DBG){ std::cout << GREEN << "[SessionManager] Default Constructor called" << RESET_COLOR << std::endl;}
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

SessionManager::~SessionManager() 
{
    if (DBG){std::cout << GREEN << "[SessionManager] Destructor called" << RESET_COLOR << std::endl;}
    
    for (size_t i = 0; i < sessions.size(); ++i) 
        delete sessions[i];
    
    sessions.clear();
}

/****************************************************
*                 Memeber Functions                *
****************************************************/

SessionManager* SessionManager::getInstance() 
{
    if (!instance)
        instance = new SessionManager();
    return instance;
}

void SessionManager::destroyInstance() 
{
    delete instance;
    instance = NULL;
}

void SessionManager::addSession(SessionData* session) 
{
    sessions.push_back(session);
}

SessionData* SessionManager::getSession(const int& id) 
{
    for (size_t i = 0; i < sessions.size(); ++i) {
        if (sessions[i]->getSessionId() == id)
            return sessions[i];
    }
    return NULL;
}

void SessionManager::printSessions() const
{
    if (sessions.empty())
    {
        std::cout << RED <<  "[Sessions] No active sessions" << RESET_COLOR << std::endl;
        return;
    }
    std::string output = "Active sessions: ";
    for (size_t i = 0; i < sessions.size(); ++i)
    {
        output += toString(sessions[i]->getSessionId());
        if (i + 1 < sessions.size())
            output += ", ";
    }
    std::cout << RED <<  "[Sessions] "  << output << RESET_COLOR << std::endl;
}