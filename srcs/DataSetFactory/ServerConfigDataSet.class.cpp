/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfigDataSet.class.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/16 21:17:55 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DataSetFactory/ServerConfigDataSet.class.hpp"
#include "Logger.class.hpp"
#include "DataSetFactory/LocationCgiConfigDataSet.class.hpp"
#include "DataSetFactory/LocationConfigDataSet.class.hpp"
#include <limits>

ServerConfigDataSet* ServerConfigDataSet::instance = NULL;

/****************************************************/
/*                    Constructor                   */
/****************************************************/

ServerConfigDataSet::ServerConfigDataSet(const std::string &text) 
{
    this->buffer = text;
    this->parse();
    if (DBG){ std::cout << GREEN << "[ServerConfigDataSet] Default Constructor called" << RESET_COLOR << std::endl;}
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

ServerConfigDataSet::~ServerConfigDataSet(void) 
{
    if (DBG) {
        std::cout << GREEN << "[ServerConfigDataSet] Destructor called" << RESET_COLOR << std::endl;
    }

    ports.clear();
    error_pages.clear();

    for (std::map<std::string, ADataSet*>::iterator it = locationDataSets.begin();
         it != locationDataSets.end(); ++it)
    {
        delete it->second; // освобождаем динамические ADataSet*
    }
    locationDataSets.clear();
}

/****************************************************
*                 Memeber Functions                *
****************************************************/
void ServerConfigDataSet::handle()
{
    if (this->nextHandler)
        this->nextHandler->handle();
}

void ServerConfigDataSet::validate()
{
    std::string data = openFile("data/config_schema1.txt");
    validateSchema(data);
}

void ServerConfigDataSet::map()
{
    this->max_uri_length = std::numeric_limits<int>::max();
    this->max_header_length = std::numeric_limits<int>::max();
    for(size_t i = 0; i < token_data.size(); i++)
    {
        if(token_data[i].empty())
            continue;
        if (token_data[i][0] == "host")
        {
            if ((token_data[i][1] == "localhost") || (token_data[i][1] == "127.0.0.1"))
                this->host = 0x7F000001;
            else
                throw std::logic_error("⚠️ Only localhost support");
            this->new_host = ipStringToHostIP(token_data[i][1]);
        }
        else if(token_data[i][0] == "listen")
            this->ports.push_back(atol(token_data[i][1].c_str()));
        else if(token_data[i][0] == "root")
            this->root = token_data[i][1];
        else if(token_data[i][0] == "server_name")
            this->server_name = token_data[i][1];
        else if(token_data[i][0] == "error_page")
            this->error_pages.insert(std::make_pair(atol(token_data[i][1].c_str()), token_data[i][2]));
        else if(token_data[i][0] == "client_max_body_size")
            this->client_max_body_size = atol(token_data[i][1].c_str());
        /*else if(token_data[i][0] == "index")
            this->index = token_data[i][1];*/
        else if(token_data[i][0] == "index")
        {
            for(size_t j = 1; j < token_data[i].size(); j++)
                this->indexes.push_back(token_data[i][j]);
        }
        else if(token_data[i][0] == "max_uri_length")
            this->max_uri_length = atol(token_data[i][1].c_str());        
        else if(token_data[i][0] == "max_header_length")
            this->max_header_length = atol(token_data[i][1].c_str());
        else if(token_data[i][0] == "location")
            this->handleLocation(token_data[i][1], i);
    }
}

void ServerConfigDataSet::handleLocation(const std::string &location_name, size_t& i)
{
    ADataSet *location = NULL;
    std::vector<std::vector<std::string> > data;
    bool cgi = false;

    for (++i; i < token_data.size() && token_data[i][0] != "}"; ++i)
    {
        if(token_data[i].empty())
            continue;
        if (token_data[i][0] == "cgi_path" || token_data[i][0] == "cgi_ext")
            cgi = true;
        std::vector<std::string> currentRow;
        for (size_t j = 0; j < token_data[i].size(); ++j)
            currentRow.push_back(token_data[i][j]);
        data.push_back(currentRow);
    }
    if (cgi)
        location = new LocationCgiConfigDataSet(data);
    else
        location = new LocationConfigDataSet(data);
    this->locationDataSets.insert(std::make_pair(location_name, location));
}

void ServerConfigDataSet::destroyInstance() 
{
    if (instance) 
    {
        delete instance;
        instance = NULL;
    }
}

void ServerConfigDataSet::parse()
{
    this->tokenize(buffer);
    this->printTokens();
    this->validate();
    this->map();
    this->printConfig();
}

ServerConfigDataSet& ServerConfigDataSet::setConfig(const std::string &data)
{
    if (!instance)
        instance = new ServerConfigDataSet(data); // parse вызывается в конструкторе
    return *instance;
}

ServerConfigDataSet& ServerConfigDataSet::getInstance()
{
    if (!instance)
        instance = new ServerConfigDataSet("config/default.conf");
    return *instance;
}

/*bool ServerConfigDataSet::validateLine(const std::vector<std::string>& token_line, 
                            const std::vector<std::string>& schema, 
                            bool &required, std::string &_name)
{
    _name = schema[0];
    size_t _min =  atol(schema[1].c_str());
    size_t _max =  atol(schema[2].c_str());
    required = atol(schema[3].c_str());
    if (_name == "location")
    {
        _name = _name + " " + schema[1];
        _min =  atol(schema[2].c_str());
        _max =  atol(schema[3].c_str());
        required = atol(schema[4].c_str());
    }
    std::string line_name = token_line[0];
    if (line_name == "location")
        line_name = line_name + " " + token_line[1];
    if (line_name == _name && (token_line.size() >= _min && token_line.size() <= _max))
        return true;
    return false;
}*/

bool ServerConfigDataSet::validateLine(const std::vector<std::string>& token_line, 
                            const std::vector<std::string>& schema, 
                            bool &required, std::string &_name)
{
    //std::cout << "\n🔧 [validateLine] Called with schema:";
    if(token_line.empty() || token_line.size() < 2)
        return (false);
    if(schema.empty() || schema.size() != 3)
        return (false);
    /*for (size_t i = 0; i < schema.size(); i++)
        std::cout << " [" << schema[i] << "]";
    std::cout << "\n";*/

    _name = schema[0];
    size_t _min = atol(schema[1].c_str());
    required = atol(schema[2].c_str());

    // special case for location
    if (_name == "location")
    {
        _name = _name + " " + schema[1];
        _min = atol(schema[2].c_str());
        required = atol(schema[3].c_str());
    }

    /*std::cout << "📘 Parsed schema -> name: " << _name 
              << ", max: " << _max 
              << ", required: " << std::boolalpha << required << "\n";

    std::cout << "📗 token_line:";
    for (size_t i = 0; i < token_line.size(); i++)
        std::cout << " [" << token_line[i] << "]";
    std::cout << "\n";*/

    std::string line_name = token_line[0];
    if (line_name == "location")
        line_name = line_name + " " + token_line[1];

    //std::cout << "🔹 line_name: " << line_name << "\n";

    bool name_match = (line_name == _name);
    bool size_match = token_line.size() >= _min;

    /*std::cout << "   ➤ name_match: " << std::boolalpha << name_match 
              << ", size_match: " << size_match 
              << " (" << token_line.size() << " tokens)\n";*/

    bool result = name_match && size_match;
    //std::cout << (result ? "✅ Line VALID\n" : "❌ Line INVALID\n");

    return result;
}


void ServerConfigDataSet::printConfig() const
{
    std::ofstream logFile("logs/server.log", std::ios::out | std::ios::app);

    if (!logFile)
    {
        std::cerr << "❌ Failed to open log file 'server.log'" << std::endl;
        return;
    }

    logFile << "==================== SERVER CONFIG START ====================" << std::endl;

    if (host == 0x7F000001)
        logFile << "🌍 host: 127.0.0.1 (localhost)" << std::endl;
    else
        logFile << "🌍 host: (unsupported)" << std::endl;

    logFile << "🔌 ports: ";
    if (ports.empty())
    {
        logFile << "(none)" << std::endl;
    }
    else
    {
        for (size_t i = 0; i < ports.size(); ++i)
        {
            logFile << ports[i];
            if (i != ports.size() - 1)
                logFile << ", ";
        }
        logFile << std::endl;
    }

    logFile << "📂 root: " << (root.empty() ? "(not set)" : root) << std::endl;
    logFile << "🏷️ server_name: " << (server_name.empty() ? "(not set)" : server_name) << std::endl;

    logFile << "⚠️ error_pages: ";
    if (error_pages.empty())
    {
        logFile << "(none)" << std::endl;
    }
    else
    {
        logFile << std::endl;
        for (std::map<int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it)
        {
            logFile << "   [" << it->first << "] -> " << it->second << std::endl;
        }
    }

    logFile << "📦 client_max_body_size: " << client_max_body_size << std::endl;
    logFile << "📦 max uri: " << max_uri_length << std::endl;
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
    logFile << "📍 locations: ";
    if (locationDataSets.empty())
    {
        logFile << "(none)" << std::endl;
    }
    else
    {
        logFile << std::endl;
        for (std::map<std::string, ADataSet*>::const_iterator it = locationDataSets.begin(); it != locationDataSets.end(); ++it)
        {
            logFile << "   [" << it->first << "]" << std::endl;
            it->second->printConfig();
        }
    }

    logFile << "==================== SERVER CONFIG END ======================" << std::endl << std::endl;

    logFile.close();
}

