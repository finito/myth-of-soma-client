#ifndef RESOURCE_CACHE_H
#define RESOURCE_CACHE_H

#include <map>
#include <boost/shared_ptr.hpp>
#include <string>
#include <algorithm>
#include "ResourceLoader.h"
#include "TextureResource.h"
#include "TextureAtlasResource.h"
#include "AnimationResource.h"
#include "Framework/Assets/TextureResourceLoader.h"
//#include "Framework/Assets/ThreadedResourceLoader.h"

namespace Framework
{

template<typename Resource, typename Generator>
class ResourceCache
{
public:
	typedef boost::shared_ptr<Resource> ResourcePtr;

	void SetBasePath(const std::string& basePath)
	{
		basePath_ = basePath;
	}

	ResourcePtr& Get(const std::string& locator)
	{
		auto it = _resources.find(locator);
		if (it == _resources.end())
		{
			ResourcePtr resource = _generator(basePath_ + locator);
			it = _resources.insert(std::make_pair(locator, resource)).first;
		}
		return it->second;
	}

	void CheckLoad()
	{
		for (auto it = _resources.begin(); it != _resources.end(); ++it)
		{
			(*it).second->CheckLoad();
		}
	}

private:
	std::map<std::string, ResourcePtr> _resources;
	Generator _generator;
	std::string basePath_;
};

template<typename Resource, typename Generator>
class TextureResourceCache
{
public:
	typedef boost::shared_ptr<Resource> ResourcePtr;

	void SetBasePath(const std::string& basePath)
	{
		basePath_ = basePath;
	}

	ResourcePtr Get(const std::string& locator, sf::Uint8 format)
	{
		auto it = _resources.find(locator);
		if (it == _resources.end())
		{
			ResourcePtr resource = _generator(basePath_ + locator, format);
			it = _resources.insert(std::make_pair(locator, resource)).first;
			loadQueue_.push_back(resource);
		}
		return it->second;
	}

	void CheckLoad()
	{
		for (auto it = loadQueue_.begin(); it != loadQueue_.end();)
		{
			ResourcePtr resource = (*it);
			if (resource->CheckLoad())
			{
				it = loadQueue_.erase(it);
				break;
			}
			else
			{
				++it;
			}
		}
	}

private:
	std::map<std::string, ResourcePtr> _resources;
	Generator _generator;
	std::string basePath_;
	std::vector<ResourcePtr> loadQueue_;
};

typedef TextureResourceCache<sf::Texture, TextureResourceLoader2<sf::Texture>> TextureCache;
extern TextureCache textureCache;

typedef ResourceCache<TextureAtlasResource, ResourceLoader<TextureAtlasResource>> TextureAtlasCache;
extern TextureAtlasCache textureAtlasCache;

typedef ResourceCache<AnimationResource, ResourceLoader<AnimationResource>> AnimationCache;
extern AnimationCache animationCache;

} // namespace Framework

#endif // RESOURCE_CACHE_H