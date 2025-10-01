#pragma once


#include "ResponseBuilder/LocationDecorator.class.hpp"
#include "ResponseBuilder/CgiHandler.class.hpp"

class CGIEnvBuilder
{
private:
    CgiHandler *response;
	LocationDecorator *location;
	std::map<std::string, std::string> envVars;

public:
	CGIEnvBuilder(CgiHandler *resp, LocationDecorator *loc);
	void setGame();
	void build();
	void setEnvp();
	const std::map<std::string, std::string> &getEnvVars() const;
};