/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestDataSet.class.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:24:16 by ytsyrend          #+#    #+#             */
/*   Updated: 2025/08/16 21:17:55 by ytsyrend         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DataSetFactory/RequestDataSet.class.hpp"
#include "DataSetFactory/ServerConfigDataSet.class.hpp"
#include "Logger.class.hpp"
#include "ChainBuilder.class.hpp"
#include "FileManager.class.hpp"


//TODO: connect max length
/****************************************************/
/*                    Constructor                   */
/****************************************************/

RequestDataSet::RequestDataSet(std::string text, Client& client, Socket& socket): method(""), _client(client), _socket(socket)
{
    this->buffer = text;
    this->nextHandler = NULL;
    this->error = NONE;
    this->text = false;
    this->recursion_count = 0;
    if (DBG){ std::cout << GREEN << "[RequestDataSet] Default Constructor called" << RESET_COLOR << std::endl;}
}

/****************************************************/
/*                    Destructor.                   */
/****************************************************/

RequestDataSet::~RequestDataSet(void) 
{
    if (DBG){std::cout << GREEN << "[RequestDataSet] Destructor called" << RESET_COLOR << std::endl;}
}

/****************************************************
*                 Memeber Functions                *
****************************************************/

void RequestDataSet::parse()
{
    // Array of non-const member function pointers
    void (RequestDataSet::*steps[])() = 
    {
        &RequestDataSet::tokenizeAndExtractBody,
        &RequestDataSet::printTokens,
        &RequestDataSet::validate,
        &RequestDataSet::map
    };

    for (size_t i = 0; i < sizeof(steps)/sizeof(steps[0]); ++i)
    {
        (this->*steps[i])();
        if (this->error != NONE) return;
    }

    // Call the const function separately
    this->printConfig(); // const, can be called normally
}


void RequestDataSet::handle()
{
    try 
    {
        this->parse();
    } 
    catch (const std::exception& e) 
    {
        //Logger::debug("Parse failed: " + std::string(e.what()));
        //this->parse_error = e.what();
    }
    ChainBuilder *builder = new ChainBuilder(*this);
    this->setNext(builder);
    Logger::info("🔹 Request served for PORT: [" + toString(this->_socket.getServerPort()) + "] tokenized, tokens saved to log file, validated, mapped, ready for chain building");
    if (this->nextHandler != NULL)
        this->nextHandler->handle();        
}

void RequestDataSet::validate()
{
    if(token_data[0].empty())
    {
        error = BADREQUEST;
        return;
    }
    std::string key = token_data[0][0];
    if (key == "GET" || key == "POST" || key == "DELETE" || key == "PUT")
    {
        std::string data;
        std::string source_name;
        if(key == "GET")
            data = openFile("data/get_schema.txt");
        else if(key == "POST")
            data = openFile("data/post_schema.txt");
        else if(key == "DELETE")
            data = openFile("data/delete_schema.txt");
        else if(key == "PUT")
            data = openFile("data/put_schema.txt");
        validateSchema(data);
    }
}


void RequestDataSet::map()
{
    session_id = 0;
    for (size_t i = 0; i < token_data.size(); ++i)
    {
        const std::string &key = token_data[i][0];
        if (key == "GET" || key == "POST" || key == "HEAD" || key == "DELETE" || key == "PUT")
        {
            method  = key;
            path    = token_data[i][1];
            version = token_data[i][2];
        }
        else if (token_data[i].size() >= 2)
        {
            std::vector<std::string> header;
            for (size_t j = 1; j < token_data[i].size(); j++)
                header.push_back(token_data[i][j]);
            headers[key] = header;
        }
        else if(token_data[i].size() == 1 && !parseHostLineToIP(key))
        {
            error = BADREQUEST;
            return;
        }
    }
    parseQuery();
    cookieFinder();
    bodyHandle();
    unsupportedRequest();
    rfcFormat();
    if(!validateContentLength())
    {
        error = BADREQUEST;
    }
}

