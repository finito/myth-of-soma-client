
#include "GUI/Moral.h"

namespace GUI
{

using namespace CEGUI;

Moral::Moral() :
value_(0),
text_(""),
color_()
{
}

void Moral::SetValue(int value)
{
	value_ = value;
	if (value_ <= -50)
	{
		text_ = "Devil";
		color_.set(255 / 255.f, 0 / 255.f, 0 / 255.f);
	}
	else if(value_ >= -49 && value_ <= -31)
	{
		text_ = "Wicked";
		color_.set(255 / 255.f, 70 / 255.f, 70 / 255.f);
	}
	else if(value_ >= -30 && value_ <= -11)
	{
		text_ = "Evil";
		color_.set(255 / 255.f, 140 / 255.f, 140 / 255.f);
	}
	else if(value_ >= -10 && value_ <= -4)
	{
		text_ = "Bad";
		color_.set(255 / 255.f, 200 / 255.f, 200 / 255.f);
	}
	else if(value_ >= -3  && value_ <=  3)
	{
		text_ = "Neutral";
		color_.set(211 / 255.f, 237 / 255.f, 251 / 255.f);
	}
	else if(value_ >=  4  && value_ <=  10)
	{
		text_ = "Good";
		color_.set(187 / 255.f, 230 / 255.f, 244 / 255.f);
	}
	else if(value_ >=  11 && value_ <=  30)
	{
		text_ = "Moral";
		color_.set(158 / 255.f, 208 / 255.f, 246 / 255.f);
	}
	else if(value_ >=  31 && value_ <=  49)
	{
		text_ = "Virtuous";
		color_.set(167 / 255.f, 190 / 255.f, 248 / 255.f);
	}
	else if(value_ >=  50)
	{
		text_ = "Angel";
		color_.set(150 / 255.f, 150 / 255.f, 255 / 255.f);
	}
}

const String& Moral::GetText() const
{
	return text_;
}

const colour& Moral::GetColor() const
{
	return color_;
}

} // namespace GUI