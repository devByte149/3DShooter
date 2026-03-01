#include "../GAME/GameComponents.h"
#include "DrawComponents.h"
#include "../CCL.h"
#include "../UTIL/Utilities.h" // sheldon read ground settings from defaults.ini

#include <string> // sheldon string helpers
#include <cctype> // sheldon tolower

namespace DRAW
{
	// sheldon lowercase helper
	static std::string ToLower(const std::string& s)
	{
		std::string out;
		out.reserve(s.size());
		for (char c : s)
			out.push_back((char)std::tolower((unsigned char)c));
		return out;
	}

	// sheldon treat Ground/Ocean/Globe name variants as terrain
	static bool IsTerrainName(const std::string& name)
	{
		std::string lower = ToLower(name);
		return (lower.find("ground") != std::string::npos) ||
			(lower.find("ocean") != std::string::npos) ||
			(lower.find("globe") != std::string::npos);
	}

	// sheldon only Ground + Ocean scroll (globe should NOT scroll on level 3)
	static bool IsScrollingTerrainName(const std::string& name)
	{
		std::string lower = ToLower(name);
		return (lower.find("ground") != std::string::npos) ||
			(lower.find("ocean") != std::string::npos);
	}

	// sheldon choose config section for terrain
	static const char* TerrainSectionForName(const std::string& name)
	{
		std::string lower = ToLower(name);
		if (lower.find("ocean") != std::string::npos)
			return "Ocean";
		return "Ground";
	}

	// This takes the eneity and adds a CPU level to it allowing for it to hold the level information before processing it. - Tuff Genda
	void Construct_CPULevel(entt::registry& registry, entt::entity entity)
	{
		auto& CPU_Level = registry.get<CPULevel>(entity);

		GW::SYSTEM::GLog log;
		log.Create("Level_Load");
		log.EnableConsoleLogging(false);

		CPU_Level.levelData.LoadLevel(CPU_Level.levelPath.c_str(), CPU_Level.modelPath.c_str(), log);
	}