bool RequestDataSet::validateContentLength()
{
    bool is_chunked = false;

    std::map<std::string, std::vector<std::string> >::iterator it;

    // Проверяем Transfer-Encoding
    it = headers.find("Transfer-Encoding");
    if (it != headers.end())
    {
        if (!it->second.empty() && it->second[0] == "chunked")
            is_chunked = true;
    }

    if (!is_chunked)
    {
        it = headers.find("Content-Length");
        if (it != headers.end())
        {
            unsigned long first_val = 0;
            bool first = true;

            for (size_t i = 0; i < it->second.size(); ++i)
            {
                const std::string &val = it->second[i];
                if (val.empty())
                    return false;

                // проверяем, что все символы цифры
                for (size_t j = 0; j < val.size(); ++j)
                    if (val[j] < '0' || val[j] > '9')
                        return false;

                unsigned long num = strtoul(val.c_str(), NULL, 10);

                if (first)
                {
                    first_val = num;
                    first = false;
                }
                else if (num != first_val)
                {
                    // два Content-Length не совпадают → 400
                    return false;
                }
            }

            content_length_value = first_val;
        }
        else
        {
            content_length_value = 0; // нет Content-Length
        }
    }
    else
    {
        content_length_value = 0; // chunked, игнорируем Content-Length
    }

    return true;
}


bool RequestDataSet::parseHostLineToIP(const std::string &line)
{
    std::string prefix = "Host:";
    if (line.compare(0, prefix.size(), prefix) != 0)
        return false;

    std::string host_str = line.substr(prefix.size());
    if (host_str.empty())
        return false;

    std::vector<int> octets;
    size_t start = 0, end = 0;
    while ((end = host_str.find('.', start)) != std::string::npos)
    {
        std::string token = host_str.substr(start, end - start);
        octets.push_back(atoi(token.c_str())); // C++98: нужен const char*
        start = end + 1;
    }
    // последний октет
    octets.push_back(atoi(host_str.substr(start).c_str())); // тоже .c_str()

    if (octets.size() != 4)
        return false;

    for (size_t i = 0; i < 4; ++i)
        if (octets[i] < 0 || octets[i] > 255)
            return false;

    // сохраняем IP как 4-байтовое число (big-endian)
    host_ip = ((octets[0] & 0xFF) << 24) |
              ((octets[1] & 0xFF) << 16) |
              ((octets[2] & 0xFF) << 8)  |
              (octets[3] & 0xFF);

    return true;
}

void RequestDataSet::unsupportedRequest()
{
    if(method != "GET" && method != "POST" && method != "HEAD" && method != "DELETE" && method != "PUT")  // must be neither GET nor POST
    {
        this->error = METHODNOTALLOWED;
        return;
    }
        
    else if(version != "HTTP/1.0" && version != "HTTP/1.1")  // allow 1.0 and 1.1
    {
        this->error = HTTPVERSIONNOTSUPPORTED;
        return;
    }

}

void RequestDataSet::tokenizeAndExtractBody()
{
    token_data.clear();
    size_t pos = 0;
    size_t buffer_len = buffer.size();
    bool first_line = true;

    while (pos < buffer_len)
    {
        size_t line_end = buffer.find("\r\n", pos);
        if (line_end == std::string::npos)
        {
            // treat rest of buffer as a line
            line_end = buffer_len;
        }

        std::string line = buffer.substr(pos, line_end - pos);
        pos = line_end + 2; // skip over \r\n

        if (line.empty())
        {
            // empty line = end of headers
            break;
        }

        std::vector<std::string> tokens;
        if (first_line)
        {
            tokens = saveRequestLine(line);
            first_line = false;
        }
        else
        {
            // normal header tokenization (space-separated)
            tokens = saveLine(line);
        }
        token_data.push_back(tokens);
        if(this->error != NONE)
            return;
    }

    // the rest is the body
    if (pos < buffer_len)
        bodyrawline = buffer.substr(pos);
    else
        bodyrawline.clear();
}

