#ifndef GUI_CHARACTER_RENDER_H
#define GUI_CHARACTER_RENDER_H

#include <CEGUI/CEGUI.h>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>

class CharacterRender
{
public:
	const static unsigned char HelmetOrHair = 0;
	const static unsigned char Body = 1;
	const static unsigned char Top = 3;
	const static unsigned char Pad = 4;
	const static unsigned char Boot = 5;
	const static unsigned char Weapon = 6;
	const static unsigned char Shield = 7;

	CharacterRender(CEGUI::String imagesetName, CEGUI::String imageName);
	~CharacterRender();

	void Update(CEGUI::Window& window, float deltaTime);

	void SetBody(unsigned short gender, unsigned short skin, unsigned short hair, bool devil);

	void SetEquipment(const unsigned char type, const unsigned short pictureId);

private:
	float elapsedTime_;
	unsigned int frame_;
	sf::RenderTexture renderTexture_;
	CEGUI::Texture* imagesetTexture_;
	CEGUI::Imageset* imageset_;
	sf::Sprite sprite_;

	unsigned short gender_;
	unsigned short skin_;
	unsigned short hair_;
	bool devil_;
	unsigned short equipmentPictureId_[8];
};

#endif // GUI_CHARACTER_RENDER_H