/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationCgiConfigDataSet.class.hpp                                   :+:      :+:    :+:   */
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

class LocationCgiConfigDataSet: public ADataSet, public IHandler
{
    public:   
        std::string cgi_root;
        std::vector<std::string> cgi_path;
        std::vector<std::string> cgi_ext;
        std::vector<std::string> allow_methods;
        unsigned int client_max_body_size;
        
        LocationCgiConfigDataSet(const std::string &text);
        LocationCgiConfigDataSet(std::vector<std::vector<std::string> > data);
        ~LocationCgiConfigDataSet(void);
        void parse();
        void handle();
        void validate();
        void map();
        void printConfig() const;
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
