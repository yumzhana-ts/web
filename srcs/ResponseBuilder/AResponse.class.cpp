/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AAResponse.class.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/27 17:13:21 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseBuilder/AResponse.class.hpp"
#include "Logger.class.hpp"

/****************************************************/
/*                    Constructor                   */
/****************************************************/

AResponse::AResponse(RequestDataSet &req)
    : version(""), status_code(0), status_text(""), headers(), body(""), 
      session_id(0), newSession(false), folder(""), path(""), error(NONE), 
      session(NULL), serve_cookie(false), raw_response(""), full_path(""), 
      request(req), page(""), directory("")
{
    error = NONE;
    raw_response = "";
    if (DBG) { std::cout << GREEN << "[AResponse] Default Constructor called" << RESET_COLOR << std::endl; }
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

AResponse::~AResponse(void) 
{
    if (DBG){ std::cout << GREEN << "[AResponse] Destructor called" << RESET_COLOR << std::endl;}
    session = NULL;
}

/****************************************************
*                 Memeber Functions                *
****************************************************/

std::string AResponse::serialize()
{
	std::ostringstream oss;

	oss << "HTTP/1.1 " << status_code << " " << status_text << "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); 
        it != headers.end(); ++it) 
    {
		oss << it->first << ": " << it->second << "\r\n";
	}
	oss << "\r\n";
	oss << body;
	return oss.str();
}

void AResponse::setSession(const int& id)
{
    session = SessionManager::getInstance()->getSession(id);
    if (session == NULL)
    {
        session = new SessionData;
        SessionManager::getInstance()->addSession(session);
    }
    if (this->session_id == 0)
    {
        std::string cookie = "session_id=" + toString(session->getSessionId()) + "; Path=/; HttpOnly";
        headers["Set-Cookie"] = cookie;
    }
    this->session_id = session->getSessionId();
}

void AResponse::handleSession()
{
    this->setSession(this->session_id);
}