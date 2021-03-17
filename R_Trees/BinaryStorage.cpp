#include "BinaryStorage.h"
#include <cassert>
#include <sstream>
#include <iostream>

BinaryStorage::BinaryStorage(int keySize)
	:
	size(keySize),
	bytes(keySize)
{}

BinaryStorage::BinaryStorage(const unsigned char bytes[], int keySize)
	:
	BinaryStorage(keySize)
{
	copy(bytes, bytes + size, this->bytes.begin());
}

BinaryStorage::BinaryStorage(const BinaryStorage& other)
	:
	BinaryStorage(other.bytes.data(), other.size)
{
}

void BinaryStorage::operator=(const BinaryStorage& other)
{
	assert(size == other.size);
	copy(other.bytes.begin(), other.bytes.end(), bytes.begin());
}

string BinaryStorage::toString(bool hexFormat) const
{
	stringstream stream;
	if (hexFormat) {
		stream << " 0x" << std::hex;
	}
	for (int i = 0; i < size; i++) {
		if (hexFormat) {
			int val = (int)bytes[i];
			stream << val;
		}
		else {
			stream << bytes[i];
		}
	}
	return stream.str();
}

int BinaryStorage::getSize()
{
	return size;
}
