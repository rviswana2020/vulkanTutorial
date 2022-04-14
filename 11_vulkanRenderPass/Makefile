CC = clang++-13
CXXFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG -g
else
	CXXFLAGS += -DNDEBUG
endif

vulkanRenderPass: vulkanRenderPass.cpp
	$(info, $(CXXFLAGS))
	$(CC) $(CXXFLAGS) -o vulkanRenderPass main.cpp vulkanRenderPass.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkanRenderPass
	./vulkanRenderPass

clean:
	rm -rf vulkanRenderPass

#USAGE:
### make clean; 
### make DEBUG=0 test
### make DEBUG=1 test
