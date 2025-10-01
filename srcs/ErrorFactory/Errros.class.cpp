/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PageNotFound.class.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/16 21:52:51 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorFactory/Errors.class.hpp"
#include "DataSetFactory/ServerConfigDataSet.class.hpp"
#include "DataSetFactory/RequestDataSet.class.hpp"
#include "Logger.class.hpp"


/****************************************************/
/*                    BadRequest                     */
/****************************************************/

BadRequest::BadRequest(RequestDataSet &req): AResponse(req) 
{
    error_code = 400;
    error_type = "Bad request";

    if (DBG){ std::cout << GREEN << "[BadRequest] Default Constructor called" << RESET_COLOR << std::endl;}
}

BadRequest::~BadRequest()
{
    if (DBG){ std::cout << GREEN << "[BadRequest] Destructor called" << RESET_COLOR << std::endl;}
}

void BadRequest::handle()
{
    this->build();
    this->raw_response = this->serialize();
    if(nextHandler)
        nextHandler->handle();
}

void BadRequest::build()
{
    std::string directory = "www/";
    std::string error_page = ServerConfigDataSet::getInstance().error_pages[400];
    std::string full_path = directory + error_page;
    std::ifstream file(full_path.c_str());
    std::stringstream body_stream;
    body_stream << file.rdbuf();
    this->body = body_stream.str();
    std::ostringstream len_stream;
    len_stream << this->body.length();
    this->status_code = 400;
    this->status_text = "Bad Request";
    this->headers["Content-Type"] = "text/html";
    this->headers["Content-Length"] = len_stream.str(); 
}

/****************************************************/
/*                    Forbidden                      */
/****************************************************/

Forbidden::Forbidden(RequestDataSet &req): AResponse(req) 
{
    error_code = 403;
    error_type = "Forbidden";
    if (DBG){ std::cout << GREEN << "[Forbidden] Default Constructor called" << RESET_COLOR << std::endl;}
}

Forbidden::~Forbidden()
{
    if (DBG){ std::cout << GREEN << "[Forbidden] Destructor called" << RESET_COLOR << std::endl;}
}

void Forbidden::handle()
{
    this->build();
    this->raw_response = this->serialize();
    if(nextHandler)
        nextHandler->handle();
}

void Forbidden::build()
{
    std::string directory = "www/";
    std::string error_page = ServerConfigDataSet::getInstance().error_pages[403];
    std::string full_path = directory + error_page;
    std::ifstream file(full_path.c_str());
    std::stringstream body_stream;
    body_stream << file.rdbuf();
    this->body = body_stream.str();
    std::ostringstream len_stream;
    len_stream << this->body.length();
    this->status_code = 403;
    this->status_text = "Forbidden";
    this->headers["Content-Type"] = "text/html";
    this->headers["Content-Length"] = len_stream.str(); 
}

/****************************************************/
/*                    PageNotFound                   */
/****************************************************/

PageNotFound::PageNotFound(RequestDataSet &req): AResponse(req) 
{
    error_code = 404;
    error_type = "Page not found";
    if (DBG){ std::cout << GREEN << "[PageNotFound] Default Constructor called" << RESET_COLOR << std::endl;}
}

PageNotFound::~PageNotFound()
{
    if (DBG){ std::cout << GREEN << "[PageNotFound] Destructor called" << RESET_COLOR << std::endl;}
}

void PageNotFound::handle()
{
    this->build();
    this->raw_response = this->serialize();
    if(nextHandler)
        nextHandler->handle();
}

void PageNotFound::build()
{
    std::string directory = "www/";
    std::string error_page = ServerConfigDataSet::getInstance().error_pages[404];
    std::string full_path = directory + error_page;
    std::ifstream file(full_path.c_str());
    std::stringstream body_stream;
    body_stream << file.rdbuf();
    this->body = body_stream.str();
    std::ostringstream len_stream;
    len_stream << this->body.length();
    this->status_code = 404;
    this->status_text = "Not Found";
    this->headers["Content-Type"] = "text/html";
    this->headers["Content-Length"] = len_stream.str(); 
}

/****************************************************/
/*                    MethodNotAllowed              */
/****************************************************/

MethodNotAllowed::MethodNotAllowed(RequestDataSet &req): AResponse(req) 
{
    error_code = 405;
    error_type = "Method not allowed";
    if (DBG){ std::cout << GREEN << "[MethodNotAllowed] Default Constructor called" << RESET_COLOR << std::endl;}
}

