
#include "Framework/GameEngine.h"
#include "GUI/Elements/CustomItemListbox.h"
#include "LoginState.h"
//#include "DebugState.h"
#include <SFML/System/Clock.hpp>
#include <CEGUI/CEGUI.h>
#include <boost/make_shared.hpp>
#include <iostream>
#include <exception>

int main()
{
	std::ofstream file("sfml-log.txt");
	sf::err().rdbuf(file.rdbuf());

	std::ofstream normalLog("log.txt");
	std::cout.rdbuf(normalLog.rdbuf());

	std::ofstream errorLog("log-error.txt");
	std::cerr.rdbuf(errorLog.rdbuf());

	try
	{
		Framework::GameEngine engine;
#ifdef _DEBUG
		engine.Initialize("Myth of Soma", 800, 600, false);
#else
		engine.Initialize("Myth of Soma", 800, 600, false);
#endif
		engine.renderWindow.setMouseCursorVisible(false);
		
		engine.gui.Initialize("somagui", engine.renderWindow.getSize().x, engine.renderWindow.getSize().y);
		engine.gui.LoadScheme("WindowsLook.scheme");
		engine.gui.LoadScheme("Soma.scheme");
		engine.gui.GetSystem().setDefaultMouseCursor((CEGUI::utf8*)"cursors", (CEGUI::utf8*)"Normal1");
		engine.gui.GetSystem().setDefaultTooltip("Soma/Tooltip");

		CEGUI::WindowFactoryManager::getSingleton().addFactory<CEGUI::TplWindowFactory<CEGUI::CustomItemListbox>>();
		CEGUI::WindowFactoryManager::getSingleton().addWindowTypeAlias("CEGUI/ItemListbox", "CustomItemListbox");

		engine.ChangeState(boost::make_shared<LoginState>(engine));

		sf::Font font;
		if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
		{
			return 1;
		}

		sf::Text text;
		text.setFont(font);
		text.setColor(sf::Color::Green);
		text.setCharacterSize(20);

		int frameCount = 0;

		float mspfMin = -1.f;
		float mspfMax = -1.f;

		sf::Clock clock;
		sf::Clock avgTimer;

		sf::Clock updateTime;
		float updateTimeSec = 0;

		sf::Clock renderTime;
		float renderTimeSec = 0;

		//double accumulatedTime = 0;
		//const double fixedFrameTime = 1 / 5.0;

		//int loops = 0;
		//const int maxFrameSkip = 5;

		while (engine.Running())
		{
			float deltaTime = clock.restart().asSeconds();

			if (avgTimer.getElapsedTime().asSeconds() >= 1.f)
			{
				float mspf = 1000.f / frameCount;

				static int frameCount2 = 0;
				if (frameCount2 > 5)
				{
					if (mspfMin == -1 || mspfMin > mspf)
						mspfMin = mspf;

					if (mspfMax == -1 || mspfMax < mspf)
						mspfMax = mspf;
				}
				else
				{
					++frameCount2;
				}

				std::stringstream stream; 
				stream << "FPS:" << frameCount << " MSPF AVG:" << mspf << " MSPF MIN:" << mspfMin << " MAX:" << mspfMax << "\n Update=" << updateTimeSec * 1000.f << " Render=" << renderTimeSec * 1000.f;
				text.setString(stream.str());

				frameCount = 0;
				avgTimer.restart();
			}

			++frameCount;

			//accumulatedTime += deltaTime;
			//
			//loops = 0;
			//while (accumulatedTime > fixedFrameTime && loops < maxFrameSkip)
			//{
			//	accumulatedTime -= fixedFrameTime;
			//	++loops;
			//}

			updateTime.restart();
			engine.Update(deltaTime);
			updateTimeSec = updateTime.restart().asSeconds();

			renderTime.restart();
			engine.renderWindow.clear();
			engine.Render();

			engine.renderWindow.pushGLStates();
			engine.renderWindow.draw(text);
			engine.renderWindow.popGLStates();
			engine.renderWindow.display();
			renderTimeSec = renderTime.restart().asSeconds();
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "Unhandled exception caught in main: " << e.what() << std::endl;
	}
	catch(...)
	{
		std::cerr << "Unknown unhandled exception caught in main" << std::endl;
	}

	return 0;
}