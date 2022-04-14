CC = clang++-13
CXXFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG -g
else
	CXXFLAGS += -DNDEBUG
endif

vulkanLogicalDevice: vulkanLogicalDevice.cpp
	$(info, $(CXXFLAGS))
	$(CC) $(CXXFLAGS) -o vulkanLogicalDevice main.cpp vulkanLogicalDevice.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkanLogicalDevice
	./vulkanLogicalDevice

clean:
	rm -rf vulkanLogicalDevice

#USAGE:
### make clean; 
### make DEBUG=0 test
### make DEBUG=1 test
