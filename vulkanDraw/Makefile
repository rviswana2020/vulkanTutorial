CC = clang++-13
CXXFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG -g
else
	CXXFLAGS += -DNDEBUG
endif

vulkanDraw: vulkanDraw.cpp
	$(info, $(CXXFLAGS))
	$(CC) $(CXXFLAGS) -o vulkanDraw main.cpp vulkanDraw.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkanDraw
	./vulkanDraw

clean:
	rm -rf vulkanDraw

#USAGE:
### make clean; 
### make DEBUG=0 test
### make DEBUG=1 test
