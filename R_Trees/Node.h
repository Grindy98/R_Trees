#pragma once
#include "IndexKey.h"
#include "Offset.h"
#include <tuple>
#include <vector>
class Node
{
public:
	friend class Converter;
	Node(int degree, pair<IndexKey, Offset> key);
	Node(int keySize, int degree);
	~Node();

	bool search(IndexKey key, Offset& where) const;
	void insert(pair<IndexKey, Offset> key, pair<Offset, Offset> newChildren);
	void insert(pair<IndexKey, Offset> key);
	tuple<shared_ptr<Node>, pair<IndexKey, Offset>, shared_ptr<Node>> split() const;

	pair<Offset, Offset> getChildren(IndexKey key) const;
	Offset getChildInBetween(IndexKey key) const;
	void setChildren(IndexKey key, pair<Offset, Offset> newChildren);
	vector<Offset> getAllChildren() const;

	bool isFull() const;
	bool isLeaf() const;

	string toString(bool isHex) const;
private:
	Node(const Node& original, int splitBeginInd, int splitEndInd);
	bool getIndex(IndexKey key, int& ind) const;
	int getNextBiggestIndex(IndexKey key) const;
	const int keySize;
	const int order;
	int arrSize;
	vector<IndexKey> indexArr;
	vector<Offset> dataOffset;
	vector<Offset> childrenOffset;
};

