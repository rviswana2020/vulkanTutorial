#include "vulkanPhysicalDevice.h"

#include <iostream>     // console reporting
#include <stdexcept>    // error handling
#include <cstdlib>      // EXIT macro definitions

/*------------------------------------------------------------------*/

int
main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch(const std::exception & e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/*------------------------------------------------------------------*/
