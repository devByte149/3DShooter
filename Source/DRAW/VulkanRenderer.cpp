//#define STB_IMAGE_IMPLEMENTATION
//#include "./Utility/stb_image.h"
//#include "DrawComponents.h"
//#include "../CCL.h"
//// component dependencies
//#include "./Utility/FileIntoString.h"
//#include "shaderc/shaderc.h" // needed for compiling shaders at runtime
//#ifdef _WIN32 // must use MT platform DLL libraries on windows
//#pragma comment(lib, "shaderc_combined.lib")
//#endif
//#include "../GAME/GameComponents.h"
//#include <imgui.h>
//#include <imgui_impl_win32.h>
//#include <imgui_impl_vulkan.h>
//#include "../APP/Window.hpp"
//#include <filesystem>
//#include <unordered_map>
//#include <algorithm>
//#include <vector>
//#include <iostream>
//#include <map>
//#include <cstdio>
//#ifdef _WIN32
//#include <Windows.h>
//#endif
//// --- HOOKING SETUP START ---
//// We need to tell the compiler that this function exists in the ImGui backend
//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//// We need a place to store Gateware's original message handler
//static WNDPROC g_GatewareWndProc = NULL;
//// This is our custom "Spy" function that steals messages for ImGui
//static LRESULT CALLBACK HookedWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//    // 1. Let ImGui see the message first
//    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
//        return true; // ImGui handled it (e.g., clicking a button)
//    // 2. If ImGui didn't want it, pass it back to Gateware so the game still works
//    return CallWindowProc(g_GatewareWndProc, hWnd, msg, wParam, lParam);
//}
//// --- HOOKING SETUP END ---
//namespace DRAW
//{
//    //*** HELPER METHODS ***//
//    VkViewport CreateViewportFromWindowDimensions(unsigned int windowWidth, unsigned int windowHeight)
//    {
//        VkViewport retval = {};
//        retval.x = 0;
//        retval.y = 0;
//        retval.width = static_cast<float>(windowWidth);
//        retval.height = static_cast<float>(windowHeight);
//        retval.minDepth = 0;
//        retval.maxDepth = 1;
//        return retval;
//    }
//    VkRect2D CreateScissorFromWindowDimensions(unsigned int windowWidth, unsigned int windowHeight)
//    {
//        VkRect2D retval = {};
//        retval.offset.x = 0;
//        retval.offset.y = 0;
//        retval.extent.width = windowWidth;
//        retval.extent.height = windowHeight;
//        return retval;
//    }
//    uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
//        VkPhysicalDeviceMemoryProperties memProperties;
//        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
//        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
//            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
//                return i;
//            }
//        }
//        return 0;
//    }
//    
//    static std::string ToLower(std::string s)
//    {
//        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::tolower(c); });
//        return s;
//    }
//    // sheldon get exe dir so we can resolve textures 
//    static std::filesystem::path GetExecutableDir()
//    {
//#ifdef _WIN32
//        char buffer[MAX_PATH] = {};
//        GetModuleFileNameA(nullptr, buffer, MAX_PATH);
//        std::filesystem::path p(buffer);
//        return p.parent_path();
//#else
//        return std::filesystem::current_path();
//#endif
//    }
//    // sheldon walk upward until we find ProjectTemplate/Textures
//    static std::filesystem::path FindTexturesFolder()
//    {
//        std::filesystem::path cur = GetExecutableDir();
//        for (int i = 0; i < 10; ++i)
//        {
//          
//            {
//                std::filesystem::path tryA = cur / "Textures";
//                if (std::filesystem::exists(tryA) && std::filesystem::is_directory(tryA))
//                    return tryA;
//            }
//            {
//                std::filesystem::path tryB = cur / "ProjectTemplate" / "Textures";
//                if (std::filesystem::exists(tryB) && std::filesystem::is_directory(tryB))
//                    return tryB;
//            }
//            if (!cur.has_parent_path())
//                break;
//            cur = cur.parent_path();
//        }
//       
//        std::filesystem::path fallback = std::filesystem::current_path() / "Textures";
//        return fallback;
//    }
//    // sheldon build a quick lookup table for textures 
//    static void BuildTextureLookup(const std::filesystem::path& texDir,
//        std::unordered_map<std::string, std::filesystem::path>& outMap)
//    {
//        outMap.clear();
//        if (!std::filesystem::exists(texDir))
//            return;
//        for (auto& entry : std::filesystem::directory_iterator(texDir))
//        {
//            if (!entry.is_regular_file())
//                continue;
//            auto p = entry.path();
//            std::string name = ToLower(p.filename().string());
//            outMap[name] = p;
//        }
//    }
//    // sheldon try to resolve a filename
//    static std::filesystem::path ResolveTexturePath(
//        const std::filesystem::path& texDir,
//        const std::unordered_map<std::string, std::filesystem::path>& lookup,
//        const std::string& requested)
//    {
//        if (requested.empty())
//            return {};
//        std::filesystem::path asPath(requested);
//        // if its already a valid path accept it
//        if (std::filesystem::exists(asPath))
//            return asPath;
//        // use just the filename
//        std::string fileOnly = ToLower(asPath.filename().string());
//        // direct lookup by exact filename
//        auto it = lookup.find(fileOnly);
//        if (it != lookup.end())
//            return it->second;
//        // try common extensions if no extension was supplied
//        if (asPath.extension().empty())
//        {
//            const char* exts[] = { ".png", ".jpg", ".jpeg", ".tga", ".bmp" };
//            for (auto e : exts)
//            {
//                std::string withExt = fileOnly + e;
//                auto it2 = lookup.find(withExt);
//                if (it2 != lookup.end())
//                    return it2->second;
//            }
//        }
//        for (auto& kv : lookup)
//        {
//            const std::string& existing = kv.first;
//            if (existing.size() >= fileOnly.size() &&
//                existing.compare(existing.size() - fileOnly.size(), fileOnly.size(), fileOnly) == 0)
//            {
//                return kv.second;
//            }
//        }
//        std::filesystem::path direct = texDir / asPath.filename();
//        if (std::filesystem::exists(direct))
//            return direct;
//        return {};
//    }
//    static bool IsAnyTerrainTextureName(const std::string& lowerName)
//    {
//        return
//            lowerName == "ground_material_0_basecolor.jpeg" ||
//            lowerName == "ground_material_0_metallicroughness.png" ||
//            lowerName == "ground_material_0_normal.png" ||
//            lowerName == "ground_material_0_specularf0.png" ||
//            lowerName == "ocean_material_0_basecolor.png" ||
//            lowerName == "ocean_diff.png" ||
//            lowerName == "ocean_bump.png" ||
//            lowerName == "albedo-diffuse.jpg" ||
//            lowerName == "globe_bumb.jpg" ||
//            lowerName == "globe_clouds.png" ||
//            lowerName == "globe_mask.png" ||
//            lowerName == "night_lights_modified.png";
//    }
//    static bool IsPlaneOrShipTextureName(const std::string& lowerName)
//    {
//        std::string name = lowerName;
//        return
//            name.find("plane") != std::string::npos ||
//            name.find("player") != std::string::npos ||
//            name.find("ship") != std::string::npos ||
//            name.find("spaceship1") != std::string::npos ||
//            name.find("enemy") != std::string::npos ||
//            name.find("diffuse") != std::string::npos ||
//            name == "planediffuse.png" ||
//            name == "spaceship1diffuse.png";
//    }
//    static int DetectLevelFromMaterials(const decltype(DRAW::CPULevel::levelData.levelMaterials)& mats)
//    {
//        bool hasGlobe = false;
//        bool hasOcean = false;
//        for (const auto& m : mats)
//        {
//            if (m.map_Kd && m.map_Kd[0] != '\0')
//            {
//                std::string n = ToLower(std::filesystem::path(m.map_Kd).filename().string());
//                if (n.find("globe") != std::string::npos || n.find("night_lights") != std::string::npos || n.find("albedo") != std::string::npos)
//                    hasGlobe = true;
//                if (n.find("ocean") != std::string::npos)
//                    hasOcean = true;
//            }
//        }
//        if (hasGlobe) return 3;
//        if (hasOcean) return 2;
//        return 1;
//    }
//    static std::string ForceTerrainTextureForLevel(int level)
//    {
//        // sheldon fix force the real filenames
//        if (level == 2) return "Ocean_material_0_baseColor.png";
//        if (level == 3) return "Albedo-diffuse.jpg";
//        return "Ground_material_0_baseColor.jpeg";
//    }
//    // sheldon create a 1x1 rgba white pixl
//    static void MakeFallbackWhite(std::vector<stbi_uc>& outPixels, int& w, int& h)
//    {
//        w = 1; h = 1;
//        outPixels = { 255, 255, 255, 255 };
//    }
//    static bool ShouldLoadForLevel(const std::string& lowerFile, int level)
//    {
//        // Always allow plane and ships in EVERY level (this fixes level 3)
//        if (IsPlaneOrShipTextureName(lowerFile))
//            return true;
//        // Terrain rules: only load the correct terrain for each level
//        if (lowerFile.rfind("ground", 0) == 0)
//            return level == 1;
//        if (lowerFile.rfind("ocean", 0) == 0)
//            return level == 2;
//        if (lowerFile.rfind("globe", 0) == 0 || lowerFile.find("albedo") != std::string::npos)
//            return level == 3;
//        if (lowerFile.find("night_lights") != std::string::npos)
//            return level == 3;
//        // Fallback: allow anything else (safety net)
//        return true;
//    }
//    static int g_lastLoadedLevel = 0;
//    static std::filesystem::path g_texDir = FindTexturesFolder();
//    static std::unordered_map<std::string, std::filesystem::path> g_texLookup;
//   
//    static void SubmitOneTime(VulkanRenderer& renderer, VkCommandPool pool, VkCommandBuffer cmd)
//    {
//        vkEndCommandBuffer(cmd);
//        VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
//        submitInfo.commandBufferCount = 1;
//        submitInfo.pCommandBuffers = &cmd;
//        vkQueueSubmit(renderer.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
//        vkQueueWaitIdle(renderer.graphicsQueue);
//    }
//    int LoadTexture(VulkanRenderer& renderer, const char* filename)
//    {
//        TextureData newTexture = {};
//        // Load image
//        int texWidth = 0, texHeight = 0, texChannels = 0;
//        stbi_uc* pixels = nullptr;
//        std::vector<stbi_uc> fallback;
//        bool usingFallback = false;
//        if (filename && filename[0] != '\0')
//        {
//            pixels = stbi_load(filename, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
//        }
//        if (!pixels || texWidth <= 0 || texHeight <= 0)
//        {
//            // Create a default 1x1 white texture so we dont crash
//            std::cout << "Failed to load texture: " << (filename ? filename : "") << std::endl;
//            MakeFallbackWhite(fallback, texWidth, texHeight);
//            pixels = fallback.data();
//            usingFallback = true;
//        }
//        VkDeviceSize imageSize = (VkDeviceSize)texWidth * (VkDeviceSize)texHeight * 4;
//        VkBuffer stagingBuffer = VK_NULL_HANDLE;
//        VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
//        VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
//        bufferInfo.size = imageSize;
//        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
//        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//        vkCreateBuffer(renderer.device, &bufferInfo, nullptr, &stagingBuffer);
//        VkMemoryRequirements memRequirements = {};
//        vkGetBufferMemoryRequirements(renderer.device, stagingBuffer, &memRequirements);
//        VkMemoryAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
//        allocInfo.allocationSize = memRequirements.size;
//        allocInfo.memoryTypeIndex = FindMemoryType(renderer.physicalDevice, memRequirements.memoryTypeBits,
//            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//        vkAllocateMemory(renderer.device, &allocInfo, nullptr, &stagingBufferMemory);
//        vkBindBufferMemory(renderer.device, stagingBuffer, stagingBufferMemory, 0);
//        void* data = nullptr;
//        vkMapMemory(renderer.device, stagingBufferMemory, 0, imageSize, 0, &data);
//        memcpy(data, pixels, (size_t)imageSize);
//        vkUnmapMemory(renderer.device, stagingBufferMemory);
//        if (!usingFallback && pixels)
//            stbi_image_free(pixels);
//        VkImageCreateInfo imageInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
//        imageInfo.imageType = VK_IMAGE_TYPE_2D;
//        imageInfo.extent.width = (uint32_t)texWidth;
//        imageInfo.extent.height = (uint32_t)texHeight;
//        imageInfo.extent.depth = 1;
//        imageInfo.mipLevels = 1;
//        imageInfo.arrayLayers = 1;
//        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB; // Standard texture format
//        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
//        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
//        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
//        vkCreateImage(renderer.device, &imageInfo, nullptr, &newTexture.image);
//        vkGetImageMemoryRequirements(renderer.device, newTexture.image, &memRequirements);
//        allocInfo.allocationSize = memRequirements.size;
//        allocInfo.memoryTypeIndex = FindMemoryType(renderer.physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
//        vkAllocateMemory(renderer.device, &allocInfo, nullptr, &newTexture.memory);
//        vkBindImageMemory(renderer.device, newTexture.image, newTexture.memory, 0);
//        VkCommandPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
//        poolInfo.queueFamilyIndex = renderer.graphicsQueueFamily;
//        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT; // sheldon temp pool
//        VkCommandPool tempPool = VK_NULL_HANDLE;
//        vkCreateCommandPool(renderer.device, &poolInfo, nullptr, &tempPool);
//        VkCommandBufferAllocateInfo cmdAllocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
//        cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//        cmdAllocInfo.commandPool = tempPool;
//        cmdAllocInfo.commandBufferCount = 1;
//        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
//        vkAllocateCommandBuffers(renderer.device, &cmdAllocInfo, &commandBuffer);
//        VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
//        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//        vkBeginCommandBuffer(commandBuffer, &beginInfo);
//        // Transition: UNDEFINED -> TRANSFER_DST
//        VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
//        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//        barrier.image = newTexture.image;
//        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//        barrier.subresourceRange.baseMipLevel = 0;
//        barrier.subresourceRange.levelCount = 1;
//        barrier.subresourceRange.baseArrayLayer = 0;
//        barrier.subresourceRange.layerCount = 1;
//        barrier.srcAccessMask = 0;
//        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//        vkCmdPipelineBarrier(commandBuffer,
//            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
//            VK_PIPELINE_STAGE_TRANSFER_BIT,
//            0, 0, nullptr, 0, nullptr, 1, &barrier);
//        VkBufferImageCopy region = {};
//        region.bufferOffset = 0;
//        region.bufferRowLength = 0;
//        region.bufferImageHeight = 0;
//        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//        region.imageSubresource.mipLevel = 0;
//        region.imageSubresource.baseArrayLayer = 0;
//        region.imageSubresource.layerCount = 1;
//        region.imageOffset = { 0, 0, 0 };
//        region.imageExtent = { (uint32_t)texWidth, (uint32_t)texHeight, 1 };
//        vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, newTexture.image,
//            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
//        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//        vkCmdPipelineBarrier(commandBuffer,
//            VK_PIPELINE_STAGE_TRANSFER_BIT,
//            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
//            0, 0, nullptr, 0, nullptr, 1, &barrier);
//        SubmitOneTime(renderer, tempPool, commandBuffer);
//        // Cleanup temp command buffer/pool
//        vkFreeCommandBuffers(renderer.device, tempPool, 1, &commandBuffer);
//        vkDestroyCommandPool(renderer.device, tempPool, nullptr);
//        // Cleanup staging
//        vkDestroyBuffer(renderer.device, stagingBuffer, nullptr);
//        vkFreeMemory(renderer.device, stagingBufferMemory, nullptr);
//        VkImageViewCreateInfo viewInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
//        viewInfo.image = newTexture.image;
//        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
//        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
//        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//        viewInfo.subresourceRange.baseMipLevel = 0;
//        viewInfo.subresourceRange.levelCount = 1;
//        viewInfo.subresourceRange.baseArrayLayer = 0;
//        viewInfo.subresourceRange.layerCount = 1;
//        vkCreateImageView(renderer.device, &viewInfo, nullptr, &newTexture.view);
//        VkSamplerCreateInfo samplerInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
//        samplerInfo.magFilter = VK_FILTER_LINEAR;
//        samplerInfo.minFilter = VK_FILTER_LINEAR;
//        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//        samplerInfo.anisotropyEnable = VK_FALSE;
//        samplerInfo.maxAnisotropy = 1.0f;
//        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
//        samplerInfo.unnormalizedCoordinates = VK_FALSE;
//        samplerInfo.compareEnable = VK_FALSE;
//        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
//        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
//        vkCreateSampler(renderer.device, &samplerInfo, nullptr, &newTexture.sampler);
//        renderer.textureList.push_back(newTexture);
//        return static_cast<int>(renderer.textureList.size() - 1);
//    }
//    // sheldon destroy all textures safely
//    static void DestroyAllTextures(VulkanRenderer& vulkanRenderer, bool skipVulkanDestroy = false)
//    {
//        if (skipVulkanDestroy || vulkanRenderer.device == VK_NULL_HANDLE)
//        {
//            vulkanRenderer.textureList.clear();
//            vulkanRenderer.textureSets.clear();
//            vulkanRenderer.textureRegistry.clear();
//            return;
//        }
//        if (vulkanRenderer.device != VK_NULL_HANDLE)
//            vkDeviceWaitIdle(vulkanRenderer.device); // sheldon stop "in use by descriptor set" errors
//        // sheldon fix free old texture descriptor sets so we don't exhaust the pool on level reloads
//        if (vulkanRenderer.descriptorPool != VK_NULL_HANDLE && !vulkanRenderer.textureSets.empty())
//        {
//            std::vector<VkDescriptorSet> validSets;
//            validSets.reserve(vulkanRenderer.textureSets.size());
//            for (auto s : vulkanRenderer.textureSets)
//            {
//                if (s != VK_NULL_HANDLE)
//                    validSets.push_back(s);
//            }
//            if (!validSets.empty())
//            {
//                vkFreeDescriptorSets(
//                    vulkanRenderer.device,
//                    vulkanRenderer.descriptorPool,
//                    (uint32_t)validSets.size(),
//                    validSets.data()
//                );
//            }
//        }
//        for (auto& tex : vulkanRenderer.textureList) {
//            if (tex.view) vkDestroyImageView(vulkanRenderer.device, tex.view, nullptr);
//            if (tex.sampler) vkDestroySampler(vulkanRenderer.device, tex.sampler, nullptr);
//            if (tex.image) vkDestroyImage(vulkanRenderer.device, tex.image, nullptr);
//            if (tex.memory) vkFreeMemory(vulkanRenderer.device, tex.memory, nullptr);
//            tex = {};
//        }
//        vulkanRenderer.textureList.clear();
//        vulkanRenderer.textureSets.clear();
//        vulkanRenderer.textureRegistry.clear();
//    }
//    // Helper to read the level data and load all required textures
//    void LoadLevelTextures(entt::registry& registry, entt::entity entity)
//    {
//        auto& vulkanRenderer = registry.get<VulkanRenderer>(entity);
//        if (!registry.all_of<DRAW::CPULevel>(entity)) return;
//        auto& level = registry.get<DRAW::CPULevel>(entity);
//        BuildTextureLookup(g_texDir, g_texLookup);
//        // Clear old textures (safe)
//        DestroyAllTextures(vulkanRenderer);
//        // Index 0: Default White
//        std::cout << "[Texture 0] Default White\n";
//        LoadTexture(vulkanRenderer, ""); // sheldon forces 1x1 white without needing a file
//        int detectedLevel = 0;
//        {
//            auto gmView = registry.view<GAME::GameManager, GAME::LevelProgress>();
//            if (gmView.begin() != gmView.end())
//            {
//                auto gm = *gmView.begin();
//                detectedLevel = registry.get<GAME::LevelProgress>(gm).currentLevel;
//            }
//            else
//            {
//                detectedLevel = DetectLevelFromMaterials(level.levelData.levelMaterials);
//            }
//        }
//        g_lastLoadedLevel = detectedLevel;
//        const std::string forcedTerrain = ForceTerrainTextureForLevel(detectedLevel);
//        const std::string forcedTerrainLower = ToLower(std::filesystem::path(forcedTerrain).filename().string());
//        int texIndex = 1; // Start counting from 1
//        for (const auto& mat : level.levelData.levelMaterials)
//        {
//            // prefer map_Kd from .mtl if present
//            if (mat.map_Kd && mat.map_Kd[0] != '\0') {
//                std::string rawPath = mat.map_Kd;
//                std::string rawFileLower = ToLower(std::filesystem::path(rawPath).filename().string());
//                if (!ShouldLoadForLevel(rawFileLower, detectedLevel))
//                    rawPath = "";
//                if (rawPath.size() && !IsPlaneOrShipTextureName(rawFileLower) && (detectedLevel == 2 || detectedLevel == 3 || IsAnyTerrainTextureName(rawFileLower)))
//                    rawPath = forcedTerrain;
//                if (!rawPath.empty())
//                {
//                    std::filesystem::path resolved = ResolveTexturePath(g_texDir, g_texLookup, rawPath);
//                    // sheldon if resolution failed try forcing the terrain texture for that level anyway
//                    if (resolved.empty() && IsAnyTerrainTextureName(rawFileLower) && !IsPlaneOrShipTextureName(rawFileLower))
//                        resolved = ResolveTexturePath(g_texDir, g_texLookup, forcedTerrain);
//                    if (!resolved.empty())
//                    {
//                        std::string resolvedLower = ToLower(resolved.filename().string());
//                        if (!ShouldLoadForLevel(resolvedLower, detectedLevel))
//                            resolved = std::filesystem::path();
//                        if (!resolved.empty() && IsAnyTerrainTextureName(resolvedLower) && !IsPlaneOrShipTextureName(resolvedLower))
//                        {
//                            if (resolvedLower != forcedTerrainLower)
//                                resolved = ResolveTexturePath(g_texDir, g_texLookup, forcedTerrain);
//                        }
//                    }
//                    std::string rawFile = mat.map_Kd ? ToLower(std::filesystem::path(mat.map_Kd).filename().string()) : "<no map_Kd>";
//                    std::string resolvedStr = resolved.empty() ? "<FAILED>" : resolved.string();
//                    std::cout << "[Level " << detectedLevel << "] Tex #" << texIndex
//                        << " | name: " << rawFile
//                        << " | should load? " << (ShouldLoadForLevel(rawFileLower, detectedLevel) ? "YES" : "NO")
//                        << " | resolved: " << resolvedStr
//                        << " ? " << (resolved.empty() ? "using white fallback" : "loading texture")
//                        << "\n";
//                    if (!resolved.empty())
//                    {
//                        std::cout << "[Texture " << texIndex << "] Loading: " << resolved.string() << "\n";
//                        LoadTexture(vulkanRenderer, resolved.string().c_str());
//                    }
//                    else
//                    {
//                        std::cout << "[Texture " << texIndex << "] No Texture (using Default)\n";
//                        LoadTexture(vulkanRenderer, ""); // sheldon default white
//                    }
//                }
//                else
//                {
//                    std::cout << "[Texture " << texIndex << "] No Texture (using Default)\n";
//                    LoadTexture(vulkanRenderer, ""); // sheldon default white
//                }
//            }
//            else {
//                std::cout << "[Texture " << texIndex << "] No Texture (using Default)\n";
//                LoadTexture(vulkanRenderer, ""); // sheldon default white
//            }
//            texIndex++;
//        }
//        // Generate Descriptor Sets for the new textures
//        vulkanRenderer.textureSets.resize(vulkanRenderer.textureList.size());
//        for (size_t i = 0; i < vulkanRenderer.textureList.size(); i++)
//        {
//            VkDescriptorSetAllocateInfo texAlloc = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
//            texAlloc.descriptorPool = vulkanRenderer.descriptorPool;
//            texAlloc.descriptorSetCount = 1;
//            texAlloc.pSetLayouts = &vulkanRenderer.textureLayout;
//            // Ensure maxSets in InitializeDescriptors is large enough.
//            VkResult ar = vkAllocateDescriptorSets(vulkanRenderer.device, &texAlloc, &vulkanRenderer.textureSets[i]);
//            // sheldon fix if allocation fails, don't crash o
//            if (ar != VK_SUCCESS)
//            {
//                vulkanRenderer.textureSets[i] = VK_NULL_HANDLE;
//                std::cout << "FAILED to allocate texture descriptor set for tex " << i << "\n";
//                continue;
//            }
//            VkDescriptorImageInfo imageInfo = {};
//            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//            imageInfo.imageView = vulkanRenderer.textureList[i].view;
//            imageInfo.sampler = vulkanRenderer.textureList[i].sampler;
//            VkWriteDescriptorSet texWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
//            texWrite.dstSet = vulkanRenderer.textureSets[i];
//            texWrite.dstBinding = 0;
//            texWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//            texWrite.descriptorCount = 1;
//            texWrite.pImageInfo = &imageInfo;
//            vkUpdateDescriptorSets(vulkanRenderer.device, 1, &texWrite, 0, nullptr);
//        }
//    }
//    void InitializeDescriptors(entt::registry& registry, entt::entity entity)
//    {
//        auto& vulkanRenderer = registry.get<VulkanRenderer>(entity);
//        unsigned int frameCount;
//        vulkanRenderer.vlkSurface.GetSwapchainImageCount(frameCount);
//        vulkanRenderer.descriptorSets.resize(frameCount);
//#pragma region Descriptor Layout
//        /* Scene Layout */
//        VkDescriptorSetLayoutBinding globalBindings[2] = {};
//        globalBindings[0] = { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, nullptr };
//        globalBindings[1] = { 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, nullptr };
//        VkDescriptorSetLayoutCreateInfo globalInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
//        globalInfo.bindingCount = 2;
//        globalInfo.pBindings = globalBindings;
//        vkCreateDescriptorSetLayout(vulkanRenderer.device, &globalInfo, nullptr, &vulkanRenderer.descriptorLayout);
//        /* Texture Layout */
//        VkDescriptorSetLayoutBinding textureBinding = { 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr };
//        VkDescriptorSetLayoutCreateInfo textureInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
//        textureInfo.bindingCount = 1;
//        textureInfo.pBindings = &textureBinding;
//        vkCreateDescriptorSetLayout(vulkanRenderer.device, &textureInfo, nullptr, &vulkanRenderer.textureLayout);
//#pragma endregion
//#pragma region Descriptor Pool
//        VkDescriptorPoolSize poolSizes[] = {
//        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, frameCount },
//        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, frameCount },
//        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2000 }, // Support up to 2000 textures
//        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100 } // Extra for ImGui
//        };
//        VkDescriptorPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
//        poolInfo.maxSets = frameCount + 2100; // Total sets available
//        poolInfo.poolSizeCount = 4;
//        poolInfo.pPoolSizes = poolSizes;
//        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
//        vkCreateDescriptorPool(vulkanRenderer.device, &poolInfo, nullptr, &vulkanRenderer.descriptorPool);
//#pragma endregion
//#pragma region Allocate Descriptor Sets
//        VkDescriptorSetAllocateInfo globalAlloc = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
//        globalAlloc.descriptorPool = vulkanRenderer.descriptorPool;
//        globalAlloc.descriptorSetCount = 1;
//        globalAlloc.pSetLayouts = &vulkanRenderer.descriptorLayout;
//        for (int i = 0; i < (int)frameCount; i++) vkAllocateDescriptorSets(vulkanRenderer.device, &globalAlloc, &vulkanRenderer.descriptorSets[i]);
//#pragma endregion
//        auto& storageBuffer = registry.emplace<VulkanGPUInstanceBuffer>(entity, VulkanGPUInstanceBuffer{ 16 });
//        auto& uniformBuffer = registry.emplace<VulkanUniformBuffer>(entity);
//        for (int i = 0; i < (int)frameCount; i++) {
//            VkDescriptorBufferInfo bInfo[2] = {
//            { uniformBuffer.buffer[i], 0, VK_WHOLE_SIZE },
//            { storageBuffer.buffer[i], 0, VK_WHOLE_SIZE }
//            };
//            VkWriteDescriptorSet writes[2] = {};
//            writes[0] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, vulkanRenderer.descriptorSets[i], 0, 0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, &bInfo[0], nullptr };
//            writes[1] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, vulkanRenderer.descriptorSets[i], 1, 0, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, nullptr, &bInfo[1], nullptr };
//            vkUpdateDescriptorSets(vulkanRenderer.device, 2, writes, 0, nullptr);
//        }
//    }
//    void InitializeGraphicsPipeline(entt::registry& registry, entt::entity entity)
//    {
//        auto& vulkanRenderer = registry.get<VulkanRenderer>(entity);
//        GW::SYSTEM::GWindow win = registry.get<GW::SYSTEM::GWindow>(entity);
//        // Create Pipeline & Layout (Thanks Tiny!)
//        VkPipelineShaderStageCreateInfo stage_create_info[2] = {};
//        // Create Stage Info for Vertex Shader
//        stage_create_info[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//        stage_create_info[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
//        stage_create_info[0].module = vulkanRenderer.vertexShader;
//        stage_create_info[0].pName = "main";
//        // Create Stage Info for Fragment Shader
//        stage_create_info[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//        stage_create_info[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//        stage_create_info[1].module = vulkanRenderer.fragmentShader;
//        stage_create_info[1].pName = "main";
//        VkPipelineInputAssemblyStateCreateInfo assembly_create_info = {};
//        assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//        assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//        assembly_create_info.primitiveRestartEnable = false;
//        VkVertexInputBindingDescription vertex_binding_description = {};
//        vertex_binding_description.binding = 0;
//        vertex_binding_description.stride = sizeof(H2B::VERTEX);
//        vertex_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//        VkVertexInputAttributeDescription vertex_attribute_description[3];
//        vertex_attribute_description[0].binding = 0;
//        vertex_attribute_description[0].location = 0;
//        vertex_attribute_description[0].format = VK_FORMAT_R32G32B32_SFLOAT;
//        vertex_attribute_description[0].offset = offsetof(H2B::VERTEX, pos);
//        vertex_attribute_description[1].binding = 0;
//        vertex_attribute_description[1].location = 1;
//        vertex_attribute_description[1].format = VK_FORMAT_R32G32B32_SFLOAT;
//        vertex_attribute_description[1].offset = offsetof(H2B::VERTEX, uvw);
//        vertex_attribute_description[2].binding = 0;
//        vertex_attribute_description[2].location = 2;
//        vertex_attribute_description[2].format = VK_FORMAT_R32G32B32_SFLOAT;
//        vertex_attribute_description[2].offset = offsetof(H2B::VERTEX, nrm);
//        VkPipelineVertexInputStateCreateInfo input_vertex_info = {};
//        input_vertex_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//        input_vertex_info.vertexBindingDescriptionCount = 1;
//        input_vertex_info.pVertexBindingDescriptions = &vertex_binding_description;
//        input_vertex_info.vertexAttributeDescriptionCount = 3;
//        input_vertex_info.pVertexAttributeDescriptions = vertex_attribute_description;
//        unsigned int windowWidth, windowHeight;
//        win.GetClientWidth(windowWidth);
//        win.GetClientHeight(windowHeight);
//        VkViewport viewport = CreateViewportFromWindowDimensions(windowWidth, windowHeight);
//        VkRect2D scissor = CreateScissorFromWindowDimensions(windowWidth, windowHeight);
//        VkPipelineViewportStateCreateInfo viewport_create_info = {};
//        viewport_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//        viewport_create_info.viewportCount = 1;
//        viewport_create_info.pViewports = &viewport;
//        viewport_create_info.scissorCount = 1;
//        viewport_create_info.pScissors = &scissor;
//        VkPipelineRasterizationStateCreateInfo rasterization_create_info = {};
//        rasterization_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//        rasterization_create_info.rasterizerDiscardEnable = VK_FALSE;
//        rasterization_create_info.polygonMode = VK_POLYGON_MODE_FILL;
//        rasterization_create_info.lineWidth = 1.0f;
//        rasterization_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
//        rasterization_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
//        rasterization_create_info.depthClampEnable = VK_FALSE;
//        rasterization_create_info.depthBiasEnable = VK_FALSE;
//        rasterization_create_info.depthBiasClamp = 0.0f;
//        rasterization_create_info.depthBiasConstantFactor = 0.0f;
//        rasterization_create_info.depthBiasSlopeFactor = 0.0f;
//        VkPipelineMultisampleStateCreateInfo multisample_create_info = {};
//        multisample_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//        multisample_create_info.sampleShadingEnable = VK_FALSE;
//        multisample_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
//        multisample_create_info.minSampleShading = 1.0f;
//        multisample_create_info.pSampleMask = VK_NULL_HANDLE;
//        multisample_create_info.alphaToCoverageEnable = VK_FALSE;
//        multisample_create_info.alphaToOneEnable = VK_FALSE;
//        VkPipelineDepthStencilStateCreateInfo depth_stencil_create_info = {};
//        depth_stencil_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
//        depth_stencil_create_info.depthTestEnable = VK_TRUE;
//        depth_stencil_create_info.depthWriteEnable = VK_TRUE;
//        depth_stencil_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
//        depth_stencil_create_info.depthBoundsTestEnable = VK_FALSE;
//        depth_stencil_create_info.minDepthBounds = 0.0f;
//        depth_stencil_create_info.maxDepthBounds = 1.0f;
//        depth_stencil_create_info.stencilTestEnable = VK_FALSE;
//        VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
//        color_blend_attachment_state.colorWriteMask = 0xF;
//        color_blend_attachment_state.blendEnable = VK_FALSE;
//        color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
//        color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR;
//        color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
//        color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
//        color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
//        color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
//        VkPipelineColorBlendStateCreateInfo color_blend_create_info = {};
//        color_blend_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//        color_blend_create_info.logicOpEnable = VK_FALSE;
//        color_blend_create_info.logicOp = VK_LOGIC_OP_COPY;
//        color_blend_create_info.attachmentCount = 1;
//        color_blend_create_info.pAttachments = &color_blend_attachment_state;
//        color_blend_create_info.blendConstants[0] = 0.0f;
//        color_blend_create_info.blendConstants[1] = 0.0f;
//        color_blend_create_info.blendConstants[2] = 0.0f;
//        color_blend_create_info.blendConstants[3] = 0.0f;
//        // Dynamic State
//        VkDynamicState dynamic_states[2] =
//        {
//            // By setting these we do not need to re-create the pipeline on Resize
//            VK_DYNAMIC_STATE_VIEWPORT,
//            VK_DYNAMIC_STATE_SCISSOR
//        };
//        VkPipelineDynamicStateCreateInfo dynamic_create_info = {};
//        dynamic_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//        dynamic_create_info.dynamicStateCount = 2;
//        dynamic_create_info.pDynamicStates = dynamic_states;
//        InitializeDescriptors(registry, entity);
//        VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
//        pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//        VkDescriptorSetLayout layouts[] = { vulkanRenderer.descriptorLayout, vulkanRenderer.textureLayout };
//        pipeline_layout_create_info.setLayoutCount = 2;
//        pipeline_layout_create_info.pSetLayouts = layouts;
//        pipeline_layout_create_info.pushConstantRangeCount = 0;
//        pipeline_layout_create_info.pPushConstantRanges = nullptr;
//        vkCreatePipelineLayout(vulkanRenderer.device, &pipeline_layout_create_info, nullptr, &vulkanRenderer.pipelineLayout);
//        // Pipeline State... (FINALLY)
//        VkGraphicsPipelineCreateInfo pipeline_create_info = {};
//        pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//        pipeline_create_info.stageCount = 2;
//        pipeline_create_info.pStages = stage_create_info;
//        pipeline_create_info.pInputAssemblyState = &assembly_create_info;
//        pipeline_create_info.pVertexInputState = &input_vertex_info;
//        pipeline_create_info.pViewportState = &viewport_create_info;
//        pipeline_create_info.pRasterizationState = &rasterization_create_info;
//        pipeline_create_info.pMultisampleState = &multisample_create_info;
//        pipeline_create_info.pDepthStencilState = &depth_stencil_create_info;
//        pipeline_create_info.pColorBlendState = &color_blend_create_info;
//        pipeline_create_info.pDynamicState = &dynamic_create_info;
//        pipeline_create_info.layout = vulkanRenderer.pipelineLayout;
//        pipeline_create_info.renderPass = vulkanRenderer.renderPass;
//        pipeline_create_info.subpass = 0;
//        pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
//        vkCreateGraphicsPipelines(vulkanRenderer.device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &vulkanRenderer.pipeline);
//    }
//    //*** SYSTEMS ***//
//    // run this code when a VulkanRenderer component is connected
//    void Construct_VulkanRenderer(entt::registry& registry, entt::entity entity)
//    {
//        if (!registry.all_of<GW::SYSTEM::GWindow>(entity))
//        {
//            std::cout << "Window not added to the registry yet!" << std::endl;
//            abort();
//            return;
//        }
//        if (!registry.all_of<VulkanRendererInitialization>(entity))
//        {
//            std::cout << "Initialization Data not added to the registry yet!" << std::endl;
//            abort();
//            return;
//        }
//        auto& vulkanRenderer = registry.get<VulkanRenderer>(entity);
//        auto& initializationData = registry.get<VulkanRendererInitialization>(entity);
//        GW::SYSTEM::GWindow win = registry.get<GW::SYSTEM::GWindow>(entity);
//#ifndef NDEBUG
//        const char* debugLayers[] = {
//        "VK_LAYER_KHRONOS_validation", // standard validation layer
//        };
//        if (-vulkanRenderer.vlkSurface.Create(win, GW::GRAPHICS::DEPTH_BUFFER_SUPPORT,
//            sizeof(debugLayers) / sizeof(debugLayers[0]),
//            debugLayers, 0, nullptr, 0, nullptr, false))
//#else
//        if (-vulkanRenderer.vlkSurface.Create(win, GW::GRAPHICS::DEPTH_BUFFER_SUPPORT))
//#endif
//        {
//            std::cout << "Failed to create Vulkan Surface!" << std::endl;
//            abort();
//            return;
//        }
//        vulkanRenderer.clrAndDepth[0].color = initializationData.clearColor;
//        vulkanRenderer.clrAndDepth[1].depthStencil = initializationData.depthStencil;
//        // Create Projection matrix
//        float aspectRatio;
//        vulkanRenderer.vlkSurface.GetAspectRatio(aspectRatio);
//        GW::MATH::GMatrix::ProjectionVulkanLHF(G2D_DEGREE_TO_RADIAN_F(initializationData.fovDegrees), aspectRatio, initializationData.nearPlane, initializationData.farPlane, vulkanRenderer.projMatrix);
//        vulkanRenderer.vlkSurface.GetDevice((void**)&vulkanRenderer.device);
//        vulkanRenderer.vlkSurface.GetPhysicalDevice((void**)&vulkanRenderer.physicalDevice);
//        vulkanRenderer.vlkSurface.GetRenderPass((void**)&vulkanRenderer.renderPass);
//        vulkanRenderer.vlkSurface.GetInstance((void**)&vulkanRenderer.instance);
//        vulkanRenderer.vlkSurface.GetGraphicsQueue((void**)&vulkanRenderer.graphicsQueue);
//        // Intialize runtime shader compiler HLSL -> SPIRV
//        shaderc_compiler_t compiler = shaderc_compiler_initialize();
//        shaderc_compile_options_t options = shaderc_compile_options_initialize();
//        shaderc_compile_options_set_source_language(options, shaderc_source_language_hlsl);
//        shaderc_compile_options_set_invert_y(options, false);
//#ifndef NDEBUG
//        shaderc_compile_options_set_generate_debug_info(options);
//#endif
//        // Vertex Shader
//        std::string vertexShaderSource = ReadFileIntoString(initializationData.vertexShaderName.c_str());
//        shaderc_compilation_result_t result = shaderc_compile_into_spv( // compile
//            compiler, vertexShaderSource.c_str(), vertexShaderSource.length(),
//            shaderc_vertex_shader, "main.vert", "main", options);
//        if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) // errors?
//        {
//            std::cout << "Vertex Shader Errors : \n" << shaderc_result_get_error_message(result) << std::endl;
//            abort();
//            return;
//        }
//        GvkHelper::create_shader_module(vulkanRenderer.device, shaderc_result_get_length(result), // load into Vulkan
//            (char*)shaderc_result_get_bytes(result), &vulkanRenderer.vertexShader);
//        shaderc_result_release(result); // done
//        // Fragment Shader
//        std::string fragmentShaderSource = ReadFileIntoString(initializationData.fragmentShaderName.c_str());
//        result = shaderc_compile_into_spv( // compile
//            compiler, fragmentShaderSource.c_str(), fragmentShaderSource.length(),
//            shaderc_fragment_shader, "main.frag", "main", options);
//        if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) // errors?
//        {
//            std::cout << "Fragment Shader Errors : \n" << shaderc_result_get_error_message(result) << std::endl;
//            abort();
//            return;
//        }
//        GvkHelper::create_shader_module(vulkanRenderer.device, shaderc_result_get_length(result), // load into Vulkan
//            (char*)shaderc_result_get_bytes(result), &vulkanRenderer.fragmentShader);
//        shaderc_result_release(result); // done
//        // Free runtime shader compiler resources
//        shaderc_compile_options_release(options);
//        shaderc_compiler_release(compiler);
//        InitializeGraphicsPipeline(registry, entity);
//        IMGUI_CHECKVERSION();
//        ImGui::CreateContext();
//        ImGuiIO& io = ImGui::GetIO();
//        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
//        GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE uwh;
//        win.GetWindowHandle(uwh);
//        HWND hwnd = static_cast<HWND>(uwh.window);
//        ImGui_ImplWin32_Init(hwnd);
//        // Install the hook to capture input for ImGui
//        g_GatewareWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)HookedWndProc);
//        ImGui_ImplVulkan_InitInfo init_info = {};
//        init_info.ApiVersion = VK_API_VERSION_1_0;
//        init_info.Instance = vulkanRenderer.instance;
//        init_info.PhysicalDevice = vulkanRenderer.physicalDevice;
//        init_info.Device = vulkanRenderer.device;
//        VkQueue queue = VK_NULL_HANDLE;
//        vulkanRenderer.vlkSurface.GetGraphicsQueue((void**)&queue);
//        init_info.Queue = queue;
//        init_info.QueueFamily = vulkanRenderer.graphicsQueueFamily;
//        init_info.DescriptorPool = vulkanRenderer.descriptorPool;
//        unsigned int imageCount = 0;
//        vulkanRenderer.vlkSurface.GetSwapchainImageCount(imageCount);
//        init_info.MinImageCount = imageCount;
//        init_info.ImageCount = imageCount;
//        init_info.PipelineInfoMain.RenderPass = vulkanRenderer.renderPass;
//        init_info.PipelineInfoMain.Subpass = 0;
//        init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
//        init_info.UseDynamicRendering = false;
//        ImGui_ImplVulkan_Init(&init_info);
//        std::cout << "Instance: " << vulkanRenderer.instance << std::endl;
//        std::cout << "PhysicalDevice: " << vulkanRenderer.physicalDevice << std::endl;
//        std::cout << "Device: " << vulkanRenderer.device << std::endl;
//        std::cout << "Queue: " << vulkanRenderer.graphicsQueue << std::endl;
//        VkCommandBuffer cmd;
//        unsigned int imageIndex;
//        vulkanRenderer.vlkSurface.GetSwapchainCurrentImage(imageIndex);
//        vulkanRenderer.vlkSurface.GetCommandBuffer(imageIndex, (void**)&cmd);
//        // Remove the initializtion data as we no longer need it
//        registry.remove<VulkanRendererInitialization>(entity);
//    }
//    int GetTexture(VulkanRenderer& renderer, std::string textureName)
//    {
//        if (textureName.empty())
//            return 0;
//        std::string key = ToLower(textureName);
//        if (renderer.textureRegistry.count(key)) {
//            return renderer.textureRegistry[key];
//        }
//        BuildTextureLookup(g_texDir, g_texLookup);
//        std::filesystem::path resolved = ResolveTexturePath(g_texDir, g_texLookup, textureName);
//        // Load it and store the index
//        int newIndex = LoadTexture(renderer, resolved.empty() ? "" : resolved.string().c_str());
//        // Update the registry
//        renderer.textureRegistry[key] = newIndex;
//        renderer.textureSets.resize(renderer.textureList.size());
//        if (renderer.descriptorPool != VK_NULL_HANDLE && renderer.textureLayout != VK_NULL_HANDLE) {
//            VkDescriptorSetAllocateInfo texAlloc = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
//            texAlloc.descriptorPool = renderer.descriptorPool;
//            texAlloc.descriptorSetCount = 1;
//            texAlloc.pSetLayouts = &renderer.textureLayout;
//            // sheldon check allocation result, if it fails keep it null and we will fall back at draw time
//            if (vkAllocateDescriptorSets(renderer.device, &texAlloc, &renderer.textureSets[newIndex]) == VK_SUCCESS) {
//                VkDescriptorImageInfo imageInfo = {};
//                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//                imageInfo.imageView = renderer.textureList[newIndex].view;
//                imageInfo.sampler = renderer.textureList[newIndex].sampler;
//                VkWriteDescriptorSet texWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
//                texWrite.dstSet = renderer.textureSets[newIndex];
//                texWrite.dstBinding = 0;
//                texWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//                texWrite.descriptorCount = 1;
//                texWrite.pImageInfo = &imageInfo;
//                vkUpdateDescriptorSets(renderer.device, 1, &texWrite, 0, nullptr);
//            }
//            else
//            {
//                renderer.textureSets[newIndex] = VK_NULL_HANDLE;
//            }
//        }
//        return newIndex;
//    }
//    // run this code when a VulkanRenderer component is updated
//    void Update_VulkanRenderer(entt::registry& registry, entt::entity entity)
//    {
//        auto& vulkanRenderer = registry.get<VulkanRenderer>(entity);
//        if (vulkanRenderer.device == VK_NULL_HANDLE)
//            return;
//        int currentLevel = 0;
//        {
//            auto gmView = registry.view<GAME::GameManager, GAME::LevelProgress>();
//            if (gmView.begin() != gmView.end())
//            {
//                auto gm = *gmView.begin();
//                currentLevel = registry.get<GAME::LevelProgress>(gm).currentLevel;
//            }
//        }
//        if (vulkanRenderer.textureList.empty() && registry.all_of<DRAW::CPULevel>(entity)) {
//            LoadLevelTextures(registry, entity);
//        }
//        else if (currentLevel != 0 && currentLevel != g_lastLoadedLevel && registry.all_of<DRAW::CPULevel>(entity)) {
//            LoadLevelTextures(registry, entity);
//        }
//        // Always reset clear color each frame, then optionally override for specific states.
//        vulkanRenderer.clrAndDepth[0].color = { 0.2f, 0.2f, 0.25f, 1.0f };
//        auto gameManagerView = registry.view<GAME::GameManager>();
//        const bool hasGameManager = !gameManagerView.empty();
//        entt::entity gameManagerEntity = hasGameManager ? gameManagerView.front() : entt::null;
//        const bool mainMenuActive = hasGameManager && registry.all_of<GAME::MainMenu>(gameManagerEntity);
//        const bool pausedActive = hasGameManager && registry.all_of<GAME::Paused>(gameManagerEntity);
//        const bool winMenuActive = hasGameManager && registry.all_of<GAME::WinMenu>(gameManagerEntity);
//        const bool gameOverActive = hasGameManager && registry.all_of<GAME::GameOver>(gameManagerEntity);
//        if (gameOverActive)
//            vulkanRenderer.clrAndDepth[0].color = { 0.25f, 0.02f, 0.02f, 1.0f };
//        if (-vulkanRenderer.vlkSurface.StartFrame(2, vulkanRenderer.clrAndDepth))
//        {
//            std::cout << "Failed to start frame!" << std::endl;
//            return;
//        }
//        ImGui_ImplVulkan_NewFrame();
//        ImGui_ImplWin32_NewFrame();
//        ImGui::NewFrame();
//        ImGuiIO& io = ImGui::GetIO();
//        bool requestWindowClose = false;
//        // Default to gameplay behavior each frame; menu states explicitly enable it.
//        io.MouseDrawCursor = false;
//        static bool wasMainMenuActive = false;
//        static float titleAnimT = 0.0f;
//        // MAIN MENU UI
//        if (mainMenuActive)
//        {
//            // Force ImGui to draw a cursor to click buttons
//            io.MouseDrawCursor = true;
//            ImDrawList* bg = ImGui::GetBackgroundDrawList();
//            const ImVec2 screen = io.DisplaySize;
//
//            // Fully cover the screen behind the stripe pattern so gameplay does not show through.
//            bg->AddRectFilled(ImVec2(0.0f, 0.0f), screen, IM_COL32(18, 18, 18, 255));
//
//            // Fullscreen diagonal stripe background (grey/white/black)
//            const float stripeSpacing = 10.0f;
//            const ImU32 stripeColors[3] = {
//                IM_COL32(130, 130, 130, 255),
//                IM_COL32(245, 245, 245, 255),
//                IM_COL32(15, 15, 15, 255)
//            };
//            int stripeIndex = 0;
//            for (float x = -screen.y; x < screen.x + screen.y; x += stripeSpacing, ++stripeIndex)
//                bg->AddLine(ImVec2(x, 0.0f), ImVec2(x + screen.y, screen.y), stripeColors[stripeIndex % 3], 5.0f);
//
//            if (!wasMainMenuActive)
//                titleAnimT = 0.0f;
//            wasMainMenuActive = true;
//            titleAnimT = (std::min)(titleAnimT + io.DeltaTime * 0.9f, 1.0f);
//
//            const char* title = "----------2042----------";
//            ImFont* font = ImGui::GetFont();
//            const float titleFontSize = 64.0f;
//            const ImVec2 titleSize = font->CalcTextSizeA(titleFontSize, FLT_MAX, 0.0f, title);
//            const float titleX = (screen.x - titleSize.x) * 0.5f; // center horizontally
//
//            const float titleStartY = -titleSize.y;
//            const float titleEndY = (screen.y - titleSize.y) * 0.5f; // vertical center target
//            const float oneMinusT = 1.0f - titleAnimT;
//            const float easedT = 1.0f - (oneMinusT * oneMinusT * oneMinusT);
//            const float titleY = titleStartY + (titleEndY - titleStartY) * easedT;
//
//            // Airplane silhouette above the title.
//            const float planeBob = std::sin((float)ImGui::GetTime() * 1.4f) * 4.0f;
//            const ImVec2 planeCenter(titleX + titleSize.x * 0.5f, titleY - 85.0f + planeBob);
//            const float s = 1.0f;
//            const ImU32 planeShadow = IM_COL32(6, 20, 10, 110);
//            const ImU32 planeColor = IM_COL32(24, 78, 42, 255);
//
//            // Shadow
//            bg->AddTriangleFilled(
//                ImVec2(planeCenter.x - 120.0f * s + 5.0f, planeCenter.y + 5.0f),
//                ImVec2(planeCenter.x + 140.0f * s + 5.0f, planeCenter.y + 14.0f * s + 5.0f),
//                ImVec2(planeCenter.x + 145.0f * s + 5.0f, planeCenter.y - 14.0f * s + 5.0f),
//                planeShadow);
//            bg->AddTriangleFilled(
//                ImVec2(planeCenter.x - 12.0f * s + 5.0f, planeCenter.y - 6.0f * s + 5.0f),
//                ImVec2(planeCenter.x + 70.0f * s + 5.0f, planeCenter.y - 44.0f * s + 5.0f),
//                ImVec2(planeCenter.x + 86.0f * s + 5.0f, planeCenter.y - 18.0f * s + 5.0f),
//                planeShadow);
//            bg->AddTriangleFilled(
//                ImVec2(planeCenter.x - 18.0f * s + 5.0f, planeCenter.y + 5.0f * s + 5.0f),
//                ImVec2(planeCenter.x + 95.0f * s + 5.0f, planeCenter.y + 44.0f * s + 5.0f),
//                ImVec2(planeCenter.x + 77.0f * s + 5.0f, planeCenter.y + 14.0f * s + 5.0f),
//                planeShadow);
//            bg->AddTriangleFilled(
//                ImVec2(planeCenter.x - 115.0f * s + 5.0f, planeCenter.y - 6.0f * s + 5.0f),
//                ImVec2(planeCenter.x - 98.0f * s + 5.0f, planeCenter.y - 42.0f * s + 5.0f),
//                ImVec2(planeCenter.x - 78.0f * s + 5.0f, planeCenter.y - 4.0f * s + 5.0f),
//                planeShadow);
//            bg->AddTriangleFilled(
//                ImVec2(planeCenter.x - 114.0f * s + 5.0f, planeCenter.y + 6.0f * s + 5.0f),
//                ImVec2(planeCenter.x - 98.0f * s + 5.0f, planeCenter.y + 38.0f * s + 5.0f),
//                ImVec2(planeCenter.x - 80.0f * s + 5.0f, planeCenter.y + 6.0f * s + 5.0f),
//                planeShadow);
//
//            // Body
//            bg->AddTriangleFilled(
//                ImVec2(planeCenter.x - 120.0f * s, planeCenter.y),
//                ImVec2(planeCenter.x + 140.0f * s, planeCenter.y + 14.0f * s),
//                ImVec2(planeCenter.x + 145.0f * s, planeCenter.y - 14.0f * s),
//                planeColor);
//            // Top wing
//            bg->AddTriangleFilled(
//                ImVec2(planeCenter.x - 12.0f * s, planeCenter.y - 6.0f * s),
//                ImVec2(planeCenter.x + 70.0f * s, planeCenter.y - 44.0f * s),
//                ImVec2(planeCenter.x + 86.0f * s, planeCenter.y - 18.0f * s),
//                planeColor);
//            // Bottom wing
//            bg->AddTriangleFilled(
//                ImVec2(planeCenter.x - 18.0f * s, planeCenter.y + 5.0f * s),
//                ImVec2(planeCenter.x + 95.0f * s, planeCenter.y + 44.0f * s),
//                ImVec2(planeCenter.x + 77.0f * s, planeCenter.y + 14.0f * s),
//                planeColor);
//            // Tail fins
//            bg->AddTriangleFilled(
//                ImVec2(planeCenter.x - 115.0f * s, planeCenter.y - 6.0f * s),
//                ImVec2(planeCenter.x - 98.0f * s, planeCenter.y - 42.0f * s),
//                ImVec2(planeCenter.x - 78.0f * s, planeCenter.y - 4.0f * s),
//                planeColor);
//            bg->AddTriangleFilled(
//                ImVec2(planeCenter.x - 114.0f * s, planeCenter.y + 6.0f * s),
//                ImVec2(planeCenter.x - 98.0f * s, planeCenter.y + 38.0f * s),
//                ImVec2(planeCenter.x - 80.0f * s, planeCenter.y + 6.0f * s),
//                planeColor);
//
//            bg->AddText(font, titleFontSize, ImVec2(titleX, titleY), IM_COL32(255, 255, 255, 255), title);
//
//            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
//            ImGui::SetNextWindowSize(screen, ImGuiCond_Always);
//            ImGui::SetNextWindowBgAlpha(0.0f);
//            ImGui::Begin("Main Menu Overlay",
//                nullptr,
//                ImGuiWindowFlags_NoDecoration |
//                ImGuiWindowFlags_NoMove |
//                ImGuiWindowFlags_NoBackground |
//                ImGuiWindowFlags_NoSavedSettings);
//
//            const float buttonW = 220.0f;
//            const float buttonH = 46.0f;
//            const float buttonX = (screen.x - buttonW) * 0.5f;
//            const float buttonsY = titleY + titleSize.y + 30.0f;
//            ImGui::SetCursorPos(ImVec2(buttonX, buttonsY));
//            // Use the same transition path as Enter in main.cpp: remove MainMenu only.
//            const bool startPressed = ImGui::Button("Start Game", ImVec2(buttonW, buttonH));
//            if (startPressed)
//            {
//                registry.remove<GAME::MainMenu>(gameManagerEntity);
//                wasMainMenuActive = false;
//                io.MouseDrawCursor = false;
//            }
//            ImGui::SetCursorPosX(buttonX);
//            if (ImGui::Button("Quit", ImVec2(buttonW, buttonH)))
//            {
//                requestWindowClose = true;
//            }
//            ImGui::End();
//        }
//        else
//        {
//            wasMainMenuActive = false;
//            // HUD UI (only active during gameplay, not in menus or pause)
//            if (hasGameManager && !mainMenuActive && !gameOverActive && !winMenuActive && !pausedActive)
//            {
//                auto gm = gameManagerEntity;
//                ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
//                ImGui::SetNextWindowSize(ImVec2(300, 160), ImGuiCond_Always);
//                ImGui::Begin("HUD", nullptr,
//                    ImGuiWindowFlags_NoDecoration |
//                    ImGuiWindowFlags_NoMove |
//                    ImGuiWindowFlags_NoResize |
//                    ImGuiWindowFlags_NoSavedSettings |
//                    ImGuiWindowFlags_NoBackground);
//                auto& score = registry.get<GAME::ScoreManager>(gm);
//                ImGui::Text("Score: %d", score.score);
//                ImGui::Text("High Score: %d", score.highScore);
//                auto players = registry.view<GAME::Player, GAME::Health>();
//                if (players.begin() != players.end())
//                {
//                    auto playerEntity = *players.begin(); // CORRECT
//                    auto& health = registry.get<GAME::Health>(playerEntity);
//                    auto& playerStats = registry.get<GAME::Player>(playerEntity);
//                    // Health bar only, no HP text - Elijah McCleave
//                    float normalizedHealth = (float)health.HP / (float)health.MaxHP;
//                    if (normalizedHealth < 0.0f) normalizedHealth = 0.0f;
//                    if (normalizedHealth > 1.0f) normalizedHealth = 1.0f;
//                    ImGui::Text("Health");
//                    ImGui::ProgressBar(normalizedHealth, ImVec2(200, 20));
//                    /* UI for Roll/Loop Mechanic */
//                    ImGui::Spacing();
//                    ImGui::Text("Loops Available:");
//                    int currentLoops = playerStats.rollCount;
//                    int maxLoops = 3;
//                    // Draw boxes for each roll
//                    for (int i = 0; i < maxLoops; ++i)
//                    {
//                        if (i < currentLoops) {
//                            // Remaining Loop: Bright Cyan
//                            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 255, 255, 255));
//                            ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 255, 255, 255));
//                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 255, 255, 255));
//                        }
//                        else {
//                            // Used Loop: Dim Gray
//                            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(75, 75, 75, 255));
//                            ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(75, 75, 75, 255));
//                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(75, 75, 75, 255));
//                        }
//                        ImGui::Button((std::string("##loop") + std::to_string(i)).c_str(), ImVec2(25, 10));
//                        ImGui::PopStyleColor(3);
//                        if (i < maxLoops - 1) ImGui::SameLine();
//                    }
//                    /* Roll UI End */
//                }
//                ImGui::End();
//            }
//            // PAUSE MENU UI
//            if (pausedActive)
//            {
//                io.MouseDrawCursor = true;
//                ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
//                ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
//                ImGui::SetNextWindowBgAlpha(1.0f);
//                ImGui::Begin("Pause Screen",
//                    nullptr,
//                    ImGuiWindowFlags_NoDecoration |
//                    ImGuiWindowFlags_NoMove |
//                    ImGuiWindowFlags_NoResize);
//                ImGui::Text("PAUSED");
//                // Returns to main menu from pause and stops music - Elijah McCleave
//                if (ImGui::Button("Main Menu"))
//                {
//                    auto pausedView = registry.view<GAME::Paused>();
//                    for (auto gm : pausedView)
//                    {
//                        registry.remove<GAME::Paused>(gm);
//                        registry.emplace_or_replace<GAME::MainMenu>(gm);
//                    }
//                    if (registry.ctx().contains<GW::AUDIO::GMusic>())
//                        registry.ctx().get<GW::AUDIO::GMusic>().Stop();
//                }
//                // Pause menu Quit: marks window closed and sets shutdown flag - Elijah McCleave
//                if (ImGui::Button("Quit"))
//                {
//                    requestWindowClose = true;
//                }
//                ImGui::End();
//            }
//            else if (winMenuActive)
//            {
//                io.MouseDrawCursor = true;
//                ImDrawList* bg = ImGui::GetBackgroundDrawList();
//                const ImVec2 screen = io.DisplaySize;
//                bg->AddRectFilled(ImVec2(0.0f, 0.0f), screen, IM_COL32(8, 22, 16, 255));
//
//                const float gridStep = 46.0f;
//                for (float x = 0.0f; x <= screen.x; x += gridStep)
//                    bg->AddLine(ImVec2(x, 0.0f), ImVec2(x, screen.y), IM_COL32(50, 95, 70, 80), 1.0f);
//                for (float y = 0.0f; y <= screen.y; y += gridStep)
//                    bg->AddLine(ImVec2(0.0f, y), ImVec2(screen.x, y), IM_COL32(50, 95, 70, 80), 1.0f);
//
//                const float t = (float)ImGui::GetTime();
//                const float scanY = std::fmod(t * 220.0f, screen.y + 120.0f) - 60.0f;
//                bg->AddRectFilled(ImVec2(0.0f, scanY), ImVec2(screen.x, scanY + 2.5f), IM_COL32(170, 255, 205, 90));
//                bg->AddRectFilled(ImVec2(0.0f, scanY + 6.0f), ImVec2(screen.x, scanY + 7.0f), IM_COL32(120, 220, 170, 45));
//
//                unsigned int glitchRng = ((unsigned int)(t * 1000.0f) * 1664525u) + 1013904223u;
//                auto glitch01 = [&glitchRng]() -> float {
//                    glitchRng = 1664525u * glitchRng + 1013904223u;
//                    return (float)((glitchRng >> 8) & 0x00FFFFFFu) / 16777215.0f;
//                };
//                const int barCount = 6;
//                for (int i = 0; i < barCount; ++i)
//                {
//                    const float y = glitch01() * screen.y;
//                    const float h = 1.0f + glitch01() * 6.0f;
//                    const float x = glitch01() * screen.x * 0.35f;
//                    const float w = screen.x * (0.55f + glitch01() * 0.45f) - x;
//                    const int a = 20 + (int)(glitch01() * 55.0f);
//                    bg->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), IM_COL32(180, 255, 220, a));
//                }
//
//                const char* title = "MISSION ACCOMPLISHED";
//                ImFont* font = ImGui::GetFont();
//                const float titleSizePx = 62.0f;
//                const ImVec2 titleSize = font->CalcTextSizeA(titleSizePx, FLT_MAX, 0.0f, title);
//                const float titleX = (screen.x - titleSize.x) * 0.5f;
//                const float titleY = screen.y * 0.20f;
//                const float jitterX = std::sin(t * 35.0f) * 1.0f + std::sin(t * 11.0f) * 0.7f;
//                const float jitterY = std::cos(t * 29.0f) * 0.8f;
//                const ImVec2 titleBase(titleX + jitterX, titleY + jitterY);
//                bg->AddText(font, titleSizePx, ImVec2(titleBase.x + 2.0f, titleBase.y + 2.0f), IM_COL32(0, 0, 0, 120), title);
//                bg->AddText(font, titleSizePx, ImVec2(titleBase.x - 1.2f, titleBase.y), IM_COL32(255, 80, 80, 120), title);
//                bg->AddText(font, titleSizePx, ImVec2(titleBase.x + 1.2f, titleBase.y), IM_COL32(80, 255, 220, 120), title);
//                bg->AddText(font, titleSizePx, titleBase, IM_COL32(210, 255, 228, 255), title);
//
//                int finalScore = 0;
//                int highScore = 0;
//                int enemiesDown = 0;
//                float timeSurvived = 0.0f;
//                if (registry.all_of<GAME::WinMenu>(gameManagerEntity))
//                    finalScore = registry.get<GAME::WinMenu>(gameManagerEntity).finalScore;
//                if (registry.all_of<GAME::ScoreManager>(gameManagerEntity))
//                {
//                    auto& score = registry.get<GAME::ScoreManager>(gameManagerEntity);
//                    highScore = score.highScore;
//                    enemiesDown = score.enemiesDown;
//                    timeSurvived = score.timeSurvivedSec;
//                }
//
//                ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
//                ImGui::SetNextWindowSize(screen, ImGuiCond_Always);
//                ImGui::SetNextWindowBgAlpha(0.0f);
//                ImGui::Begin("Win Debrief Overlay",
//                    nullptr,
//                    ImGuiWindowFlags_NoDecoration |
//                    ImGuiWindowFlags_NoMove |
//                    ImGuiWindowFlags_NoSavedSettings |
//                    ImGuiWindowFlags_NoBackground);
//
//                char finalScoreText[64] = {};
//                char highScoreText[64] = {};
//                char enemiesText[64] = {};
//                char timeText[64] = {};
//                std::snprintf(finalScoreText, sizeof(finalScoreText), "Final Score: %d", finalScore);
//                std::snprintf(highScoreText, sizeof(highScoreText), "High Score: %d", highScore);
//                std::snprintf(enemiesText, sizeof(enemiesText), "Enemies Down: %d", enemiesDown);
//                std::snprintf(timeText, sizeof(timeText), "Time Survived: %.1fs", timeSurvived);
//
//                const float statsY = titleY + titleSize.y + 34.0f;
//                const ImVec2 fsz = ImGui::CalcTextSize(finalScoreText);
//                const ImVec2 hsz = ImGui::CalcTextSize(highScoreText);
//                const ImVec2 esz = ImGui::CalcTextSize(enemiesText);
//                const ImVec2 tsz = ImGui::CalcTextSize(timeText);
//                const float statsJitterX = std::sin(t * 18.0f) * 0.6f;
//                ImGui::SetCursorPos(ImVec2((screen.x - fsz.x) * 0.5f + statsJitterX, statsY));
//                ImGui::Text("%s", finalScoreText);
//                ImGui::SetCursorPos(ImVec2((screen.x - hsz.x) * 0.5f - statsJitterX, statsY + 24.0f));
//                ImGui::Text("%s", highScoreText);
//                ImGui::SetCursorPos(ImVec2((screen.x - esz.x) * 0.5f + statsJitterX, statsY + 48.0f));
//                ImGui::Text("%s", enemiesText);
//                ImGui::SetCursorPos(ImVec2((screen.x - tsz.x) * 0.5f - statsJitterX, statsY + 72.0f));
//                ImGui::Text("%s", timeText);
//
//                const float buttonW = 220.0f;
//                const float buttonH = 46.0f;
//                const float buttonX = (screen.x - buttonW) * 0.5f;
//                ImGui::SetCursorPos(ImVec2(buttonX, statsY + 114.0f));
//                if (ImGui::Button("Main Menu", ImVec2(buttonW, buttonH)))
//                {
//                    auto allGameOver = registry.view<GAME::GameOver>();
//                    for (auto e : allGameOver)
//                        registry.remove<GAME::GameOver>(e);
//                    auto gameManagerView = registry.view<GAME::GameManager>();
//                    if (!gameManagerView.empty())
//                    {
//                        auto gm = gameManagerView.front();
//                        registry.remove<GAME::WinMenu>(gm);
//                        registry.emplace_or_replace<GAME::MainMenu>(gm);
//                        if (registry.ctx().contains<GW::AUDIO::GMusic>())
//                            registry.ctx().get<GW::AUDIO::GMusic>().Stop();
//                    }
//                }
//                ImGui::SetCursorPosX(buttonX);
//                if (ImGui::Button("Next Level", ImVec2(buttonW, buttonH)))
//                {
//                    auto gameManagerView = registry.view<GAME::GameManager>();
//                    if (!gameManagerView.empty())
//                    {
//                        auto gm = gameManagerView.front();
//                        registry.remove<GAME::WinMenu>(gm);
//                        if (registry.ctx().contains<GW::AUDIO::GMusic>())
//                            registry.ctx().get<GW::AUDIO::GMusic>().Play(true);
//                        int nextLevel = 1;
//                        if (registry.all_of<GAME::LevelProgress>(gm))
//                        {
//                            auto& prog = registry.get<GAME::LevelProgress>(gm);
//                            nextLevel = (prog.currentLevel % 3) + 1;
//                        }
//                        registry.emplace_or_replace<GAME::LevelLoadRequest>(gm, GAME::LevelLoadRequest{ nextLevel });
//                    }
//                }
//                ImGui::SetCursorPosX(buttonX);
//                if (ImGui::Button("Quit", ImVec2(buttonW, buttonH)))
//                {
//                    requestWindowClose = true;
//                }
//                ImGui::End();
//            }
//            // GAME OVER UI
//            else if (gameOverActive)
//            {
//                // Show cursor on game over
//                io.MouseDrawCursor = true;
//                ImDrawList* bg = ImGui::GetBackgroundDrawList();
//                const ImVec2 screen = io.DisplaySize;
//                bg->AddRectFilled(ImVec2(0.0f, 0.0f), screen, IM_COL32(20, 4, 6, 255));
//
//                unsigned int rng = 0x7A1B2C3Du;
//                auto rand01 = [&rng]() -> float {
//                    rng = 1664525u * rng + 1013904223u;
//                    return (float)((rng >> 8) & 0x00FFFFFFu) / 16777215.0f;
//                };
//                const int dotCount = 85;
//                for (int i = 0; i < dotCount; ++i)
//                {
//                    const ImVec2 c(screen.x * rand01(), screen.y * rand01());
//                    const float sizeBias = rand01();
//                    const float r = 3.0f + (sizeBias * sizeBias) * 38.0f;
//                    const int a = 115 + (int)(rand01() * 95.0f);
//                    bg->AddCircleFilled(c, r, IM_COL32(158, 20, 24, a), 20);
//                    bg->AddCircleFilled(ImVec2(c.x - r * 0.18f, c.y - r * 0.12f), r * 0.55f, IM_COL32(188, 34, 38, a - 20), 16);
//                }
//
//                const char* gameOverTitle = "GAME OVER";
//                ImFont* font = ImGui::GetFont();
//                const float goTitleSizePx = 70.0f;
//                const ImVec2 goTitleSize = font->CalcTextSizeA(goTitleSizePx, FLT_MAX, 0.0f, gameOverTitle);
//                const float goTitleX = (screen.x - goTitleSize.x) * 0.5f;
//                const float goTitleY = (screen.y * 0.5f) - 140.0f;
//                bg->AddText(font, goTitleSizePx, ImVec2(goTitleX + 3.0f, goTitleY + 3.0f), IM_COL32(0, 0, 0, 130), gameOverTitle);
//                bg->AddText(font, goTitleSizePx, ImVec2(goTitleX, goTitleY), IM_COL32(255, 235, 235, 255), gameOverTitle);
//
//                ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
//                ImGui::SetNextWindowSize(screen, ImGuiCond_Always);
//                ImGui::SetNextWindowBgAlpha(0.0f);
//                ImGui::Begin("Game Over Overlay",
//                    nullptr,
//                    ImGuiWindowFlags_NoDecoration |
//                    ImGuiWindowFlags_NoMove |
//                    ImGuiWindowFlags_NoSavedSettings |
//                    ImGuiWindowFlags_NoBackground);
//
//                auto gmView = registry.view<GAME::ScoreManager>();
//                if (gmView.begin() != gmView.end())
//                {
//                    auto& score = registry.get<GAME::ScoreManager>(*gmView.begin());
//                    char finalScoreText[64] = {};
//                    char highScoreText[64] = {};
//                    std::snprintf(finalScoreText, sizeof(finalScoreText), "Final Score: %d", score.score);
//                    std::snprintf(highScoreText, sizeof(highScoreText), "High Score: %d", score.highScore);
//                    const ImVec2 finalSize = ImGui::CalcTextSize(finalScoreText);
//                    const ImVec2 highSize = ImGui::CalcTextSize(highScoreText);
//                    ImGui::SetCursorPos(ImVec2((screen.x - finalSize.x) * 0.5f, goTitleY + goTitleSize.y + 18.0f));
//                    ImGui::Text("%s", finalScoreText);
//                    ImGui::SetCursorPos(ImVec2((screen.x - highSize.x) * 0.5f, goTitleY + goTitleSize.y + 42.0f));
//                    ImGui::Text("%s", highScoreText);
//                }
//
//                const float buttonW = 220.0f;
//                const float buttonH = 46.0f;
//                const float buttonX = (screen.x - buttonW) * 0.5f;
//                ImGui::SetCursorPos(ImVec2(buttonX, goTitleY + goTitleSize.y + 80.0f));
//                if (ImGui::Button("Main Menu"))
//                {
//                    auto allGameOver = registry.view<GAME::GameOver>();
//                    for (auto e : allGameOver)
//                        registry.remove<GAME::GameOver>(e);
//                    auto gameManagerView = registry.view<GAME::GameManager>();
//                    if (!gameManagerView.empty())
//                    {
//                        auto gm = gameManagerView.front();
//                        registry.emplace_or_replace<GAME::MainMenu>(gm);
//                        if (registry.ctx().contains<GW::AUDIO::GMusic>())
//                            registry.ctx().get<GW::AUDIO::GMusic>().Stop();
//                    }
//                }
//                // Restart button: reloads level 1 and starts music - Elijah McCleave
//                ImGui::SetCursorPosX(buttonX);
//                if (ImGui::Button("Restart", ImVec2(buttonW, buttonH)))
//                {
//                    auto allGameOver = registry.view<GAME::GameOver>();
//                    for (auto e : allGameOver)
//                        registry.remove<GAME::GameOver>(e);
//                    auto gameManagerView = registry.view<GAME::GameManager>();
//                    if (!gameManagerView.empty())
//                    {
//                        auto gm = gameManagerView.front();
//                        if (registry.ctx().contains<GW::AUDIO::GMusic>())
//                            registry.ctx().get<GW::AUDIO::GMusic>().Play(true);
//                        registry.emplace_or_replace<GAME::LevelLoadRequest>(gm, GAME::LevelLoadRequest{ 1 });
//                    }
//                }
//                // Game over Quit: marks window closed and sets shutdown flag - Elijah McCleave
//                ImGui::SetCursorPosX(buttonX);
//                if (ImGui::Button("Quit", ImVec2(buttonW, buttonH)))
//                {
//                    requestWindowClose = true;
//                }
//                ImGui::End();
//            }
//            else if (!mainMenuActive)
//            {
//                // Fallback HUD - only when not on main menu (hides score on main menu) - Elijah McCleave
//                io.MouseDrawCursor = false;
//                auto gmView = registry.view<GAME::ScoreManager>();
//                if (gmView.begin() != gmView.end())
//                {
//                    auto& score = registry.get<GAME::ScoreManager>(*gmView.begin());
//                    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
//                    ImGui::SetNextWindowBgAlpha(0.35f);
//                    ImGui::Begin("HUD",
//                        nullptr,
//                        ImGuiWindowFlags_NoDecoration |
//                        ImGuiWindowFlags_AlwaysAutoResize |
//                        ImGuiWindowFlags_NoInputs);
//                    ImGui::Text("Score: %d", score.score);
//                    ImGui::End();
//                }
//            }
//        }
//        auto win = registry.get<GW::SYSTEM::GWindow>(entity);
//        //std::cout << "Device handle before frame work: " << vulkanRenderer.device << std::endl;
//        unsigned int frame;
//        vulkanRenderer.vlkSurface.GetSwapchainCurrentImage(frame);
//        VkCommandBuffer commandBuffer;
//        unsigned int currentBuffer;
//        vulkanRenderer.vlkSurface.GetSwapchainCurrentImage(currentBuffer);
//        vulkanRenderer.vlkSurface.GetCommandBuffer(currentBuffer, (void**)&commandBuffer);
//        unsigned int windowWidth, windowHeight;
//        win.GetClientWidth(windowWidth);
//        win.GetClientHeight(windowHeight);
//        VkViewport viewport = CreateViewportFromWindowDimensions(windowWidth, windowHeight);
//        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
//        VkRect2D scissor = CreateScissorFromWindowDimensions(windowWidth, windowHeight);
//        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
//        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanRenderer.pipeline);
//        // Update uniform and storage buffers
//        registry.patch<VulkanUniformBuffer>(entity);
//
//        // TODO: Build draw instructions
//        auto instances = registry.view<GeometryData, GPUInstance>(entt::exclude<DoNotRender>);
//
//        // --- Optimized Sorting - Dylan W ---
//        struct DrawItem {
//            GeometryData geo;
//            GPUInstance gpu;
//        };
//
//        std::vector<DrawItem> renderList;
//        renderList.reserve(instances.size_hint());
//
//        for (auto ent : instances) {
//            renderList.push_back({ registry.get<GeometryData>(ent), registry.get<GPUInstance>(ent) });
//        }
//
//        // We sort our local list instead of asking EnTT to physically swap memory arrays around every single frame
//        std::sort(renderList.begin(), renderList.end(), [](const DrawItem& a, const DrawItem& b) {
//            if (a.geo.materialIndex != b.geo.materialIndex) return a.geo.materialIndex < b.geo.materialIndex;
//            return a.geo.indexStart < b.geo.indexStart;
//            });
//        // -------------------------
//
//        // --- Optimized batching algorithm - Dylan W ---
//        std::vector<std::pair<GeometryData, int>> drawInstructions;
//        std::vector<GPUInstance> bufferList;
//        bufferList.reserve(renderList.size());
//
//        for (const auto& item : renderList) {
//            bufferList.push_back(item.gpu);
//
//            // If the list is empty or this geometry is different from the last one we checked, start a new batch
//            if (drawInstructions.empty() ||
//                drawInstructions.back().first.materialIndex != item.geo.materialIndex ||
//                drawInstructions.back().first.indexStart != item.geo.indexStart)
//            {
//                drawInstructions.push_back({ item.geo, 1 });
//            }
//            else
//            {
//                // Otherwise, just increment the counter of the current batch
//                drawInstructions.back().second++;
//            }
//        }
//
//        // Check for presence of the buffers first as they take a few frames before they are created
//        if (registry.all_of<VulkanVertexBuffer, VulkanIndexBuffer>(entity))
//        {
//            auto& vertexBuffer = registry.get<VulkanVertexBuffer>(entity);
//            auto& indexBuffer = registry.get<VulkanIndexBuffer>(entity);
//            if (vertexBuffer.buffer != VK_NULL_HANDLE && indexBuffer.buffer != VK_NULL_HANDLE)
//            {
//                VkDeviceSize offsets[] = { 0 };
//                vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer.buffer, offsets);
//                vkCmdBindIndexBuffer(commandBuffer, indexBuffer.buffer, 0, VkIndexType::VK_INDEX_TYPE_UINT32);
//            }
//
//            // --- Optimized memory transfer ---
//            registry.emplace<std::vector<GPUInstance>>(entity, std::move(bufferList));
//            registry.patch<VulkanGPUInstanceBuffer>(entity);
//            // ---------------------------------
//
//            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
//                vulkanRenderer.pipelineLayout, 0, 1, &vulkanRenderer.descriptorSets[frame], 0, nullptr);
//            // TODO: Draw all the things that need drawing
//            unsigned instanceCount = 0;
//            for (const auto& draw : drawInstructions) // Updated to const ref for performance - DW
//            {
//                int matIndex = draw.first.materialIndex;
//                // Safety check
//                if (matIndex < 0 || matIndex >= (int)vulkanRenderer.textureSets.size())
//                {
//                    matIndex = 0;
//                }
//                VkDescriptorSet texSet = vulkanRenderer.textureSets[matIndex];
//                if (texSet == VK_NULL_HANDLE)
//                    texSet = vulkanRenderer.textureSets[0];
//                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
//                    vulkanRenderer.pipelineLayout, 1, 1, &texSet, 0, nullptr);
//                vkCmdDrawIndexed(commandBuffer, draw.first.indexCount, draw.second, draw.first.indexStart,
//                    draw.first.vertexStart, instanceCount);
//                instanceCount += draw.second;
//            }
//        }
//        ImGui::Render();
//        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
//        vulkanRenderer.vlkSurface.EndFrame(true);
//        if (requestWindowClose)
//            registry.emplace_or_replace<APP::WindowClosed>(entity);
//    }
//    // run this code when a VulkanRenderer component is updated
//    void Destroy_VulkanRenderer(entt::registry& registry, entt::entity entity)
//    {
//        auto& vulkanRenderer = registry.get<VulkanRenderer>(entity);
//        vulkanRenderer.shuttingDown = true;
//        if (g_GatewareWndProc && registry.all_of<GW::SYSTEM::GWindow>(entity)) {
//            GW::SYSTEM::GWindow win = registry.get<GW::SYSTEM::GWindow>(entity);
//            GW::SYSTEM::UNIVERSAL_WINDOW_HANDLE uwh;
//            win.GetWindowHandle(uwh);
//            SetWindowLongPtr((HWND)uwh.window, GWLP_WNDPROC, (LONG_PTR)g_GatewareWndProc);
//        }
//        DestroyAllTextures(vulkanRenderer, true);
//        registry.remove<VulkanIndexBuffer>(entity);
//        registry.remove<VulkanVertexBuffer>(entity);
//        registry.remove<VulkanGPUInstanceBuffer>(entity);
//        registry.remove<VulkanUniformBuffer>(entity);
//    }
//    // Use this MACRO to connect the EnTT Component Logic
//    CONNECT_COMPONENT_LOGIC() {
//        // register the Window component's logic
//        registry.on_construct<VulkanRenderer>().connect<Construct_VulkanRenderer>();
//        registry.on_update<VulkanRenderer>().connect<Update_VulkanRenderer>();
//        registry.on_destroy<VulkanRenderer>().connect<Destroy_VulkanRenderer>();
//    }
//} // namespace DRAW
