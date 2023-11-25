# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abouramd <abouramd@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/13 15:38:44 by abouramd          #+#    #+#              #
#    Updated: 2023/10/13 16:01:19 by abouramd         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME:= webserv
CXX:= c++
CXXFLAGS:= -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3

SRC_pars:= Client.cpp Config.cpp  FileType.cpp  findSL.cpp  ft_read_data.cpp  ft_split.cpp  ip_bin.cpp  Location.cpp  Server.cpp  Socket.cpp
SRC_resp:= cgi.cpp delete.cpp  error_handling.cpp  get.cpp  main.cpp  tools.cpp auto_index.cpp
SRC_reqe:= boundaries.cpp  Cgi.cpp  chunked.cpp  postRes.cpp  reqParse.cpp  String.cpp  Tools.cpp

HEAD:= reqParser/Client.hpp pars/FileType.hpp pars/Server.hpp reqParser/Cgi.hpp\
 reqParser/String.hpp responses/responses.hpp pars/Config.hpp pars/Location.hpp\
 pars/Socket.hpp reqParser/reqParse.hppreqParser/Tools.hpp

INC:= -I pars -I responses -I reqParser
SRC:= $(addprefix pars/, $(SRC_pars)) $(addprefix responses/, $(SRC_resp)) $(addprefix reqParser/, $(SRC_reqe)) main.cpp
OBJ:= $(SRC:.cpp=.o)

all:$(NAME)

$(NAME):$(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) $(INC)
	@echo "\033[1;32m ✅ webserv done.\033[0m"

.cpp.o:
	@$(CXX) $(CXXFLAGS) -o $@ -c $< $(INC)
	@echo "\033[1;34m - creat $@ from $<.\033[0m"

clean:
	@rm -rf $(OBJ)
	@echo "\033[1;31m ❎ remove the object files of webserv.\033[0m"

fclean:clean
	@rm -rf $(NAME)
	@echo "\033[1;31m ❎ remove webserv.\033[0m"

re:fclean all
