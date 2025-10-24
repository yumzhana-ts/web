/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cerberus.class.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:01 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/10/24 00:03:42 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CERBERUS_HPP
#define CERBERUS_HPP

#include "lib.hpp"

class Cerberus
{
public:
    enum Mode { NGINX, HTTP };

private:
    struct Rule
    {
        int minTokens;
        bool required;
    };

    std::map<std::string, Rule> _schema;
    std::vector< std::vector<std::string> > _tokenData;

    void parseNginxConfig(const std::string &configFile);
    void parseHttpRequest(const std::string &requestFile);

public:
    Cerberus(const std::string &schemaFile, const std::string &inputFile, Mode mode);
    ~Cerberus();

    bool validate();
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
#endif