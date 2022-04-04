#include "vulkanInstance.h"

#include <stdexcept>
#include <iostream>

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
// Local helpers
/*------------------------------------------------------------------*/
static void
printGLFWExtensions(const char **glfwExtensions,
                    uint32_t glfwExtensionCount) {
    std::cout << "Extension Count: " << glfwExtensionCount << std::endl;

    for(int i = 0; i < glfwExtensionCount; ++i) {
        std::cout << glfwExtensions[i] << std::endl;
    }
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
// instance connects application to vulkan library
// Involves specifying
//   - Info about application to enable driver level optimization
//     specific to application
//   - Additional extention and use of any validation layer

void
HelloTriangleApplication::createVkInstance() {
    // Info about application -- optional
    VkApplicationInfo appInfo {};

        // type of the structure
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

        // nullptr/pointer to structure extending this structure
        appInfo.pNext = nullptr;

        // Null terminated UTF-8 application name or nullptr
        appInfo.pApplicationName = "Hello Triangle";

        // unsigned int value for driver supplied application version
        // Version has major(7), minor(10), and patch(12) fields
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

        // Null terminated UTF-8 engine name or nullptr
        appInfo.pEngineName = "No Engine";

        // unsigned int value for driver supplied engine version
        // Version has major(7), minor(10), and patch(12) fields
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

        // vulkan version -- highest version of vulkan that app is designed to use
        // Version has variant(3), major(7), minor(10), and patch(12) fields
        // calls VK_MAKE_API_VERSION(0, 1, 0, 0)
        appInfo.apiVersion = VK_API_VERSION_1_0;

    // get glfw's extension names needed to interface with window system
    uint32_t glfwExtensionCount = 0;
    const char ** glfwExtensions = nullptr;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    printGLFWExtensions(glfwExtensions, glfwExtensionCount);

    // Create Instance Info structure
    VkInstanceCreateInfo createInfo {};

        // type of the structure
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

        // nullptr/pointer to structure extending this structure
        createInfo.pNext = nullptr;

        // flags indicating the behavior of the instance
        createInfo.flags = 0;

        // nullptr/pointer to VkApplicationInfo object
        createInfo.pApplicationInfo = &appInfo;

        // count of global layers to enable
        // here the word "global" applies to the entire applcation
        createInfo.enabledLayerCount = 0;

        // nullptr/pointer to array of enabledLayerCount null-terminated
        // UTF-8 strings containing the name of the layers to enable for
        // create instance.
        // Layers loaded in the order they are listed in this array
        // First array element being closest to application
        // Last array element being closest to the driver

        createInfo.ppEnabledLayerNames = nullptr;

        // count of global extensions to enable
        createInfo.enabledExtensionCount = glfwExtensionCount;

        // nullptr/pointer to array of enabledExtensionCount null-terminated
        // UTF-8 strings containing the names of extension to be enabled
        createInfo.ppEnabledExtensionNames = glfwExtensions;

    //now invoke api to create Vk Instance
    VkResult result = vkCreateInstance(
                         &createInfo,    // pointer to structure with create info
                         nullptr,        // custom allocator callback
                         &instance       // pointer to variable that store
                                         // handle to vk create instance object
                      );

    if(result != VK_SUCCESS) {
        std::cout << "create Instance return value: " << result << std::endl;
        throw std::runtime_error("failed to create instance!");
    }
}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::initVulkan() {
   initWindow();
   createVkInstance();
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

   // all other vulkan object should be destroyed before this!!!
   // destroy vk instance
   vkDestroyInstance(
      instance,   // handle to vk instance object
      nullptr     // custom host memory allocator
   );

   //destroy glfw window
   glfwDestroyWindow(window);
   window = nullptr;

   // terminate glfw
   glfwTerminate();
}

/*------------------------------------------------------------------*/
