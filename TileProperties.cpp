
#include "TileProperties.h"
#include <cassert>

unsigned char TileProperties::bitMask_[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

void TileProperties::Load(const std::string& filename)
{
	FILE* file = fopen(filename.c_str(), "rb");
	if (file == 0)
	{
		// TODO: Add error output here
		return;
	}

	fseek(file, 4, SEEK_CUR);
	fread(&width_, sizeof(width_), 1, file);
	fread(&height_, sizeof(height_), 1, file);

	properties_.clear();
	properties_.shrink_to_fit();
	properties_.resize(width_ * height_);

	sf::Vector2i cellLimit;
	cellLimit.x = (width_ + 1) / 2;
	cellLimit.y = (height_ + 1) / 2;

	int readSize = cellLimit.x * cellLimit.y + (cellLimit.x - 1) * (cellLimit.y - 1);
	int loadCount = (readSize % 8) ? (readSize / 8) + 1 : readSize / 8;

	std::vector<unsigned char> buffer;
	buffer.resize(loadCount);

	fread(&buffer[0], sizeof(buffer[0]), loadCount, file);

	int readCount = 0;
	int roll = 0;

	for (int y = 0; y < height_-1; ++y)
	{
		for (int x = 0; x < width_-1; ++x)
		{
			if ((x + y) % 2 == 0)
			{
				properties_[y*width_+x].move = ((buffer[readCount] & bitMask_[roll]) >> roll) ? true : false;
				++roll;
				if (roll == 8)
				{
					roll = 0;
					++readCount;
				}
			}
		}
	}

	fread(&readCount, sizeof(readCount), 1, file);
	for (int i = 0; i < readCount; ++i)
	{
		short x = 0;
		short y = 0;
		fread(&x, sizeof(x), 1, file);
		fread(&y, sizeof(y), 1, file);

		short value = -1;
		fread(&value, sizeof(value), 1, file);

		properties_[y*width_+x].event = value;
	}

	fread(&readCount, sizeof(readCount), 1, file);
	for (int i = 0; i < readCount; ++i)
	{
		short x = 0;
		short y = 0;
		fread(&x, sizeof(x), 1, file);
		fread(&y, sizeof(y), 1, file);

		short value = -1;
		fread(&value, sizeof(value), 1, file);

		properties_[y*width_+x].house = value;
	}

	buffer.resize(readSize);
	fread(&buffer[0], sizeof(buffer[0]), readSize, file);
	readCount = 0;
	for (int y = 0; y < height_-1; ++y)
	{
		for (int x = 0; x < width_-1; ++x)
		{
			if ((x + y) % 2 == 0)
			{
				properties_[y*width_+x].area = buffer[readCount++];
			}
		}
	}

	fclose(file);
}

bool TileProperties::GetMovable(const sf::Vector2i& cell) const
{
	//assert(cell.x >= 0 && cell.x < _width);
	//assert(cell.y >= 0 && cell.y < _height);
	if (cell.x < 0 || cell.x >= width_ || cell.y < 0 || cell.y >= height_)
	{
		return false;
	}

	return properties_[cell.y*width_+cell.x].move;
}

int TileProperties::GetEvent(const sf::Vector2i& cell) const
{
	//assert(cell.x >= 0 && cell.x < _width);
	//assert(cell.y >= 0 && cell.y < _height);
	if (cell.x < 0 || cell.x >= width_ || cell.y < 0 || cell.y >= height_)
	{
		return -1;
	}
	return static_cast<int>(properties_[cell.y*width_+cell.x].event);
}

int TileProperties::GetHouse(const sf::Vector2i& cell) const
{
	//assert(cell.x >= 0 && cell.x < _width);
	//assert(cell.y >= 0 && cell.y < _height);
	if (cell.x < 0 || cell.x >= width_ || cell.y < 0 || cell.y >= height_)
	{
		return -1;
	}
	return static_cast<int>(properties_[cell.y*width_+cell.x].house);
}

int TileProperties::GetArea(const sf::Vector2i& cell) const
{
	//assert(cell.x >= 0 && cell.x < _width);
	//assert(cell.y >= 0 && cell.y < _height);
	if (cell.x < 0 || cell.x >= width_ || cell.y < 0 || cell.y >= height_)
	{
		return 0;
	}
	return static_cast<int>(properties_[cell.y*width_+cell.x].area);
}

void TileProperties::SetMovable(const sf::Vector2i& cell, bool value)
{
	if (cell.x < 0 || cell.x >= width_ || cell.y < 0 || cell.y >= height_)
	{
		return;
	}
	properties_[cell.y*width_+cell.x].move = value;
}

int TileProperties::GetWidth() const
{
	return width_;
}

int TileProperties::GetHeight() const
{
	return height_;
}