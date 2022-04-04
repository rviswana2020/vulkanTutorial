CLFAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan

vulkanWindow: vulkanWindow.cpp
	g++ $(CfLAGS) -o vulkanWindow main.cpp vulkanWindow.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkanWindow
	./vulkanWindow

clean:
	rm -rf vulkanWindow
