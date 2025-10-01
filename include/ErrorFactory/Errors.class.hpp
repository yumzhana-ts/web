#include <iostream>
#include <map>
#include "DataSetFactory/RequestDataSet.class.hpp"
#include "../ChainOfResponsibility.class.hpp"
#include "../ResponseBuilder/AResponse.class.hpp"
#include "AError.class.hpp"


class InternalServerError: public AError, public IHandler, public AResponse
{
    public:
        InternalServerError(RequestDataSet &req);
        virtual ~InternalServerError(void);
        void handle();
        void build();
};

class PageNotFound: public AError, public IHandler, public AResponse
{
    public:
        PageNotFound(RequestDataSet &req);
        virtual ~PageNotFound(void);
        void handle();
        void build();
};

class BadRequest: public AError, public IHandler, public AResponse
{
public:
    BadRequest(RequestDataSet &req);
    virtual ~BadRequest();
    void handle();
    void build();
};

class Forbidden: public AError, public IHandler, public AResponse
{
public:
    Forbidden(RequestDataSet &req);
    virtual ~Forbidden();
    void handle();
    void build();
};


class MethodNotAllowed: public AError, public IHandler, public AResponse
{
public:
    MethodNotAllowed(RequestDataSet &req);
    virtual ~MethodNotAllowed();
    void handle();
    void build();
};

class PayloadTooLarge: public AError, public IHandler, public AResponse
{
public:
    PayloadTooLarge(RequestDataSet &req);
    virtual ~PayloadTooLarge();
    void handle();
    void build();
};

class BadGateway: public AError, public IHandler, public AResponse
{
public:
    BadGateway(RequestDataSet &req);
    virtual ~BadGateway();
    void handle();
    void build();
};

class GatewayTimeout: public AError, public IHandler, public AResponse
{
public:
    GatewayTimeout(RequestDataSet &req);
    virtual ~GatewayTimeout();
    void handle();
    void build();
};

class HTTPVersionNotSupported: public AError, public IHandler, public AResponse
{
public:
    HTTPVersionNotSupported(RequestDataSet &req);
    virtual ~HTTPVersionNotSupported();
    void handle();
    void build();
};

class URITooLong : public AError, public IHandler, public AResponse
{
public:
    URITooLong(RequestDataSet &req);
    virtual ~URITooLong();
    void handle();
    void build();
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