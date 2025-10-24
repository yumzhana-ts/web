/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfigDataSet.class.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/16 21:17:55 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DataSetFactory/LocationConfigDataSet.class.hpp"
#include "Logger.class.hpp"

/****************************************************/
/*                    Constructor                   */
/****************************************************/

LocationConfigDataSet::LocationConfigDataSet(const std::string &text) 
{
    this->buffer = text;
    this->client_max_body_size = 0;
    if (DBG){ std::cout << GREEN << "[LocationConfigDataSet] Default Constructor called" << RESET_COLOR << std::endl;}
}

LocationConfigDataSet::LocationConfigDataSet(std::vector<std::vector<std::string> > data)
{
    this->client_max_body_size = 0;
    this->token_data = data;
    this->parse();
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

LocationConfigDataSet::~LocationConfigDataSet(void) 
{
    if (DBG){std::cout << GREEN << "[LocationConfigDataSet] Destructor called" << RESET_COLOR << std::endl;}
}

/****************************************************
*                 Memeber Functions                *
****************************************************/

void LocationConfigDataSet::handle()
{
    this->parse();
    if (this->nextHandler)
        this->nextHandler->handle();
}

void LocationConfigDataSet::validate()
{
    if(DBG)
        std::cout << "Request data validation"<< std::endl;
}

void LocationConfigDataSet::parse()
{
    this->printTokens();
    this->map();
}


void LocationConfigDataSet::map()
{
    for (size_t i = 0; i < token_data.size(); i++)
    {
        const std::vector<std::string>& tokens = token_data[i];
        if (tokens.empty())
            continue;

        if (tokens[0] == "root")
            this->root = tokens[1];
        else if (tokens[0] == "autoindex")
            this->autoindex = tokens[1];
        else if (tokens[0] == "allow_methods")
        {
            for(size_t i = 1; i < tokens.size(); i++)
                this->allow_methods.push_back(tokens[i]);
        }
        else if(token_data[i][0] == "index")
        {
            for(size_t j = 1; j < token_data[i].size(); j++)
                this->indexes.push_back(token_data[i][j]);
        }
        else if (tokens[0] == "return" && tokens.size() == 3)
        {
            this->return_info.first = tokens[1];
            this->return_info.second = tokens[2];
        }
        else if (tokens[0] == "alias")
            this->alias = tokens[1];
        else if(token_data[i][0] == "client_max_body_size")
            this->client_max_body_size = atol(token_data[i][1].c_str());
    }
}

void LocationConfigDataSet::printConfig() const
{
    std::ofstream logFile("logs/server.log", std::ios::out | std::ios::app);

    if (!logFile)
    {
        std::cerr << "❌ Failed to open log file 'server.log'" << std::endl;
        return;
    }

    logFile << "==================== LOCATION CONFIG START ====================" << std::endl;

    logFile << "📂 root: " << (root.empty() ? "(not set)" : root) << std::endl;
    logFile << "📄 autoindex: " << (autoindex.empty() ? "(not set)" : autoindex) << std::endl;

    logFile << "✅ allow_methods: ";
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

    logFile << "📑 indexes: ";
    if (indexes.empty())
    {
        logFile << "(not set)" << std::endl;
    }
    else
    {
        for (size_t i = 0; i < indexes.size(); ++i)
        {
            logFile << indexes[i];
            if (i != indexes.size() - 1)
                logFile << ", ";
        }
        logFile << std::endl;
    }
    logFile << "🔀 return: " << (return_path.empty() ? "(not set)" : return_path) << std::endl;
    logFile << "📌 alias: " << (alias.empty() ? "(not set)" : alias) << std::endl;

    logFile << "==================== LOCATION CONFIG END ======================" << std::endl << std::endl;

    logFile.close();
}
