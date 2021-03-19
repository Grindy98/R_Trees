#pragma once
#include <memory>
#include <vector>
#include "Offset.h"
#include "Rect.h"

class Node
{
public:
	friend int main();
	static std::shared_ptr<Node> deserialize(const std::vector<char>& bytes, unsigned degree);
	static std::vector<char> serialize(const Node& node);
	static unsigned byteSize(unsigned degree);

private:
	const unsigned degree;
	unsigned arrSize;
	const bool isLeaf;
	std::vector<Rect> childrenBB;

	std::vector<Offset> childrenOffset;
	std::vector<Offset> shapeDataOffset;

	struct serializeHeader {
		unsigned isLeaf : 1;
		unsigned arrSize : 31;
	};
	Node(unsigned degree, bool isLeaf, unsigned arrSize);
};