std::vector<std::string> RequestDataSet::saveRequestLine(const std::string &line)
{
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type space = line.find(' ', start);

    while (space != std::string::npos)
    {
        if (space == start)
        {
            // two spaces in a row → invalid request line
            tokens.clear();
            return tokens;
        }
        tokens.push_back(line.substr(start, space - start));
        start = space + 1;
        space = line.find(' ', start);
    }

    // add last token
    if (start < line.size())
    {
        std::string last_token = line.substr(start);
        // trim trailing '\r' if present
        if (!last_token.empty() && last_token[last_token.size() - 1] == '\r')
        {
            last_token.erase(last_token.size() - 1);
        }
        tokens.push_back(last_token);
    }
    else
    {
        // line ends with space → invalid
        tokens.clear();
        return tokens;
    }
    if(tokens.size() > 1 && tokens[1].size() > ServerConfigDataSet::getInstance().max_uri_length - 1)
    {
        //std::cout << tokens[1] << std::endl;
        error = URI_TOOLONG;
        return tokens;
    }
    return tokens;
}



std::string RequestDataSet::extractBody(const std::string &buf)
{
    size_t pos = buf.find("\r\n\r\n");
    if (pos == std::string::npos)
        pos = buf.find("\n\n");

    if (pos != std::string::npos)
        return buf.substr(pos + ((buf[pos] == '\r') ? 4 : 2));

    return buf;
}


std::vector<std::string> RequestDataSet::saveLine(const std::string &line)
{
    std::vector<std::string> tokens;
    if(line.size() > 10000)
    {
        error = BADREQUEST;
        return tokens;
    }
    std::stringstream ss(line);
    std::string token;
    while (ss >> token)
        tokens.push_back(token);
    return tokens;
}

void RequestDataSet::debugPrint(size_t line_no,
                                const std::string &line,
                                const std::vector<std::string> &line_tokens)
{
    std::cout << "Line " << line_no << ": \"" << line << "\"\n";
    std::cout << "  Tokens (" << line_tokens.size() << "): ";
    for (size_t i = 0; i < line_tokens.size(); ++i)
        std::cout << "[" << line_tokens[i] << "] ";
    std::cout << std::endl;
}


void RequestDataSet::parseQuery()
{
    size_t qpos = path.find('?');
    if (qpos == std::string::npos)
        return;

    query_string = path.substr(qpos + 1);
    path = path.substr(0, qpos);

    std::stringstream ss(query_string);
    std::string param;
    while (std::getline(ss, param, '&'))
    {
        size_t eq = param.find('=');
        query_params[param.substr(0, eq)] = param.substr(eq + 1);
    }
}

bool RequestDataSet::validateLine(const std::vector<std::string>& token_line, 
                                    const std::vector<std::string>& schema, 
                                    bool &required, 
                                    std::string &_name)
{
    if (token_line.empty() || schema.size() < 4)
    {
        Logger::debug("triggered bad request 4");
        error = BADREQUEST;
        return false;
    }
        
    _name    = schema[0];
    size_t _min     = std::strtoul(schema[1].c_str(), NULL, 10);
    size_t _max     = std::strtoul(schema[2].c_str(), NULL, 10);
    required        = std::strtoul(schema[3].c_str(), NULL, 10);

    std::string line_name = token_line[0];
    if (line_name == _name) {
        if (token_line.size() >= _min && token_line.size() <= _max)
            return true;
        else
            return false;
    }
    return false;
}

void RequestDataSet::addKeyValuePairs(const std::string& str, std::map<std::string, std::string>& outMap)
{
    size_t start = 0;
    while (start < str.length())
    {
        size_t end = str.find_first_of(";&", start);
        if (end == std::string::npos)
            end = str.length();

        std::string pair = str.substr(start, end - start);

        while (!pair.empty() && std::isspace(pair[0]))
            pair.erase(0,1);

        while (!pair.empty() && std::isspace(pair[pair.length()-1]))
            pair.erase(pair.length()-1,1);

        size_t eqPos = pair.find('=');
        if (eqPos != std::string::npos)
        {
            std::string key = pair.substr(0, eqPos);
            std::string value = pair.substr(eqPos + 1);

            outMap[key] = value;
        }
        size_t eqSlash = pair.find('/');
        if (eqSlash != std::string::npos)
        {
            std::string key = pair.substr(0, eqSlash);
            std::string value = pair.substr(eqSlash + 1);

            outMap[key] = value;
        }
        start = end + 1;
    }
}

