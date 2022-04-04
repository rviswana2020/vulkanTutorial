CLFAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan

vulkanBasecode: vulkanBasecode.cpp
	g++ $(CfLAGS) -o vulkanBasecode main.cpp vulkanBasecode.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkanBasecode
	./vulkanBasecode

clean:
	rm -rf vulkanBasecode
