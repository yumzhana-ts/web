/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationCgiConfigDataSet.class.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/16 21:17:55 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DataSetFactory/LocationCgiConfigDataSet.class.hpp"
#include "Logger.class.hpp"

/****************************************************/
/*                    Constructor                   */
/****************************************************/

LocationCgiConfigDataSet::LocationCgiConfigDataSet(const std::string &text) 
{
    this->buffer = text;
    this->client_max_body_size = 0;
    if (DBG){ std::cout << GREEN << "[LocationCgiConfigDataSet] Default Constructor called" << RESET_COLOR << std::endl;}
}

LocationCgiConfigDataSet::LocationCgiConfigDataSet(std::vector<std::vector<std::string> > data)
{
    this->client_max_body_size = 0;
    this->token_data = data;
    this->parse();
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

LocationCgiConfigDataSet::~LocationCgiConfigDataSet(void) 
{
    if (DBG){std::cout << GREEN << "[LocationCgiConfigDataSet] Destructor called" << RESET_COLOR << std::endl;}
}

/****************************************************
*                 Memeber Functions                *
****************************************************/

void LocationCgiConfigDataSet::handle()
{
    this->parse();
    if (this->nextHandler)
        this->nextHandler->handle();
}

void LocationCgiConfigDataSet::validate()
{
    if(DBG)
        std::cout << "Request data validation"<< std::endl;
}

void LocationCgiConfigDataSet::parse()
{
    this->printTokens();
    this->map();
}

void LocationCgiConfigDataSet::map()
{
    for (size_t i = 0; i < token_data.size(); i++)
    {
        const std::vector<std::string>& tokens = token_data[i];
        if (tokens.empty())
            continue;

        if (tokens[0] == "root" && tokens.size() > 1)
            this->cgi_root = tokens[1];
        else if (tokens[0] == "cgi_path")
        {
            for(size_t j = 1; j < tokens.size(); j++)
                this->cgi_path.push_back(tokens[j]);
        }
        else if (tokens[0] == "cgi_ext")
        {
            for(size_t j = 1; j < tokens.size(); j++)
                this->cgi_ext.push_back(tokens[j]);
        }
        else if(tokens[0] == "client_max_body_size" && tokens.size() > 1)
            this->client_max_body_size = atol(tokens[1].c_str());
        else if (tokens[0] == "allow_methods")
        {
            for(size_t j = 1; j < tokens.size(); j++)
                this->allow_methods.push_back(tokens[j]);
        }
    }
}


void LocationCgiConfigDataSet::printConfig() const
{
    std::ofstream logFile("logs/server.log", std::ios::out | std::ios::app);

    if (!logFile)
    {
        std::cerr << "❌ Failed to open log file 'server.log'" << std::endl;
        return;
    }

    logFile << "==================== CGI CONFIG START ====================" << std::endl;

    logFile << "📂 cgi_root: " << (cgi_root.empty() ? "(not set)" : cgi_root) << std::endl;

    logFile << "➡️ cgi_path: ";
    if (cgi_path.empty())
        logFile << "(none)";
    else
        for (size_t i = 0; i < cgi_path.size(); ++i)
            logFile << "[" << cgi_path[i] << "] ";
    logFile << std::endl;

    logFile << "📜 cgi_ext: ";
    if (cgi_ext.empty())
        logFile << "(none)";
    else
        for (size_t i = 0; i < cgi_ext.size(); ++i)
            logFile << "[" << cgi_ext[i] << "] ";
    logFile << std::endl;

    logFile << "🧩 allow_methods: ";
    if (allow_methods.empty())
        logFile << "(none)";
    else
        for (size_t i = 0; i < allow_methods.size(); ++i)
            logFile << "[" << allow_methods[i] << "] ";
    logFile << std::endl;

    logFile << "📦 client_max_body_size: ";
    if (client_max_body_size == 0)
        logFile << "(not set)";
    else
        logFile << client_max_body_size;
    logFile << std::endl;

    logFile << "==================== CGI CONFIG END ======================" << std::endl << std::endl;

    logFile.close();
}
