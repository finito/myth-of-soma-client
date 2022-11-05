#ifndef ANIMATION_RESOURCE_H
#define ANIMATION_RESOURCE_H

#include <boost/shared_ptr.hpp>
#include "Framework/Graphics/Animation.h"
#include "AsyncFileLoad.h"

namespace Framework
{

class AnimationResource : public Animation
{
public:
	AnimationResource() : Animation(), item_(nullptr)
	{
	}

	explicit AnimationResource(AsyncLoadItem* item) : Animation(), item_(item)
	{
	}

	void SetLoaded(bool value)
	{
		loaded_ = value;
	}

	bool CheckLoad()
	{
		return loaded_;
	}

	//bool CheckLoad()
	//{
	//	justLoaded_ = false;
	//	if (item_ != nullptr && item_->overlapped.hEvent == 0)
	//	{

	//		LoadFromMemory(item_->buffer, item_->size);
	//		free(item_->buffer);
	//		free(item_);
	//		item_ = nullptr;
	//		justLoaded_ = true;
	//	}
	//	return justLoaded_;
	//}

private:
	AsyncLoadItem* item_;
	bool justLoaded_;
	bool loaded_;
};

} // namespace Framework

#endif // ANIMATION_RESOURCE_H