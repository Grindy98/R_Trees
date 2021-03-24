#include "Node.h"
#include <cassert>
#include <limits>

Node::Node(unsigned degree, bool isLeaf)
	:
	Node(degree, isLeaf, 0)
{
}

shared_ptr<Node> Node::deserialize(const vector<char>& bytes, unsigned degree)
{
	assert(bytes.size() == byteSize(degree));
	const char* p = bytes.data();
	auto header = *reinterpret_cast<const serializeHeader*>(p);
	p += sizeof(serializeHeader);
	auto newNode = shared_ptr<Node>(new Node(degree, header.isLeaf == 1, header.arrSize));
	
	auto rectArr = reinterpret_cast<const Rect*>(p);
	copy(rectArr, rectArr + header.arrSize, newNode->childrenBB.begin());
	p += sizeof(Rect) * (2 * degree);
	auto offArr = reinterpret_cast<const Offset*>(p);

	copy(offArr, offArr + header.arrSize, newNode->childrenOrDataOffset.begin());
	return newNode;
}

vector<char> Node::serialize(const Node& node)
{
	assert((unsigned)node.arrSize <= 2 * node.Degree);
	vector<char> bytes(byteSize(node.Degree));
	char* p = bytes.data();
	serializeHeader header = { node.IsLeaf ? 1u : 0u, (unsigned)node.arrSize };
	*reinterpret_cast<serializeHeader*>(p) = header;
	p += sizeof(serializeHeader);

	auto rectArr = reinterpret_cast<Rect*>(p);
	copy(node.childrenBB.begin(), node.childrenBB.end(), rectArr);
	p += sizeof(Rect) * (2 * node.Degree);
	auto offArr = reinterpret_cast<Offset*>(p);

	copy(node.childrenOrDataOffset.data(), node.childrenOrDataOffset.data() + node.arrSize, offArr);
	return bytes;
}

unsigned Node::byteSize(unsigned degree)
{
	// One array of BBs and another one of Offsets(depending if node is leaf or not, changes purpose)
	// Arrays are only 2 * degree in size because the extra space at last index would be always empty
	// after split
	return sizeof(serializeHeader) + (sizeof(Rect) + sizeof(Offset))* 2 * degree;
}

vector<int> Node::searchColliding(Point p) const
{
	vector<int> colliding;
	colliding.reserve(2 * Degree + 1);
	for (int i = 0; i < arrSize; i++) {
		if (childrenBB[i].contains(p)) {
			colliding.push_back(i);
		}
	}
	return colliding;
}

vector<int> Node::searchColliding(Rect r) const
{
	vector<int> colliding;
	colliding.reserve(2 * Degree + 1);
	for (int i = 0; i < arrSize; i++) {
		if (childrenBB[i].intersects(r)) {
			colliding.push_back(i);
		}
	}
	return colliding;
}

int Node::searchFirstIncluded(Rect r) const
{
	for (int i = 0; i < arrSize; i++) {
		if (childrenBB[i].contains(r)) {
			return i;
		}
	}
	return -1;
}

int Node::searchMinBBExtension(Rect r) const
{
	double minArea = numeric_limits<double>::infinity();
	int minInd = -1;
	for (int i = 0; i < arrSize; i++) {
		double currArea = childrenBB[i].getMinExtraAreaToCover(r);
		if (currArea < minArea) {
			minArea = currArea;
			minInd = i;
		}
	}
	assert(minInd != -1);
	return minInd;
}

pair<Rect*, Offset> Node::getChild(int index)
{
	assert(index < arrSize);
	return make_pair(&childrenBB[index], childrenOrDataOffset[index]);
}

bool Node::isFull() const
{
	return arrSize == 2 * Degree + 1;
}

int Node::getArrSize() const
{
	return arrSize;
}

void Node::insert(Rect rectToInsert, Offset offsetToInsert)
{
	assert((unsigned)arrSize <= 2 * Degree);
	childrenBB[arrSize] = rectToInsert;
	childrenOrDataOffset[arrSize] = offsetToInsert;
	arrSize++;
}

void Node::insert(Point pointToInsert, Offset offsetToInsert)
{
	insert(Rect(pointToInsert, pointToInsert), offsetToInsert);
}

