
#include "GUI/SelectCharacterWindow.h"

#include <iostream>
#include <boost/format.hpp>

namespace GUI
{

using namespace CEGUI;

SelectCharacterWindow::SelectCharacterWindow() :
activeCharacterSlot_(0),
characterCount_(0)//,
//characterRender1_("CharacterRender1", "texture"),
//characterRender2_("CharacterRender2", "texture"),
//characterRender3_("CharacterRender3", "texture")
{
	try
	{
		characterRenderSlot_[0] = 1;
		characterRenderSlot_[1] = 2;
		characterRenderSlot_[2] = 3;

		window_ = WindowManager::getSingleton().loadWindowLayout("characterselectwindow.layout");
		window_->activate();	
		window_->subscribeEvent(Window::EventWindowUpdated, Event::Subscriber(&SelectCharacterWindow::HandleWindowUpdated, this));
		window_->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&SelectCharacterWindow::HandleKeyPress, this));

		AnimationManager::getSingletonPtr()->loadAnimationsFromXML("characterselect.animation");

		okButton_			= static_cast<PushButton*>(window_->getChild("CharacterSelect/OKButton"));
		
		exitButton_			= static_cast<PushButton*>(window_->getChild("CharacterSelect/ExitButton"));
		
		humanWorldButton_	= static_cast<PushButton*>(window_->getChild("CharacterSelect/HumanWorldButton"));
		
		devilWorldButton_	= static_cast<PushButton*>(window_->getChild("CharacterSelect/DevilWorldButton"));
		
		deleteButton_		= static_cast<PushButton*>(window_->getChild("CharacterSelect/DeleteButton"));
		
		selectServerButton_ = static_cast<PushButton*>(window_->getChild("CharacterSelect/SelectServerButton"));
		selectServerButton_->hide();

		rotateLeftButton_	= static_cast<PushButton*>(window_->getChild("CharacterSelect/RotateCharacterLeftButton"));
		rotateLeftButton_->disable();
		rotateLeftButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&SelectCharacterWindow::HandleRotateLeft, this));	
		
		rotateRightButton_	= static_cast<PushButton*>(window_->getChild("CharacterSelect/RotateCharacterRightButton"));
		rotateRightButton_->disable();
		rotateRightButton_->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&SelectCharacterWindow::HandleRotateRight, this));

		name_				= window_->getChild("CharacterSelect/Name");
		guildName_			= window_->getChild("CharacterSelect/Guild");
		age_				= window_->getChild("CharacterSelect/Age");
		position_			= window_->getChild("CharacterSelect/Position");
		moral_				= window_->getChild("CharacterSelect/Moral");
		level_				= window_->getChild("CharacterSelect/Level");
		health_				= window_->getChild("CharacterSelect/Health");
		mana_				= window_->getChild("CharacterSelect/Mana");
		stamina_			= window_->getChild("CharacterSelect/Stamina");
		mainWeapon_			= window_->getChild("CharacterSelect/MainWeapon");
		mainSkill_			= window_->getChild("CharacterSelect/MainSkill");
		strength_			= window_->getChild("CharacterSelect/Strength");
		intelligence_		= window_->getChild("CharacterSelect/Intelligence");
		dexterity_			= window_->getChild("CharacterSelect/Dexterity");
		wisdom_				= window_->getChild("CharacterSelect/Wisdom");
		charisma_			= window_->getChild("CharacterSelect/Charisma");
		constitution_		= window_->getChild("CharacterSelect/Constitution");
		messageBox_			= window_->getChild("CharacterSelect/MessageBox");
		messageBox_->hide();
		messageBoxOKCancel_ = window_->getChild("CharacterSelect/MessageBoxOKCancel");
		messageBoxOKCancel_->hide();

		ClearCharacterDataText();

		window_->getChild("CharacterSelect/Character1")->setProperty("Image", "set:CharacterRender1 image:texture");
		window_->getChild("CharacterSelect/Character2")->setProperty("Image", "set:CharacterRender2 image:texture");
		window_->getChild("CharacterSelect/Character3")->setProperty("Image", "set:CharacterRender3 image:texture");

		System::getSingleton().setGUISheet(window_);
	}
	catch (Exception& e)
	{
		std::cerr << "SelectCharacterWindow: Failed to initialize: " << e.what() << std::endl;
	}
}

