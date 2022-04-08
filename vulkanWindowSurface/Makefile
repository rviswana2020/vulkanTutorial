CC = clang++-13
CXXFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG -g
else
	CXXFLAGS += -DNDEBUG
endif

vulkanWindowSurface: vulkanWindowSurface.cpp
	$(info, $(CXXFLAGS))
	$(CC) $(CXXFLAGS) -o vulkanWindowSurface main.cpp vulkanWindowSurface.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkanWindowSurface
	./vulkanWindowSurface

clean:
	rm -rf vulkanWindowSurface

#USAGE:
### make clean; 
### make DEBUG=0 test
### make DEBUG=1 test
