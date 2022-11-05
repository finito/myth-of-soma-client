#ifndef TEXTURE_RESOURCE_LOADER_H
#define TEXTURE_RESOURCE_LOADER_H

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "Framework/Assets/AsyncFileLoad.h"
#include <queue>
#include "Framework/Assets/TextureResource.h"
#include <SFML/System/Thread.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Clock.hpp>

namespace Framework
{

class TextureResourceLoader
{
	typedef boost::shared_ptr<TextureResource> ResourcePtr;

public:
	//struct ResourceData
	//{
	//	std::string filename;
	//	ResourcePtr resource;
	//};

	TextureResourceLoader()
	{
		//thread = new sf::Thread(&TextureResourceLoader::ProcessQueue, this);
		//thread->launch();
	}

	ResourcePtr operator()(const std::string& name, sf::Uint8 format)
	{
		//ResourceData resourceData;
		//resourceData.filename = name;
		//resourceData.resource = boost::make_shared<TextureResource>();
		//loadQueue_.push(resourceData);
		//return resourceData.resource;
		AsyncLoadItem* item = AsyncLoad(name);
		return boost::make_shared<TextureResource>(item, format);
	}

private:
	//void ProcessQueue()
	//{
	//	for (;;)
	//	{
	//		while (!loadQueue_.empty())
	//		{
	//			ResourceData resourceData = loadQueue_.front();
	//			sf::Image* image = new sf::Image();
	//			image->loadFromFile(resourceData.filename);
	//			resourceData.resource->SetImage(image);
	//			loadQueue_.pop();
	//		}
	//		sf::sleep(sf::milliseconds(1));
	//	}
	//}

	//std::queue<ResourceData> loadQueue_;
	//sf::Thread* thread;
};

} // namespace Framework

#endif // TEXTURE_RESOURCE_LOADER_H