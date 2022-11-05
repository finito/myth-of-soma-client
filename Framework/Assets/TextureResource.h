#ifndef TEXTURE_RESOURCE_H
#define TEXTURE_RESOURCE_H

#include <boost/shared_ptr.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "Framework/Assets/AsyncFileLoad.h"

namespace Framework
{

class TextureResource : public sf::Texture
{
public:
	TextureResource() : sf::Texture()//, image_(nullptr)
	{
	}

	explicit TextureResource(AsyncLoadItem* item, sf::Uint8 format) : sf::Texture(), item_(item), format_(format), loaded_(false)
	{
	}

	//void SetImage(sf::Image* image)
	//{
	//	image_ = image;
	//}

	//bool CheckLoad()
	//{
	//	justLoaded_ = false;
	//	if (image_ != nullptr)
	//	{
	//		loadFromImage(*image_);
	//		delete image_;
	//		image_ = nullptr;
	//		justLoaded_ = true;
	//	}
	//	return justLoaded_;
	//}

	bool CheckLoad()
	{
		justLoaded_ = false;
		if (item_ != nullptr && item_->overlapped.hEvent == 0)
		{
			loadFromMemory(item_->buffer, item_->size, format_);
			free(item_->buffer);
			free(item_);
			item_ = nullptr;
			justLoaded_ = true;
			loaded_ = true;
		}
		return justLoaded_;
	}

	bool IsLoaded() const
	{
		return loaded_;
	}

private:
	AsyncLoadItem* item_;
	bool justLoaded_;
	//sf::Image* image_;
	sf::Uint8 format_;
	bool loaded_;
};

} // namespace Framework

#endif // TEXTURE_RESOURCE_H