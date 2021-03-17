#pragma once
#include "BinaryStorage.h"

class IndexKey : public BinaryStorage
{
public:
	friend class Converter;
	friend class DataFile;
	explicit IndexKey(int keySize);
	IndexKey(const unsigned char bytes[], int keySize);
	IndexKey(const IndexKey& other);

	bool operator<(const IndexKey& rhs) const;
	bool operator==(const IndexKey& rhs) const;
};