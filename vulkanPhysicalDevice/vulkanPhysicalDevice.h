#pragma once

#define GLFW_INCLUDE_VULKAN     // enable glfw to include vulkan headers
#include <GLFW/glfw3.h>

/*------------------------------------------------------------------*/
// Hello Triangle Application Class
/*------------------------------------------------------------------*/

class HelloTriangleApplication {

    public:
        // top level run function that
        //      -- initializes all glfw/vulkan objects
        //      -- run rendering of images onto the screen
        //      -- cleansup glfw/vulkan objects
        void run();

    private:
        void initWindow();              // glfw window init code
        void setupDebugMessenger();     // debug messenger setup code
        void createVulkanInstance();    // vulkan instance creation code
        void pickPhysicalDevice();      // vulkan physical device code
        void initVulkan();              // vulkan init code
        void mainLoop();                // main rendering loop
        void cleanup();                 // cleanup/release all glfw/vulkan objects

    private:
        GLFWwindow *window = nullptr;            // screen to render images
        // debug callback handle
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        VkInstance instance = VK_NULL_HANDLE;    // vulkan instance
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;   
};

/*------------------------------------------------------------------*/
