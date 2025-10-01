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

#include "ChainBuilder.class.hpp"
#include "Logger.class.hpp"
#include "ResponseBuilder/SessionData.class.hpp"
#include "DataSetFactory/ServerConfigDataSet.class.hpp"
#include "ErrorFactory/Errors.class.hpp"
#include "ErrorFactory/ErrorFactory.class.hpp"
#include "ResponseBuilder/LocationDecorator.class.hpp"
#include "ResponseBuilder/CGIEnvBuilder.class.hpp"

/****************************************************/
/*                    Constructor                   */
/****************************************************/

ChainBuilder::ChainBuilder(RequestDataSet &req):request(req)
{
    this->error_text = request.error;
    this->nextHandler = NULL;
    if (DBG){ std::cout << GREEN << "[ChainBuilder] Default Constructor called" << RESET_COLOR << std::endl;}
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

ChainBuilder::~ChainBuilder()
{
    response = NULL;
    if (DBG)
        std::cout << GREEN << "[ChainBuilder] Destructor called" << RESET_COLOR << std::endl;
}

/****************************************************
*                 Memeber Functions                *
****************************************************/

void ChainBuilder::handle()
{   
    try
    {
        this->validateRequest();
        this->buildResponse();
        if (this->nextHandler)
        {
            this->handleLocation();
            this->nextHandler->handle();
            this->validateResponse();
        }
    }
    catch(std::exception &e)
    {
        this->buildErrorResponse();
    }
    logResponse();
    SendResponseHandler(request._socket, request._client, response->raw_response);
}


void ChainBuilder::handleLocation()
{
    AResponse *resp = dynamic_cast<AResponse*>(this->nextHandler);
    if (resp) 
    {
        LocationDecorator decorator(resp);
        this->validateResponse();
        if(decorator.cgi == true)
            this->handleCgi(&decorator);
    }
}


void ChainBuilder::validateResponse()
{
    if (!this->nextHandler)
        return;
    AResponse* resp = dynamic_cast<AResponse*>(this->nextHandler);
    if (!resp)
        return;
    error_text = resp->getError();
    if (error_text != NONE)
    {
        delete this->nextHandler;
        throw std::logic_error("");
    }
}

void ChainBuilder::buildErrorResponse()
{
    ErrorFactory factory(request);
    error = factory.createError(error_text);
    response = dynamic_cast<AResponse*>(error);
    this->setNext(dynamic_cast<IHandler*>(error));
    this->nextHandler->handle();
}

void ChainBuilder::buildResponse()
{
    if (request.method == "GET" || request.method == "HEAD")
        handleGet();
    else if (request.method == "POST")
        handlePost();
    else if (request.method == "DELETE")
        handleDelete();
    else handleUnsupportedMethod();
}


void ChainBuilder::handleGet()
{
    GetResponse *get = new GetResponse(request);
    response = get;
    this->setNext(get);
}

void ChainBuilder::handlePost()
{
    PostResponse *post = new PostResponse(request);
    response = post;
    this->setNext(post);
}

void ChainBuilder::handleDelete()
{
    DeleteResponse *del = new DeleteResponse(request);
    response = del;
    this->setNext(del);
}

void ChainBuilder::handleCgi(LocationDecorator *decorator)
{
    if(this->nextHandler != NULL)
        delete this->nextHandler;
    CgiHandler *cgi = new CgiHandler(request, decorator->full_path, request.session_id);
    response = cgi;
    CGIEnvBuilder(cgi, decorator);
    this->setNext(cgi);
}

void ChainBuilder::handleUnsupportedMethod()
{
    if(this->error_text != NONE)
        this->error_text = METHODNOTALLOWED;
    throw std::logic_error("");
}

void ChainBuilder::validateRequest()
{
    if(error_text != NONE)
        throw std::logic_error("");
}

void ChainBuilder::logResponse()
{
    std::string emoji = (response->getStatusCode() >= 200 && response->getStatusCode() < 300) ? "✉️ " :
                        (response->getStatusCode() >= 400 && response->getStatusCode() < 500) ? "🦩 " :
                        (response->getStatusCode() >= 500) ? "💀" : "ℹ️ ";

    Logger::info(emoji + " Served " + request.method + " response to path: " + request.path +
                    " with status code: " + toString(response->getStatusCode()));
}