# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ytsyrend <ytsyrend@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/05 18:06:28 by ytsyrend          #+#    #+#              #
#    Updated: 2025/09/11 01:23:20 by ytsyrend         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	main

# Compiler
CPP			=	c++

# Compiler Flags
CPPFLAGS	=	-Wall -Werror -Wextra -std=c++98 -I. -Iinclude
RM 			= 	rm -rf

# Directories
OBJS_DIR 	= 	objs
SRC_DIRS 	= 	srcs srcs/DataSetFactory srcs/ResponseBuilder srcs/ErrorFactory

MAIN 		= 	main.cpp
TEST 		= 	$(SRC_DIRS)/tests.cpp
COMMON_SRCS = \
	srcs/ChainOfResponsibility.class.cpp \
	srcs/DataSetFactory/ADataSet.class.cpp \
	srcs/DataSetFactory/Cerberus.class.cpp \
	srcs/DataSetFactory/LocationCgiConfigDataSet.class.cpp \
	srcs/DataSetFactory/LocationConfigDataSet.class.cpp \
	srcs/DataSetFactory/RequestDataSet.class.cpp \
	srcs/DataSetFactory/ServerConfigDataSet.class.cpp \
	srcs/ErrorFactory/AError.class.cpp \
	srcs/ErrorFactory/ErrorFactory.class.cpp \
	srcs/ErrorFactory/Errros.class.cpp \
	srcs/FileManager.class.cpp \
	srcs/Logger.class.cpp \
	srcs/ResponseBuilder/DeleteResponse.class.cpp \
	srcs/ResponseBuilder/PostResponse.class.cpp \
	srcs/ResponseBuilder/PutResponse.class.cpp \
	srcs/ResponseBuilder/SendResponseHandler.class.cpp \
	srcs/ResponseBuilder/SessionData.class.cpp \
	srcs/ResponseBuilder/LocationDecorator.cpp \
	srcs/ResponseBuilder/AResponse.class.cpp \
	srcs/ResponseBuilder/CGIEnvBuilder.class.cpp \
	srcs/ResponseBuilder/CgiHandler.class.cpp \
	srcs/ResponseBuilder/GetResponse.class.cpp \
	srcs/Sessions.class.cpp \
	srcs/Socket.class.cpp \
	srcs/ChainBuilder.class.cpp \
	srcs/Client.class.cpp \
	srcs/utils.cpp \
	srcs/EventDispatcher.class.cpp


# Source and Object Files
SRCS 		= 	$(COMMON_SRCS) $(MAIN)
TEST_SRCS 	= 	$(COMMON_SRCS) $(TEST)
OBJS 		= 	$(SRCS:%.cpp=$(OBJS_DIR)/%.o)
TEST_OBJS 	= 	$(TEST_SRCS:%.cpp=$(OBJS_DIR)/%.o)

all : $(OBJS_DIR) $(NAME)

# Compile and Link the Project
$(NAME) : $(OBJS)
	$(CPP) $(CPPFLAGS) $^ -o $@
	@echo "main: Compilation successful."

test : $(TEST_OBJS)
	$(CPP) $(CPPFLAGS) $^ -o $@
	@echo "test: Compilation successful."


# Compile Source Files into Object Files
$(OBJS_DIR)/%.o: %.cpp | $(OBJS_DIR)
	@mkdir -p $(@D)
	$(CPP) $(CPPFLAGS) -c $< -o $@


# Create the Object File Directory
$(OBJS_DIR):
	@mkdir -p $(OBJS_DIR)/$(SRC_DIRS)


# Clean Objects and Executable
clean :
	$(RM) $(OBJS_DIR)
	@echo "Object files and directory cleaned."

fclean : clean
	$(RM) $(NAME) 
	@echo "Executable removed."

tclean : clean
	$(RM) test
	@echo "Executable removed."


VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

leaks:
	@echo "\033[1;33m\nRunning Valgrind and checking for memory leaks...\033[0m"
	@touch valgrind_report.txt
	# Run server under Valgrind in background
	-$(VALGRIND) --log-file=valgrind_report.txt ./$(NAME) &

# Define symbolic constants for color codes
CYAN=\033[1;36m
GREEN=\033[1;32m"
RESET_COLOR=\033[0m


.PHONY : clean fclean re all bonus

re: fclean all