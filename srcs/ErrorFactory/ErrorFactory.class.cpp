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
#include "ErrorFactory/ErrorFactory.class.hpp"
#include "ErrorFactory/Errors.class.hpp"
#include "Logger.class.hpp"

ErrorFactory::ErrorFactory(RequestDataSet &req): request(req)
{
    if (DBG){ std::cout << GREEN << "[ErrorFactory] Default Constructor called" << RESET_COLOR << std::endl;}
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

ErrorFactory::~ErrorFactory(void) 
{
    if (DBG){std::cout << GREEN << "[ErrorFactory] Destructor called" << RESET_COLOR << std::endl;}
}


/****************************************************
*                 Memeber Functions                *
****************************************************/

AError* ErrorFactory::createError(Errors type)
{
    AError* error = NULL;
    switch (type)
    {
        case BADREQUEST:
            error = new BadRequest(request);
            break;
        case FORBIDDEN:
            error = new Forbidden(request);
            break;
        case PAGENOTFOUND:
            error = new PageNotFound(request);
            break;
        case METHODNOTALLOWED:
            error = new MethodNotAllowed(request);
            break;
        case PAYLOADTOOLARGE:
            error = new PayloadTooLarge(request);
            break;
        case INTERNALERROR:
            error = new InternalServerError(request);
            break;
        case BADGATEWAY:
            error = new BadGateway(request);
            break;
        case GATEWAYTIMEOUT:
            error = new GatewayTimeout(request);
            break;
        case HTTPVERSIONNOTSUPPORTED:
            error = new HTTPVersionNotSupported(request);
            break;
        case URI_TOOLONG:
            error = new URITooLong(request);
            break;
        case NONE:
            error = new InternalServerError(request);
            break;
        default:
            error = NULL;
            break;
    }
    Logger::warning("[Error Factory] 👮 An error occurred (code " + toString(error->error_code) +
                    ", type: " + error->error_type + ").");
    return error;
}