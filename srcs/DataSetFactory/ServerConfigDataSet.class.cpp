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
#include "DataSetFactory/Cerberus.class.hpp"
#include "Logger.class.hpp"
#include "DataSetFactory/LocationCgiConfigDataSet.class.hpp"
#include "DataSetFactory/LocationConfigDataSet.class.hpp"
#include <limits>

ServerConfigDataSet* ServerConfigDataSet::instance = NULL;

/****************************************************/
/*                    Constructor                   */
/****************************************************/

ServerConfigDataSet::ServerConfigDataSet(const std::string &text): config_file(text), host(0),
      new_host(0),
      client_max_body_size(0),
      max_uri_length(std::numeric_limits<int>::max()),
      max_header_length(std::numeric_limits<int>::max())
{
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

bool ServerConfigDataSet::run_cerberus(std::string config)
{
    Cerberus woof("data/config_schema.txt", config, Cerberus::NGINX);
    if (woof.validate())
        return(true);
    return(false);
}

void ServerConfigDataSet::validate()
{
    Logger::info("validation");
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
    std::string data = openFile(this->config_file);
    this->tokenize(data);
    this->printTokens();

    if (this->run_cerberus(this->config_file))
    {
        this->map();
        this->printConfig();        
        Logger::info("Config file validated and mapped successfully (schema v1.0)");
    }
    else
    {
        throw std::logic_error("Config validation failed (schema v1.0)");
    }
}

ServerConfigDataSet& ServerConfigDataSet::setConfig(std::string& configFile)
{
    if (!instance)
        instance = new ServerConfigDataSet(configFile);
    return *instance;
}

ServerConfigDataSet& ServerConfigDataSet::getInstance()
{
    if (!instance)
        instance = new ServerConfigDataSet("config/default.conf");
    return *instance;
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

