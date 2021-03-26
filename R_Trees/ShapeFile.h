#pragma once
#include "DataFile.h"
class ShapeFile : public DataFile
{
public:
	ShapeFile(string name);
	// Inherited via DataFile
	virtual Entry getEntry(Offset off) override;
	virtual char getDataFileType() const override;
	virtual unsigned getNumberOfElements() const override;
private:

};

