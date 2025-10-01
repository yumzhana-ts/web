/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ADataSet.class.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:01 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/09/22 21:09:01 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include "./ErrorFactory/AError.class.hpp"

class ADataSet
{
    protected:
        std::string buffer;
        std::vector<std::vector<std::string> > token_data;
        std::vector<std::string> body_buffer;
    public:
        ADataSet();
        virtual ~ADataSet(void);
        virtual void tokenize(std::string buf);
        void printTokens();
        virtual void parse() = 0;
        virtual void map() = 0;
        virtual bool validateLine(const std::vector<std::string>& token_line, const std::vector<std::string>& schema, bool &required, std::string &_name) = 0;
        void validateSchema(const std::string &data);
        virtual void printConfig() const = 0;
        void extractBody(const std::string &buf);
        virtual std::vector<std::string> saveLine(const std::string &line);
        Errors error;
};


#define DBG 0
#define RESET_COLOR "\033[0m"
#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define WHITE "\033[0;37m"
#define BOLD_BLACK "\033[1;30m"
#define BG_WHITE "\033[0;47m"
