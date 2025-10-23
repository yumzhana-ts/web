#include "EventDispatcher.class.hpp"
#include "FileManager.class.hpp"
#include "Sessions.class.hpp"
#include "Logger.class.hpp"

EventDispatcher* EventDispatcher::g_server = NULL;
volatile sig_atomic_t EventDispatcher::g_running = 1;

EventDispatcher::EventDispatcher(): epoll_fd(-1)
{
}

EventDispatcher::~EventDispatcher()
{
    std::map<int, Socket*>::iterator it;
    for (it = sockets.begin(); it != sockets.end(); ++it)
    {
        close(it->first);
        delete it->second;
    }
    sockets.clear();

    std::map<int, Client*>::iterator cit;
    for (cit = clients.begin(); cit != clients.end(); ++cit)
    {
        close(cit->first);
        delete cit->second;
    }
    clients.clear();

    if (epoll_fd != -1)
        close(epoll_fd);
    g_server = NULL;
}

void EventDispatcher::shutdown()
{
    ServerConfigDataSet::getInstance().destroyInstance();
    SessionManager::getInstance()->destroyInstance();
    FileManager::getInstance()->destroyInstance();
}



void EventDispatcher::signalHandler(int signum)
{
    Logger::info("⚠️ [Event Dispatcher] Signal received, shutting down...");
    g_running = 0;
    if (g_server)
    {
        std::map<int, Socket*> sockets = g_server->getSockets();
        std::map<int, Socket*>::iterator it;
        for (it = sockets.begin(); it != sockets.end(); ++it)
        {
            int fd = it->first;
            if (fd != -1)
            {
                close(fd);
                Logger::debug("[Event Dispatcher] Closed server socket " + toString(fd));
            }
        }
    }
    (void)signum;
}

Socket* EventDispatcher::getSocket(int fd)
{
    std::map<int, Socket*>::iterator it = sockets.find(fd);
    if (it != sockets.end())
        return it->second;
    return NULL;
}

void EventDispatcher::setupConfig(std::string configFile) 
{
    try 
    {
        ServerConfigDataSet::setConfig(configFile);
        Logger::info("✅ [Serverconfig Dataset] Configuration loaded from " + configFile);
    } 
    catch (const std::exception& e) 
    {
        //Logger::error("Failed to load config: " + std::string(e.what()));

    }
}

void EventDispatcher::setupSignals() 
{
    std::signal(SIGINT, EventDispatcher::signalHandler);
    std::signal(SIGTERM, EventDispatcher::signalHandler);
}

/**
 * @brief Creates an epoll instance and adds the server socket fd to monitor for incoming connections.
 * @param fd - File descriptor of the server socket to add to epoll.
 * @throws std::runtime_error if epoll creation or adding the fd fails.
 */
void EventDispatcher::addServerToEpoll(int server_fd)
{
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1)
        throw std::runtime_error("[Event Dispatcher] Failed to add server socket to epoll");
}

void EventDispatcher::setupSockets()
{
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
        throw std::runtime_error("[Event Dispatcher] Failed to create epoll instance");
    std::vector<unsigned int> socketPorts = ServerConfigDataSet::getInstance().ports;
    if(socketPorts.empty())
        throw std::runtime_error("[Event Dispatcher] Failed to create sockets.");
    for(size_t i = 0; i < socketPorts.size(); i++)
        this->setupSocket(socketPorts[i]);
}

void EventDispatcher::setupSocket(int port)
{
    Socket* serverSocket = NULL;
    try 
    {
        serverSocket = new Socket(port);
        serverSocket->setup();
        int server_fd = serverSocket->getServerFd();
        set_non_blocking(server_fd);
        addServerToEpoll(server_fd);
        this->sockets[server_fd] = serverSocket; // only assign after success
    } 
    catch(...) 
    {

        delete serverSocket; // clean up if any step throws
        throw;               // rethrow exception
    }
}

/**
 * @brief Adds a client socket fd to the epoll instance for monitoring with edge-triggered input events.
 * @param epoll_fd - File descriptor of the epoll instance.
 * @param fd - File descriptor of the client socket to add.
 * @throws std::runtime_error if adding the client fd to epoll fails (also closes the fd).
 */
