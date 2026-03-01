//#include "DrawComponents.h"
//#include "../CCL.h"
//namespace DRAW
//{
//	//*** HELPERS ***//
//	void AddSceneData(entt::registry& registry, entt::entity entity)
//	{
//		auto& sceneData = registry.emplace<SceneData>(entity,
//			SceneData{ { -1.0f, -1.0f, 2.0f }, { 0.9f, 0.9f, 0.9f }, { 0.2f, 0.2f, 0.2f } });
//
//		GW::MATH::GVector::NormalizeF(sceneData.sunDirection, sceneData.sunDirection);
//		auto& renderer = registry.get<VulkanRenderer>(entity);
//		sceneData.projectionMatrix = renderer.projMatrix;
//	}
//
//	//*** SYSTEMS ***//
//	// Forward Declare
//	void Destroy_VulkanVertexBuffer(entt::registry& registry, entt::entity entity);
//
//	void Update_VulkanVertexBuffer(entt::registry& registry, entt::entity entity) {
//		auto& vertex_buffer = registry.get<VulkanVertexBuffer>(entity);
//		// upload the buffer to the GPU
//		if (registry.all_of<VulkanRenderer, std::vector<H2B::VERTEX>>(entity)) {
//			auto& vkRenderer = registry.get<VulkanRenderer>(entity);
//			// Skip Vulkan calls during quit to avoid crash - Elijah McCleave
//			if (vkRenderer.shuttingDown)
//				return;
//			// if there is already a vertex buffer attached, lets delete it
//			if (vertex_buffer.buffer != VK_NULL_HANDLE)
//				Destroy_VulkanVertexBuffer(registry, entity);
//			// if there is a cpu buffer attached, lets upload it to the GPU then delete it
//			auto& vertex_data = registry.get<std::vector<H2B::VERTEX>>(entity);
//			// Transfer triangle data to the vertex buffer. (staging would be preferred here)
//			GvkHelper::create_buffer(vkRenderer.physicalDevice, vkRenderer.device, sizeof(H2B::VERTEX) * vertex_data.size(),
//				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
//				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vertex_buffer.buffer, &vertex_buffer.memory);
//			GvkHelper::write_to_buffer(vkRenderer.device, vertex_buffer.memory,
//				vertex_data.data(), sizeof(H2B::VERTEX) * vertex_data.size());
//			// remove the Vertex Data
//			if (vkRenderer.device != VK_NULL_HANDLE)
//				vkDeviceWaitIdle(vkRenderer.device);
//			registry.remove<std::vector<H2B::VERTEX>>(entity);
//		}
//	}
//
//	void Destroy_VulkanVertexBuffer(entt::registry& registry, entt::entity entity)
//	{
//		if (!registry.all_of<VulkanRenderer, VulkanVertexBuffer>(entity))
//			return;
//
//		auto& renderer = registry.get<VulkanRenderer>(entity);
//
//		// Skip cleanup during quit to avoid vkDeviceWaitIdle crash - Elijah McCleave
//		if (renderer.device == VK_NULL_HANDLE || renderer.shuttingDown)
//			return;
//
//		auto& vb = registry.get<VulkanVertexBuffer>(entity);
//
//		if (renderer.device != VK_NULL_HANDLE)
//			vkDeviceWaitIdle(renderer.device);
//
//		if (vb.buffer != VK_NULL_HANDLE)
//		{
//			vkDestroyBuffer(renderer.device, vb.buffer, nullptr);
//			vb.buffer = VK_NULL_HANDLE;
//		}
//
//		if (vb.memory != VK_NULL_HANDLE)
//		{
//			vkFreeMemory(renderer.device, vb.memory, nullptr);
//			vb.memory = VK_NULL_HANDLE;
//		}
//	}
//
//	// Forward declare
//	void Destroy_VulkanIndexBuffer(entt::registry& registry, entt::entity entity);
//
//	void Update_VulkanIndexBuffer(entt::registry& registry, entt::entity entity) {
//		auto& index_buffer = registry.get<VulkanIndexBuffer>(entity);
//		// upload the buffer to the GPU
//		if (registry.all_of<VulkanRenderer, std::vector<unsigned int>>(entity)) {
//			// if there is already a gpu buffer attached, lets delete it
//			if (index_buffer.buffer != VK_NULL_HANDLE)
//				Destroy_VulkanIndexBuffer(registry, entity);
//			// if there is index data attached, lets upload it to the GPU then delete it
//			auto& vkRenderer = registry.get<VulkanRenderer>(entity);
//
//			// Skip Vulkan calls during quit to avoid crash - Elijah McCleave
//			if (vkRenderer.shuttingDown)
//				return;
//			auto& index_data = registry.get<std::vector<unsigned int>>(entity);
//
//			// Transfer triangle data to the vertex buffer. (staging would be preferred here)
//			GvkHelper::create_buffer(vkRenderer.physicalDevice, vkRenderer.device, sizeof(unsigned int) * index_data.size(),
//				VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
//				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &index_buffer.buffer, &index_buffer.memory);
//			GvkHelper::write_to_buffer(vkRenderer.device, index_buffer.memory,
//				index_data.data(), sizeof(unsigned int) * index_data.size());
//			// remove the index data
//			if (vkRenderer.device != VK_NULL_HANDLE)
//				vkDeviceWaitIdle(vkRenderer.device);
//
//			registry.remove<std::vector<unsigned int>>(entity);
//		}
//	}
//
//	void Destroy_VulkanIndexBuffer(entt::registry& registry, entt::entity entity) {
//
//		// If renderer no longer exists → stop
//		if (!registry.all_of<VulkanRenderer>(entity))
//			return;
//
//		auto& vkRenderer = registry.get<VulkanRenderer>(entity);
//
//		// Skip cleanup during quit to avoid vkDeviceWaitIdle crash - Elijah McCleave
//		if (vkRenderer.device == VK_NULL_HANDLE || vkRenderer.shuttingDown)
//			return;
//
//		// Normal destroy path
//		if (registry.all_of<VulkanIndexBuffer>(entity)) {
//
//			auto& index_buffer = registry.get<VulkanIndexBuffer>(entity);
//
//			if (vkRenderer.device != VK_NULL_HANDLE)
//				vkDeviceWaitIdle(vkRenderer.device);
//			
//
//			if (vkRenderer.device != VK_NULL_HANDLE && index_buffer.buffer != VK_NULL_HANDLE)
//			{
//				vkDestroyBuffer(vkRenderer.device, index_buffer.buffer, nullptr);
//				index_buffer.buffer = VK_NULL_HANDLE;
//			}
//
//
//			if (index_buffer.memory != VK_NULL_HANDLE)
//			{
//				vkFreeMemory(vkRenderer.device, index_buffer.memory, nullptr);
//				index_buffer.memory = VK_NULL_HANDLE;
//			}
//		}
//	}
//
//
//	void Construct_VulkanGPUInstanceBuffer(entt::registry& registry, entt::entity entity) {
//		auto& bufferComponent = registry.get<VulkanGPUInstanceBuffer>(entity);
//		auto& renderer = registry.get<VulkanRenderer>(entity);
//
//		unsigned int frameCount;
//		renderer.vlkSurface.GetSwapchainImageCount(frameCount);
//		bufferComponent.memory.resize(frameCount);
//		bufferComponent.buffer.resize(frameCount);
//
//		for (unsigned int i = 0; i < frameCount; i++)
//		{
//			GvkHelper::create_buffer(renderer.physicalDevice, renderer.device, sizeof(GPUInstance)*bufferComponent.element_count,
//				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
//				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &bufferComponent.buffer[i], &bufferComponent.memory[i]);
//		}
//		
//	}
//		
//	// Forward declare
//	void Destroy_VulkanGPUInstanceBuffer(entt::registry& registry, entt::entity entity);
//
//	void Update_VulkanGPUInstanceBuffer(entt::registry& registry, entt::entity entity) {
//		if (!registry.all_of<std::vector<GPUInstance>>(entity))
//			return; // No Instances, so nothing to write. Bail
//
//		auto& gpuBuffer = registry.get<VulkanGPUInstanceBuffer>(entity);		
//		auto& instances = registry.get<std::vector<GPUInstance>>(entity);
//		auto& renderer = registry.get<VulkanRenderer>(entity);
//
//		// Resize buffer if needed
//		if (instances.size() > gpuBuffer.element_count)
//		{			
//			Destroy_VulkanGPUInstanceBuffer(registry, entity);
//
//			while (instances.size() > gpuBuffer.element_count)
//			{
//				gpuBuffer.element_count *= 2; // Double the storage size if we ran out
//			}
//			
//			for (unsigned int i = 0; i < gpuBuffer.buffer.size(); i++)
//			{
//				GvkHelper::create_buffer(renderer.physicalDevice, renderer.device, sizeof(GPUInstance) * gpuBuffer.element_count,
//					VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
//					VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &gpuBuffer.buffer[i], &gpuBuffer.memory[i]);
//
//				VkDescriptorBufferInfo storageBufferInfo = { gpuBuffer.buffer[i], 0, VK_WHOLE_SIZE };
//				VkWriteDescriptorSet storageWrite = {};
//				storageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//				storageWrite.dstSet = renderer.descriptorSets[i];
//				storageWrite.dstBinding = 1; // 1 For the storage buffer
//				storageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//				storageWrite.descriptorCount = 1;
//				storageWrite.pBufferInfo = &storageBufferInfo;
//
//				vkUpdateDescriptorSets(renderer.device, 1, &storageWrite, 0, nullptr);
//			}
//		}		
//
//		if (instances.size() > 0)
//		{
//			unsigned int sizeInBytes = sizeof(GPUInstance) * instances.size();
//			unsigned int frame;
//			renderer.vlkSurface.GetSwapchainCurrentImage(frame);
//
//			GvkHelper::write_to_buffer(renderer.device, gpuBuffer.memory[frame], instances.data(), sizeInBytes);
//		}
//
//		registry.remove<std::vector<GPUInstance>>(entity);
//	}
//
//	void Destroy_VulkanGPUInstanceBuffer(entt::registry& registry, entt::entity entity)
//{
//    if (!registry.all_of<VulkanRenderer, VulkanGPUInstanceBuffer>(entity))
//        return;
//
//    auto& renderer = registry.get<VulkanRenderer>(entity);
//
//    // Skip cleanup during quit to avoid vkDeviceWaitIdle crash - Elijah McCleave
//    if (renderer.device == VK_NULL_HANDLE || renderer.shuttingDown)
//        return;
//
//    auto& gpuBuffer = registry.get<VulkanGPUInstanceBuffer>(entity);
//
//    vkDeviceWaitIdle(renderer.device);
//
//    for (auto& handle : gpuBuffer.buffer)
//    {
//        if (handle != VK_NULL_HANDLE)
//        {
//            vkDestroyBuffer(renderer.device, handle, nullptr);
//            handle = VK_NULL_HANDLE; // CRITICAL
//        }
//    }
//
//    for (auto& memory : gpuBuffer.memory)
//    {
//        if (memory != VK_NULL_HANDLE)
//        {
//            vkFreeMemory(renderer.device, memory, nullptr);
//            memory = VK_NULL_HANDLE; // CRITICAL
//        }
//    }
//}
//
//
//
//	void Construct_VulkanUniformBuffer(entt::registry& registry, entt::entity entity) {
//
//		auto& bufferComponent = registry.get<VulkanUniformBuffer>(entity);
//		auto& renderer = registry.get<VulkanRenderer>(entity);
//
//		// If there isn't a SceneData, add one
//		if (!registry.all_of<SceneData>(entity))
//		{
//			AddSceneData(registry, entity);
//		}
//
//		unsigned int frameCount;
//		renderer.vlkSurface.GetSwapchainImageCount(frameCount);
//		bufferComponent.memory.resize(frameCount);
//		bufferComponent.buffer.resize(frameCount);
//
//		for (unsigned int i = 0; i < frameCount; i++)
//		{
//			GvkHelper::create_buffer(renderer.physicalDevice, renderer.device, sizeof(SceneData),
//				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
//				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &bufferComponent.buffer[i], &bufferComponent.memory[i]);
//		}
//	}
//
//	void Update_VulkanUniformBuffer(entt::registry& registry, entt::entity entity) {
//		auto& gpuBuffer = registry.get<VulkanUniformBuffer>(entity);
//		auto& renderer = registry.get<VulkanRenderer>(entity);
//		auto& data = registry.get<SceneData>(entity);
//
//		// TODO : Update Dynamic parts of the Scene Data here
//		// We really only support one camera, so use the first one
//		auto& camera = registry.get<Camera>(registry.view<Camera>().front());
//
//		data.camPos = camera.camMatrix.row4;
//		GW::MATH::GMatrix::InverseF(camera.camMatrix, data.viewMatrix);
//
//		unsigned int frame;
//		renderer.vlkSurface.GetSwapchainCurrentImage(frame);
//		GvkHelper::write_to_buffer(renderer.device, gpuBuffer.memory[frame], &data, sizeof(SceneData));
//	}
//
//	void Destroy_VulkanUniformBuffer(entt::registry& registry, entt::entity entity)
//	{
//		if (!registry.all_of<VulkanRenderer, VulkanUniformBuffer>(entity))
//			return;
//
//		auto& renderer = registry.get<VulkanRenderer>(entity);
//
//		// Skip cleanup during quit to avoid vkDeviceWaitIdle crash - Elijah McCleave
//		if (renderer.device == VK_NULL_HANDLE || renderer.shuttingDown)
//			return;
//
//		auto& gpuBuffer = registry.get<VulkanUniformBuffer>(entity);
//
//		vkDeviceWaitIdle(renderer.device);
//
//		for (auto& handle : gpuBuffer.buffer)
//		{
//			if (handle != VK_NULL_HANDLE)
//			{
//				vkDestroyBuffer(renderer.device, handle, nullptr);
//				handle = VK_NULL_HANDLE;
//			}
//		}
//
//		for (auto& memory : gpuBuffer.memory)
//		{
//			if (memory != VK_NULL_HANDLE)
//			{
//				vkFreeMemory(renderer.device, memory, nullptr);
//				memory = VK_NULL_HANDLE;
//			}
//		}
//	}
//
//	// Use this MACRO to connect the EnTT Component Logic
//	CONNECT_COMPONENT_LOGIC() {
//		// register the Window component's logic
//		registry.on_update<VulkanVertexBuffer>().connect<Update_VulkanVertexBuffer>();
//		registry.on_destroy<VulkanVertexBuffer>().connect<Destroy_VulkanVertexBuffer>();
//
//		registry.on_update<VulkanIndexBuffer>().connect<Update_VulkanIndexBuffer>();
//		registry.on_destroy<VulkanIndexBuffer>().connect<Destroy_VulkanIndexBuffer>();
//
//		registry.on_construct<VulkanGPUInstanceBuffer>().connect<Construct_VulkanGPUInstanceBuffer>();
//		registry.on_update<VulkanGPUInstanceBuffer>().connect<Update_VulkanGPUInstanceBuffer>();
//		registry.on_destroy<VulkanGPUInstanceBuffer>().connect<Destroy_VulkanGPUInstanceBuffer>();
//
//		registry.on_construct<VulkanUniformBuffer>().connect<Construct_VulkanUniformBuffer>();
//		registry.on_update<VulkanUniformBuffer>().connect<Update_VulkanUniformBuffer>();
//		registry.on_destroy<VulkanUniformBuffer>().connect<Destroy_VulkanUniformBuffer>();
//	}
//
//} // namespace DRAW