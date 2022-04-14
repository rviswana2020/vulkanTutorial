#include "vulkanDraw.h"

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <fstream>

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

// device extension container
std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
    const bool enableValidationLayers= false;
#else
    const bool enableValidationLayers = true;
#endif


/*------------------------------------------------------------------*/
// Classes
/*------------------------------------------------------------------*/

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};


struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

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
checkDeviceExtSupported(VkPhysicalDevice & device) {
    // extension count
    uint32_t extCnt = 0;

    // enumerate Device extension properties
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCnt, nullptr);

    assert(extCnt > 0);

    // allocate buffer to hold extension names
    std::vector<VkExtensionProperties> availableExts(extCnt);

    // enumerate Device extension properties
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCnt,
                                        availableExts.data());

    std::set<std::string> availableExtsSet;
    for(auto e : availableExts) {
        availableExtsSet.insert(e.extensionName);
    }

    #ifndef NDEBUG
        for_each(availableExts.begin(), availableExts.end(),
                 [](auto &e) {
                    std::cout << INTENT_SPACE << INTENT_STR
                              << e.extensionName << std::endl;
                             });
    #endif

    for(auto & e : deviceExtensions) {
        if(availableExtsSet.count(e) == 0)
            return false;
     }

    return true;
}

/*------------------------------------------------------------------*/

static QueueFamilyIndices
findQueueFamilies(VkPhysicalDevice &device, VkSurfaceKHR &surface) {
    QueueFamilyIndices indices;

    // let us get Queue family properties
    uint32_t qFamilyCnt = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &qFamilyCnt, nullptr);

    if(qFamilyCnt == 0) {
        throw std::runtime_error("No queue family properties found");
    }

    // allocate buffer to hold queue family properties
    std::vector<VkQueueFamilyProperties> qFamilies(qFamilyCnt);

    vkGetPhysicalDeviceQueueFamilyProperties(device, &qFamilyCnt, qFamilies.data());

    // Let us iterate over queue families and check if it works for our needs

    for(size_t i = 0; i < qFamilies.size(); ++i) {
        if(qFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        // find presentation family
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i , surface, &presentSupport);

        if(presentSupport) {
            indices.presentFamily = i;
        }

        if(indices.isComplete()) {
            #ifndef NDEBUG
                std::cout << "graphics family: " << indices.graphicsFamily.value()
                          << std::endl;

                std::cout << "present family: " << indices.presentFamily.value()
                          << std::endl;
            #endif

            break;
        }
    }

    return indices;
}

/*------------------------------------------------------------------*/

static
SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice &device,
                                              VkSurfaceKHR &surface) {
    //most swapchain queries take device and surface as first two parameters
    // let us query capabilities

    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,
                                             surface,
                                             &details.capabilities);

    #ifndef NDEBUG
        std::cout << INTENT_STR << "surface capabilities" << std::endl;

        std::cout << INTENT_SPACE << INTENT_STR << "minImageCount: "
                  << details.capabilities.minImageCount << std::endl;

        std::cout << INTENT_SPACE << INTENT_STR << "maxImageCount: "
                  << details.capabilities.maxImageCount << std::endl;
    #endif

    // query for supported surface formats
    uint32_t formatCnt;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCnt, nullptr);

    if(formatCnt != 0) {
        details.formats.resize(formatCnt);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCnt,
                                            details.formats.data());
    }

    // query for present mode
    uint32_t presentModeCnt;

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCnt,
                                              nullptr);

    if(presentModeCnt != 0) {
        details.presentModes.resize(presentModeCnt);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                 &presentModeCnt, details.presentModes.data());
    }

    return details;
}

/*------------------------------------------------------------------*/

static bool
isDeviceSuitable(VkPhysicalDevice & device, VkSurfaceKHR &surface) {
    QueueFamilyIndices indices = findQueueFamilies(device, surface);

    bool extsSupported = checkDeviceExtSupported(device);

    bool swapchainAdequate = false;
    if(extsSupported) {
        SwapchainSupportDetails swapchainSupport = querySwapchainSupport(device,
                                                                         surface);
        swapchainAdequate = !swapchainSupport.formats.empty() &&
                            !swapchainSupport.presentModes.empty();
    }

    return indices.isComplete() && extsSupported && swapchainAdequate;
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

static VkSurfaceFormatKHR
chooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for( const auto & curAvailableFormat : availableFormats) {
        if(curAvailableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
           curAvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return curAvailableFormat;
        }
    }

    return availableFormats[0];
}

/*------------------------------------------------------------------*/

