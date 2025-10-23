#ifndef LIBRARIES_HPP
# define LIBRARIES_HPP

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <map>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/wait.h>
#include <stdexcept>
#include <csignal>
#define PROJECT_ROOT "Desktop/Git/webserver"




template <typename T>
std::string toString(const T &value);

#include "srcs/utils.tpp"

#endif