void EventDispatcher::addClientToEpoll(int client_fd)
{
    epoll_event ev;
    ev.events = EPOLLIN | EPOLLET | EPOLLRDHUP;  // add EPOLLRDHUP
    ev.data.fd = client_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
    {
        close(client_fd);
        throw std::runtime_error("[Event Dispatcher] Failed to add fd " + toString(client_fd) + " to epoll");
    }
}

/**
 * @brief Accepts a new client connection, sets it non-blocking, adds it to epoll,
 *        and stores it in the client map.
 * @throws runtime_error or logs error if client creation or epoll addition fails.
 */
void EventDispatcher::setupClient(Socket &socket)
{
    try 
    {
        Client* client = new Client(socket);
        int client_fd = client->getClientFd();
        set_non_blocking(client_fd);
        addClientToEpoll(client_fd);
        clients.insert(std::make_pair(client_fd, client));
        //Logger::debug(" ✅ [Event Dispatcher] Client setup complete: fd=" + toString(client_fd) + ", non-blocking, epoll added");
    }
    catch (const std::exception& e) 
    {
        Logger::error(std::string("[Event Dispatcher] Failed to create client: ") + e.what());
    }
}


/**
 * @brief Handles the client request-response cycle.
 *        If any error occurs, removes client from epoll and client map, and closes socket.
 * @param client - Reference to the client object to handle.
 */
void EventDispatcher::handleClient(Client& client)
{
    int fd = client.getClientFd();
    try
    {
        client.getRequest();
        Socket* serverSock = client.getServerSocket();
        ChainOfResponsibility chain(client.raw_request, client, *serverSock);
    }
    catch (const std::exception& e)
    {
        Logger::error("[Event Dispatcher] Client [" + toString(fd) + "] failed: " + e.what());
        std::map<int, Client*>::iterator it = clients.find(fd);
        if (it != clients.end())
        {
            if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
                throw std::runtime_error("epoll DEL");
            close(fd);
            delete it->second;
            clients.erase(it);
            Logger::error("[Event Dispatcher] Closed and removed client fd " + toString(fd));
        }
        else
            Logger::error("[Event Dispatcher] Client fd " + toString(fd) + " not found in clients map");
    }
}

/**
 * @brief Main event loop: waits for epoll events and dispatches accordingly.
 *        If event is on server socket, accepts new client.
 *        Otherwise, processes activity on client sockets.
 */
void EventDispatcher::run()
{
    const int MAX_EVENTS = 64;
    epoll_event events[MAX_EVENTS];

    Logger::info("🚀 [Event Dispatcher] Server started, entering main loop...");

    while (g_running)
    {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (n == -1) 
        {
            if (errno == EINTR) 
                continue; // interrupted by signal, retry
            Logger::error("epoll_wait() failed, errno=" + toString(errno));
            break;
        }
        for (int i = 0; i < n; ++i)
        {
            int fd = events[i].data.fd;
            epoll_event& ev = events[i];

            // Server socket: accept new client
            Socket* serverSock = getSocket(fd);
            if (serverSock != NULL)
            {
                setupClient(*serverSock);
                continue;
            }
            //else 500 

            // Client socket: check for hangup or error
            std::map<int, Client*>::iterator it = clients.find(fd);
            if (it == clients.end()) 
            {
                Logger::error("[Event Dispatcher] Client fd " + toString(fd) + " not found");
                continue;
            }

            if (ev.events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
                    throw std::runtime_error("epoll -1");
                close(fd);
                delete it->second;
                clients.erase(it);
                continue;
            }

            // Handle normal readable events
            if (ev.events & EPOLLIN)
            {
                handleClient(*it->second);
                Logger::info("🤖 [Event Dispatcher] Server online, standing by for connections");
                FileManager::getInstance()->printAllFiles();
                SessionManager::getInstance()->printSessions();
            }
        }
    }
    Logger::debug("[Event Dispatcher] Exited from server loop.");
}


void set_non_blocking(int fd) 
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        throw std::runtime_error("fcntl F_GETFL failed");
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) 
        throw std::runtime_error("fcntl F_SETFL failed");
}
