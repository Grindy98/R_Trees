#include "IndexKey.h"
#include <cassert>
#include <sstream>

IndexKey::IndexKey(int keySize)
	:
	BinaryStorage(keySize)
{}

IndexKey::IndexKey(const unsigned char bytes[], int keySize)
	:
	BinaryStorage(bytes, keySize)
{}

IndexKey::IndexKey(const IndexKey& other)
	:
	BinaryStorage(other)
{}
bool IndexKey::operator<(const IndexKey& rhs) const
{
	assert(size == rhs.size);
	return memcmp(bytes.data(), rhs.bytes.data(), size) < 0;
}

bool IndexKey::operator==(const IndexKey& rhs) const
{
	assert(size == rhs.size);
	return memcmp(bytes.data(), rhs.bytes.data(), size) == 0;
}