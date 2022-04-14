CC = clang++-13
CXXFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG -g
else
	CXXFLAGS += -DNDEBUG
endif

vulkanQueueFamilies: vulkanQueueFamilies.cpp
	$(info, $(CXXFLAGS))
	$(CC) $(CXXFLAGS) -o vulkanQueueFamilies main.cpp vulkanQueueFamilies.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkanQueueFamilies
	./vulkanQueueFamilies

clean:
	rm -rf vulkanQueueFamilies

#USAGE:
### make clean; 
### make DEBUG=0 test
### make DEBUG=1 test
