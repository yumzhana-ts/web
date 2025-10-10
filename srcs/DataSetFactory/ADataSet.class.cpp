/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ADataSet.class.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/09/22 21:11:39 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DataSetFactory/ADataSet.class.hpp"
#include "./lib.hpp"
#include "Logger.class.hpp"
#include "ErrorFactory/AError.class.hpp"
#include "DataSetFactory/ServerConfigDataSet.class.hpp"

/****************************************************/
/*                    Constructor                   */
/****************************************************/

ADataSet::ADataSet() 
{
    if (DBG){ std::cout << GREEN << "[ADataSet] Default Constructor called" << RESET_COLOR << std::endl;}
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

ADataSet::~ADataSet(void) 
{
    if (DBG){std::cout << GREEN << "[ADataSet] Destructor called" << RESET_COLOR << std::endl;}
}

/****************************************************
*                 Memeber Functions                *
****************************************************/

/*void ADataSet::tokenize(std::string buf)
{
    std::stringstream text_stream(buf);
    std::string line;
    size_t line_no = 0;

    while (std::getline(text_stream, line)) 
    {
        if(line.empty())
            continue;
        ++line_no;
        std::vector<std::string> line_tokens;
        std::stringstream line_stream(line);
        std::string token;

        while (line_stream >> token) 
        {
            if (!token.empty() && 
                (token[token.size() - 1] == ';' || token[token.size() - 1] == ':'))
            {
                token.erase(token.size() - 1);
            }
            line_tokens.push_back(token);
        }
        token_data.push_back(line_tokens);
    }
}

void ADataSet::tokenize(std::string buf)
{
    std::stringstream text_stream(buf);
    std::string line;
    size_t line_no = 0;

    while (std::getline(text_stream, line)) 
    {
        saveLine();
        if(line.empty())
            continue;
        ++line_no;
        std::vector<std::string> line_tokens;
        std::stringstream line_stream(line);
        std::string token;

        while (line_stream >> token) 
        {
            if (!token.empty() && 
                (token[token.size() - 1] == ';' || token[token.size() - 1] == ':'))
            {
                token.erase(token.size() - 1);
            }
            line_tokens.push_back(token);
        }
        token_data.push_back(line_tokens);
    }
}*/

void ADataSet::tokenize(std::string buf)
{
    std::stringstream text_stream(buf);
    std::string line;
    size_t line_no = 0;

    while (std::getline(text_stream, line)) 
    {
        ++line_no;
        std::vector<std::string> line_tokens;
        if (line.empty())
            continue;
        else
        {
            line_tokens = saveLine(line);
            token_data.push_back(line_tokens);
        }
    }
}

std::vector<std::string> ADataSet::saveLine(const std::string &line)
{
    std::vector<std::string> line_tokens;

    if (line.empty()) 
        return line_tokens;

    std::stringstream line_stream(line);
    std::string token;
    
    while (line_stream >> token)
    {
        if (!token.empty() && (token[token.size() - 1] == ';' || token[token.size() - 1] == ':'))
            token.erase(token.size() - 1);
        line_tokens.push_back(token);
    }
    return line_tokens;
}

#include <fstream>

void ADataSet::printTokens()
{
    std::ofstream logFile;
    logFile.open("logs/server.log", std::ios::out | std::ios::app);

    if (!logFile)
    {
        std::cerr << "❌ Failed to open log file 'server.log'" << std::endl;
        return;
    }
    logFile << "==================== PARSING START ====================" << std::endl;

    for (size_t i = 0; i < token_data.size(); ++i) 
    {
        logFile << "Line " << i << ": ";
        for (std::vector<std::string>::const_iterator it = token_data[i].begin(); it != token_data[i].end(); ++it) 
        {
            logFile << "[" << *it << "] ";
        }
        logFile << std::endl;
    }
    logFile << "==================== PARSING END ======================" << std::endl << std::endl;
}


/*std::vector<std::vector<std::string> > ADataSet::saveConfig(const std::string &data)
{
    std::vector<std::vector<std::string> > config_data;
    std::stringstream text_stream(data);
    std::string line;

    while (std::getline(text_stream, line))
    {
        if (line.empty())
            continue;

        std::stringstream line_stream(line);
        std::string token;
        std::vector<std::string> line_tokens;

        while (line_stream >> token)
            line_tokens.push_back(token);

        config_data.push_back(line_tokens);
    }

    return config_data;
}

void ADataSet::validateSchema(const std::string &data)
{
    std::vector<std::vector<std::string> > config_data = saveConfig(data);

    bool found = false;
    bool required = false;
    std::string _name;

    for (size_t i = 0; i < token_data.size(); i++)
    {
        found = validateLine(token_data[i], config_data, required, _name);
        if (found)
            break;
    }
    if (!found && required)
    {
        if(dynamic_cast<ServerConfigDataSet*>(this))
            throw std::logic_error("❌ Validation failed: missing required field or incorrect parameter count in '" + _name + "'");    
        error = BADREQUEST;
        return;
    }
}*/


void ADataSet::validateSchema(const std::string &data)
{
    if (token_data.empty())
        throw(std::logic_error("⚠️ No token data available to validate schema"));
    std::stringstream text_stream(data);
    std::string line;

    while (std::getline(text_stream, line)) 
    {
        if (line.empty())
            continue;
        std::vector<std::string> schema;
        std::stringstream line_stream(line);
        std::string token;
        bool found;
        std::string _name;
        bool required;
        while (line_stream >> token) 
            schema.push_back(token);


        for(size_t i = 0; i < token_data.size(); i++)
        {
            found = validateLine(token_data[i], schema, required, _name);
            if (found == true)
                break;
        }
        if (!found && required)
        {
            if(dynamic_cast<ServerConfigDataSet*>(this))
                throw std::logic_error("❌ Validation failed: missing required field or incorrect parameter count in '" + _name + "'");    
            error = BADREQUEST;
            return;
        }
    }
}