MethodNotAllowed::~MethodNotAllowed()
{
    if (DBG){ std::cout << GREEN << "[MethodNotAllowed] Destructor called" << RESET_COLOR << std::endl;}
}

void MethodNotAllowed::handle()
{
    this->build();
    this->raw_response = this->serialize();
    if(nextHandler)
        nextHandler->handle();
}

void MethodNotAllowed::build()
{
    std::string directory = "www/";
    std::string error_page = ServerConfigDataSet::getInstance().error_pages[405];
    std::string full_path = directory + error_page;
    std::ifstream file(full_path.c_str());
    std::stringstream body_stream;
    body_stream << file.rdbuf();
    this->body = body_stream.str();
    std::ostringstream len_stream;
    len_stream << this->body.length();
    this->status_code = 405;
    this->status_text = "Method Not Allowed";
    this->headers["Content-Type"] = "text/html";
    this->headers["Content-Length"] = len_stream.str(); 
}

/****************************************************/
/*                    PayloadTooLarge               */
/****************************************************/

PayloadTooLarge::PayloadTooLarge(RequestDataSet &req): AResponse(req) 
{
    error_code = 413;
    error_type = "Payload too large";
    if (DBG){ std::cout << GREEN << "[PayloadTooLarge] Default Constructor called" << RESET_COLOR << std::endl;}
}

PayloadTooLarge::~PayloadTooLarge()
{
    if (DBG){ std::cout << GREEN << "[PayloadTooLarge] Destructor called" << RESET_COLOR << std::endl;}
}

void PayloadTooLarge::handle()
{
    this->build();
    this->raw_response = this->serialize();
    if(nextHandler)
        nextHandler->handle();
}

void PayloadTooLarge::build()
{
    std::string directory = "www/";
    std::string error_page = ServerConfigDataSet::getInstance().error_pages[413];
    std::string full_path = directory + error_page;
    std::ifstream file(full_path.c_str());
    std::stringstream body_stream;
    body_stream << file.rdbuf();
    this->body = body_stream.str();
    std::ostringstream len_stream;
    len_stream << this->body.length();
    this->status_code = 413;
    this->status_text = "Payload Too Large";
    this->headers["Content-Type"] = "text/html";
    this->headers["Content-Length"] = len_stream.str(); 
}

/****************************************************/
/*                    InternalServerError           */
/****************************************************/

InternalServerError::InternalServerError(RequestDataSet &req): AResponse(req) 
{
    error_code = 500;
    error_type = "Internal error";
    if (DBG){ std::cout << GREEN << "[InternalServerError] Default Constructor called" << RESET_COLOR << std::endl;}
}

InternalServerError::~InternalServerError()
{
    if (DBG){ std::cout << GREEN << "[InternalServerError] Destructor called" << RESET_COLOR << std::endl;}
}

void InternalServerError::handle()
{
    this->build();
    this->raw_response = this->serialize();
    if(nextHandler)
        nextHandler->handle();
}

void InternalServerError::build()
{
    std::string directory = "www/";
    std::string error_page = ServerConfigDataSet::getInstance().error_pages[500];
    std::string full_path = directory + error_page;
    std::ifstream file(full_path.c_str());
    std::stringstream body_stream;
    body_stream << file.rdbuf();
    this->body = body_stream.str();
    std::ostringstream len_stream;
    len_stream << this->body.length();
    this->status_code = 500;
    this->status_text = "Internal Server Error";
    this->headers["Content-Type"] = "text/html";
    this->headers["Content-Length"] = len_stream.str(); 
}

/****************************************************/
/*                    BadGateway                     */
/****************************************************/

BadGateway::BadGateway(RequestDataSet &req): AResponse(req) 
{
    error_code = 502;
    error_type = "Bad gateway";
    if (DBG){ std::cout << GREEN << "[BadGateway] Default Constructor called" << RESET_COLOR << std::endl;}
}

BadGateway::~BadGateway()
{
    if (DBG){ std::cout << GREEN << "[BadGateway] Destructor called" << RESET_COLOR << std::endl;}
}

void BadGateway::handle()
{
    this->build();
    this->raw_response = this->serialize();
    if(nextHandler)
        nextHandler->handle();
}

