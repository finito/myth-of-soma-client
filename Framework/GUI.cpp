
#include "GUI.h"
#include <iostream>

namespace Framework
{

GUI::GUI() :
renderer_(nullptr),
system_(nullptr),
root_(nullptr)
{
}

void GUI::Initialize(const std::string& folder, const int width, const int height)
{
	try
	{
		// Create the CEGUI system with a renderer
		renderer_ = &CEGUI::OpenGLRenderer::create(CEGUI::Size(static_cast<float>(width),
			static_cast<float>(height)));
		system_   = &CEGUI::System::create(*renderer_);

		// Initialize the resource provider
		CEGUI::DefaultResourceProvider* rp =
			static_cast<CEGUI::DefaultResourceProvider*>(system_->getResourceProvider());

		// Set resource group directories
		rp->setResourceGroupDirectory("schemes",	folder + "/schemes/");
		rp->setResourceGroupDirectory("imagesets",	folder + "/imagesets/");
		rp->setResourceGroupDirectory("fonts",		folder + "/fonts/");
		rp->setResourceGroupDirectory("layouts",	folder + "/layouts/");
		rp->setResourceGroupDirectory("looknfeels", folder + "/looknfeel/");
		rp->setResourceGroupDirectory("animations", folder + "/animations");

		// Set default resource groups
		CEGUI::Imageset::setDefaultResourceGroup("imagesets");
		CEGUI::Font::setDefaultResourceGroup("fonts");
		CEGUI::Scheme::setDefaultResourceGroup("schemes");
		CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
		CEGUI::WindowManager::setDefaultResourceGroup("layouts");
		CEGUI::AnimationManager::setDefaultResourceGroup("animations");
	}
	catch(CEGUI::Exception& e)
	{
		std::cerr << "CEGUI Exception: " << e.what() << std::endl;
		throw std::runtime_error("GUI: Failed to initialize.");
	}
}

void GUI::LoadScheme(const std::string& filename)
{
	CEGUI::SchemeManager::getSingleton().create(filename);
}

void GUI::LoadImageset(const std::string& filename)
{
	CEGUI::ImagesetManager::getSingleton().create(filename);
}

void GUI::SetParentWindow(CEGUI::Window& window)
{
	root_ = system_->setGUISheet(&window);
}

void GUI::AddChildWindow(CEGUI::Window& window)
{
	if (root_)
	{
		root_->addChildWindow(&window);
	}
}

CEGUI::System& GUI::GetSystem() const
{
	if (system_ == nullptr)
	{
		throw std::runtime_error("GUI: CEGUI System has not been created!");
	}
	return *system_;
}

CEGUI::MouseButton GUI::SFMLMouseButtonToCEGUIMouseButton(sf::Mouse::Button button)
{
	switch (button)
	{
	case sf::Mouse::Left:
		return CEGUI::LeftButton;
	case sf::Mouse::Right:
		return CEGUI::RightButton;
	case sf::Mouse::Middle:
		return CEGUI::MiddleButton;
	case sf::Mouse::XButton1:
		return CEGUI::X1Button;
	case sf::Mouse::XButton2:
		return CEGUI::X2Button;
	default:
		return CEGUI::NoButton;
	}
	return CEGUI::NoButton;
}

CEGUI::Key::Scan GUI::SFMLKeyToCEGUIKey(sf::Keyboard::Key code)
{
	switch (code)
	{
	case sf::Keyboard::BackSpace:
		return CEGUI::Key::Backspace;
	case sf::Keyboard::Tab:
		return CEGUI::Key::Tab;
	case sf::Keyboard::Return:
		return CEGUI::Key::Return;
	case sf::Keyboard::Pause:
		return CEGUI::Key::Pause;
	case sf::Keyboard::Escape:
		return CEGUI::Key::Escape;
	case sf::Keyboard::Space:
		return CEGUI::Key::Space;
	case sf::Keyboard::Comma:
		return CEGUI::Key::Comma;
	case sf::Keyboard::Dash:
		return CEGUI::Key::Minus;
	case sf::Keyboard::Period:
		return CEGUI::Key::Period;
	case sf::Keyboard::Slash:
		return CEGUI::Key::Slash;
	case sf::Keyboard::Num0:
		return CEGUI::Key::Zero;
	case sf::Keyboard::Num1:
		return CEGUI::Key::One;
	case sf::Keyboard::Num2:
		return CEGUI::Key::Two;
	case sf::Keyboard::Num3:
		return CEGUI::Key::Three;
	case sf::Keyboard::Num4:
		return CEGUI::Key::Four;
	case sf::Keyboard::Num5:
		return CEGUI::Key::Five;
	case sf::Keyboard::Num6:
		return CEGUI::Key::Six;
	case sf::Keyboard::Num7:
		return CEGUI::Key::Seven;
	case sf::Keyboard::Num8:
		return CEGUI::Key::Eight;
	case sf::Keyboard::Num9:
		return CEGUI::Key::Nine;
	case sf::Keyboard::SemiColon:
		return CEGUI::Key::Semicolon;
	case sf::Keyboard::Equal:
		return CEGUI::Key::Equals;
	case sf::Keyboard::LBracket:
		return CEGUI::Key::LeftBracket;
	case sf::Keyboard::BackSlash:
		return CEGUI::Key::Backslash;
	case sf::Keyboard::RBracket:
		return CEGUI::Key::RightBracket;
	case sf::Keyboard::A:
		return CEGUI::Key::A;
	case sf::Keyboard::B:
		return CEGUI::Key::B;
	case sf::Keyboard::C:
		return CEGUI::Key::C;
	case sf::Keyboard::D:
		return CEGUI::Key::D;
	case sf::Keyboard::E:
		return CEGUI::Key::E;
	case sf::Keyboard::F:
		return CEGUI::Key::F;
	case sf::Keyboard::G:
		return CEGUI::Key::G;
	case sf::Keyboard::H:
		return CEGUI::Key::H;
	case sf::Keyboard::I:
		return CEGUI::Key::I;
	case sf::Keyboard::J:
		return CEGUI::Key::J;
	case sf::Keyboard::K:
		return CEGUI::Key::K;
	case sf::Keyboard::L:
		return CEGUI::Key::L;
	case sf::Keyboard::M:
		return CEGUI::Key::M;
	case sf::Keyboard::N:
		return CEGUI::Key::N;
	case sf::Keyboard::O:
		return CEGUI::Key::O;
	case sf::Keyboard::P:
		return CEGUI::Key::P;
	case sf::Keyboard::Q:
		return CEGUI::Key::Q;
	case sf::Keyboard::R:
		return CEGUI::Key::R;
	case sf::Keyboard::S:
		return CEGUI::Key::S;
	case sf::Keyboard::T:
		return CEGUI::Key::T;
	case sf::Keyboard::U:
		return CEGUI::Key::U;
	case sf::Keyboard::V:
		return CEGUI::Key::V;
	case sf::Keyboard::W:
		return CEGUI::Key::W;
	case sf::Keyboard::X:
		return CEGUI::Key::X;
	case sf::Keyboard::Y:
		return CEGUI::Key::Y;
	case sf::Keyboard::Z:
		return CEGUI::Key::Z;
	case sf::Keyboard::Delete:
		return CEGUI::Key::Delete;
	case sf::Keyboard::Numpad0:
		return CEGUI::Key::Numpad0;
	case sf::Keyboard::Numpad1:
		return CEGUI::Key::Numpad1;
	case sf::Keyboard::Numpad2:
		return CEGUI::Key::Numpad2;
	case sf::Keyboard::Numpad3:
		return CEGUI::Key::Numpad3;
	case sf::Keyboard::Numpad4:
		return CEGUI::Key::Numpad4;
	case sf::Keyboard::Numpad5:
		return CEGUI::Key::Numpad5;
	case sf::Keyboard::Numpad6:
		return CEGUI::Key::Numpad6;
	case sf::Keyboard::Numpad7:
		return CEGUI::Key::Numpad7;
	case sf::Keyboard::Numpad8:
		return CEGUI::Key::Numpad8;
	case sf::Keyboard::Numpad9:
		return CEGUI::Key::Numpad9;
	case sf::Keyboard::Divide:
		return CEGUI::Key::Divide;
	case sf::Keyboard::Multiply:
		return CEGUI::Key::Multiply;
	case sf::Keyboard::Subtract:
		return CEGUI::Key::Subtract;
	case sf::Keyboard::Add:
		return CEGUI::Key::Add;
	case sf::Keyboard::Up:
		return CEGUI::Key::ArrowUp;
	case sf::Keyboard::Down:
		return CEGUI::Key::ArrowDown;
	case sf::Keyboard::Right:
		return CEGUI::Key::ArrowRight;
	case sf::Keyboard::Left:
		return CEGUI::Key::ArrowLeft;
	case sf::Keyboard::Insert:
		return CEGUI::Key::Insert;
	case sf::Keyboard::Home:
		return CEGUI::Key::Home;
	case sf::Keyboard::End:
		return CEGUI::Key::End;
	case sf::Keyboard::PageUp:
		return CEGUI::Key::PageUp;
	case sf::Keyboard::PageDown:
		return CEGUI::Key::PageDown;
	case sf::Keyboard::F1:
		return CEGUI::Key::F1;
	case sf::Keyboard::F2:
		return CEGUI::Key::F2;
	case sf::Keyboard::F3:
		return CEGUI::Key::F3;
	case sf::Keyboard::F4:
		return CEGUI::Key::F4;
	case sf::Keyboard::F5:
		return CEGUI::Key::F5;
	case sf::Keyboard::F6:
		return CEGUI::Key::F6;
	case sf::Keyboard::F7:
		return CEGUI::Key::F7;
	case sf::Keyboard::F8:
		return CEGUI::Key::F8;
	case sf::Keyboard::F9:
		return CEGUI::Key::F9;
	case sf::Keyboard::F10:
		return CEGUI::Key::F10;
	case sf::Keyboard::F11:
		return CEGUI::Key::F11;
	case sf::Keyboard::F12:
		return CEGUI::Key::F12;
	case sf::Keyboard::F13:
		return CEGUI::Key::F13;
	case sf::Keyboard::F14:
		return CEGUI::Key::F14;
	case sf::Keyboard::F15:
		return CEGUI::Key::F15;
	case sf::Keyboard::RShift:
		return CEGUI::Key::RightShift;
	case sf::Keyboard::LShift:
		return CEGUI::Key::LeftShift;
	case sf::Keyboard::RControl:
		return CEGUI::Key::RightControl;
	case sf::Keyboard::LControl:
		return CEGUI::Key::LeftControl;
	case sf::Keyboard::RAlt:
		return CEGUI::Key::RightAlt;
	case sf::Keyboard::LAlt:
		return CEGUI::Key::LeftAlt;
	case sf::Keyboard::LSystem:
		return CEGUI::Key::LeftWindows;
	case sf::Keyboard::RSystem:
		return CEGUI::Key::RightWindows;
	default:
		return static_cast<CEGUI::Key::Scan>(0);
	}
	return static_cast<CEGUI::Key::Scan>(0);
}

bool GUI::HandleEvent(const sf::RenderWindow& w, const sf::Event e)
{
	switch (e.type)
	{
	case sf::Event::TextEntered:
		return system_->injectChar(e.text.unicode);
	case sf::Event::KeyPressed:
		return system_->injectKeyDown(SFMLKeyToCEGUIKey(e.key.code));
	case sf::Event::KeyReleased:
		return system_->injectKeyUp(SFMLKeyToCEGUIKey(e.key.code));
	case sf::Event::MouseMoved:
		return system_->injectMousePosition(static_cast<float>(sf::Mouse::getPosition(w).x),
			static_cast<float>(sf::Mouse::getPosition(w).y));
	case sf::Event::MouseButtonPressed:
		return system_->injectMouseButtonDown(SFMLMouseButtonToCEGUIMouseButton(e.mouseButton.button));
	case sf::Event::MouseButtonReleased:
		return system_->injectMouseButtonUp(SFMLMouseButtonToCEGUIMouseButton(e.mouseButton.button));
	case sf::Event::MouseWheelMoved:
		return system_->injectMouseWheelChange(static_cast<float>(e.mouseWheel.delta));
		// default:
		// std::cout << "GUI HandleEvent: Unknown event type" << std::endl;
	}
	return false;
}

CEGUI::Texture& GUI::CreateTexture()
{
	return renderer_->createTexture();
}

void GUI::FireEvent(const CEGUI::String windowName, const CEGUI::String eventName)
{
	CEGUI::Window* window = CEGUI::System::getSingleton().getGUISheet()->getChild(windowName);
	if (window && window->isVisible())
	{
		window->fireEvent(eventName, CEGUI::EventArgs());
	}
}

} // namespace Framework