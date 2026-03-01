#ifndef DRAW_COMPONENTS_H
#define DRAW_COMPONENTS_H

#include <string> // sheldon test
#include <unordered_map>
#include "./Utility/load_data_oriented.h"

namespace DRAW
{
	//*** TAGS ***//
	struct DoNotRender {};

	struct LevelEntity {}; // sheldon tag anything that belongs to the loaded level so we can delete it on level swap

	//*** COMPONENTS ***//
	struct TextureData
	{
		VkImage image = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkImageView view = VK_NULL_HANDLE;
		VkSampler sampler = VK_NULL_HANDLE;
	};

	struct VulkanRendererInitialization
	{
		std::string vertexShaderName;
		std::string fragmentShaderName;
		VkClearColorValue clearColor;
		VkClearDepthStencilValue depthStencil;
		float fovDegrees;
		float nearPlane;
		float farPlane;
	};

	struct VulkanRenderer
	{
		GW::GRAPHICS::GVulkanSurface vlkSurface;
		VkDevice device = nullptr;
		VkPhysicalDevice physicalDevice = nullptr;
		VkRenderPass renderPass;
		VkShaderModule vertexShader = nullptr;
		VkShaderModule fragmentShader = nullptr;
		VkPipeline pipeline = nullptr;
		VkPipelineLayout pipelineLayout = nullptr;
		GW::MATH::GMATRIXF projMatrix;

		VkDescriptorPool descriptorPool = nullptr;

		VkDescriptorSetLayout descriptorLayout = nullptr;
		std::vector<VkDescriptorSet> descriptorSets;

		// Textures
		VkDescriptorSetLayout textureLayout = nullptr;
		std::vector<VkDescriptorSet> textureSets;
		std::vector<TextureData> textureList;

		std::unordered_map<std::string, int> textureRegistry;
		
		VkClearValue clrAndDepth[2];
		VkInstance instance = VK_NULL_HANDLE;
		VkQueue graphicsQueue = VK_NULL_HANDLE;
		unsigned int graphicsQueueFamily;
		bool shuttingDown = false;  // Set true when Quit pressed, tells buffer cleanup to skip Vulkan calls - Elijah McCleave
	};

	struct VulkanVertexBuffer
	{
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
	};

	struct VulkanIndexBuffer
	{
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
	};

	struct GeometryData
	{
		unsigned int indexStart, indexCount, vertexStart;

		// Rename to materialIndex to match the .h2b data structure - DW
		unsigned int materialIndex = 0;

		// Sort by materialIndex to batch draw calls - DW
		inline bool operator < (const GeometryData a) const {
			if (materialIndex != a.materialIndex) return materialIndex < a.materialIndex;
			return indexStart < a.indexStart;
		}
	};

	struct GPUInstance
	{
		GW::MATH::GMATRIXF	transform;
		H2B::ATTRIBUTES		matData;
	};

	struct Name
	{
		std::string value;
	}; // sheldon store object name for lookups

	struct VulkanGPUInstanceBuffer
	{
		unsigned long long element_count = 1;
		std::vector<VkBuffer> buffer;
		std::vector<VkDeviceMemory> memory;
	};

	struct SceneData
	{
		GW::MATH::GVECTORF sunDirection, sunColor, sunAmbient, camPos;
		GW::MATH::GMATRIXF viewMatrix, projectionMatrix;
	};

	struct VulkanUniformBuffer
	{
		std::vector<VkBuffer> buffer;
		std::vector<VkDeviceMemory> memory;
	};

	struct Camera
	{
		GW::MATH::GMATRIXF camMatrix;
	};

	struct CPULevel
	{
		std::string levelPath;
		std::string modelPath;
		Level_Data levelData;
	};

	struct MeshCollection
	{
		std::vector<entt::entity> entities;
		GW::MATH::GOBBF collider;
	};

	struct ModelManager
	{
		std::unordered_map<std::string, MeshCollection> meshCollections;
	};

	struct GPULevel {};

	// Function declaration for cross-file visibility - DW
	int GetTexture(VulkanRenderer& renderer, std::string textureName);
} // namespace DRAW
#endif // !DRAW_COMPONENTS_H

