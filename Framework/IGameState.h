#ifndef IGAME_STATE_H
#define IGAME_STATE_H

#include <boost/shared_ptr.hpp>

namespace Framework
{

class GameEngine;

class IGameState
{
public:
	virtual ~IGameState() {}

	void Update(float deltaTime)
	{
		DoUpdate(deltaTime);
	}

	void Render()
	{
		DoRender();
	}

protected:
	IGameState() : popMe_(false)
	{
	}

	virtual void DoUpdate(float deltaTime) = 0;
	virtual void DoRender() = 0;

public:
	bool popMe_;
};

} // namespace Framework

#endif // IGAME_STATE_H