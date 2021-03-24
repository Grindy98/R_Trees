#pragma once
#include <string>
#include "Offset.h"
#include "Rect.h"

using namespace std;
class DataFile
{
public:
	struct Entry {
		string name;
		string tag;
		Rect BB;
	};
	virtual Entry getEntry(Offset off) = 0;
	virtual char getDataFileType() const = 0;
	virtual unsigned getNumberOfElements() const = 0;
};

