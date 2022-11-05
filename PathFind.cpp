
#include "PathFind.h"
#include "Zone.h"

std::list<std::pair<int, int>> PathFind::FindPath(const Zone& zone, const int startX, const int startY, const int goalX, const int goalY)
{
	for (auto it = openNodes_.begin(); it != openNodes_.end(); ++it)
	{
		delete (*it);
	}
	openNodes_.clear();

	for (auto it = closedNodes_.begin(); it != closedNodes_.end(); ++it)
	{
		delete (*it);
	}
	closedNodes_.clear();

	Node* start = new Node;
	start->g = 0;
	start->h = (abs(startX - goalX) + abs(startY - goalY));
	start->f = start->g + start->h;
	start->x = startX;
	start->y = startY;

	openNodes_.push_back(start);
	std::make_heap(openNodes_.begin(), openNodes_.end());

	int stepLimit = zone.IsMovable(sf::Vector2i(goalX, goalY)) ? 20 : 10;

	Node* lowestDistance = start;
	Node* current = nullptr;
	while (!openNodes_.empty() && stepLimit > 0)
	{
		current = openNodes_.front();
		std::pop_heap(openNodes_.begin(), openNodes_.end(), HeapCompare());
		openNodes_.pop_back();
		closedNodes_.push_back(current);

		if (current->f < lowestDistance->f) lowestDistance = current;

		if (current->x == goalX && current->y == goalY)
		{
			break;
		}

		// Using a static movement cost of 1.
		int cost = current->g + 1;

		int neighbourCoords[8][2] =
		{
			{-1, -1}, //UpLeft
			{-1,  1}, //DownLeft
			{ 1,  1}, //DownRight
			{ 1, -1}, //UpRight
			{-2,  0}, //Left
			{ 0,  2}, //Down
			{ 2,  0}, //Right
			{ 0, -2}, //Up
		};

		for (int i = 0; i < 8; ++i)
		{
			const int neighbourX = current->x + neighbourCoords[i][0];
			const int neighbourY = current->y + neighbourCoords[i][1];

			if (!zone.IsMovable(sf::Vector2i(neighbourX, neighbourY)))
			{
				continue;
			}

			auto openResult = openNodes_.end();
			for (auto openNode = openNodes_.begin(); openNode != openNodes_.end(); ++openNode)
			{
				if ((*openNode)->x == neighbourX && (*openNode)->y == neighbourY)
				{
					openResult = openNode;
					break;
				}
			}

			if (openResult != openNodes_.end() && cost < (*openResult)->g)
			{
				delete (*openResult);
				openNodes_.erase(openResult);
				std::make_heap(openNodes_.begin(), openNodes_.end(), HeapCompare());
				continue;
			}

			auto closedResult = closedNodes_.end();
			for (auto closedNode = closedNodes_.begin(); closedNode != closedNodes_.end(); ++closedNode)
			{
				if ((*closedNode)->x == neighbourX && (*closedNode)->y == neighbourY)
				{
					closedResult = closedNode;
					break;
				}
			}

			if (closedResult != closedNodes_.end() && cost < (*closedResult)->g)
			{
				delete (*closedResult);
				closedNodes_.erase(closedResult);
				continue;
			}

			if (openResult == openNodes_.end() && closedResult == closedNodes_.end())
			{
				Node* neighbour = new Node;
				neighbour->parent = current;
				neighbour->g = cost;
				neighbour->h = (abs(neighbourX - goalX) + abs(neighbourY - goalY));
				neighbour->f = neighbour->g + neighbour->h;
				neighbour->x = neighbourX;
				neighbour->y = neighbourY;
				openNodes_.push_back(neighbour);
				std::push_heap(openNodes_.begin(), openNodes_.end(), HeapCompare());
			}
		}
		--stepLimit;
	}

	std::list<std::pair<int, int>> path;
	Node* node = lowestDistance;
	while (node)
	{
		path.push_front(std::make_pair<int, int>(node->x, node->y));
		node = node->parent;
	}
	return path;
}