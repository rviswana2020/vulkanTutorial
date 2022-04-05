CC = clang++-13
CXXFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG
else
	CXXFLAGS += -DNDEBUG
endif


vulkanInstance: vulkanInstance.cpp
	$(info $(CXXFLAGS))
	$(CC) $(CXXFLAGS) -o vulkanInstance main.cpp vulkanInstance.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkanInstance
	./vulkanInstance

clean:
	rm -rf vulkanInstance
