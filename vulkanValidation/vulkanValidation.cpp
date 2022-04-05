#include "vulkanValidation.h"

#include <iostream>

/*------------------------------------------------------------------*/
// Constants
/*------------------------------------------------------------------*/

static const char INTENT_SPACE     = '\t';
static const char * INTENT_STR     = "...";

static const uint32_t WIDTH     = 800;
static const uint32_t HEIGHT     = 600;
static const char * TITLE        = "Hello Triangle Application";

/*------------------------------------------------------------------*/
// Public inferface definitions
/*------------------------------------------------------------------*/

void
HelloTriangleApplication::run() {
    initVulkan();
    mainLoop();
    cleanup();
}

/*------------------------------------------------------------------*/
// Private inferface definitions
/*------------------------------------------------------------------*/

void
HelloTriangleApplication::initWindow() {
    // initialize glfw
    glfwInit();

    // create glfw with no api
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // no resizable window
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // create window
    window = glfwCreateWindow(
                    WIDTH,      // in screen coordinates, must be  > 0
                    HEIGHT,     // in screen coordinates, must be  > 0
                    TITLE,      // nullptr/utf-8 null terminated string
                    nullptr,    // monitor to use full screen mode
                                // nullptr for windowed mode
                    nullptr     // share
                                // - window whose context to share resource with
                                // - nullptr -- not to share resource
                                // - applicable only to opengl context
             );
}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::createVulkanInstance() {
    // Info about application -- ooptional
    VkApplicationInfo appInfo {};

        // structure type
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

        // nullptr/pointer to structure extending this structure
        appInfo.pNext = nullptr;

        // nullptr/null terminated UTF-8 developer supplied application name
        appInfo.pApplicationName = "Hello Triangle";

        // unsigned int value for developer supplied application version
        // Version has major(7), minor(10), patch(12) fields
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

        // nulltptr/ null terminated UTF-8 developer supplied engine name
        appInfo.pEngineName = "No Engine";

        // unsigned int value for developer supplied engine version
        // Version has major(7), minor(10), patch(12) fields
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

        // vulkan api version -- highest version of vulkan api that app is
        // designed to support.
        // version has Variant(3), major(7), minor(10), and patch(12) fields
        // calls VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

    // create Instance Info structure
    VkInstanceCreateInfo createInfo {};

        // structure type
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

        // nullptr/pointer to structure extending this structure
        createInfo.pNext = nullptr;

        // flags indicating the behavior of the instance
        createInfo.flags = 0;

        // nullptr/pointer to VkApplicationInfo object
        createInfo.pApplicationInfo = &appInfo;

        //count of global layers to enable
        // here "global" applies to the entire application
        createInfo.enabledLayerCount = 0;

        // nullptr/pointer to array of enabledLayerCount null-terminated
        // UTF-8 strings containing the name of the layers to enable during
        // create instance
        createInfo.ppEnabledLayerNames = nullptr;

        // count of global extentions to enable
        createInfo.enabledExtensionCount = 0;

        // nullptr/pointer to array of enabledLayerCount null-terminated
        // UTF-8 strings containing the name of the extensions to enable during
        // create instance
        createInfo.ppEnabledExtensionNames = nullptr;

    // invoke api to create vulkan instance
    VkResult result = vkCreateInstance(
                        &createInfo,
                        nullptr,
                        &instance
                      );

    if(result != VK_SUCCESS) {
        std::cerr << "create instance return value: " << result << std::endl;
        throw std::runtime_error("failed to create instance!!!");
    }
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
    // destroy window
    glfwDestroyWindow(window);

    // terminate glfw
    glfwTerminate();
}

/*------------------------------------------------------------------*/
