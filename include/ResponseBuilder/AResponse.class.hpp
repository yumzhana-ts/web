/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AResponse.class.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:01 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/22 22:29:41 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <sstream>
#include "./ErrorFactory/AError.class.hpp"
#include "SessionData.class.hpp"
#include "./Sessions.class.hpp"

class RequestDataSet;

class AResponse
{
    protected:
        //RequestDataSet &request;
        std::string version; // e.g. "HTTP/1.1"
        int status_code; // e.g. 200
        std::string status_text; // e.g. "OK"
        std::map<std::string, std::string> headers; // e.g. headers["Content-Length"] == body.size()
        std::string body; // e.g. Hello, world!
        int session_id;
        bool newSession;
        std::string folder;
        std::string path;
        Errors error;
        SessionData* session;
        bool serve_cookie;
    public:
        std::string raw_response;
        void setStatusCode(int code){status_code = code;}
        //AResponse();
        AResponse(RequestDataSet &req);
        std::string full_path;
        virtual ~AResponse(void);
        std::string serialize();
        virtual void build() = 0; 
        RequestDataSet &request;
        Errors getError(){return (this->error);}
        void setError(Errors err){this->error = err;}
        std::string page;
        std::string directory;
        std::string location;
        void setSession(const int& id);
        void handleSession();
        int getStatusCode(){return (status_code);}
        int getSessionId(){return (session_id);}
        void printResponse() const;
};

#include "DataSetFactory/RequestDataSet.class.hpp"
#define DBG 0
#define RESET_COLOR "\033[0m"
#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define WHITE "\033[0;37m"
#define BOLD_BLACK "\033[1;30m"
#define BG_WHITE "\033[0;47m"

