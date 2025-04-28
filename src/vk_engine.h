// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include "vk_descriptors.h"

#include <vk_types.h>

struct DeletionQueue {
  std::deque<std::function<void()>> deletors;

  void push_function(std::function<void()>&& function) {
    deletors.push_back(function);
  }

  void flush() {
    // reverse iterate the deletion queue to execute all the functions
    for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
      (*it)();  // call functors
    }

    deletors.clear();
  }
};

struct FrameData {
  VkCommandPool _commandPool;
  VkCommandBuffer _mainCommandBuffer;
  VkSemaphore _swapchainSemaphore, _renderSemaphore;
  VkFence _renderFence;
  DeletionQueue _deletionQueue;
};
struct ComputePushConstants {
  glm::vec4 data1;
  glm::vec4 data2;
  glm::vec4 data3;
  glm::vec4 data4;
};
struct ComputeEffect {
  const char* name;

  VkPipeline pipeline;
  VkPipelineLayout layout;

  ComputePushConstants data;
};

constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanEngine {
 public:
  // initializes everything in the engine
  void init();

  // shuts down the engine
  void cleanup();
  void draw_background(VkCommandBuffer cmd);

  void draw_geometry(VkCommandBuffer cmd);
  // draw loop
  void draw();

  // run main loop
  void run();
  void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);

  FrameData _frames[FRAME_OVERLAP];

  FrameData& get_current_frame();

  VkQueue _graphicsQueue;
  uint32_t _graphicsQueueFamily;

  VkInstance _instance;                       // Vulkan library handle
  VkDebugUtilsMessengerEXT _debug_messenger;  // Vulkan debug output handle
  VkPhysicalDevice _chosenGPU;  // GPU chosen as the default device
  VkDevice _device;             // Vulkan device for commands
  VkSurfaceKHR _surface;        // Vulkan window surface
  VkExtent2D _windowExtent{1028, 512};

  bool _isInitialized{false};
  int _frameNumber{0};
  bool stop_rendering{false};

  struct SDL_Window* _window{nullptr};
  bool bUseValidationLayers = true;

  static VulkanEngine& Get();
  // swapchain
  VkSwapchainKHR _swapchain;
  VkFormat _swapchainImageFormat;

  std::vector<VkImage> _swapchainImages;
  std::vector<VkImageView> _swapchainImageViews;
  VkExtent2D _swapchainExtent;

  DescriptorAllocator globalDescriptorAllocator;

  VkDescriptorSet _drawImageDescriptors;
  VkDescriptorSetLayout _drawImageDescriptorLayout;

  VkPipeline _gradientPipeline;
  VkPipelineLayout _gradientPipelineLayout;
  // immediate submit structures
  VkFence _immFence;
  VkCommandBuffer _immCommandBuffer;
  VkCommandPool _immCommandPool;


  void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);
 private:
  void init_imgui();

  VkPipelineLayout _trianglePipelineLayout;
  VkPipeline _trianglePipeline;

  VmaAllocator _allocator;
  DeletionQueue _mainDeletionQueue;

  AllocatedImage _drawImage;
  VkExtent2D _drawExtent;

  void init_triangle_pipeline();
  void init_pipelines();
  void init_background_pipelines();
  void init_descriptors();
  void init_vulkan();
  void init_swapchain();
  void init_commands();
  void init_sync_structures();
  void create_swapchain(uint32_t width, uint32_t height);
  void destroy_swapchain();
  std::vector<ComputeEffect> backgroundEffects;
  int currentBackgroundEffect{ 0 };

};
