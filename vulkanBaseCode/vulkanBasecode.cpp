#include "vulkanBasecode.h"

#include <iostream>

static const char * INTENTSTR = "...";
static const char INTENTSPACE = '\t';

/*------------------------------------------------------------------*/
// Public interface definition
/*------------------------------------------------------------------*/

void
HelloTriangleApplication::run() {
	std::cout << INTENTSTR << "Hello World Run" << std::endl;
   initVulkan();
   mainLoop();
   cleanup();
}

/*------------------------------------------------------------------*/
// Private interface definition
/*------------------------------------------------------------------*/

void
HelloTriangleApplication::initVulkan() {
	std::cout << INTENTSPACE << INTENTSTR << "vulkan Initialization"
              << std::endl;
}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::mainLoop() {
	std::cout << INTENTSPACE << INTENTSTR << "vulkan rendering "
              << std::endl;
}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::cleanup() {
	std::cout << INTENTSPACE << INTENTSTR << "vulkan cleanup"
              << std::endl;
}

/*------------------------------------------------------------------*/
