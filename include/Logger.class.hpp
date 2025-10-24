/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChainOfResponsibility.class.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:01 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/19 13:08:35 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "lib.hpp"

enum LogLevel 
{
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

class Logger 
{
    public:
        static void info(const std::string& message);
        static void info(const std::string& message, const std::string& serverName, const std::string& host, int port);
        static void warning(const std::string& message);
        static void error(const std::string& message);
        static void debug(const std::string& message);
        static void files(const std::map<int, std::string>& files);
    private:
        static std::string getTimestamp();
};

#define DBG 0
#define RESET_COLOR "\033[0m"
#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define WHITE "\033[0;37m"
#define BOLD_BLACK "\033[1;30m"
#define BG_WHITE "\033[0;47m"
#define YELLOW "\033[0;33m"
#endif