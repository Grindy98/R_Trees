#include "Node.h"
#include <cassert>

std::shared_ptr<Node> Node::deserialize(const std::vector<char>& bytes, unsigned degree)
{
	assert(bytes.size() == byteSize(degree));
	const char* p = bytes.data();
	auto header = *reinterpret_cast<const serializeHeader*>(p);
	p += sizeof(serializeHeader);
	auto newNode = std::shared_ptr<Node>(new Node(degree, header.isLeaf == 1, header.arrSize));
	
	auto rectArr = reinterpret_cast<const Rect*>(p);
	std::copy(rectArr, rectArr + header.arrSize, newNode->childrenBB.begin());
	p += sizeof(Rect) * (2 * degree);
	auto offArr = reinterpret_cast<const Offset*>(p);
	if (header.isLeaf) {
		std::copy(offArr, offArr + header.arrSize, newNode->shapeDataOffset.begin());
	}
	else {
		std::copy(offArr, offArr + header.arrSize, newNode->childrenOffset.begin());
	}
	return newNode;
}

std::vector<char> Node::serialize(const Node& node)
{
	std::vector<char> bytes(byteSize(node.degree));
	char* p = bytes.data();
	serializeHeader header = { node.isLeaf ? 1 : 0, node.arrSize };
	*reinterpret_cast<serializeHeader*>(p) = header;
	p += sizeof(serializeHeader);

	auto rectArr = reinterpret_cast<Rect*>(p);
	std::copy(node.childrenBB.begin(), node.childrenBB.end(), rectArr);
	p += sizeof(Rect) * (2 * node.degree);
	auto offArr = reinterpret_cast<Offset*>(p);
	if (node.isLeaf) {
		std::copy(node.shapeDataOffset.data(), node.shapeDataOffset.data() + node.arrSize, offArr);
	}
	else {
		std::copy(node.childrenOffset.data(), node.childrenOffset.data() + node.arrSize, offArr);
	}
	return bytes;
}

unsigned Node::byteSize(unsigned degree)
{
	// One array of BBs and another one of Offsets(depending if node is leaf or not, changes purpose)
	// Arrays are only 2 * degree in size because the extra space at last index would be always empty
	// after split
	return sizeof(serializeHeader) + (sizeof(Rect) + sizeof(Offset))* 2 * degree;
}

Node::Node(unsigned degree, bool isLeaf, unsigned arrSize)
	:
	degree(degree),
	isLeaf(isLeaf),
	arrSize(arrSize),
	childrenBB(2 * degree + 1),
	// Alternate the vector size between the 2 based on the isLeaf param
	childrenOffset(isLeaf ? 0 : 2 * degree + 1),
	shapeDataOffset(isLeaf ? 2 * degree + 1 : 0)
{
}
