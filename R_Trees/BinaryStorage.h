#pragma once
using namespace std;
#include <string>
#include <vector>

class BinaryStorage
{
public:
	friend class DataFile;
	explicit BinaryStorage(int keySize);
	BinaryStorage(const unsigned char bytes[], int keySize);
	BinaryStorage(const BinaryStorage& other);
	void operator= (const BinaryStorage& other);

	string toString(bool hexFormat) const;

	int getSize();
protected:
	vector<unsigned char> bytes;
	int size;

};

