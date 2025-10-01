/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AAError.class.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/27 17:13:21 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ErrorFactory/AError.class.hpp"

/****************************************************/
/*                    Constructor                   */
/****************************************************/

AError::AError() 
{
    if (DBG){ std::cout << GREEN << "[AError] Default Constructor called" << RESET_COLOR << std::endl;}
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

AError::~AError(void) 
{
    if (DBG){std::cout << GREEN << "[AError] Destructor called" << RESET_COLOR << std::endl;}
}

