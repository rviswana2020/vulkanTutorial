CLFAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan

vulkanInstance: vulkanInstance.cpp
	g++ $(CfLAGS) -o vulkanInstance main.cpp vulkanInstance.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkanInstance
	./vulkanInstance

clean:
	rm -rf vulkanInstance
