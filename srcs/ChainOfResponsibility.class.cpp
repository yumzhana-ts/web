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

#include "DataSetFactory/ADataSet.class.hpp"
#include "ChainOfResponsibility.class.hpp"
#include "ChainBuilder.class.hpp"
#include "DataSetFactory/RequestDataSet.class.hpp"


/****************************************************/
/*                    Constructor                   */
/****************************************************/

IHandler::IHandler() : nextHandler(NULL)
{
    if (DBG) { std::cout << GREEN << "[IHandler] Default Constructor called" << RESET_COLOR << std::endl; }
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

IHandler::~IHandler() 
{
    if (nextHandler)
        delete nextHandler;
    if (DBG) std::cout << GREEN << "[IHandler] Destructor called" << RESET_COLOR << std::endl;
}

/****************************************************
*                 Memeber Functions                *
****************************************************/

IHandler* IHandler::setNext(IHandler* handler)
{
    this->nextHandler = handler;
    return handler;
}

/****************************************************/
/*                    Constructor                   */
/****************************************************/

ChainOfResponsibility::ChainOfResponsibility(std::string &buf, Client& client, Socket& socket) 
{
    request = new RequestDataSet(buf, client, socket);
    request->handle();
    if (DBG){ std::cout << GREEN << "[ChainOfResponsibility] Default Constructor called" << RESET_COLOR << std::endl;}
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

ChainOfResponsibility::~ChainOfResponsibility() 
{
    delete request;
    if (DBG) std::cout << GREEN << "[ChainOfResponsibility] Destructor called" << RESET_COLOR << std::endl;
}

/****************************************************
*                 Memeber Functions                *
****************************************************/

