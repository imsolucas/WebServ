# Compiler & Flags
CXX = c++
CXXFLAGS = -std=c++98 -g3 -Wall -Wextra -Werror -Wpedantic

# Client
CLIENT_SRCS = $(wildcard Client/*.cpp)
CLIENT_OBJS = $(addprefix Client/obj/, $(notdir $(CLIENT_SRCS:.cpp=.o)))
CLIENT_EXE = client

Client : $(CLIENT_EXE)

$(CLIENT_EXE) : $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $(CLIENT_OBJS) -o $(CLIENT_EXE)

Client/obj/%.o : Client/%.cpp Client/Client.hpp
	mkdir -p Client/obj/
	$(CXX) $(CXXFLAGS) -IClient -c $< -o $@

# Server
SERVER_SRCS = $(wildcard Server/*.cpp)
SERVER_OBJS = $(addprefix Server/obj/, $(notdir $(SERVER_SRCS:.cpp=.o)))
SERVER_EXE = server

Server : $(SERVER_EXE)

$(SERVER_EXE) : $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $(SERVER_OBJS) -o $(SERVER_EXE)

Server/obj/%.o : Server/%.cpp Server/Server.hpp
	mkdir -p Server/obj/
	$(CXX) $(CXXFLAGS) -IServer -c $< -o $@

# Misc.
all : Server Client

clean :
	rm -rf Client/obj/
	rm -rf Server/obj/

fclean : clean
	rm -f $(CLIENT_EXE)
	rm -f $(SERVER_EXE)

re : fclean client server

.PHONY : client server clean fclean re
