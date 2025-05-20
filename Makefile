CXX = c++
CXXFLAGS = -std=c++98 -g3 -Wall -Wextra -Werror -Wpedantic
INCS = -Iinclude

SRC_DIRS = src src/WebServer src/Config src/Listener
OBJ_DIRS = $(sort $(dir $(OBJS)))

vpath %.cpp $(SRC_DIRS)

SRCS = $(foreach module, $(SRC_DIRS), $(wildcard $(module)/*.cpp))
OBJS = $(addprefix obj/, $(SRCS:.cpp=.o))
HEADERS = $(wildcard include/*.hpp) $(wildcard include/*.h) $(foreach module, $(SRC_DIR), $(wildcard $(module)/*.tpp))
EXE = webserv

all : $(EXE)

$(EXE) : $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(EXE)

obj/%.o : %.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIRS)
	@$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $@

clean:
	@rm -rf obj/

fclean : clean
	@rm -f $(EXE)

re : fclean all

.PHONY : all clean fclean re
