#pragma once
#include "IndexFile.h"
#include "DataFile.h"
#include "Node.h"
#include "Converter.h"
class BTree
{
public:
	BTree(IndexFile& idxf, DataFile& dataf);
	BTree(const BTree&) = delete;
	~BTree();
	bool isKeyOffset(IndexKey key, Offset& result);
	bool insert(pair<IndexKey, Offset> indexDataPair);

	string toString(bool isHex);
private:
	// For efficiency, always store the root in memory
	shared_ptr<Node> root;
	IndexFile& idxf;
	DataFile& dataf;
	Converter conv;
};

