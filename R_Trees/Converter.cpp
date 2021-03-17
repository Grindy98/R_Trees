#include "Converter.h"
#include <cassert>

Converter::Converter(IndexFile& idx)
	:
	idx(idx),
	keySize(idx.getHeader().keySize),
	degree(idx.getHeader().degree)
{
}

shared_ptr<Node> Converter::getNodeFromFile(Offset off)
{
	if (!off.notNull()) {
		return nullptr;
	}
	assert(off.getVal() <= (idx.getEOFOffset() + idx.getEntryByteSize()).getVal());
	shared_ptr<Node> ret = make_shared<Node>(keySize, degree);
	int size = idx.getEntryByteSize();
	vector<unsigned char> readBytes(size);
	idx.read((char*)readBytes.data(), size, off);
	ret->arrSize = *reinterpret_cast<uint32_t*>(readBytes.data());
	unsigned char* indexArr = readBytes.data() + 4;
	Offset* dataOffArr = reinterpret_cast<Offset*>(indexArr + (2 * degree + 1) * keySize);
	Offset* childrenOffArr = reinterpret_cast<Offset*>(indexArr + (2 * degree + 1) * (keySize + 8));
	for (int i = 0; i < ret->arrSize; i++) {
		ret->indexArr[i] = IndexKey(indexArr + i * keySize, keySize);
		ret->dataOffset[i] = dataOffArr[i];
		ret->childrenOffset[i] = childrenOffArr[i];
	}
	// Last pointer carried over
	ret->childrenOffset[ret->arrSize] = childrenOffArr[ret->arrSize];

	return ret;
}

void Converter::setNodeToFile(const Node& node, Offset off)
{
	assert(keySize == node.keySize && degree == node.order);
	int size = idx.getEntryByteSize();
	char* writeBytes = new char[size] {};
	*reinterpret_cast<uint32_t*>(writeBytes) = (uint32_t)node.arrSize;
	char* indexArr = writeBytes + 4;
	Offset* dataOffArr = reinterpret_cast<Offset*>(indexArr + (2 * degree + 1) * keySize);
	Offset* childrenOffArr = reinterpret_cast<Offset*>(indexArr + (2 * degree + 1) * (keySize + 8));
	for (int i = 0; i < node.arrSize; i++) {
		copy(node.indexArr[i].bytes.begin(), node.indexArr[i].bytes.end(), indexArr + i * keySize);
		dataOffArr[i] = node.dataOffset[i];
		childrenOffArr[i] = node.childrenOffset[i];
	}
	// Last pointer carried over
	childrenOffArr[node.arrSize] = node.childrenOffset[node.arrSize];

	idx.write(writeBytes, size, off);
	delete[] writeBytes;
}
