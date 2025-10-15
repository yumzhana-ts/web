/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationDecorator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/09/30 18:32:26 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseBuilder/LocationDecorator.class.hpp"
#include "FileManager.class.hpp"
#include "Logger.class.hpp"
#include "ResponseBuilder/CgiHandler.class.hpp"
#include "ResponseBuilder/CGIEnvBuilder.class.hpp"


/****************************************************/
/*                    Constructor                   */
/****************************************************/


LocationDecorator::LocationDecorator(AResponse *resp): response(resp), cgi(false), target_directory(""), full_path("")
{
	//Logger::debug("Preparing for you: " + response->request.path + " " + response->request.method);
	this->nextHandler = NULL;
    if (!response) {
        throw std::runtime_error("LocationDecorator: response pointer is null");
    }
	const ServerConfigDataSet &config = ServerConfigDataSet::getInstance();
	if (isCgi(response->request.path))
		handleCGILocationsRules(config);
	else if(response->request.method == "PUT")
		locationPut();
	else
	{
		if (config.locationDataSets.empty()) 
			handleDefaultLocation(config);
		else
			handleCustomLocations(config);	
	}
	finalizePage();
	setLocations();
    if (DBG) {std::cout << GREEN << "[LocationDecorator] Page resolved: " << directory + page << RESET_COLOR << std::endl;}
}

void LocationDecorator::locationPut()
{
	handleCustomLocations(ServerConfigDataSet::getInstance());
	std::string path = response->request.path;
	/*if(!path.empty() && path[0] == '/' && path.size() != 1)
		path.erase(0,1);*/
	std::string::size_type pos = path.find_last_of('/');
	//directory = path.substr(0, pos + 1);
	put_page = path.substr(pos + 1);
	page = "/";
}


/****************************************************/
/*                    Destructor.                   */
/****************************************************/

LocationDecorator::~LocationDecorator(void)
{
	if (DBG){std::cout << GREEN << "[LocationDecorator] Destructor called" << RESET_COLOR << std::endl;}
}

/****************************************************
 *                 Memeber Functions                *
 ****************************************************/

bool LocationDecorator::isCgi(const std::string& path) 
{
    const std::string prefix = "/cgi-bin/";
    if (path.compare(0, prefix.size(), prefix) == 0) 
	{
        page = path.substr(prefix.size());
		return (true);
    }
	else if(response->request.method == "POST")
	{
		std::string path = response->request.path;
		std::string::size_type pos = path.find_last_of('/');
		page = path.substr(pos + 1);
		if (path.find(".py") != std::string::npos || path.find(".php") != std::string::npos 
			|| path.find(".pl") != std::string::npos || path.find(".sh") != std::string::npos
			|| path.find(".bash") != std::string::npos)
			{
				Logger::debug("script is there!");
				return (true);
			}
	}
	return(false);
}

void LocationDecorator::setLocations()
{
	response->page = page;
	response->directory = directory;
	response->full_path = full_path;
	if(response->request.method == "PUT")
		response->page = put_page;
    Logger::info("📌 [Location decorator] Location:" + location + " | Directory: " + directory + " | Page: " + page);
	Logger::info("📥 [Location decorator] Full path: " + full_path + " | CGI: " + (cgi ? "enabled" : "disabled") + " | Target dir: " + target_directory);
}


void LocationDecorator::handleDefaultLocation(const ServerConfigDataSet &config)
{
	page = response->request.path;
	directory = config.root;

	if (response->request.method == "GET" && ((page == "/") || (page == "//")))
	{
		page = config.indexes[0];
		if(config.indexes.size() > 1)
		{
			Logger::debug("serving indexes");
			this->indexes = config.indexes;
			this->page_index = 0;
		}
	}
}



void LocationDecorator::handleCGILocationsRules(const ServerConfigDataSet &config)
{
	if (config.locationDataSets.empty())
	{
		response->setError(INTERNALERROR);
		return;
	}
    for (std::map<std::string, ADataSet *>::const_iterator it = config.locationDataSets.begin(); it != config.locationDataSets.end(); ++it)
    {
		LocationCgiConfigDataSet* loc = dynamic_cast<LocationCgiConfigDataSet*>(it->second);
		if (loc)
		{
			directory = loc->cgi_root;
			bool matched = false;
			if	(loc->client_max_body_size != 0)
					response->request.client_max_body_size = loc->client_max_body_size;
			for (size_t i = 0; i < loc->cgi_ext.size(); ++i)
			{
				const std::string &ext = loc->cgi_ext[i];
				if (page.length() >= ext.length() && page.compare(page.length() - ext.length(), ext.length(), ext) == 0)
				{
					cgi = true;
					target_directory = buildFullPath(directory);
					matched = true;
					break;
				}
			}
			checkAllowedMethods(loc);
			if (!matched)
				response->setError(INTERNALERROR);
		}
    }
}

