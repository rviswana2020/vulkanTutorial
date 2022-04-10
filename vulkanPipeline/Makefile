CC = clang++-13
CXXFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG -g
else
	CXXFLAGS += -DNDEBUG
endif

vulkanPipeline: vulkanPipeline.cpp
	$(info, $(CXXFLAGS))
	$(CC) $(CXXFLAGS) -o vulkanPipeline main.cpp vulkanPipeline.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkanPipeline
	./vulkanPipeline

clean:
	rm -rf vulkanPipeline

#USAGE:
### make clean; 
### make DEBUG=0 test
### make DEBUG=1 test
