#include "vulkanLogicalDevice.h"

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
static const uint32_t HEIGHT    = 600;
static const char * TITLE       = "Hello Triangle Application";

    // construct validation layer name array
    std::vector<const char *> validationLayer = {
        "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

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

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
               VkDebugUtilsMessageTypeFlagsEXT msgType,
               const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
               void *pUserData) {
    std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

/*------------------------------------------------------------------*/

static void
populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT & createInfo) {
    createInfo = {};

    // structure type
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    // nullptr/pointer to a structure extending this structure
    createInfo.pNext = nullptr;

    // flags is 0 and reserved for future use
    createInfo.flags = 0;

    // specify the severity of events will cause this callback to be called
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    // specify the type of events that will call this callback to be called
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    // register callback that will be called
    createInfo.pfnUserCallback = debugCallback;

}

/*------------------------------------------------------------------*/

static VkResult
createDebugUtilsMessengerEXT(
        VkInstance &instance,
        const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
        const VkAllocationCallbacks *pAllocator,
        VkDebugUtilsMessengerEXT *pDebugMessenger) {

    VkResult result = VK_ERROR_EXTENSION_NOT_PRESENT;

    // get proc address of vkCreateDebugUtilsMessengerEXT and invoke
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
                        instance,
                        "vkCreateDebugUtilsMessengerEXT");

    if(func != nullptr) {
        result = func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }

    return result;
}

/*------------------------------------------------------------------*/

static void
destroyDebugUtilsMessengerEXT(
        VkInstance &instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks *pAllocator) {

    // get proc address of vkCreateDebugUtilsMessengerEXT and invoke
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
                        instance,
                        "vkDestroyDebugUtilsMessengerEXT");

    if(func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }

}

/*------------------------------------------------------------------*/

static std::vector<std::string>
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

    // add optional message callback
    if(enableValidationLayers) {
        requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        printNames(requiredExtensions, "Required Extension Names");
    }

    return requiredExtensions;
}

/*------------------------------------------------------------------*/

static bool
isDeviceSuitable(VkPhysicalDevice & device) {
    return true;
}

/*------------------------------------------------------------------*/
static void printDeviceSpecification(VkPhysicalDevice &device) {
    VkPhysicalDeviceProperties deviceProperties;

    //get all device properties
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;

    // print them
    std::cout << INTENT_STR << "Current Device Property: " << std::endl;
    std::cout << INTENT_SPACE << INTENT_STR
              << "Device Name: " << deviceProperties.deviceName << std::endl;
    std::cout << INTENT_SPACE << INTENT_STR
              << "Device VendorID: " << deviceProperties.vendorID << std::endl;
    std::cout << INTENT_SPACE << INTENT_STR
              << "Device Type: " << deviceProperties.deviceType << std::endl;

    // get all device features
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    // print them
    std::cout << INTENT_STR << "Current Device Features: " << std::endl;
    std::cout << INTENT_SPACE << INTENT_STR
              << "Geometry Shader :" << deviceFeatures.geometryShader << std::endl;

    std::cout << INTENT_SPACE << INTENT_STR
              << "Shader Float64 :" << deviceFeatures.shaderFloat64 << std::endl;

    std::cout << INTENT_SPACE << INTENT_STR
              << "Shader Int64 :" << deviceFeatures.shaderInt64 << std::endl;
}

/*------------------------------------------------------------------*/

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
HelloTriangleApplication::setupDebugMessenger() {
    if(!enableValidationLayers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo {};

    populateDebugMessengerCreateInfo(createInfo);
    VkResult result = createDebugUtilsMessengerEXT(instance,
                                                   &createInfo,
                                                   nullptr,
                                                   &debugMessenger);

    if(result != VK_SUCCESS) {
        std::cout << "debug Messenger callback error: " << result << std::endl;
        throw std::runtime_error("cannot create debug utils callback");
    }
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

    if(enableValidationLayers) {
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

    }

    // create Instance Info structure
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
    VkInstanceCreateInfo createInfo {};

        // structure type
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

        // nullptr/pointer to structure extending this structure
        createInfo.pNext = nullptr;

        // flags indicating the behavior of the instance
        createInfo.flags = 0;

        // nullptr/pointer to VkApplicationInfo object
        createInfo.pApplicationInfo = &appInfo;

        if(enableValidationLayers) {
            //count of global layers to enable
               // here "global" applies to the entire application
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayer.size());

            // nullptr/pointer to array of enabledLayerCount null-terminated
            // UTF-8 strings containing the name of the layers to enable during
            // create instance
            createInfo.ppEnabledLayerNames = validationLayer.data();
            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
        }
        else {
            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;
        }

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
HelloTriangleApplication::pickPhysicalDevice() {
    // enumerate supported physical system
    uint32_t deviceCnt = 0;

    // invoke vkEnumeratePhysicalDevice to get the count
    vkEnumeratePhysicalDevices(instance, &deviceCnt, nullptr);

    if(deviceCnt == 0) {
        throw std::runtime_error("failed to find GPUs with vulkan support!");
    }

    // allocate memory
    std::vector<VkPhysicalDevice> devices(deviceCnt);

    // get the device list
    vkEnumeratePhysicalDevices(instance, &deviceCnt, devices.data());

    for(auto & device : devices) {
        #ifndef NDEBUG
            printDeviceSpecification(device);
        #endif

        if(isDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }

    if(physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("no suitable GPU found");
    }
}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::initVulkan() {
    initWindow();
    createVulkanInstance();
    setupDebugMessenger();
    pickPhysicalDevice();
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

    // destroy debug Messenger callback
    if(enableValidationLayers) {
        destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

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
