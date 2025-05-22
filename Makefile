CXX = c++
CXXFLAGS = -std=c++98 -g3 -Wall -Wextra -Werror -Wpedantic
INCS = -Iinclude

SRC_DIRS = src src/WebServer src/Config src/Http src/utils
OBJ_DIRS = $(sort $(dir $(OBJS)))

vpath %.cpp $(SRC_DIRS)

SRCS = $(foreach module, $(SRC_DIRS), $(wildcard $(module)/*.cpp))
OBJS = $(addprefix obj/, $(SRCS:.cpp=.o))
HEADERS = $(wildcard include/*.hpp) $(wildcard include/*.h) $(foreach module, $(SRC_DIRS), $(wildcard $(module)/*.tpp))
EXE = webserv

all : $(EXE)

$(EXE) : $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(EXE)

obj/%.o : %.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIRS)
	@$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $@

TEST_SRCS = $(wildcard test/*.cpp)
TEST_OBJS = $(addprefix obj/, $(TEST_SRCS:.cpp=.o))
TEST_HEADERS = test/test.h
TEST_EXE = unit-tests

test : fclean $(TEST_EXE)
	./$(TEST_EXE)

$(TEST_EXE) : $(filter-out obj/src/main.o, $(OBJS)) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(INCS) $^ -o $@

obj/test/%.o : test/%.cpp $(HEADERS) $(TEST_HEADERS)
	mkdir -p obj/test
	$(CXX) $(CXXFLAGS) $(INCS) -c $< -o $@

clean :
	@rm -rf obj/

fclean : clean
	@rm -f $(EXE)

re : fclean all

.PHONY : all clean fclean re