SelectCharacterWindow::~SelectCharacterWindow()
{
	AnimationManager::getSingletonPtr()->destroyAnimation("RotateLeftSlot1ToSlot3");
	AnimationManager::getSingletonPtr()->destroyAnimation("RotateLeftSlot2ToSlot1");
	AnimationManager::getSingletonPtr()->destroyAnimation("RotateLeftSlot3ToSlot2");
	AnimationManager::getSingletonPtr()->destroyAnimation("RotateRightSlot1ToSlot2");
	AnimationManager::getSingletonPtr()->destroyAnimation("RotateRightSlot2ToSlot3");
	AnimationManager::getSingletonPtr()->destroyAnimation("RotateRightSlot3ToSlot1");
	window_->destroy();
}

void SelectCharacterWindow::SetOKButtonEventHandler(Event::Subscriber subscriber)
{
	okButton_->subscribeEvent(PushButton::EventClicked, subscriber);
}

void SelectCharacterWindow::SetExitButtonEventHandler(Event::Subscriber subscriber)
{
	exitButton_->subscribeEvent(PushButton::EventClicked, subscriber);
}


bool SelectCharacterWindow::HandleWindowUpdated(const EventArgs& e)
{
	//const float deltaTime = static_cast<const UpdateEventArgs&>(e).d_timeSinceLastFrame;
	//if (!characterData_[0].name_.empty())
	//{
	//	characterRender1_.Update(*window_->getChild("CharacterSelect/Character1"), deltaTime);
	//}

	//if (!characterData_[1].name_.empty())
	//{
	//	characterRender3_.Update(*window_->getChild("CharacterSelect/Character3"), deltaTime);
	//}	

	//if (!characterData_[2].name_.empty())
	//{
	//	characterRender2_.Update(*window_->getChild("CharacterSelect/Character2"), deltaTime);
	//}
	return true;
}

bool SelectCharacterWindow::HandleKeyPress(const EventArgs& e)
{
	const KeyEventArgs& keyArgs = static_cast<const KeyEventArgs&>(e);
	switch (keyArgs.scancode)
	{
	case Key::Return:
		okButton_->fireEvent(PushButton::EventClicked, EventArgs());
		return true;
	}
	return false;
}

void SelectCharacterWindow::SetCharacterData(const CharacterData& characterData, const int slot)
{
	if (slot < 0 || slot >= 3 || characterCount_ >= 3)
	{
		return;
	}

	characterData_[slot] = characterData;
	//if (slot == 0)
	//{
	//	SetCharacterRender(characterRender1_, characterData_[slot]);
	//}
	//else if (slot == 1)
	//{	
	//	SetCharacterRender(characterRender3_, characterData_[slot]);
	//}
	//else if (slot == 2)
	//{
	//	SetCharacterRender(characterRender2_, characterData_[slot]);
	//}
	++characterCount_;
}

void SelectCharacterWindow::SetCharacterRender(CharacterRender& characterRender, const CharacterData& characterData)
{
	const bool devil =  characterData.class_ >= 10 ? true : false;
	characterRender.SetBody(characterData.gender_, characterData.skin_, characterData.hair_, devil);
	for (int i = 0; i < 10; ++i)
	{
		characterRender.SetEquipment(i, characterData.equippedItem_[i]);
	}
}

void SelectCharacterWindow::SetActiveCharacterSlot(const int slot)
{
	if (slot < 0 || slot >= 3)
		return;

	activeCharacterSlot_ = slot;
}

void SelectCharacterWindow::HandleCharacterDataLoadComplete()
{
	rotateLeftButton_->enable();
	rotateRightButton_->enable();
	SetCharacterDataText();
}

void SelectCharacterWindow::SetCharacterDataText()
{
	const CharacterData& c = characterData_[activeCharacterSlot_];

	name_->setText(c.name_);
	guildName_->setText(c.guildName_);
	age_->setText(PropertyHelper::intToString(c.age_));
	position_->setText(PropertyHelper::intToString(c.fame_));
	moralValue_.SetValue(c.moral_);
	moral_->setText(moralValue_.GetText());
	moral_->setProperty("TextColours",
		PropertyHelper::colourToString(moralValue_.GetColor()));
	level_->setText(PropertyHelper::intToString(c.level_));
	health_->setText(boost::str(boost::format("%1% / %2%") % c.health_ % c.maximumHealth_));
	mana_->setText(boost::str(boost::format("%1% / %2%") % c.mana_ % c.maximumMana_));
	stamina_->setText(boost::str(boost::format("%1% / %2%") % c.stamina_ % c.maximumStamina_));
	//_mainWeapon;
	//_mainSkill;
	strength_->setText(boost::str(boost::format("%1$.1f") % (c.strength_ / 10.f)));
	intelligence_->setText(boost::str(boost::format("%1$.1f")  % (c.intelligence_ / 10.f)));
	dexterity_->setText(boost::str(boost::format("%1$.1f") % (c.dexterity_ / 10.f)));
	wisdom_->setText(boost::str(boost::format("%1$.1f") % (c.wisdom_ / 10.f)));
	charisma_->setText(boost::str(boost::format("%1$.1f") % (c.charisma_ / 10.f)));
	constitution_->setText(boost::str(boost::format("%1$.1f") % (c.constitution_ / 10.f)));
}

