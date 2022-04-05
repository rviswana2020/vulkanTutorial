CC = clang++-13
CXXFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG -g
else
	CXXFLAGS += -DNDEBUG
endif

vulkanValidation: vulkanValidation.cpp
	$(info, $(CXXFLAGS))
	$(CC) $(CXXFLAGS) -o vulkanValidation main.cpp vulkanValidation.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkanValidation
	./vulkanValidation

clean:
	rm -rf vulkanValidation

#USAGE:
### make clean; 
### make DEBUG=0 test
### make DEBUG=1 test
