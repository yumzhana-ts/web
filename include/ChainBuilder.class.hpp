/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChainOfResponsibility.class.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:01 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/19 13:08:35 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "lib.hpp"
#include "ChainOfResponsibility.class.hpp"
#include "ResponseBuilder/SendResponseHandler.class.hpp"
#include "ResponseBuilder/DeleteResponse.class.hpp"
#include "ResponseBuilder/GetResponse.class.hpp"
#include "ResponseBuilder/PostResponse.class.hpp"
#include "ResponseBuilder/PutResponse.class.hpp"
#include "ResponseBuilder/AResponse.class.hpp"
#include "ErrorFactory/AError.class.hpp"
#include "ResponseBuilder/CgiHandler.class.hpp"
#include "ResponseBuilder/LocationDecorator.class.hpp"

//fix to reference
class ChainBuilder: public IHandler
{
    public:
        RequestDataSet request;
        AResponse *response;
        Errors error_text;
        AError* error;
        ChainBuilder(RequestDataSet &req);
        virtual ~ChainBuilder(void);
        void handle();
        void buildResponse();
        void modeHandler(IHandler* current_handler);
        IHandler* setHandler(IHandler* current_handler, IHandler* next_handler);
        void test_CGI();
        void validateRequest();
        void buildErrorResponse();
        void validateResponse();
        void handleCgi(std::string script);
        void handleGet();
        void handlePost();
        void handleDelete();
        void handlePut();
        void handleUnsupportedMethod();
        void logResponse();
        void handleLocation();
        void handleCgi(std::string script, std::string target);
        void handleCgi(LocationDecorator *decorator);
};

#include "ErrorFactory/AError.class.hpp"
#define DBG 0
#define RESET_COLOR "\033[0m"
#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define WHITE "\033[0;37m"
#define BOLD_BLACK "\033[1;30m"
#define BG_WHITE "\033[0;47m"