void SelectCharacterWindow::ClearCharacterDataText()
{
	name_->setText("");
	guildName_->setText("");
	age_->setText("");
	position_->setText("");
	moral_->setText("");
	level_->setText("");
	health_->setText("");
	mana_->setText("");
	stamina_->setText("");
	mainWeapon_->setText("");
	mainSkill_->setText("");
	strength_->setText("");
	intelligence_->setText("");
	dexterity_->setText("");
	wisdom_->setText("");
	charisma_->setText("");
	constitution_->setText("");
}

bool SelectCharacterWindow::HandleRotateLeft(const EventArgs& e)
{
	rotateLeftButton_->disable();
	ClearCharacterDataText();

	--activeCharacterSlot_;
	if (activeCharacterSlot_ < 0)
		activeCharacterSlot_ = 2;

	HandleRotate(RotateDirection::Left);

	return true;
}

bool SelectCharacterWindow::HandleRotateRight(const EventArgs& e)
{
	rotateRightButton_->disable();
	ClearCharacterDataText();

	++activeCharacterSlot_;
	if (activeCharacterSlot_ > 2)
		activeCharacterSlot_ = 0;

	HandleRotate(RotateDirection::Right);
	return true;
}

bool SelectCharacterWindow::HandleRotateLeftAnimationEnded(const EventArgs& e)
{
	SetCharacterDataText();
	rotateLeftButton_->enable();
	return true;
}

bool SelectCharacterWindow::HandleRotateRightAnimationEnded(const EventArgs& e)
{
	SetCharacterDataText();
	rotateRightButton_->enable();
	return true;
}

void SelectCharacterWindow::HandleRotate(RotateDirection::Enum _direction)
{
	for (int i = 0; i < 3; ++i)
	{
		std::string windowName = "";
		switch (i + 1)
		{
		case 1:
			windowName = "CharacterSelect/Character1";
			break;
		case 2:
			windowName = "CharacterSelect/Character2";
			break;
		case 3:
			windowName = "CharacterSelect/Character3";
			break;
		}
		Window* targetWindow = window_->getChild(windowName);

		targetWindow->removeEvent(AnimationInstance::EventAnimationEnded);
		
		std::string animationName = "";
		if (_direction == RotateDirection::Left)
		{
			switch (characterRenderSlot_[i])
			{
			case 1:
				characterRenderSlot_[i] = 3;
				animationName = "RotateLeftSlot1ToSlot3";
				break;
			case 2:
				characterRenderSlot_[i] = 1;
				animationName = "RotateLeftSlot2ToSlot1";
				targetWindow->subscribeEvent(AnimationInstance::EventAnimationEnded,
					Event::Subscriber(&SelectCharacterWindow::HandleRotateLeftAnimationEnded, this));
				break;
			case 3:
				characterRenderSlot_[i] = 2;
				animationName = "RotateLeftSlot3ToSlot2";
				break;
			}
		}
		else if (_direction == RotateDirection::Right)
		{
			switch (characterRenderSlot_[i])
			{
			case 1:
				characterRenderSlot_[i] = 2;
				animationName = "RotateRightSlot1ToSlot2";
				break;
			case 2:
				characterRenderSlot_[i] = 3;
				animationName = "RotateRightSlot2ToSlot3";
				targetWindow->subscribeEvent(AnimationInstance::EventAnimationEnded,
					Event::Subscriber(&SelectCharacterWindow::HandleRotateRightAnimationEnded, this));
				break;
			case 3:
				characterRenderSlot_[i] = 1;
				animationName = "RotateRightSlot3ToSlot1";
				break;
			}
		}

		AnimationInstance * instance =  AnimationManager::getSingletonPtr()->instantiateAnimation(animationName);
		instance->setTargetWindow(targetWindow);
		instance->start();
	}
}

const std::string SelectCharacterWindow::GetActiveCharacterName() const
{
	return characterData_[activeCharacterSlot_].name_;
}

} // namespace GUI