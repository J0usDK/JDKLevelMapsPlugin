#pragma once
#include <string>
#include <atomic>

#include <CryCore/BaseTypes.h>

namespace JDKLevelMaps::Baking
{
	enum class EBakeStage : uint8
	{
		Idle = 0,
		ExtractingData,
		PrePassAnalysis,
		WritingTiles,
		Finished
	};

	struct SBakeProgress
	{
		std::atomic<EBakeStage> currentStage{ EBakeStage::Idle };

		std::atomic<float> writeProgress{ 0.0f };
		std::atomic<float> imageProgress{ 0.0f };

		std::atomic<bool> isCompleted{ false };
		std::atomic<bool> isSuccess{ false };

		std::string resultMessage;
	};
}