vector<pair<shared_ptr<Node>, Rect>> Node::split() const
{
	assert(arrSize == 2 * Degree + 1);
	shared_ptr<Node> n1 = make_shared<Node>(Degree, IsLeaf);
	shared_ptr<Node> n2 = make_shared<Node>(Degree, IsLeaf);
	// Choose furthest away Rects and insert them
	auto seedIndexes = pickSeedsForSplit();
	Rect r1 = childrenBB[seedIndexes.first];
	Rect r2 = childrenBB[seedIndexes.second];
	n1->insert(r1, childrenOrDataOffset[seedIndexes.first]);
	n2->insert(r2, childrenOrDataOffset[seedIndexes.second]);

	// Insert all other remaining nodes
	vector<int> remainingNodes;
	remainingNodes.reserve(arrSize - 2);
	for (int i = 0; i < arrSize; i++) {
		if (seedIndexes.first == i || seedIndexes.second == i) {
			continue;
		}
		remainingNodes.push_back(i);
	}
	// Assign each remaining nodes to one or the other
	while (!remainingNodes.empty()) {
		// Also make sure that we have enough entries to assign
		if (remainingNodes.size() + n1->arrSize == Degree) {
			// Assign the rest of the nodes to n1
			for (int i : remainingNodes)
			{
				n1->insert(childrenBB[i], childrenOrDataOffset[i]);
				r1 = r1.extend(childrenBB[i]);
			}
			break;
		}
		if (remainingNodes.size() + n2->arrSize == Degree) {
			// Assign the rest of the nodes to n2
			for (int i : remainingNodes)
			{
				n2->insert(childrenBB[i], childrenOrDataOffset[i]);
				r2 = r2.extend(childrenBB[i]);
			}
			break;
		}
		// Find the maximum discrepancy for each of them
		int maxInd = -1;
		double maxDiscrepancy = -1.0;
		for (int i : remainingNodes) {
			double currDiscrepancy = r1.getMinExtraAreaToCover(childrenBB[i]);
			currDiscrepancy -= r2.getMinExtraAreaToCover(childrenBB[i]);
			// Discrepancy is taken as an abs value -- negative is a flag for r2 > r1
			if (abs(currDiscrepancy) > maxDiscrepancy) {
				maxDiscrepancy = currDiscrepancy;
				maxInd = i;
			}
		}
		// Now add the entry to the correct group
		assert(maxInd != -1);
		if (maxDiscrepancy > 0.0) {
			// Add to n2
			n2->insert(childrenBB[maxInd], childrenOrDataOffset[maxInd]);
			r2 = r2.extend(childrenBB[maxInd]);
		}
		else {
			// Add to n1
			n1->insert(childrenBB[maxInd], childrenOrDataOffset[maxInd]);
			r1 = r1.extend(childrenBB[maxInd]);
		}
		// Remove the inserted element from vector
		auto position = lower_bound(remainingNodes.begin(), remainingNodes.end(), maxInd);
		remainingNodes.erase(position);
	}
	assert(n1->arrSize >= Degree);
	assert(n2->arrSize >= Degree);
	return {make_pair(n1, r1), make_pair(n2, r2)};
}

pair<int, int> Node::pickSeedsForSplit() const
{
	assert(arrSize >= 2);
	double maxArea = -numeric_limits<double>::infinity();
	pair<int, int> maxPair = make_pair(-1, -1);
	vector<double> test;
	for (int i = 0; i < arrSize; i++) {
		for (int j = i + 1; j < arrSize; j++)
		{
			// Compare for most wasteful pair based on formula
			Rect currMBB = childrenBB[i].extend(childrenBB[j]);
			double currArea = currMBB.getArea() - childrenBB[i].getArea() - childrenBB[j].getArea();
			test.push_back(currArea);
			if (currArea > maxArea) {
				maxArea = currArea;
				maxPair = make_pair(i, j);
			}
		}
	}
	assert(maxPair != make_pair(-1, -1));
	return maxPair;
}

Node::Node(unsigned degree, bool isLeaf, int arrSize)
	:
	Degree(degree),
	IsLeaf(isLeaf),
	arrSize(arrSize),
	childrenBB(2 * degree + 1),
	childrenOrDataOffset(2 * degree + 1)
{
}
