/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PutPutResponse.class.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/10/01 20:55:39 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseBuilder/PutResponse.class.hpp"
#include "DataSetFactory/ServerConfigDataSet.class.hpp"
#include "Logger.class.hpp"
#include "FileManager.class.hpp"
#include "./lib.hpp"

/****************************************************/
/*                    Constructor                   */
/****************************************************/

PutResponse::PutResponse(RequestDataSet &req): AResponse(req)
{
    newSession = false;
    serve_cookie = false;
    nextHandler = NULL;
    if (DBG){ std::cout << GREEN << "[PutResponse] Default Constructor called" << RESET_COLOR << std::endl;}
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

PutResponse::~PutResponse(void) 
{
    if (DBG){std::cout << GREEN << "[PutResponse] Destructor called" << RESET_COLOR << std::endl;}
}

/****************************************************
*                 Memeber Functions                *
****************************************************/

void PutResponse::handle()
{
    this->upload();
    this->build();
    if(error == NONE)
        this->raw_response = this->serialize();
}


void PutResponse::build() 
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
}

void PutResponse::upload()
{
    if (request.body.size() > ServerConfigDataSet::getInstance().client_max_body_size) 
    {
		this->setError(PAYLOADTOOLARGE);
		return;
    }
	std::string filename = this->page;
	std::string filefullpath = this->directory + filename;
	try
	{
		FileManager::getInstance()->uploadFile(this->directory, filename, request.text, request.body);
	}
	catch(const std::exception &e)
	{
		this->setError(INTERNALERROR);
	}
} 