void RequestDataSet::bodyHandle()
{
    std::map<std::string, std::vector<std::string> >::iterator it = headers.find("Content-Type:");
    if (it != headers.end() && !it->second.empty()) 
    {
        if (it->second[0] == "application/x-www-form-urlencoded") 
        {
            handleBodyData();
        }
        else
        {
            text = false;
            this->body = this->bodyrawline;
        }
    }
}

void RequestDataSet::rfcFormat()
{
    std::map<std::string, std::vector<std::string> >::iterator it = headers.find("Host");
    if (it != headers.end())
    {
        Logger::debug("triggered bad request 5");
        this->error = BADREQUEST;
        return;
    }
    std::map<std::string, std::vector<std::string> >::iterator it_ac = headers.find("Accept-Language");
    if (it_ac != headers.end())
    {
        Logger::debug("triggered bad request 6");
        this->error = BADREQUEST;
        return;
    }
}


void RequestDataSet::handleBodyData()
{
    const std::string &key = this->bodyrawline;
    const std::string prefixMessage = "message=";
    if (key.compare(0, prefixMessage.size(), prefixMessage) == 0)
    {
        text = true;
        body = urlDecode(key.substr(prefixMessage.size()));
        return;
    }
    const std::string prefixMethod = "_method=";
    if (key.compare(0, prefixMethod.size(), prefixMethod) == 0)
    {
        this->addKeyValuePairs(key, this->method_map);
        if(!this->method_map.empty())
        {
            method = method_map["_method"];
            /*Logger::debug("method" + method);
            int id = atoi(method_map["resource_id"].c_str());
            if (!FileManager::getInstance()->deleteFile(id))
            {
                error = INTERNALERROR;
                return;
            }*/
        }
        return;
    }
    size_t pos = key.find('=');
    if (pos != std::string::npos) 
    {
        std::string k = key.substr(0, pos);
        std::string v = key.substr(pos + 1);
        this->body_map[k] = v;
        body = v;
    }
}

void RequestDataSet::cookieFinder()
{
    const std::string key = "Cookie:"; // убедись, что без двоеточия
    std::map<std::string, std::vector<std::string> >::iterator it = headers.find(key);

    if (it == headers.end())
    {
        session_id = 0;
        return;
    }
    for (size_t i = 0; i < it->second.size(); ++i)
    {
        const std::string &keyv = it->second[i];
        const std::string prefixMessage = "session_id";
        if (keyv.compare(0, prefixMessage.size(), prefixMessage) == 0)
        {
            std::string cookies = it->second[i];
            addKeyValuePairs(cookies, this->cookie_map);
            break;            
        }
    }
    std::map<std::string, std::string>::iterator cit = cookie_map.find("session_id");
    if (cit != cookie_map.end())
    {
        session_id = atol(cit->second.c_str());
    }
    else
    {
        session_id = 0;
    }
}



