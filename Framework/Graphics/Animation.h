#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <sstream>

namespace Framework
{

class Animation
{
public:
	Animation();

	bool loadFromFile(const std::string& filename);
	bool loadFromMemory(const void* data, std::size_t size);

	short GetKeyFrameByTime(unsigned int animationNumber, char direction, float time, bool& finished, bool loop = false);
	short GetKeyFrameByFrameNumber(unsigned int animationNumber, char direction, short keyFrameNumber);

	float GetLength(unsigned int animationNumber) const;

	unsigned int GetKeyFrameCount(unsigned int animationNumber) const;

private:
	bool loadFromStream(std::istream& stream);

	struct AnimationData
	{
		char directionCount;
		short keyFrameCount;
		std::vector<short> keyFrames;
		float frameDuration;
	};
	std::vector<AnimationData> animations_;
};

} // namespace Framework

#endif // ANIMATION_H