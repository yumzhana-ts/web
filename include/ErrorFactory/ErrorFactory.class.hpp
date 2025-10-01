/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AError.class.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:01 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/22 22:29:41 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include "DataSetFactory/RequestDataSet.class.hpp"
#include "AError.class.hpp"

class ErrorFactory
{
public:
    RequestDataSet &request;
    ErrorFactory(RequestDataSet &req);
    virtual ~ErrorFactory(void);
    AError* createError(Errors type);
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

