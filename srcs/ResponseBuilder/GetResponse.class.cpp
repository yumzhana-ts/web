/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GetResponse.class.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/16 21:52:51 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseBuilder/GetResponse.class.hpp"
#include "Logger.class.hpp"
#include "DataSetFactory/ServerConfigDataSet.class.hpp"
#include "DataSetFactory/RequestDataSet.class.hpp"

/****************************************************/
/*                    Constructor                   */
/****************************************************/

GetResponse::GetResponse(RequestDataSet &req): AResponse(req)
{
    newSession = false;
    serve_cookie = false;
    nextHandler = NULL;
    if (DBG){ std::cout << GREEN << "[GetResponse] Default Constructor called" << RESET_COLOR << std::endl;}
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

GetResponse::~GetResponse(void) 
{
    if (DBG){std::cout << GREEN << "[GetResponse] Destructor called" << RESET_COLOR << std::endl;}
}

/****************************************************
*                 Memeber Functions                *
****************************************************/


void GetResponse::handle()
{
    if (serve_cookie == true)
        this->handleSession();
    this->build();
    if(error == NONE)
        this->raw_response = this->serialize();
}

void GetResponse::build()
{
    if (request.path == "/favicon.ico" || request.method == "HEAD") 
    {
        this->status_code = 204;
        this->status_text = "";
        this->body = "";
        this->headers["Content-Length"] = "0";
        return;
    }
    std::ifstream file(full_path.c_str());
    std::stringstream body_stream;
    body_stream << file.rdbuf();
    this->body = body_stream.str();
    std::ostringstream len_stream;
    len_stream << this->body.length();
    this->headers["Content-Length"] = len_stream.str();
    if(!this->status_code)
        this->status_code = 200;
    this->status_text = "OK";
    this->headers["Content-Type"] = "text/html";
}
