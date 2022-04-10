#include "vulkanPipeline.h"

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <unordered_set>
#include <set>
#include <algorithm>

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
HelloTriangleApplication::initVulkan() {
    initWindow();
    createVulkanInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapchain();
    createImageViews();
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
