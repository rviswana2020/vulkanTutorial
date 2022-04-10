CC = clang++-13
CXXFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXXFLAGS += -DDEBUG -g
else
	CXXFLAGS += -DNDEBUG
endif

vulkanImageView: vulkanImageView.cpp
	$(info, $(CXXFLAGS))
	$(CC) $(CXXFLAGS) -o vulkanImageView main.cpp vulkanImageView.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkanImageView
	./vulkanImageView

clean:
	rm -rf vulkanImageView

#USAGE:
### make clean; 
### make DEBUG=0 test
### make DEBUG=1 test
