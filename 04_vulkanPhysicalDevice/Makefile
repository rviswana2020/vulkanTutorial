CC = clang++-13
CXXFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG -g
else
	CXXFLAGS += -DNDEBUG
endif

vulkanPhysicalDevice: vulkanPhysicalDevice.cpp
	$(info, $(CXXFLAGS))
	$(CC) $(CXXFLAGS) -o vulkanPhysicalDevice main.cpp vulkanPhysicalDevice.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkanPhysicalDevice
	./vulkanPhysicalDevice

clean:
	rm -rf vulkanPhysicalDevice

#USAGE:
### make clean; 
### make DEBUG=0 test
### make DEBUG=1 test
