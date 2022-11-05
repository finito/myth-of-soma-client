#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <vector>
#include <boost/shared_ptr.hpp>
#include "IGameState.h"
#include <SFML/Graphics.hpp>
#include "GUI.h"

namespace Framework
{

class GameEngine
{
public:
	GameEngine();

	void Initialize(const std::string& title, int width = 800,
					int height = 600, bool verticalSync = true);

	void Update(float deltaTime);
	void Render();

	void ChangeState(const boost::shared_ptr<IGameState>& state);
	//void PushState(const boost::shared_ptr<IGameState>& state);
	//void PopState();

	bool Running() const;
	void Quit();

	sf::RenderWindow renderWindow;
	GUI gui;

private:
	bool running_;
	std::vector<boost::shared_ptr<IGameState>> stateStack_;
};

} // namespace Framework

#endif // GAME_ENGINE_H