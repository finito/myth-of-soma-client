#ifndef THREADED_RESOURCE_LOADER_H
#define THREADED_RESOURCE_LOADER_H

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <queue>

namespace Framework
{

template<typename T>
class ThreadedResourceLoader
{
	typedef boost::shared_ptr<T> ResourcePtr;

public:
	ThreadedResourceLoader()
	{
		boost::thread(boost::bind(&ProcessQueue, this));
	}

	ResourcePtr operator()(const std::string& name, sf::Uint8 format)
	{
		boost::mutex::scoped_lock lock(mutex_);
		ResourceData* resourceData = new ResourceData();
		resourceData->filename = name;
		resourceData->loaded = false;
		queue_.push(resourceData);
		lock.unlock();
		conditionVariable_.notify_one();
		return boost::make_shared<T>();
	}

private:
	static void ProcessQueue(ThreadedResourceLoader* loader)
	{
		while (true)
		{
			boost::mutex::scoped_lock lock(loader->mutex_);
			while (loader->queue_.empty())
			{
				loader->conditionVariable_.wait(lock);
			}

			ResourceData* resourceData = loader->queue_.front();
			loader->queue_.pop();

			resourceData->resource->loadFromFile(resourceData->filename);
			resourceData->loaded = true;
		}
	}

	struct ResourceData
	{
		std::string filename;
		sf::Image resource;
		bool loaded;
	};
	std::queue<ResourceData*> queue_;
	boost::mutex mutex_;
	boost::condition_variable conditionVariable_;
};

} // namespace Framework

#endif // THREADED_RESOURCE_LOADER_H