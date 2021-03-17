#pragma once
#include "IndexFile.h"
#include "Node.h"
class Converter
{
public:
	Converter(IndexFile& idx);
	shared_ptr<Node> getNodeFromFile(Offset off);
	void setNodeToFile(const Node& node, Offset off);

private:
	IndexFile& idx;
	const int keySize, degree;
};

