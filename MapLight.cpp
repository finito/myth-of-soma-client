
#include "MapLight.h"

void MapLight::Load(const std::string& filename)
{
	FILE* file = fopen(filename.c_str(), "rb");
	if (file == 0)
	{
		// TODO: Add error output here
		return;
	}

	struct _LIGHT_SOURCE_HEADER
	{
		char szID[4]; // "LSDF"
		char szRemark[64];
		int nQt;
	};

	// Skip ID and Remark
	fseek(file, 68, SEEK_CUR);

	int lightCount = 0;
	fread(&lightCount, sizeof(lightCount), 1, file);

	lights_.clear();
	lights_.shrink_to_fit();
	lights_.resize(lightCount);

	for (int i = 0; i < lightCount; ++i)
	{
		unsigned int color;
		fread(&color, sizeof(color), 1, file);

		int left;
		fread(&left, sizeof(left), 1, file);

		int top;
		fread(&top, sizeof(top), 1, file);

		int right;
		fread(&right, sizeof(right), 1, file);

		int bottom;
		fread(&bottom, sizeof(bottom), 1, file);

		lights_[i].color.r = color & 0xff;
		lights_[i].color.g = (color & 0x0000ff00) >> 8;
		lights_[i].color.b = (color & 0x00ff0000) >> 16;
		lights_[i].color.a = 255;

		lights_[i].position.x = static_cast<float>((left + right)  / 2);
		lights_[i].position.y = static_cast<float>((top  + bottom) / 2);

		lights_[i].scale.x = 4.5f;
		lights_[i].scale.y = 4.5f;
	}

	fclose(file);
}

std::vector<Framework::Light>& MapLight::GetLights()
{
	return lights_;
}

Framework::Light* MapLight::AddLight(const sf::Vector2f& position, const sf::Vector2f& scale,
								  const sf::Color& color)
{
	lights_.push_back(Framework::Light(position, scale, color));
	return &lights_.back();
}