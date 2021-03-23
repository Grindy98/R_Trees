#pragma once
#include <memory>
#include <vector>
#include "Offset.h"
#include "Rect.h"
using namespace std;

class Node
{
public:
	friend int main();
	Node(unsigned degree, bool isLeaf);

	static shared_ptr<Node> deserialize(const vector<char>& bytes, unsigned degree);
	static vector<char> serialize(const Node& node);
	static unsigned byteSize(unsigned degree);

	vector<int> searchColliding(Point p) const;
	vector<int> searchColliding(Rect r) const;

	pair<Rect*, Offset> getChild(int index);
	int getArrSize();

	void insert(Rect rectToInsert, Offset offsetToInsert);
	void insert(Point pointToInsert, Offset offsetToInsert);

	vector<pair<shared_ptr<Node>, Rect>> split() const;

	const unsigned Degree;
	const bool IsLeaf;
private:
	int arrSize;
	vector<Rect> childrenBB;

	vector<Offset> childrenOrDataOffset;

	pair<int, int> pickSeedsForSplit() const;

	struct serializeHeader {
		unsigned isLeaf : 1;
		unsigned arrSize : 31;
	};
	Node(unsigned degree, bool isLeaf, int arrSize);
};