void RequestDataSet::printConfig() const
{
    std::ofstream logFile;
    logFile.open("logs/request.log", std::ios::out | std::ios::app);

    if (!logFile)
    {
        std::cerr << "❌ Failed to open log file 'request.log'" << std::endl;
        return;
    }

    logFile << "==================== REQUEST START ====================" << std::endl;

    logFile << "📝 method: " << (method.empty() ? "(not set)" : method) << std::endl;
    logFile << "🌐 path: " << (path.empty() ? "(not set)" : path) << std::endl;
    logFile << "❓ query_string: " << (query_string.empty() ? "(not set)" : query_string) << std::endl;
    logFile << "⚡ version: " << (version.empty() ? "(not set)" : version) << std::endl;
    logFile << "🆔 session_id: " << session_id << std::endl;

    logFile << "📬 headers: ";
    if (headers.empty())
        logFile << "(none)" << std::endl;
    else
    {
        logFile << std::endl;
        for (std::map<std::string, std::vector<std::string> >::const_iterator it = headers.begin(); it != headers.end(); ++it)
        {
            logFile << "   [" << it->first << "] -> ";
            if (it->second.empty())
                logFile << "(empty)";
            else
            {
                for (size_t i = 0; i < it->second.size(); ++i)
                {
                    logFile << it->second[i];
                    if (i + 1 < it->second.size())
                        logFile << ", ";
                }
            }
            logFile << std::endl;
        }
    }

    logFile << "📦 body: " << (body.empty() ? "(empty)" : body) << std::endl;

    logFile << "🗂️ bodyData: ";
    if (bodyData.empty())
        logFile << "(none)" << std::endl;
    else
    {
        logFile << std::endl;
        for (std::map<std::string, std::string>::const_iterator it = bodyData.begin(); it != bodyData.end(); ++it)
            logFile << "   [" << it->first << "] -> " << it->second << std::endl;
    }

    logFile << "📌 boundary: " << (boundary.empty() ? "(not set)" : boundary) << std::endl;

    logFile << "🍪 cookie_map: ";
    if (cookie_map.empty())
        logFile << "(none)" << std::endl;
    else
    {
        logFile << std::endl;
        for (std::map<std::string, std::string>::const_iterator it = cookie_map.begin(); it != cookie_map.end(); ++it)
            logFile << "   [" << it->first << "] -> " << it->second << std::endl;
    }

    logFile << "📦 multipart_map: ";
    if (multipart_map.empty())
        logFile << "(none)" << std::endl;
    else
    {
        logFile << std::endl;
        for (std::map<std::string, std::string>::const_iterator it = multipart_map.begin(); it != multipart_map.end(); ++it)
            logFile << "   [" << it->first << "] -> " << it->second << std::endl;
    }
    logFile << "🗺️ method_map: ";
    if (method_map.empty())
        logFile << "(none)" << std::endl;
    else
    {
        logFile << std::endl;
        for (std::map<std::string, std::string>::const_iterator it = method_map.begin(); it != method_map.end(); ++it)
            logFile << "   [" << it->first << "] -> " << it->second << std::endl;
    }

    logFile << "🗺️ query params: ";
    if (query_params.empty())
        logFile << "(none)" << std::endl;
    else
    {
        logFile << std::endl;
        for (std::map<std::string, std::string>::const_iterator it = query_params.begin(); it != query_params.end(); ++it)
            logFile << "   [" << it->first << "] -> " << it->second << std::endl;
    }

    logFile << "📦 parts: ";

    if (parts.empty())
        logFile << "(none)" << std::endl;
    else
    {
        logFile << std::endl;
        for (size_t i = 0; i < parts.size(); ++i)
        {
            logFile << "   Part " << i << ":" << std::endl;

            // Print headers (vector)
            if (parts[i].headers.empty())
                logFile << "      headers (vector): (none)" << std::endl;
            else
            {
                logFile << "      headers (vector):" << std::endl;
                for (std::map<std::string, std::vector<std::string> >::const_iterator it = parts[i].headers.begin(); it != parts[i].headers.end(); ++it)
                {
                    logFile << "         [" << it->first << "] -> ";
                    for (size_t j = 0; j < it->second.size(); ++j)
                    {
                        logFile << it->second[j];
                        if (j + 1 < it->second.size())
                            logFile << ", ";
                    }
                    logFile << std::endl;
                }
            }

            // Print headers_map (string->string)
            if (parts[i].headers_map.empty())
                logFile << "      headers_map: (none)" << std::endl;
            else
            {
                logFile << "      headers_map:" << std::endl;
                for (std::map<std::string, std::string>::const_iterator it = parts[i].headers_map.begin(); it != parts[i].headers_map.end(); ++it)
                    logFile << "         [" << it->first << "] -> " << it->second << std::endl;
            }

            logFile << "      body: " << (parts[i].body.empty() ? "(empty)" : parts[i].body) << std::endl;
        }
    }
    logFile << "==================== REQUEST END ======================" << std::endl << std::endl;

    logFile.close();
}

