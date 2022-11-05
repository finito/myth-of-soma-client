#ifndef THREADED_RESOURCE_CACHE_H
#define THREADED_RESOURCE_CACHE_H

#include <map>
#include <boost/shared_ptr.hpp>
#include <string>
#include <algorithm>
#include "ResourceLoader.h"
#include "TextureResource.h"
#include "TextureAtlasResource.h"
#include "AnimationResource.h"
#include "Framework/Assets/TextureResourceLoader.h"

namespace Framework
{

template<typename Resource, typename Generator>
class ThreadedResourceCache
{
public:
	typedef boost::shared_ptr<Resource> ResourcePtr;

	ThreadedResourceCache()
	{
		boost::thread(boost::bind(&ThreadedResourceCache::ProcessQueue, this));
	}

	ResourcePtr Get(const std::string& locator)
	{
		boost::scoped_lock<boost::mutex> lock(mutex_);
		auto it = _resources.find(locator);
		if (it == _resources.end())
		{
			AddToQueue(locator);
			it = _resources.insert(std::make_pair(locator, nullptr)).first;
			return nullptr;
		}
		return it->second;
	}

private:
	void AddToQueue(const std::string& locator)
	{
		boost::scoped_lock<boost::mutex> lock(mutex_);
		queue_.push(locator);
		lock.unlock();
		conditionVariable_.notify_one();
	}

	void ProcessQueue()
	{
		while (true)
		{
			boost::scoped_lock<boost::mutex> lock(mutex_);
			while (queue_.empty())
			{
				conditionVariable_.wait(lock);
			}

			const std::string& locator = queue_.front();
			queue_.pop();

			ResourcePtr resource = _generator(locator);

			auto it = _resources.find(locator);
			if (it != _resources.end())
			{
				
				it->second = resource;
			}
		}
	}

	std::map<std::string, ResourcePtr> _resources;
	Generator _generator;

	std::queue<std::string> queue_;

	boost::mutex mutex_;
	boost::condition_variable conditionVariable_;
};

} // namespace Framework

#endif // THREADED_RESOURCE_CACHE_H