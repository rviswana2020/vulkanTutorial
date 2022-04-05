#include "vulkanValidation.h"

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <unordered_set>

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
// Local Helpers
/*------------------------------------------------------------------*/

template<typename T>
static void
printNames(std::vector<T> & extensions, const char * header) {
    std::cout << INTENT_STR << "Printing " << header << std::endl;
    for(auto name : extensions) {
        std::cout << INTENT_SPACE << INTENT_STR << name << std::endl;
    }
}

/*------------------------------------------------------------------*/

std::vector<std::string>
getSupportedExtensions() {
    uint32_t supportedExtCount = 0;

    vkEnumerateInstanceExtensionProperties(
        nullptr,
        &supportedExtCount,
        nullptr
    );

    assert(supportedExtCount > 0);

    // allocate buffer to hold all names
    std::vector<VkExtensionProperties> supportedExtProperties(supportedExtCount);

    // call again but with buffer to get all properties
    vkEnumerateInstanceExtensionProperties(
        nullptr,
        &supportedExtCount,
        supportedExtProperties.data()
    );

    std::vector<std::string> supportedExtNames;

    for(auto &property : supportedExtProperties) {
        supportedExtNames.emplace_back(property.extensionName);
    }

    #ifndef NDEBUG
        printNames(supportedExtNames, "Supported Extension Names");
    #endif

    return supportedExtNames;
}

/*------------------------------------------------------------------*/

static bool
checkGLFWExtensionSupport(std::vector<const char *> &reqExt,
                          std::vector<std::string> supportedExt) {

    std::unordered_set<std::string> supportedExtSet(supportedExt.begin(), supportedExt.end());
    for(auto ext : reqExt) {
        if(supportedExtSet.count(ext) == 0) {
           return false;
        }
    }
    return true;
}

/*------------------------------------------------------------------*/

static std::vector<std::string>
getSupportedValidationLayerNames() {

    uint32_t layerCnt = 0;

    vkEnumerateInstanceLayerProperties(
                    &layerCnt,
                    nullptr
                    );

    // allocate buffer to hold layer names
    std::vector<VkLayerProperties> availableLayers(layerCnt);

    // call again but with buffer to get all layer names
    vkEnumerateInstanceLayerProperties(
                    &layerCnt,
                    availableLayers.data()
                    );

    std::vector<std::string> layerNames;

    for(auto & property : availableLayers) {
        layerNames.emplace_back(property.layerName);
    }

    #ifndef NDEBUG
        printNames(layerNames, "Available Layer Names");
    #endif
    return layerNames;
}

/*------------------------------------------------------------------*/

static bool
checkValidationLayerSupport(std::vector<const char *> & reqLayers,
                            std::vector<std::string> & availableLayers) {

    std::unordered_set availableLayersSet(
                availableLayers.begin(), availableLayers.end());

    for(auto & reqLayerName : reqLayers) {
        if(availableLayersSet.count(reqLayerName) == 0) {
            return false;
        }
    }

    return true;
}

/*------------------------------------------------------------------*/

static std::vector<const char *>
getRequiredExtensions() {
    uint32_t glfwRequiredExtCount = 0;
    const char ** glfwRequiredExtensions;

    glfwRequiredExtensions = glfwGetRequiredInstanceExtensions(
                                    &glfwRequiredExtCount);

    std::vector<const char *> requiredExtensions(
                                glfwRequiredExtensions,
                                glfwRequiredExtensions + glfwRequiredExtCount);

    assert(glfwRequiredExtCount > 0);

    #ifndef NDEBUG
        printNames(requiredExtensions, "Required Extension Names");
    #endif

    return requiredExtensions;
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

    // get required glfw extensions
    auto requiredExt = getRequiredExtensions();

    // construct validation layer name array
    std::vector<const char *> validationLayer = {
        "VK_LAYER_KHRONOS_validation"
    };

    #ifndef NDEBUG
        auto supportedExt = getSupportedExtensions();
        auto status = checkGLFWExtensionSupport(requiredExt, supportedExt);
        if(!status) {
            throw std::runtime_error("required extensions are not supported");
        }

        auto availableLayerNames = getSupportedValidationLayerNames();

        status = checkValidationLayerSupport(validationLayer, availableLayerNames);

        if(!status) {
            throw std::runtime_error("Validation layers requested, but unavailable");
        }

    #endif

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
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayer.size());

        // nullptr/pointer to array of enabledLayerCount null-terminated
        // UTF-8 strings containing the name of the layers to enable during
        // create instance
        createInfo.ppEnabledLayerNames = validationLayer.data();

        // count of global extentions to enable
        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExt.size());
        // nullptr/pointer to array of enabledLayerCount null-terminated
        // UTF-8 strings containing the name of the extensions to enable during
        // create instance
        createInfo.ppEnabledExtensionNames = requiredExt.data();

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

    // All other vulkan objects should be released before this!!!
    // destroy instance
    vkDestroyInstance(
        instance,       // handle to instance object
        nullptr         // host memory allocator
    );

    // destroy window
    glfwDestroyWindow(window);

    // terminate glfw
    glfwTerminate();
}

/*------------------------------------------------------------------*/