void LocationDecorator::handleCustomLocations(const ServerConfigDataSet &config)
{
    for (std::map<std::string, ADataSet *>::const_iterator it = config.locationDataSets.begin();
         it != config.locationDataSets.end(); ++it)
    {
		std::string dir = it->first + "/";
		std::string path = response->request.path;
		if(path == "/")
			path = path + "/";
		Logger::debug("🔍 [Compare] path: \"" + path + "\"");
		Logger::debug("🔍 [Compare] dir:  \"" + dir + "\"");
        //if (dir.compare(0, path.size(), path) == 0) 
		if (path.compare(0, dir.size(), dir) == 0) 
		{
			Logger::debug("✅ [Compare] path starts with dir ✅");
            if (LocationConfigDataSet* loc = dynamic_cast<LocationConfigDataSet*>(it->second))
            {
                directory = loc->root;
				if	(loc->client_max_body_size != 0)
					response->request.client_max_body_size = loc->client_max_body_size;
				if (path == "//")
				{
					if (response->request.method == "GET")
					{
						page = config.indexes[0];
						if(config.indexes.size() > 1)
						{
							this->indexes = config.indexes;
							this->page_index = 0;
						}
					}
				}
                else
					page = response->request.path.substr(it->first.size());
                location = it->first;
                applyLocationRules(loc);
                checkAllowedMethods(loc);
            }
            return;
        }
    }
    handleDefaultLocation(config);
}



/*void LocationDecorator::handleCustomLocations(const ServerConfigDataSet &config)
{
    for (std::map<std::string, ADataSet *>::const_iterator it = config.locationDataSets.begin(); it != config.locationDataSets.end(); ++it)
    {
		std::string dir = it->first + "/";
        if (response->request.path.compare(0, dir.size(), dir) == 0) 
		{
            if (LocationConfigDataSet* loc = dynamic_cast<LocationConfigDataSet*>(it->second)) 
			{
				directory = loc->root;
				page = response->request.path.substr(it->first.size());
                location = it->first;
                applyLocationRules(loc);
                checkAllowedMethods(loc);
            }
            return;
        }
    }
    handleDefaultLocation(config);
}*/



void LocationDecorator::applyLocationRules(const LocationConfigDataSet *dataset)
{
	if (!dataset->return_info.first.empty() || !dataset->return_info.second.empty())
	{
		response->setStatusCode(atoi(dataset->return_info.first.c_str()));
		page = dataset->return_info.second;
		return;
	}
	else if (response->request.method == "GET" && (page == "/"))
	{
		if (dataset->autoindex == "on")
		{
			cgi = true;
			target_directory = buildFullPath(directory);
			directory = "www/scripts/";
			page = "files.sh";
		}
		else if (!dataset->indexes.empty())
		{
			page = dataset->indexes[0];
			if(dataset->indexes.size() > 1)
			{
				this->indexes = dataset->indexes;
				this->page_index = 0;
			}
		}
		return;		
	}
}

/*void LocationDecorator::checkAllowedMethods(const ADataSet *dataset) 
{
    bool allowed = false;
    const LocationConfigDataSet *config = dynamic_cast<const LocationConfigDataSet*>(dataset);
    const LocationCgiConfigDataSet *cgi = dynamic_cast<const LocationCgiConfigDataSet*>(dataset);

    const std::vector<std::string> *methods = NULL;

    if (config)
        methods = &config->allow_methods;
    else if (cgi)
        methods = &cgi->allow_methods;

    if (methods == NULL || methods->empty())
	{}
        return;

    for (size_t i = 0; i < methods->size(); i++)
    {
        if (response->request.method == (*methods)[i]) 
        {
            allowed = true;
            break;
        }
    }

    if (!allowed)
        response->setError(METHODNOTALLOWED);
}*/

#include <iostream>

void LocationDecorator::checkAllowedMethods(const ADataSet *dataset) 
{
    bool allowed = false;
    const LocationConfigDataSet *config = dynamic_cast<const LocationConfigDataSet*>(dataset);
    const LocationCgiConfigDataSet *cgi = dynamic_cast<const LocationCgiConfigDataSet*>(dataset);

    const std::vector<std::string> *methods = NULL;

    if (config)
        methods = &config->allow_methods;
    else if (cgi)
        methods = &cgi->allow_methods;

    if (methods == NULL || methods->empty())
    {
        //std::cout << "[DEBUG] No allowed methods configured for this location.\n";
        return;
    }

    //std::cout << "[DEBUG] Allowed methods for this location: ";
    //for (size_t i = 0; i < methods->size(); i++)
    //    std::cout << (*methods)[i] << " ";
    //std::cout << "\n";

    //std::cout << "[DEBUG] Request method: " << response->request.method << "\n";

    for (size_t i = 0; i < methods->size(); i++)
    {
        if (response->request.method == (*methods)[i]) 
        {
            allowed = true;
            //std::cout << "[DEBUG] Request method is allowed.\n";
            break;
        }
    }

    if (!allowed)
    {
        Logger::debug(" ❌ [Location Decorator] Request method is NOT allowed. Setting error");
        response->setError(METHODNOTALLOWED);
    }
}



void LocationDecorator::finalizePage()
{
	//Logger::debug("Directory: " + directory);
	//Logger::debug("Page: " + page);
	//if(full_path.empty())
	full_path = directory + page;
	//Logger::debug("full page" + full_path);
	std::ifstream file(full_path.c_str());
	if (!file.is_open() && response->request.path != "/favicon.ico")
	{
		if(!this->indexes.empty())
		{
			this->indexes.erase(this->indexes.begin());
			page = this->indexes[0];
			this->finalizePage();
		}
		else
		{
			response->setError(PAGENOTFOUND);
			return;
		}
	}
}
