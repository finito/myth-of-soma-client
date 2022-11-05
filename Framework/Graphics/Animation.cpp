
#include "Animation.h"
#include <fstream>
#include <iostream>

namespace Framework
{

Animation::Animation()
{
}

bool Animation::loadFromFile(const std::string& filename)
{
	std::ifstream file(filename.c_str());
	if (!file)
	{
		std::cerr << "Animation file could not be read filename=" << filename << "\n";
		return false;
	}
	return loadFromStream(file);
}

bool Animation::loadFromMemory(const void* data, std::size_t size)
{
	std::istringstream sstream(reinterpret_cast<const char*>(data));
	return loadFromStream(sstream);
}

bool Animation::loadFromStream(std::istream& stream)
{
	unsigned int animationCount;
	stream >> animationCount;

	animations_.reserve(animationCount);

	for (unsigned int i = 0; i < animationCount; ++i)
	{
		AnimationData animationData;

		unsigned int directionCount;
		stream >> directionCount;
		animationData.directionCount = directionCount;

		for (char j = 0; j < animationData.directionCount; ++j)
		{
			unsigned int keyFrameCount;
			stream >> keyFrameCount;
			animationData.keyFrameCount = keyFrameCount;

			animationData.keyFrames.reserve(animationData.keyFrameCount);
			for (short k = 0; k < animationData.keyFrameCount; ++k)
			{
				int keyFrame;
				stream >> keyFrame;
				animationData.keyFrames.push_back(keyFrame);
			}
		}
		unsigned int fps;
		stream >> fps;
		if (fps == 0)
		{
			fps = 10;
		}
		animationData.frameDuration = 1.0f / fps;

		animations_.push_back(animationData);
	}

	return true;
}

short Animation::GetKeyFrameByTime(const unsigned int animationNumber, const char direction, const float time, bool& finished, bool loop)
{
	finished = false;

	if (animationNumber >= animations_.size() || direction >= animations_[animationNumber].directionCount)
	{
		return 0;
	}

	short keyFrameNumber = static_cast<short>(time / animations_[animationNumber].frameDuration);
	const short keyFrameCount = animations_[animationNumber].keyFrameCount;
	if (!loop)
	{
		keyFrameNumber = std::min(static_cast<short>(keyFrameCount - 1), keyFrameNumber);
		finished = keyFrameNumber == keyFrameCount - 1 ? true : false;
	}
	else
	{
		keyFrameNumber = keyFrameNumber % keyFrameCount;
	}

	if (keyFrameNumber < 0 || keyFrameNumber >= keyFrameCount)
	{
		return 0;
	}

	return animations_[animationNumber].keyFrames[direction * keyFrameCount + keyFrameNumber];
}

short Animation::GetKeyFrameByFrameNumber(const unsigned int animationNumber, const char direction, const short keyFrameNumber)
{
	if (animationNumber >= animations_.size() || direction >= animations_[animationNumber].directionCount)
	{
		return 0;
	}

	short keyFrameCount = animations_[animationNumber].keyFrameCount;
	if (keyFrameNumber < 0 || keyFrameNumber >= keyFrameCount)
	{
		return 0;
	}

	return animations_[animationNumber].keyFrames[direction * keyFrameCount + keyFrameNumber];
}

float Animation::GetLength(const unsigned int animationNumber) const
{
	return animations_[animationNumber].frameDuration * animations_[animationNumber].keyFrameCount;
}

unsigned int Animation::GetKeyFrameCount(const unsigned int animationNumber) const
{
	return animations_[animationNumber].keyFrameCount;
}

} // namespace Framework