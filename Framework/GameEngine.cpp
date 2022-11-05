
#include "GameEngine.h"

namespace Framework
{

GameEngine::GameEngine() :
running_(true)
{
}

void GameEngine::Initialize(const std::string& title, const int width,
							const int height, const bool verticalSync)
{
	renderWindow.create(sf::VideoMode(width, height,
						 sf::VideoMode::getDesktopMode().bitsPerPixel), title);
	renderWindow.setVerticalSyncEnabled(verticalSync);
}

void GameEngine::Update(const float deltaTime)
{
	if (!stateStack_.empty() && renderWindow.isOpen())
	{
		stateStack_.back()->Update(deltaTime);
		for (auto it = stateStack_.begin(); it != stateStack_.end();)
		{
			if ((*it)->popMe_)
			{
				it = stateStack_.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
	else
	{
		Quit();
	}
}

void GameEngine::Render()
{
	if (!stateStack_.empty() && renderWindow.isOpen())
	{
		stateStack_.back()->Render();
	}
	else
	{
		Quit();
	}
}

void GameEngine::ChangeState(const boost::shared_ptr<IGameState>& state)
{
	if (!stateStack_.empty())
	{
		stateStack_.back()->popMe_ = true;
	}
	stateStack_.push_back(state);
}

//void GameEngine::PushState(const boost::shared_ptr<IGameState>& state)
//{
//	_stateStack.push_back(state);
//}
//
//void GameEngine::PopState()
//{
//	_stateStack.pop_back();
//}

bool GameEngine::Running() const
{
	return running_;
}

void GameEngine::Quit()
{
	running_ = false;
}

} // namespace Framework