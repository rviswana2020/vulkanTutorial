#include "vulkanInstance.h"

// Window name and dimension
static const uint32_t WIDTH  = 800;
static const uint32_t HEIGHT = 600;
static const char *TITLE     = "vulkan";

/*------------------------------------------------------------------*/
// Public interface definition
/*------------------------------------------------------------------*/

void
HelloTriangleApplication::run() {
   initVulkan();
   mainLoop();
   cleanup();
}

/*------------------------------------------------------------------*/
// Private interface definition
/*------------------------------------------------------------------*/

void
HelloTriangleApplication::initWindow() {
   // initialize glfw
   glfwInit();

   // create glfw with no api
   glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

   // no resizable window
   glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

   // create Window
   window = glfwCreateWindow(
      WIDTH,        // in screen coordinates, must be > 0
      HEIGHT,       // in screen coordinates, must be > 0
      TITLE,        // utf-8 encoded, null terminated string
      nullptr,      // monitor to use for full screen mode
                    // nullptr for windowed mode
      nullptr       // share
                    //   - window whose context to share resources with
                    //   - nullptr -- not to share resources
                    //   - applicable only to opengl context
    );
}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::initVulkan() {
   initWindow();
}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::mainLoop() {
   while(!glfwWindowShouldClose(window)) {
      glfwPollEvents();
   }
}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::cleanup() {
   //destroy glfw window
   glfwDestroyWindow(window);
   window = nullptr;

   // terminate glfw
   glfwTerminate();
}

/*------------------------------------------------------------------*/
