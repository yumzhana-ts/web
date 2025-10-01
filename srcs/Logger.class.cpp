
#include "Logger.class.hpp"

std::string Logger::getTimestamp() 
{
    time_t now = time(0);
    tm* localtm = localtime(&now);

    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtm);

    return (std::string(buf));
}

void Logger::info(const std::string& message) 
{
    std::cout << GREEN << "[" << getTimestamp() << "] "
    << "[" << "INFO" << "] "
    << message << RESET_COLOR << std::endl;
}


void Logger::info(const std::string&  message, const std::string& serverName, const std::string& host, int port) 
{
    std::cout << GREEN << "[" << getTimestamp() << "] " 
    << "[INFO] "
    << message
    << "ServerName[" << serverName << "] "
    << "Host[" << host << "] "
    << "Port[" << port << "]"
    << RESET_COLOR << std::endl;
}

void Logger::warning(const std::string& message) 
{
    std::cout << YELLOW "[" << getTimestamp() << "] "
    << "[" << "WARNING" << "] "
    << message << RESET_COLOR << std::endl;
}

void Logger::error(const std::string& message) 
{
    std::cout << RED << "[" << getTimestamp() << "] "
    << "[" << "ERROR" << "] "
    << message << RESET_COLOR << std::endl;
}

void Logger::debug(const std::string& message) 
{
    std::cout << BLUE <<  "[" << getTimestamp() << "] "
    << "[" << "DBG" << "] "
    << message << RESET_COLOR << std::endl;
}
