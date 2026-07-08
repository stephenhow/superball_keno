# Compiler and flags
CXX       = mpic++
CXXFLAGS  = -Wall -O3 -std=c++17
#CXXFLAGS  = -Wall -g -fsanitize=address -fno-omit-frame-pointer -std=c++14

# Targets
TARGETS   = play

PLAY_SRCS = main.cpp
PLAY_OBJS = $(PLAY_SRCS:.cpp=.o)

# Default build rule
all: $(TARGETS)

play: $(PLAY_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lncurses

# Generic compilation rule
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Cleanup
clean:
	rm -f *.o $(TARGETS)