	// This does a similar thing as the CPU level but it works with vulkan to render each model passed into the GPU entity instead. - Tuff Genda
	void Construct_GPULevel(entt::registry& registry, entt::entity entity)
	{
		auto& CPU_Level = registry.get<CPULevel>(entity);

		// sheldon make sure these CPU arrays are refreshed for the new level
		registry.emplace<VulkanVertexBuffer>(entity);
		registry.emplace<VulkanIndexBuffer>(entity);
		registry.emplace<std::vector<H2B::VERTEX>>(entity, CPU_Level.levelData.levelVertices);
		registry.emplace<std::vector<unsigned int>>(entity, CPU_Level.levelData.levelIndices);

		// sheldon do NOT re-emplace ModelManager each level (avoid stale refs)
		if (!registry.ctx().contains<ModelManager>())
			registry.ctx().emplace<ModelManager>();
		else
			registry.ctx().get<ModelManager>().meshCollections.clear();

		// sheldon force gpu buffer rebuild using the new cpu arrays
		registry.patch<VulkanVertexBuffer>(entity);
		registry.patch<VulkanIndexBuffer>(entity);

		auto& modelList = registry.ctx().get<ModelManager>().meshCollections;

		// sheldon config names so we dont touch plane/bullet
		std::string playerName = "Plane";
		std::string bulletName = "Bullet";
		if (registry.ctx().contains<UTIL::Config>())
		{
			auto cfg = registry.ctx().get<UTIL::Config>().gameConfig;
			try { playerName = (*cfg).at("Player").at("model").as<std::string>(); }
			catch (...) {}
			try { bulletName = (*cfg).at("Bullet").at("model").as<std::string>(); }
			catch (...) {}
		}
		std::string playerLower = ToLower(playerName);
		std::string bulletLower = ToLower(bulletName);

		// sheldon grab terrain x offset so everything lines up
		float terrainOffsetX = 0.0f;
		bool foundTerrainOffset = false;
		for (auto& obj : CPU_Level.levelData.blenderObjects)
		{
			if (IsTerrainName(obj.blendername))
			{
				auto terrainMat = CPU_Level.levelData.levelTransforms[obj.transformIndex];
				terrainOffsetX = terrainMat.row4.x;
				foundTerrainOffset = true;
				break;
			}
		}

		for (auto& object : CPU_Level.levelData.blenderObjects)
		{
			auto& model = CPU_Level.levelData.levelModels[object.modelIndex];

			MeshCollection dynamicObjects;
			dynamicObjects.collider = CPU_Level.levelData.levelColliders[model.colliderIndex];

			for (int meshIndex = 0; meshIndex < model.meshCount; ++meshIndex)
			{
				auto meshEntity = registry.create();
				registry.emplace<DRAW::LevelEntity>(meshEntity); // sheldon this mesh belongs to the current level
				registry.emplace<DRAW::Name>(meshEntity, DRAW::Name{ object.blendername });

				auto& meshInfo = CPU_Level.levelData.levelMeshes[model.meshStart + meshIndex];

				GeometryData geodata;
				geodata.indexStart = model.indexStart + meshInfo.drawInfo.indexOffset;
				geodata.indexCount = meshInfo.drawInfo.indexCount;
				geodata.vertexStart = model.vertexStart;

				// sheldon IMPORTANT: match texture set index (0 = default white, materials start at 1)
				geodata.materialIndex = (model.materialStart + meshInfo.materialIndex) + 1;

				registry.emplace<GeometryData>(meshEntity, geodata);

				GPUInstance GPU_instance;
				GPU_instance.transform = CPU_Level.levelData.levelTransforms[object.transformIndex];

				// sheldon apply terrain x shift to everything else (not terrain)
				if (foundTerrainOffset && !IsTerrainName(object.blendername))
					GPU_instance.transform.row4.x += terrainOffsetX;

				// sheldon raise enemies to y=20 (name-based)
				{
					std::string n = ToLower(object.blendername);
					bool isWall = (n.find("wall") != std::string::npos);
					bool isPlayer = (n == playerLower);
					bool isBullet = (n == bulletLower) || (n.find("bullet") != std::string::npos);

					if (!IsTerrainName(object.blendername) && !isWall && !isPlayer && !isBullet)
						GPU_instance.transform.row4.y = 20.0f; // sheldon enemy height match
				}

				GPU_instance.matData = CPU_Level.levelData.levelMaterials[model.materialStart + meshInfo.materialIndex].attrib;
				registry.emplace<GPUInstance>(meshEntity, GPU_instance);

				if (model.isCollidable) {
					registry.emplace<GAME::Collidable>(meshEntity);
					registry.emplace<GAME::Obstacle>(meshEntity);
					registry.emplace<GAME::Transform>(meshEntity, GPU_instance.transform);
					registry.emplace<MeshCollection>(meshEntity, dynamicObjects);
				}

				// sheldon scroll only ground/ocean, not globe
				if (IsScrollingTerrainName(object.blendername))
				{
					float speed = 12.0f;
					int tileCount = 8;
					float overlapZ = 0.5f;
					float tileLengthZ = 50.0f;
					float scaleX = 1.0f;
					float scaleZ = 1.0f;

					if (registry.ctx().contains<UTIL::Config>())
					{
						auto cfg = registry.ctx().get<UTIL::Config>().gameConfig;
						const char* sec = TerrainSectionForName(object.blendername);
						try { speed = (*cfg).at(sec).at("speed").as<float>(); }
						catch (...) {}
						try { tileCount = (*cfg).at(sec).at("tiles").as<int>(); }
						catch (...) {}
						try { overlapZ = (*cfg).at(sec).at("overlap").as<float>(); }
						catch (...) {}
						try { tileLengthZ = (*cfg).at(sec).at("length").as<float>(); }
						catch (...) {}
						try { scaleX = (*cfg).at(sec).at("scaleX").as<float>(); }
						catch (...) {}
						try { scaleZ = (*cfg).at(sec).at("scaleZ").as<float>(); }
						catch (...) {}
					}

					if (tileCount < 2) tileCount = 2;
					if (tileLengthZ < 1.0f) tileLengthZ = 1.0f;
					if (overlapZ < 0.0f) overlapZ = 0.0f;
					if (overlapZ > (tileLengthZ - 0.01f)) overlapZ = tileLengthZ - 0.01f;
					if (scaleX < 0.01f) scaleX = 0.01f;
					if (scaleZ < 0.01f) scaleZ = 0.01f;

					GW::MATH::GMatrix::ScaleLocalF(GPU_instance.transform, GW::MATH::GVECTORF{ scaleX, 1.0f, scaleZ, 0.0f }, GPU_instance.transform);
					registry.get<GPUInstance>(meshEntity).transform = GPU_instance.transform;

					const float tileSpacingZ = tileLengthZ - overlapZ;
					const float wrapMinZ = -(tileLengthZ + tileSpacingZ * (tileCount - 1));
					const float wrapOffsetZ = tileSpacingZ * tileCount;

					registry.emplace_or_replace<GAME::GroundScroll>(meshEntity, GAME::GroundScroll{ speed, wrapMinZ, wrapOffsetZ });

					for (int i = 1; i < tileCount; ++i)
					{
						auto groundTile = registry.create();
						registry.emplace<DRAW::LevelEntity>(groundTile); // sheldon this tile belongs to the current level
						registry.emplace<DRAW::Name>(groundTile, DRAW::Name{ object.blendername });
						registry.emplace<GeometryData>(groundTile, geodata);

						GPUInstance tileGPU = GPU_instance;
						tileGPU.transform.row4.z += (tileSpacingZ * i);
						registry.emplace<GPUInstance>(groundTile, tileGPU);

						registry.emplace_or_replace<GAME::GroundScroll>(groundTile, GAME::GroundScroll{ speed, wrapMinZ, wrapOffsetZ });
					}
				}

				if (model.isDynamic)
				{
					registry.emplace<DoNotRender>(meshEntity);
					dynamicObjects.entities.push_back(meshEntity);
				}
			}

			if (model.isDynamic)
				modelList.insert({ object.blendername, dynamicObjects });
		}
	}

	// This destroys the collections of meshes for the level so that there is no memory leaks. - Tuff Genda
	void Destroy_MeshCollection(entt::registry& registry, entt::entity entity)
	{
		auto& collection = registry.get<MeshCollection>(entity);

		for (auto meshEnt : collection.entities) {
			if (registry.valid(meshEnt)) {
				registry.destroy(meshEnt);
			}
		}
	}

	// Use this MACRO to connect the EnTT Component Logic
	CONNECT_COMPONENT_LOGIC() {
		// register the Window component's logic
		registry.on_construct<CPULevel>().connect<Construct_CPULevel>();
		registry.on_construct<GPULevel>().connect<Construct_GPULevel>();
		registry.on_destroy<MeshCollection>().connect<Destroy_MeshCollection>();
	}

} // namespace DRAW
