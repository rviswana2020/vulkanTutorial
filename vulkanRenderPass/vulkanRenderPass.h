#pragma once

#define GLFW_INCLUDE_VULKAN     // enable glfw to include vulkan headers
#include <GLFW/glfw3.h>

#include <vector>
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
        void createLogicalDevice();     // vulkan logical device code
        void createSurface();           // vulkan surface creation code
        void createSwapchain();         // vulkan swapchain code
        void createImageViews();
        void createGraphicsPipeline();
        void createRenderPass();
        void initVulkan();              // vulkan init code
        void mainLoop();                // main rendering loop
        void cleanup();                 // cleanup/release all glfw/vulkan objects

    private:
        GLFWwindow *window = nullptr;            // screen to render images
        // debug callback handle
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        VkInstance instance = VK_NULL_HANDLE;    // vulkan instance
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphicsQueue = VK_NULL_HANDLE;  //opaque handle to queue object
        VkQueue presentQueue = VK_NULL_HANDLE;  //opaque handle to queue object
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchainImages;
        VkFormat swapchainImageFormat;
        VkExtent2D swapchainExtent;
        std::vector<VkImageView> swapchainImageViews;
		VkRenderPass renderPass;
        VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;
};

/*------------------------------------------------------------------*/
