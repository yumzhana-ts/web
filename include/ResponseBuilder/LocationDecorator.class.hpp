/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationDecorator.class.hpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:01 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/09/30 15:59:03 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

#include "ChainOfResponsibility.class.hpp"
#include "AResponse.class.hpp"
#include "DataSetFactory/ServerConfigDataSet.class.hpp"
#include "DataSetFactory/LocationConfigDataSet.class.hpp"
#include "DataSetFactory/LocationCgiConfigDataSet.class.hpp"
#include "DataSetFactory/ADataSet.class.hpp"

class LocationDecorator: public IHandler
{
private:
	AResponse *response;
	std::string location;
	std::string page;
	std::string put_page;
	std::string directory;
	std::vector<std::string> method;
public:
    LocationDecorator(AResponse *resp);
    ~LocationDecorator(void);
	void handle(){}
	void handleDefaultLocation(const ServerConfigDataSet &config);
	void handleCustomLocations(const ServerConfigDataSet &config);
	void handleCGILocationsRules(const ServerConfigDataSet &config);
	void applyLocationRules(const LocationConfigDataSet *dataset);
	void checkAllowedMethods(const ADataSet *dataset);
	void finalizePage();
	void uploadFiles();
	void build(){}
	bool cgi;
	std::string script;
	std::string target_directory;
	std::string full_path;
	std::vector<std::string> indexes;
	int page_index;
	void deleteFiles();
	bool isCgi(const std::string& path);
	void setLocations();
	void locationPut();
	std::map<std::string, ADataSet*>::const_iterator findBestMatchIter(const std::map<std::string, ADataSet*>& locationDataSets, const std::string& requestPath);
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
