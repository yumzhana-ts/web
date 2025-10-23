/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestDataSet.class.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:01 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/16 21:18:55 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include "ADataSet.class.hpp"
#include "../ChainOfResponsibility.class.hpp"
#include "ResponseBuilder/GetResponse.class.hpp"
#include "ResponseBuilder/PostResponse.class.hpp"
#include "ResponseBuilder/DeleteResponse.class.hpp"
#include "./ErrorFactory/AError.class.hpp"

class RequestDataSet: public ADataSet, public IHandler
{
    public:
        RequestDataSet(std::string text, Client& client, Socket& socket);
        RequestDataSet(){}
        virtual ~RequestDataSet(void);
        std::string method;
        std::string path;
        std::string query_string;
        std::string version;
        std::map<std::string, std::string> query_params;
        std::map<std::string, std::vector<std::string> > headers;
        std::map<std::string, std::string> cookie_map;
        std::map<std::string, std::string> multipart_map;
        std::map<std::string, std::string> method_map;
        std::map<std::string, std::string> body_map;
        std::vector<std::string> body_buffers;
        unsigned int client_max_body_size;
        int content_length_value;
        std::string body;
        int session_id;
        bool text;
        Client _client;
        Socket _socket;
        size_t recursion_count;
        uint32_t host_ip;
        void tokenizeAndExtractBody();
        std::string extractBody(const std::string &buf);
        class Part {
        public:
            std::map<std::string, std::vector<std::string> > headers;
            std::map<std::string, std::string> headers_map;
            std::string body;
            std::vector<std::vector<std::string> > token_headers;
            Part() {}
        };

        std::string boundaryStart;
        std::string boundaryEnd;
        std::string boundary;
        std::vector<Part> parts;
        std::map<std::string, std::string> bodyData;
        std::string bodyrawline;
        void parse();
        void handle();
        void validate();
        void map();
        void parseQuery();
        void printConfig() const;
        void cookieFinder();
        void handleBodyData();
        void bodyHandle();
        void addKeyValuePairs(const std::string& str, std::map<std::string, std::string>& outMap);
        void debugPrint(size_t line_no, const std::string &line, const std::vector<std::string> &line_tokens);
        std::vector<std::string> saveLine(const std::string &line);
        void unsupportedRequest();
        std::vector<std::string> saveRequestLine(const std::string &line);
        void rfcFormat();
        bool parseHostLineToIP(const std::string &line);
        bool validateContentLength();
};


#define DBG 0
// Define ANSI escape sequences for colors
#define RESET_COLOR "\033[0m"
#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define WHITE "\033[0;37m"
#define BOLD_BLACK "\033[1;30m"
#define BG_WHITE "\033[0;47m"
