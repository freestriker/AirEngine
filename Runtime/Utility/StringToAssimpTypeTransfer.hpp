#pragma once
#include "ContructorMacro.hpp"
#include "ExportMacro.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

namespace AirEngine
{
	namespace Runtime
	{
		namespace Utility
		{
			class AIR_ENGINE_API StringToAssimpTypeTransfer final
			{
			private:
				StringToAssimpTypeTransfer();
				~StringToAssimpTypeTransfer() = default;
				NO_COPY_MOVE(StringToAssimpTypeTransfer);

			public:
				static constexpr uint32_t DEFAULT_POST_PROCESS_STEPS =
					aiProcess_Triangulate |
					aiProcess_ImproveCacheLocality |
					0;
				static constexpr uint32_t INVALID_POST_PROCESS_STEPS =
					~(aiProcess_PreTransformVertices);
			private:
				const static std::unordered_map<std::string, uint32_t> AI_POST_PROCESS_STEPS_MAP;
			public:
				inline static uint32_t PostProcessStepsStringsToPostProcessSteps(const std::vector<std::string>& postProcessStepsStrings)
				{
					uint32_t postProcessSteps = DEFAULT_POST_PROCESS_STEPS;
					for (const auto& postProcessStep : postProcessStepsStrings)
					{
						postProcessSteps |= AI_POST_PROCESS_STEPS_MAP.at(postProcessStep);
					}
					postProcessSteps &= INVALID_POST_PROCESS_STEPS;

					return postProcessSteps;
				}
				inline static uint32_t RemoveInvalidPostProcessSteps(uint32_t postProcessSteps)
				{
					return postProcessSteps &= INVALID_POST_PROCESS_STEPS;
				}
			};
		}
	}
}