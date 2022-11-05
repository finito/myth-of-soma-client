#ifndef RESOURCE_LOADER_H
#define RESOURCE_LOADER_H

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <SFML/Config.hpp>
#include "Framework/Assets/AsyncFileLoad.h"

namespace Framework
{

template<typename T>
class ResourceLoader
{
	typedef boost::shared_ptr<T> ResourcePtr;

public:
	ResourcePtr operator()(const std::string& name)
	{
		ResourcePtr resource = boost::make_shared<T>();
		resource->loadFromFile(name);
		return resource;
		//AsyncLoadItem* item = AsyncLoad(name);
		//if (item != nullptr)
		//{
		//	return boost::make_shared<T>(item);
		//}
		//return boost::make_shared<T>();
	}
};

template<typename T>
class TextureResourceLoader2
{
	typedef boost::shared_ptr<T> ResourcePtr;

public:
	ResourcePtr operator()(const std::string& name, sf::Uint8 format)
	{
		ResourcePtr resource = boost::make_shared<T>();
		resource->loadFromFile(name, format);
		return resource;
		//AsyncLoadItem* item = AsyncLoad(name);
		//if (item != nullptr)
		//{
		//	return boost::make_shared<T>(item);
		//}
		//return boost::make_shared<T>();
	}
};

} // namespace Framework

#endif // RESOURCE_LOADER_H