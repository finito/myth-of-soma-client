
#include "RandomNumber.h"

namespace Utils
{
	namespace Random
	{
		int Range(int a, int b)
		{
			if (a > b) std::swap(a, b);
			boost::random::uniform_int_distribution<> dist(a, b);
			return dist(gen);
		}
	}
}