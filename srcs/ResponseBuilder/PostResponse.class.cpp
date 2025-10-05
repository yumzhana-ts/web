/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostResponse.class.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/16 21:52:51 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseBuilder/PostResponse.class.hpp"
#include "DataSetFactory/ServerConfigDataSet.class.hpp"
#include "Logger.class.hpp"
#include "FileManager.class.hpp"
#include "./lib.hpp"

/****************************************************/
/*                    Constructor                   */
/****************************************************/

PostResponse::PostResponse(RequestDataSet &req): AResponse(req)
{
    newSession = false;
    serve_cookie = false;
    nextHandler = NULL;
    if (DBG){ std::cout << GREEN << "[PostResponse] Default Constructor called" << RESET_COLOR << std::endl;}
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

PostResponse::~PostResponse(void) 
{
    if (DBG){std::cout << GREEN << "[PostResponse] Destructor called" << RESET_COLOR << std::endl;}
}

/****************************************************
*                 Memeber Functions                *
****************************************************/

void PostResponse::handle()
{
    this->upload();
    this->build();
    if(error == NONE)
        this->raw_response = this->serialize();
}

void PostResponse::build() 
{
    this->body = request.body; // используем тело из запроса
    this->status_code = 200;
    this->status_text = "OK";
    this->headers["Content-Type"] = "text/html";

    std::ostringstream len_stream;
    len_stream << this->body.length();
    this->headers["Content-Length"] = len_stream.str();
}


/*void PostResponse::build() 
{
    full_path = "www/error/201.html";
    std::ifstream html_file(full_path.c_str());
    std::stringstream body_stream;
    body_stream << html_file.rdbuf();
    this->body = body_stream.str();
    std::ostringstream len_stream;
    len_stream << this->body.length();
    this->status_code = 201;
    this->status_text = "Created";
    this->headers["Content-Type"] = "text/html";
    this->headers["Content-Length"] = len_stream.str();
}*/

void PostResponse::upload()
{
    if (request.body.size() > ServerConfigDataSet::getInstance().client_max_body_size) 
    {
		this->setError(PAYLOADTOOLARGE);
		return;
    }
	std::string filename = "upload_" + getTimestampForFilename();
	std::string filefullpath = this->directory + filename;
	try
	{
		FileManager::getInstance()->uploadFile(this->directory, filename, request.text, request.body);
	}
	catch(const std::exception &e)
	{
		this->setError(INTERNALERROR);
	}
    full_path = "www/error/201.html";
}