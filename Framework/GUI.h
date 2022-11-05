#ifndef GUI_H
#define GUI_H

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/CEGUIOpenGLRenderer.h>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <string>

namespace Framework
{

class GUI
{
public:
	GUI();

	void Initialize(const std::string& folder, int width, int height);

	bool HandleEvent(const sf::RenderWindow& w, sf::Event e);

	void LoadScheme(const std::string& filename);
	void LoadImageset(const std::string& filename);

	void SetParentWindow(CEGUI::Window& window);
	void AddChildWindow(CEGUI::Window&);

	CEGUI::System& GetSystem() const;
	CEGUI::Texture& GUI::CreateTexture();

	void FireEvent(const CEGUI::String windowName, const CEGUI::String eventName);

private:
	CEGUI::MouseButton SFMLMouseButtonToCEGUIMouseButton(sf::Mouse::Button button);
	CEGUI::Key::Scan   SFMLKeyToCEGUIKey(sf::Keyboard::Key code);

private:
	CEGUI::System*         system_;
	CEGUI::OpenGLRenderer* renderer_;
	CEGUI::Window*		   root_;
};

} // namespace Framework

#endif // GUI_H