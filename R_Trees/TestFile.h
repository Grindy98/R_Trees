#pragma once
#include "DataFile.h"
class TestFile : public DataFile
{
	// Inherited via DataFile
	virtual Entry getEntry(Offset off) override;
	virtual char getDataFileType() const override;
	virtual unsigned getNumberOfElements() const override;
};

