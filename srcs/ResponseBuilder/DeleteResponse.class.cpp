/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteResponse.class.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/16 21:52:51 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseBuilder/DeleteResponse.class.hpp"
#include "Logger.class.hpp"
#include "FileManager.class.hpp"

/****************************************************/
/*                    Constructor                   */
/****************************************************/

DeleteResponse::DeleteResponse(RequestDataSet &req): AResponse(req)
{
    newSession = false;
    serve_cookie = false;
    nextHandler = NULL;
    if (DBG){ std::cout << GREEN << "[DeleteResponse] Default Constructor called" << RESET_COLOR << std::endl;}
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

DeleteResponse::~DeleteResponse(void) 
{
    if (DBG){std::cout << GREEN << "[DeleteResponse] Destructor called" << RESET_COLOR << std::endl;}
}

/****************************************************
*                 Memeber Functions                *
****************************************************/

void DeleteResponse::handle()
{
    this->deleteFile();
    this->build();
    this->raw_response = this->serialize();
    if (this->nextHandler)
        this->nextHandler->handle();
}

void DeleteResponse::build()
{
    full_path = "www/error/200.html";
    std::ifstream html_file(full_path.c_str());
    std::stringstream body_stream;
    body_stream << html_file.rdbuf();
    this->body = body_stream.str();
    std::ostringstream len_stream;
    len_stream << this->body.length();
    this->status_code = 200;
    this->status_text = "Deleted";
    this->headers["Content-Type"] = "text/html";
    this->headers["Content-Length"] = len_stream.str();
}

void DeleteResponse::deleteFile()
{
	int id = 0;
	if (!request.query_params.empty())
		id = atoi(request.query_params.begin()->second.c_str());
	try
	{
		FileManager::getInstance()->deleteFile(id, this->directory);
	}
	catch(const std::exception &e)
	{
		this->setError(INTERNALERROR);
	}
}

/*

void LocationDecorator::deleteFiles()
{
	int id = 0;
	if (!response->request.query_params.empty())
		id = atoi(response->request.query_params.begin()->second.c_str());
	try
	{
		FileManager::getInstance()->deleteFile(id, directory);
	}
	catch(const std::exception &e)
	{
		response->setError(INTERNALERROR);
	}
}*/