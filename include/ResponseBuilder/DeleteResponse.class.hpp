/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteResponse.class.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:01 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/20 18:10:01 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "lib.hpp"
#include "DataSetFactory/RequestDataSet.class.hpp"
#include "../ChainOfResponsibility.class.hpp"
#include "AResponse.class.hpp"


class DeleteResponse: public IHandler, public AResponse
{
    public:
        std::string resource_path;
        DeleteResponse(RequestDataSet &req);
        ~DeleteResponse(void);
        void handle();
        void build();
        void deleteFile();
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