void BadGateway::build()
{
    std::string directory = "www/";
    std::string error_page = ServerConfigDataSet::getInstance().error_pages[502];
    std::string full_path = directory + error_page;
    std::ifstream file(full_path.c_str());
    std::stringstream body_stream;
    body_stream << file.rdbuf();
    this->body = body_stream.str();
    std::ostringstream len_stream;
    len_stream << this->body.length();
    this->status_code = 502;
    this->status_text = "Bad Gateway";
    this->headers["Content-Type"] = "text/html";
    this->headers["Content-Length"] = len_stream.str(); 
}

/****************************************************/
/*                    GatewayTimeout                 */
/****************************************************/

GatewayTimeout::GatewayTimeout(RequestDataSet &req): AResponse(req) 
{
    error_code = 504;
    error_type = "Gateway timeout";
    if (DBG){ std::cout << GREEN << "[GatewayTimeout] Default Constructor called" << RESET_COLOR << std::endl;}
}

GatewayTimeout::~GatewayTimeout()
{
    if (DBG){ std::cout << GREEN << "[GatewayTimeout] Destructor called" << RESET_COLOR << std::endl;}
}

void GatewayTimeout::handle()
{
    this->build();
    this->raw_response = this->serialize();
    if(nextHandler)
        nextHandler->handle();
}

void GatewayTimeout::build()
{
    std::string directory = "www/";
    std::string error_page = ServerConfigDataSet::getInstance().error_pages[504];
    std::string full_path = directory + error_page;
    std::ifstream file(full_path.c_str());
    std::stringstream body_stream;
    body_stream << file.rdbuf();
    this->body = body_stream.str();
    std::ostringstream len_stream;
    len_stream << this->body.length();
    this->status_code = 504;
    this->status_text = "Gateway Timeout";
    this->headers["Content-Type"] = "text/html";
    this->headers["Content-Length"] = len_stream.str(); 
}

/****************************************************/
/*                    HTTPVersionNotSupported       */
/****************************************************/

HTTPVersionNotSupported::HTTPVersionNotSupported(RequestDataSet &req): AResponse(req) 
{
    error_code = 505;
    error_type = "HTTP version not supported";
    if (DBG){ std::cout << GREEN << "[HTTPVersionNotSupported] Default Constructor called" << RESET_COLOR << std::endl;}
}

HTTPVersionNotSupported::~HTTPVersionNotSupported()
{
    if (DBG){ std::cout << GREEN << "[HTTPVersionNotSupported] Destructor called" << RESET_COLOR << std::endl;}
}

void HTTPVersionNotSupported::handle()
{
    this->build();
    this->raw_response = this->serialize();
    if(nextHandler)
        nextHandler->handle();
}

void HTTPVersionNotSupported::build()
{
    std::string directory = "www/";
    std::string error_page = ServerConfigDataSet::getInstance().error_pages[505];
    std::string full_path = directory + error_page;
    std::ifstream file(full_path.c_str());
    std::stringstream body_stream;
    body_stream << file.rdbuf();
    this->body = body_stream.str();
    std::ostringstream len_stream;
    len_stream << this->body.length();
    this->status_code = 505;
    this->status_text = "HTTP Version Not Supported";
    this->headers["Content-Type"] = "text/html";
    this->headers["Content-Length"] = len_stream.str(); 
}

/****************************************************/
/*                    URI TOO LONG                  */
/****************************************************/


URITooLong::URITooLong(RequestDataSet &req): AResponse(req) 
{
    error_code = 414;
    error_type = "URI Too Long";
    if (DBG){ std::cout << GREEN << "[URITooLong] Default Constructor called" << RESET_COLOR << std::endl;}
}

URITooLong::~URITooLong()
{
    if (DBG){ std::cout << GREEN << "[URITooLong] Destructor called" << RESET_COLOR << std::endl;}
}

void URITooLong::handle()
{
    this->build();
    this->raw_response = this->serialize();
    if (nextHandler)
        nextHandler->handle();
}

void URITooLong::build()
{
    std::string directory = "www/";
    std::string error_page = ServerConfigDataSet::getInstance().error_pages[414];
    std::string full_path = directory + error_page;
    std::ifstream file(full_path.c_str());
    std::stringstream body_stream;
    body_stream << file.rdbuf();
    this->body = body_stream.str();

    std::ostringstream len_stream;
    len_stream << this->body.length();

    this->status_code = 414;
    this->status_text = "URI Too Long";
    this->headers["Content-Type"] = "text/html";
    this->headers["Content-Length"] = len_stream.str();
}