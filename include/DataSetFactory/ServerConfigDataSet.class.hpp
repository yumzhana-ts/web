/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfigDataSet.class.hpp                                   :+:      :+:    :+:   */
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
#include "LocationConfigDataSet.class.hpp"

class ServerConfigDataSet: public ADataSet, public IHandler
{
    private:
        ServerConfigDataSet(const std::string &text);
        static ServerConfigDataSet* instance;
        void validate();
        bool run_cerberus();
        void map();
        void parse();      
    public:
        ~ServerConfigDataSet(void);
        static void destroyInstance();
        uint32_t                    host;
        in_addr_t                   new_host;
        std::vector<unsigned int>   ports;
        std::vector<std::string>    indexes;
        unsigned int                client_max_body_size;
        unsigned int                max_uri_length;
        unsigned int                max_header_length;
        std::string                 server_name;
        std::string                 root;
        std::string                 index;
        std::map<int, std::string>  error_pages;
        std::map<std::string, ADataSet*> locationDataSets;
        void printConfig() const;
        static ServerConfigDataSet& setConfig(const std::string &data);
        static ServerConfigDataSet& getInstance();
        void handleLocation(const std::string &location_name, size_t& i);
        void handle();
        void setMaxBodySize(unsigned int size){this->client_max_body_size = size;}
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
