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
SRC:= Client.cpp Config.cpp Location.cpp Server.cpp Socket.cpp ft_read_data.cpp ft_split.cpp reqParser/reqParse.cpp reqParser/postRes.cpp main.cpp
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
