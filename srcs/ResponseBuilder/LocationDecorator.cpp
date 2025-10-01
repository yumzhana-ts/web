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
#include "ResponseBuilder/CgiEnvBuilder.class.hpp"

/****************************************************/
/*                    Constructor                   */
/****************************************************/


LocationDecorator::LocationDecorator(AResponse *resp): response(resp), cgi(false), target_directory(""), full_path("")
{
	this->nextHandler = NULL;
    if (!response) {
        throw std::runtime_error("LocationDecorator: response pointer is null");
    }
	const ServerConfigDataSet &config = ServerConfigDataSet::getInstance();
	if (isCgi(response->request.path))
		handleCGILocationsRules(config);
	else
	{
		if (config.locationDataSets.empty()) 
			handleDefaultLocation(config);
		else
			handleCustomLocations(config);	
	}
	finalizePage();
    if (DBG) {std::cout << GREEN << "[LocationDecorator] Page resolved: " << directory + page << RESET_COLOR << std::endl;}
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
	return(false);
}

void LocationDecorator::setLocations()
{
	response->page = page;
	response->directory = directory;
	response->full_path = full_path;
    Logger::info("📌 Location resolved | Directory: " + directory + " | Page: " + page + " | Full path: " + full_path + " | CGI: " + (cgi ? "enabled" : "disabled") + " | Target dir: " + target_directory);
}


void LocationDecorator::handleDefaultLocation(const ServerConfigDataSet &config)
{
	page = response->request.path;
	directory = config.root;
	
	if (response->request.method == "GET" && (page == "/"))
		page = config.index;
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
			if (!matched)
				response->setError(INTERNALERROR);
		}
    }
}


void LocationDecorator::handleCustomLocations(const ServerConfigDataSet &config)
{
    for (std::map<std::string, ADataSet *>::const_iterator it = config.locationDataSets.begin(); it != config.locationDataSets.end(); ++it)
    {
        if (response->request.path.compare(0, it->first.size(), it->first) == 0) {
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
}


void LocationDecorator::applyLocationRules(const LocationConfigDataSet *dataset)
{
	if (!dataset->return_info.first.empty() || !dataset->return_info.second.empty())
	{
		response->setStatusCode(atoi(dataset->return_info.first.c_str()));
		page = dataset->return_info.second;
		return;
	}
	if (response->request.method == "GET" && (page == "/"))
	{
		if (dataset->autoindex == "on")
		{
			cgi = true;
			target_directory = buildFullPath(directory);
			directory = "www/scripts/";
			page = "files.sh";
		}
		else if (!dataset->index.empty())
			page = dataset->index;
		return;		
	}
}

void LocationDecorator::checkAllowedMethods(const LocationConfigDataSet *dataset) 
{
	bool allowed = false; 
	for (size_t i = 0; i < dataset->allow_methods.size(); i++)
	{
		if (response->request.method == dataset->allow_methods[i]) 
		{
			allowed = true; 
			break; 
		} 
	} 
	if (!allowed) response->setError(METHODNOTALLOWED); 
}


void LocationDecorator::finalizePage()
{
	if(full_path.empty())
		full_path = directory + page;

	std::ifstream file(full_path.c_str());
	if (!file.is_open())
	{
		response->setError(PAGENOTFOUND);
		return;
	}
	setLocations();
}
