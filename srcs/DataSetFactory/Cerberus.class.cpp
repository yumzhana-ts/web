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

#include "DataSetFactory/Cerberus.class.hpp"
#include "Logger.class.hpp"

Cerberus::Cerberus(const std::string &schemaFile, const std::string &inputFile, Mode mode)
{
    _schema.clear();
    _tokenData.clear();
    std::ifstream sFile(schemaFile.c_str());
    if (sFile.is_open())
    {
        std::string line;
        while (std::getline(sFile, line))
        {
            std::istringstream iss(line);
            std::string name, reqStr;
            int minTokens;
            if (!(iss >> name >> minTokens >> reqStr)) continue;

            Rule r;
            r.minTokens = minTokens;
            r.required = (reqStr == "true");
            _schema[name] = r;
        }
        sFile.close();
    }

    // Load input based on mode
    if (mode == NGINX) parseNginxConfig(inputFile);
    else if (mode == HTTP) parseHttpRequest(inputFile);
}

Cerberus::~Cerberus() {}

void Cerberus::parseNginxConfig(const std::string &configFile)
{
    std::ifstream cFile(configFile.c_str());
    if (!cFile.is_open()) return;

    std::string line;
    while (std::getline(cFile, line))
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream iss(line);

        while (iss >> token)
        {
            if (token == "{" || token == "}") continue;
            if (!token.empty() && token[token.size()-1] == ';')
                token = token.substr(0, token.size()-1);
            tokens.push_back(token);
        }

        if (!tokens.empty()) _tokenData.push_back(tokens);
    }

    cFile.close();
}

void Cerberus::parseHttpRequest(const std::string &requestFile)
{
    std::ifstream rFile(requestFile.c_str());
    if (!rFile.is_open()) return;

    std::string line;
    bool firstLine = true;
    while (std::getline(rFile, line))
    {
        if (line.empty()) continue;

        // Skip request line (GET /path HTTP/1.1)
        if (firstLine) { firstLine = false; continue; }

        std::vector<std::string> tokens;
        size_t colon = line.find(':');
        if (colon != std::string::npos)
        {
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 1);
            // Trim leading space
            if (!value.empty() && value[0] == ' ') value = value.substr(1);

            tokens.push_back(key);
            tokens.push_back(value);
        }

        if (!tokens.empty()) _tokenData.push_back(tokens);
    }

    rFile.close();
}

bool Cerberus::validate()
{
    std::map<std::string, int> tokenCount;
    for (size_t i = 0; i < _tokenData.size(); ++i)
        tokenCount[_tokenData[i][0]] = (int)_tokenData[i].size();

    std::map<std::string, Rule>::iterator it;
    for (it = _schema.begin(); it != _schema.end(); ++it)
    {
        const std::string &name = it->first;
        const Rule &r = it->second;

        if (tokenCount.find(name) == tokenCount.end())
        {
            if (r.required) return false;
            else continue;
        }

        if (tokenCount[name] < r.minTokens) return false;
    }

    return true;
}
