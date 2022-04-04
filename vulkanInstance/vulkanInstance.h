#define GLFW_INCLUDE_VULKAN     // enble glfw to include vulkan header
#include <GLFW/glfw3.h>

/*------------------------------------------------------------------*/
// Hello Triangle class
/*------------------------------------------------------------------*/

class HelloTriangleApplication {
    public:
        // top level run function that
        //    -- initializes all glfw/vulkan objects
        //    -- run rendering
        //    -- cleanup objects
        void run();

    private:
        void initWindow();            // glfw window init code
        void createVkInstance();      // vulkan instance code
        void initVulkan();            // vulkan init code
        void mainLoop();              // main rendering loop
        void cleanup();               // cleanup/release all glfw/vulkan objects

    private:
        GLFWwindow *window = nullptr; // screen to render images
        VkInstance instance;          // vulkan instance
};

/*------------------------------------------------------------------*/
