#ifndef PATH_FIND_H
#define PATH_FIND_H

#include <vector>
#include <list>

class Zone;

class PathFind
{
public:
	std::list<std::pair<int, int>> FindPath(const Zone& zone, int startX, int startY, int goalX, int goalY);

private:
	struct Node
	{
		Node() : parent(nullptr), f(0), g(0), h(0), x(0), y(0) {}

		Node* parent;
		int f;
		int g;
		int h;
		int x;
		int y;		
	};

	struct HeapCompare
	{
		bool operator() (const Node* lhs, const Node* rhs) const
		{
			return lhs->f > rhs->f;
		}
	};

	std::vector<Node*> openNodes_;
	std::vector<Node*> closedNodes_;
};

#endif // PATH_FIND_H