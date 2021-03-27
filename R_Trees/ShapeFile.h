#pragma once
#include "DataFile.h"
#include "shapefil.h"
#include <filesystem>

class ShapeFile : public DataFile
{
public:
	ShapeFile(string dirName);
	~ShapeFile();
	// Inherited via DataFile
	virtual Entry getEntry(Offset off) override;
	virtual char getDataFileType() const override;
	virtual Offset getNumberOfElements() const override;
private:
	vector<SHPHandle> shpHandles;
	vector<DBFHandle> dbfHandles;
	vector<Offset> IShapeOffset;
};

