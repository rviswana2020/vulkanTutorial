#include "vulkanInstance.h"

#include <iostream>		// console reporting
#include <stdexcept>	// error handling
#include <cstdlib>		// return macro definition

/*------------------------------------------------------------------*/

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch(const std::exception & e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/*------------------------------------------------------------------*/
