CC = clang++-13
CXXFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG -g
else
	CXXFLAGS += -DNDEBUG
endif

vulkanSwapchain: vulkanSwapchain.cpp
	$(info, $(CXXFLAGS))
	$(CC) $(CXXFLAGS) -o vulkanSwapchain main.cpp vulkanSwapchain.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkanSwapchain
	./vulkanSwapchain

clean:
	rm -rf vulkanSwapchain

#USAGE:
### make clean; 
### make DEBUG=0 test
### make DEBUG=1 test
