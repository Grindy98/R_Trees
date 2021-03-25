#pragma once
#include "DataFile.h"
#include <fstream>
#include <vector>

class TextFile : public DataFile
{
public:
	TextFile(string fileName);
	// Inherited via DataFile
	virtual Entry getEntry(Offset off) override;
	virtual char getDataFileType() const override;
	virtual unsigned getNumberOfElements() const override;
private:
	fstream textStream;
	vector<streampos> lineBegins;

};