static VkPresentModeKHR
chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for(const auto & availablePresentMode : availablePresentModes) {
        if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return availablePresentMode;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

/*------------------------------------------------------------------*/

static VkExtent2D
chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow* window) {
    auto &curExtent = capabilities.currentExtent;
    if(curExtent.width != std::numeric_limits<uint32_t>::max()) {
        return curExtent;
    }
    else {
        int width = 0;
        int height = 0;

        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width,
                                       capabilities.minImageExtent.width,
                                       capabilities.maxImageExtent.width);

        actualExtent.height = std::clamp(actualExtent.height,
                                       capabilities.minImageExtent.height,
                                       capabilities.maxImageExtent.width);

        return actualExtent;
    }
}

/*------------------------------------------------------------------*/

static std::vector<char>
readFile(const std::string &filename) {
    // std::ios::ate -> start reading at the end of the file
    // std::ios::binary - read the file as binary file
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    // check if the file can be opened
    if(!file.is_open()) {
        throw std::runtime_error("failed to open shader file!!!");
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    // move to front and read entire content
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    //close the file
    file.close();

    assert(fileSize == buffer.size());

    return buffer;
}

/*------------------------------------------------------------------*/

static VkShaderModule
createShaderModule(VkDevice &device, const std::vector<char> &code) {
    // wrap the shader code in a VkShaderModule object

    // NOTE: the size of the bytecode is specified in bytes
    // bytecode pointer is a uint32_t pointer rather than char.
    // so use reinterpret_cast

    VkShaderModuleCreateInfo createInfo {};

        //structure type
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

        // shader code size and buffer pointer
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    // create shader module
    VkShaderModule shaderModule = VK_NULL_HANDLE;

    VkResult result = vkCreateShaderModule(device, &createInfo, nullptr,
                                           &shaderModule);

    if(result != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
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
HelloTriangleApplication::createSurface() {
    VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    if(result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
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

        if(isDeviceSuitable(device, surface)) {
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
HelloTriangleApplication::createLogicalDevice() {

    // find the queue family
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

    // queue priority value
    float queuePriority = 1.0f;

    // Populate VkDeviceQueueCreateInfo structure
    std::vector<VkDeviceQueueCreateInfo> qCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                              indices.presentFamily.value()
                                             };

    for(uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo qCreateInfo {};
            // structure type
           qCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;

            // nullpr/pointer to structure that extends this structure
            qCreateInfo.pNext = nullptr;

            // flags indicating the behavior of the queue
            qCreateInfo.flags = 0;

            // queue family Index
            qCreateInfo.queueFamilyIndex = queueFamily;

            // how many queues to create in the queue family
            qCreateInfo.queueCount = 1;

            // normalized floating point values, specifying priorities of
            // work to be submitted to each created queue
            qCreateInfo.pQueuePriorities = &queuePriority;
            qCreateInfos.emplace_back(qCreateInfo);
    }

    // Specifying about device features
    // for now, just leave it at the default initialized state
    VkPhysicalDeviceFeatures deviceFeatures {};

    // create logical device
    VkDeviceCreateInfo createInfo {};

        // structure type
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        // nullptr/pointer to structure that extends this structure
        createInfo.pNext = nullptr;

        // flags reserved for future use
        createInfo.flags = 0;

        // count of VkDeviceQueueCreateInfo
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(qCreateInfos.size());

        // pointer to arrays of VkDevieQueueCreateInfo structures describing
        // the queues that are requested to be created along with the
        // logical device.
        createInfo.pQueueCreateInfos = qCreateInfos.data();

        if(enableValidationLayers) {
            // global validation layer count -- decrecated
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayer   .size());

            // global validation layer names -- deprecated
            createInfo.ppEnabledLayerNames = validationLayer.data();
        }
        else  {
            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;
        }

        // global extension count -- deprecated
        createInfo.enabledExtensionCount = static_cast<uint32_t>(
                    deviceExtensions.size());

        // global extension names -- deprecated
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        // strucure containing boolean indicators of all of the features
        // to be enabled
        createInfo.pEnabledFeatures = &deviceFeatures;

        VkResult result = vkCreateDevice(physicalDevice, &createInfo,
                                         nullptr, &device);

        if(result != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        // get created queue handles
        vkGetDeviceQueue(device, indices.graphicsFamily.value(),
                         0, &graphicsQueue);

        vkGetDeviceQueue(device, indices.presentFamily.value(),
                         0, &presentQueue);
}

/*------------------------------------------------------------------*/
void
HelloTriangleApplication::createSwapchain() {
    //get swap chain support
    auto swapchainSupport = querySwapchainSupport(physicalDevice, surface);

    auto surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
    auto presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);
    auto extent = chooseSwapExtent(swapchainSupport.capabilities, window);

    uint32_t imageCnt = swapchainSupport.capabilities.minImageCount + 1;

    if(swapchainSupport.capabilities.maxImageCount > 0 &&
        imageCnt > swapchainSupport.capabilities.maxImageCount) {
        imageCnt = swapchainSupport.capabilities.maxImageCount;
    }

    // get indices
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                     indices.presentFamily.value()};

    // create VkSwapchainCreateInfoKHR
    VkSwapchainCreateInfoKHR createInfo {};

        // structure type
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

        // surface
        createInfo.surface = surface;
        createInfo.minImageCount = imageCnt;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        if(indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else  {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(device, &createInfo,
                                           nullptr, &swapchain);
    if( result != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(device, swapchain, &imageCnt, nullptr);
    swapchainImages.resize(imageCnt);

    vkGetSwapchainImagesKHR(device, swapchain, &imageCnt, swapchainImages.data());

    swapchainImageFormat = surfaceFormat.format;
    swapchainExtent = extent;
}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::createImageViews() {
    swapchainImageViews.resize(swapchainImages.size());

    for(size_t i = 0; i < swapchainImages.size(); ++i) {
        //create VkImageViewCreateInfo
        VkImageViewCreateInfo createInfo {};

            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapchainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapchainImageFormat;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

        VkResult result = vkCreateImageView(device, &createInfo, nullptr,
                                            &swapchainImageViews[i]);

        if(result != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::createGraphicsPipeline() {

    // vertex and fragment shader code
    auto vertShaderCode = readFile("shaders/vert.spv");
    auto fragShaderCode = readFile("shaders/frag.spv");

    // create shader module
    VkShaderModule vertShaderModule = createShaderModule(device, vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(device, fragShaderCode);

    //assign shaders to pipeline stages
    VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo {};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {
                                            vertShaderStageInfo,
                                            fragShaderStageInfo };

    // Fixed pipeline stage setup
    // vertex input stage - describes the format of the vertex data
    // specify Bindings and Attrinute Descriptions

    VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;

        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;

        // Input Assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Viewport stage setting
        VkViewport viewport {};

            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float) swapchainExtent.width;
            viewport.height = (float) swapchainExtent.height;

            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

        // scissor setting
        VkRect2D scissor {};
            scissor.offset = {0, 0};
            scissor.extent = swapchainExtent;


        // pipeline viewport state
        VkPipelineViewportStateCreateInfo viewportState {};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.pViewports = &viewport;
            viewportState.scissorCount = 1;
            viewportState.pScissors = &scissor;

        // Rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizer {};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;

            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth = 1.0f;

            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

            rasterizer.depthBiasEnable = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f;
            rasterizer.depthBiasClamp = 0.0f;
            rasterizer.depthBiasSlopeFactor = 0.0f;

        // Multisampling
        VkPipelineMultisampleStateCreateInfo multisampling {};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            multisampling.minSampleShading = 1.0f;
            multisampling.pSampleMask = nullptr;
            multisampling.alphaToCoverageEnable = VK_FALSE;
            multisampling.alphaToOneEnable = VK_FALSE;


        // Color blend per attachment
        VkPipelineColorBlendAttachmentState colorBlendAttachment {};
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                                  VK_COLOR_COMPONENT_G_BIT |
                                                  VK_COLOR_COMPONENT_B_BIT |
                                                  VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        // Global colorBlend state
        VkPipelineColorBlendStateCreateInfo colorBlending {};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY;
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f;
            colorBlending.blendConstants[1] = 0.0f;
            colorBlending.blendConstants[2] = 0.0f;
            colorBlending.blendConstants[3] = 0.0f;

        // Dynamcic state configuration
        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
        };

        VkPipelineDynamicStateCreateInfo dynamicState {};
            dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
            dynamicState.pDynamicStates = dynamicStates.data();

        // pipeline layout setup
        VkPipelineLayoutCreateInfo pipelineLayoutInfo {};

            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 0;
            pipelineLayoutInfo.pSetLayouts = nullptr;
            pipelineLayoutInfo.pushConstantRangeCount = 0;
            pipelineLayoutInfo.pPushConstantRanges = nullptr;

        VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutInfo,
                                                 nullptr, &pipelineLayout);

        if(result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }

    VkGraphicsPipelineCreateInfo pipelineInfo {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = nullptr;

        pipelineInfo.layout = pipelineLayout;

        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1,
                                           &pipelineInfo, nullptr,
                                           &graphicsPipeline);

   if(result != VK_SUCCESS) {
      throw std::runtime_error("failed to create graphics pipeline!!!");
   }

    // destroy shader module
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);

}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::createRenderPass() {
    VkAttachmentDescription colorAttachment {};

        colorAttachment.format = swapchainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

    // dependencies
    VkSubpassDependency dependency {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;

        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;

        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;


    VkResult result = vkCreateRenderPass(device, &renderPassInfo, nullptr,
                                         &renderPass);

    if(result != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!!!");
    }
}

/*------------------------------------------------------------------*/
void
HelloTriangleApplication::createFramebuffers() {
    swapchainFramebuffers.resize(swapchainImageViews.size());

    for(size_t i = 0; i < swapchainImageViews.size(); ++i) {
        VkImageView attachments[] = {
            swapchainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapchainExtent.width;
        framebufferInfo.height = swapchainExtent.height;
        framebufferInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(device, &framebufferInfo, nullptr,
                                              &swapchainFramebuffers[i]);

        if(result != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::createCommandPool() {
    QueueFamilyIndices qFamilyIndices = findQueueFamilies(physicalDevice, surface);

    VkCommandPoolCreateInfo poolInfo {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = qFamilyIndices.graphicsFamily.value();

        VkResult result = vkCreateCommandPool(device, &poolInfo, nullptr,
                                              &commandPool);

        if(result != VK_SUCCESS) {
            throw  std::runtime_error("failed to create command pool!");
        }
}

/*------------------------------------------------------------------*/
void
HelloTriangleApplication::createCommandBuffer() {
    VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VkResult result = vkAllocateCommandBuffers(device, &allocInfo,
                                                   &commandBuffer);

    if(result != VK_SUCCESS) {
       throw std::runtime_error("failed to allocate command buffers!");
    }
}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::recordCommandBuffer(VkCommandBuffer commandBuffer,
                                              uint32_t imageIdx) {
    VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

    VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);

    if(result != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    // start the render pass
    VkRenderPassBeginInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapchainFramebuffers[imageIdx];

    renderPassInfo.renderArea.offset = {0,0};
    renderPassInfo.renderArea.extent = swapchainExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    //Basic draw commands
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    result = vkEndCommandBuffer(commandBuffer);

    if(result != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkResult result = vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                                        &imageAvailableSemaphore);
    if(result != VK_SUCCESS) {
        throw std::runtime_error("failed to create image available semaphore");
    }

    result = vkCreateSemaphore(device, &semaphoreInfo, nullptr,
                                        &renderFinishedSemaphore);
    if(result != VK_SUCCESS) {
        throw std::runtime_error("failed to create render finished semaphore");
    }

    result = vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence);
    if(result != VK_SUCCESS) {
        throw std::runtime_error("failed to create in flight fence");
    }
}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::initVulkan() {
    initWindow();
    createVulkanInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapchain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createCommandBuffer();
    createSyncObjects();
}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::drawFrame() {
    // wait for the previous frame to finish
        vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

    // above is the blocking call.  Once done, we need manual reset
        vkResetFences(device, 1, &inFlightFence);

   // acquire image from swap chain
    uint32_t imageIdx;
    vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphore,
                          VK_NULL_HANDLE, &imageIdx);

    // record the command buffer
    vkResetCommandBuffer(commandBuffer, 0);
    recordCommandBuffer(commandBuffer, imageIdx);

    // Submitting the command buffer
    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {
                        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                                        };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VkResult result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence);

    if(result != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // presentation
    VkPresentInfoKHR presentInfo {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapchains[] = {swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &imageIdx;

        presentInfo.pResults = nullptr;

    vkQueuePresentKHR(presentQueue, &presentInfo);
}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::mainLoop() {
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame();
       }

        vkDeviceWaitIdle(device);
}

/*------------------------------------------------------------------*/

void
HelloTriangleApplication::cleanup() {
    // destroy semaphore and fences
    vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
    vkDestroyFence(device, inFlightFence, nullptr);

    // destroy commandpool
    vkDestroyCommandPool(device, commandPool, nullptr);

    // destroy framebuffers
    for(auto framebuffer : swapchainFramebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    // destroy pipeline
    vkDestroyPipeline(device, graphicsPipeline, nullptr);

    // destroy pipeline layout
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

    // destroy render pass
    vkDestroyRenderPass(device, renderPass, nullptr);

    // destroy image view
    for(auto imageView : swapchainImageViews) {
       vkDestroyImageView(device, imageView, nullptr);
    }

    // destroy swapchain
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    // Destroy logical device
    vkDestroyDevice(device, nullptr);

    // destroy debug Messenger callback
    if(enableValidationLayers) {
        destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroySurfaceKHR(instance, surface, nullptr);

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
