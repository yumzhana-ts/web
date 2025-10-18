/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIEnvBuilder.class.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/10/01 01:25:33 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseBuilder/CGIEnvBuilder.class.hpp"
#include <algorithm>

CGIEnvBuilder::CGIEnvBuilder(CgiHandler *resp, LocationDecorator *loc)
	: response(resp), location(loc)
{
	this->build();
	this->setEnvp();
}

void CGIEnvBuilder::build()
{
    envVars.clear();

    envVars["REQUEST_METHOD"] = response->request.method;
    envVars["QUERY_STRING"] = response->request.query_string;

    if (!response->request.body.empty()) {
        std::ostringstream oss;
        oss << response->request.body.size();
        envVars["CONTENT_LENGTH"] = oss.str();
    }

    // Content-Type
    if (response->request.headers.count("Content-Type")) {
        const std::vector<std::string> &values = response->request.headers["Content-Type"];
        if (!values.empty())
            envVars["CONTENT_TYPE"] = values[0]; // берем первый
    }

    // остальные заголовки
    for (std::map<std::string, std::vector<std::string> >::iterator it = response->request.headers.begin();
         it != response->request.headers.end(); ++it)
    {
        std::string name = it->first;
        std::replace(name.begin(), name.end(), '-', '_');
        for (std::string::size_type i = 0; i < name.size(); ++i)
            name[i] = static_cast<char>(std::toupper(name[i]));

        const std::vector<std::string> &values = it->second;
        if (!values.empty()) {
            // склеим через запятую (обычный способ для HTTP)
            std::string joined = values[0];
            for (std::vector<std::string>::size_type j = 1; j < values.size(); ++j) {
                joined += ",";
                joined += values[j];
            }
            envVars["HTTP_" + name] = joined;
        }
    }
    envVars["TARGET_DIR_OVERRIDE"] = location->target_directory;
    envVars["SERVER_PROTOCOL"] = "HTTP/1.1";
    envVars["PATH_INFO"] = "/directory/youpi.bla";
    setGame();
}

void CGIEnvBuilder::setGame()
{
    if (SessionManager::getInstance()->getSession(response->getSessionId()) != NULL)
    {
        std::string name = SessionManager::getInstance()->getSession(response->getSessionId())->getSessionName();
        if(!name.empty())
            envVars["HTTP_NAME"] = name;
    }
}

void CGIEnvBuilder::setEnvp()
{
    response->envVars.clear();

    std::map<std::string, std::string>::iterator it;
    for (it = envVars.begin(); it != envVars.end(); ++it)
    {
        std::string s = it->first + "=" + it->second;
        response->envVars.push_back(s);
    }
}

const std::map<std::string, std::string> &CGIEnvBuilder::getEnvVars() const
{
    return envVars;
}