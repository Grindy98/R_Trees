#include "ShapeFile.h"

ShapeFile::ShapeFile(string name)
{
}

DataFile::Entry ShapeFile::getEntry(Offset off)
{
    return Entry();
}

char ShapeFile::getDataFileType() const
{
    return 0;
}

unsigned ShapeFile::getNumberOfElements() const
{
    return 0;
}
