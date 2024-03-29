#include "vulkanInstance.h"

#include <stdexcept>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <unordered_set>
#include <string>

// Window name and dimension
static const uint32_t WIDTH  = 800;
static const uint32_t HEIGHT = 600;
static const char *TITLE     = "02_vulkanInstance";

static const char INTENT_SPACE = '\t';
static const char * INTENT_STR = "...";

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
printGLFWExtensions(std::vector<const char *> & reqExtensions) {
    std::cout << "Extension Count: " << reqExtensions.size() << std::endl;

    std::cout << INTENT_STR << "Printing GLFW required Extensions: " << std::endl;
    for(auto curReqExt : reqExtensions) {
       std::cout << INTENT_SPACE << INTENT_STR << curReqExt << std::endl;
    }
}

/*------------------------------------------------------------------*/

static void
checkGLFWExtensionSupport(std::vector<const char *> & reqExtensions) {

    // query vulkan for supported extensions
    uint32_t supportedExtensionCnt = 0;
    vkEnumerateInstanceExtensionProperties(
       nullptr,
       &supportedExtensionCnt,
       nullptr
    );

    assert(supportedExtensionCnt > 0);

    // allocate buffer to hold all the names
    std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCnt);

    // call again with buffer
    vkEnumerateInstanceExtensionProperties(
       nullptr,
       &supportedExtensionCnt,
       supportedExtensions.data()
    );

    // Check required glfw Extensions in vulkan supported extensions
    std::unordered_set<std::string> supportedExtNameSet;
    for(auto ext : supportedExtensions) {
       supportedExtNameSet.emplace(ext.extensionName);
    }

    // print all names
    std::cout << INTENT_STR << "vulkan Supported Extensions: "<< std::endl;
    std::for_each(
       std::begin(supportedExtNameSet),
       std::end(supportedExtNameSet),
       [](auto name){
          std::cout << INTENT_SPACE << INTENT_STR
                    << name << std::endl;
            });

    for(auto curReqExt : reqExtensions) {
       if(supportedExtNameSet.count(curReqExt) == 0) {
          throw std::runtime_error("unsupported glfw extensions");
        }
    }
};

/*------------------------------------------------------------------*/

static std::vector<const char *>
getRequiredExtensions() {
    uint32_t glfwRequiredExtCnt = 0;
    const char ** glfwRequiredExt;

    glfwRequiredExt = glfwGetRequiredInstanceExtensions(&glfwRequiredExtCnt);

    std::vector<const char *> requiredExtensions(glfwRequiredExt,
                                         glfwRequiredExt + glfwRequiredExtCnt);
    #ifndef NDEBUG
        printGLFWExtensions(requiredExtensions);
        checkGLFWExtensionSupport(requiredExtensions);
    #endif

    return requiredExtensions;
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
HelloTriangleApplication::createVulkanInstance() {
    // Info about application -- optional
    VkApplicationInfo appInfo {};

        // type of the structure
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

        // nullptr/pointer to structure extending this structure
        appInfo.pNext = nullptr;

        // Null terminated UTF-8 developer supplied application name or nullptr
        appInfo.pApplicationName = "Hello Triangle";

        // unsigned int value for developer supplied application version
        // Version has major(7), minor(10), and patch(12) fields
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

        // Null terminated UTF-8 engine name or nullptr
        appInfo.pEngineName = "No Engine";

        // unsigned int value for developer supplied engine version
        // Version has major(7), minor(10), and patch(12) fields
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

        // vulkan version -- highest version of vulkan that app is designed to use
        // Version has variant(3), major(7), minor(10), and patch(12) fields
        // calls VK_MAKE_API_VERSION(0, 1, 0, 0)
        appInfo.apiVersion = VK_API_VERSION_1_0;

    // get glfw's extension names needed to interface with window system
    auto requiredExtensions = getRequiredExtensions();

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
        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());

        // nullptr/pointer to array of enabledExtensionCount null-terminated
        // UTF-8 strings containing the names of extension to be enabled
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

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
   createVulkanInstance();
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
