#ifndef UTILS_RANDOM_NUMBER_H
#define UTILS_RANDOM_NUMBER_H

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <ctime>

static boost::mt19937 gen(static_cast<unsigned int>(std::time(0)));

namespace Utils
{
	namespace Random
	{
		int Range(int a, int b);
	}
}

#endif // UTILS_RANDOM_NUMBER_H