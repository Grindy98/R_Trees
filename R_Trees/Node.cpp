#include "Node.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <iostream>

Node::~Node()
{
}

Node::Node(int order, pair<IndexKey, Offset> key)
	:
	Node(key.first.getSize(), order)
{
	arrSize = 1;
	indexArr[0] = key.first;
	dataOffset[0] = key.second;
	// No children, empty node
	childrenOffset[0] = childrenOffset[1] = Offset();
}

Node::Node(int keySize, int order)
	:
	keySize(keySize),
	order(order),
	indexArr(2 * order + 1, IndexKey(keySize)),
	dataOffset(2 * order + 1, Offset()),
	childrenOffset(2 * order + 2, Offset())
{
	arrSize = 0;
}

Node::Node(const Node& original, int splitBeginInd, int splitEndInd)
	:
	Node(original.keySize, original.order)
{
	assert(original.isFull() && splitEndInd - splitBeginInd == order);
	arrSize = order;
	copy(original.indexArr.begin() + splitBeginInd, original.indexArr.begin() + splitEndInd, indexArr.begin());
	copy(original.dataOffset.begin() + splitBeginInd, original.dataOffset.begin() + splitEndInd, dataOffset.begin());
	copy(original.childrenOffset.begin() + splitBeginInd, original.childrenOffset.begin() + splitEndInd + 1, childrenOffset.begin());
}


bool Node::search(IndexKey key, Offset& where) const
{
	int ind = -1;
	if (getIndex(key, ind)) {
		where = dataOffset[ind];
		return true;
	}
	return false;
}

void Node::insert(pair<IndexKey, Offset> key, pair<Offset, Offset> newChildren)
{
	assert(!isFull());
	// Assert leaf status based on if the offset values are null or not
	assert(newChildren.first.notNull() ? !isLeaf() : isLeaf());
	int ind = -1;
	// Make sure key is unique
	bool found = getIndex(key.first, ind);
	assert(!found);
	// ind stores index where insertion should occur
	copy_backward(indexArr.begin() + ind, indexArr.begin() + arrSize, indexArr.begin() + arrSize + 1);
	copy_backward(dataOffset.begin() + ind, dataOffset.begin() + arrSize, dataOffset.begin() + arrSize + 1);
	
	indexArr[ind] = key.first;
	dataOffset[ind] = key.second;

	arrSize++;
	if (newChildren.first.notNull()) {
		copy_backward(childrenOffset.begin() + ind, childrenOffset.begin() + arrSize, childrenOffset.begin() + arrSize + 1);
		setChildren(key.first, newChildren);
	}
	else {
		childrenOffset[arrSize] = Offset();
	}
}

void Node::insert(pair<IndexKey, Offset> key)
{
	insert(key, make_pair(Offset(), Offset()));
}

tuple<shared_ptr<Node>, pair<IndexKey, Offset>, shared_ptr<Node>> Node::split() const
{
	auto left = shared_ptr<Node>(new Node(*this, 0, order));
	auto middle = make_pair(IndexKey(indexArr[order]), Offset(dataOffset[order]));
	auto right = shared_ptr<Node>(new Node(*this, order + 1, 2 * order + 1));
	return make_tuple(move(left), middle, move(right));
}

pair<Offset, Offset> Node::getChildren(IndexKey key) const
{
	int ind = -1;
	if (!getIndex(key, ind)) {
		assert(false);
	}
	return make_pair(childrenOffset[ind], childrenOffset[ind + 1]);
}

Offset Node::getChildInBetween(IndexKey key) const
{
	int ind = -1;
	if (getIndex(key, ind)) {
		assert(false);
	}
	return childrenOffset[ind];
}

void Node::setChildren(IndexKey key, pair<Offset, Offset> newChildren)
{
	int ind = -1;
	if (!getIndex(key, ind)) {
		assert(false);
	}
	childrenOffset[ind] = newChildren.first;
	childrenOffset[ind + 1] = newChildren.second;
}

vector<Offset> Node::getAllChildren() const
{
	vector<Offset> ret;
	ret.assign(childrenOffset.begin(), childrenOffset.begin() + arrSize + 1);
	return ret;
}

bool Node::isFull() const
{
	return arrSize == 2 * order + 1;
}

bool Node::isLeaf() const
{
	for (int i = 0; i < arrSize + 1; i++) {
		if (childrenOffset[i].notNull()) {
			return false;
		}
	}
	return true;
}

string Node::toString(bool isHex) const
{
	stringstream ret;
	ret << arrSize << " " << std::hex;
	for (int i = 0; i < arrSize; i++) {
		ret << indexArr[i].toString(isHex) << " 0x" << dataOffset[i].getVal() << " | ";
	}
	ret << " || ";
	for (int i = 0; i < arrSize + 1; i++) {
		ret << "0x" << childrenOffset[i].getVal() << " | ";
	}
	ret << "\n";
	return ret.str();
}

bool Node::getIndex(IndexKey key, int& ind) const
{
	ind = getNextBiggestIndex(key);
	if (ind == arrSize) {
		return false;
	}
	return indexArr[ind] == key;
}

int Node::getNextBiggestIndex(IndexKey key) const
{
	assert(arrSize > 0);
	int left = 0, right = arrSize;
	while (left < right) {
		int mid = (left + right) / 2;
		if (indexArr[mid] == key) {
			// Found index
			return mid;
		}
		if (indexArr[mid] < key) {
			left = mid + 1;
		}
		else {
			right = mid;
		}
	}
	return left;
}
