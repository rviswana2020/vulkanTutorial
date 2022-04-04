CLFAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan

vulkanInstance: vulkanInstance.cpp
	clang++-13 $(CfLAGS) -o vulkanInstance main.cpp vulkanInstance.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkanInstance
	./vulkanInstance

clean:
	rm -rf vulkanInstance
