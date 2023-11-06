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
<<<<<<< HEAD
CXXFLAGS:= -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3 
SRC:= FileType.cpp findSL.cpp Client.cpp Config.cpp Location.cpp Server.cpp Socket.cpp ft_read_data.cpp ft_split.cpp reqParser/reqParse.cpp reqParser/postRes.cpp main.cpp responses/main.cpp responses/tools.cpp responses/methods.cpp responses/cgi.cpp responses/error_handling.cpp
=======
CXXFLAGS:= -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3
SRC:= FileType.cpp findSL.cpp Client.cpp Config.cpp Location.cpp Server.cpp Socket.cpp ft_read_data.cpp ft_split.cpp reqParser/reqParse.cpp reqParser/postRes.cpp reqParser/Cgi.cpp reqParser/Tools.cpp main.cpp
>>>>>>> master
OBJ:= $(SRC:.cpp=.o)

all:$(NAME)

$(NAME):$(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)

fclean:clean
	rm -rf $(NAME)

re:fclean all
