#ifndef TEXTURE_ATLAS_RESOURCE_H
#define TEXTURE_ATLAS_RESOURCE_H

#include <boost/shared_ptr.hpp>
#include "Framework/Graphics/TextureAtlas.h"
#include "AsyncFileLoad.h"

namespace Framework
{

class TextureAtlasResource : public TextureAtlas
{
public:
	TextureAtlasResource() : TextureAtlas(), item_(nullptr)
	{
	}

	explicit TextureAtlasResource(AsyncLoadItem* item) : TextureAtlas(), item_(item)
	{
	}

	bool CheckLoad()
	{
		justLoaded_ = false;
		if (item_ != nullptr && item_->overlapped.hEvent == 0)
		{
			//loadFromMemory(item_->buffer, item_->size);
			//free(item_->buffer);
			//free(item_);
			//item_ = nullptr;
			//justLoaded_ = true;
		}
		return justLoaded_;
	}

private:
	AsyncLoadItem* item_;
	bool justLoaded_;
};

} // namespace Framework

#endif // TEXTURE_ATLAS_RESOURCE_H