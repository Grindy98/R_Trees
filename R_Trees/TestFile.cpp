#include "TestFile.h"

DataFile::Entry TestFile::getEntry(Offset off)
{
    return Entry();
}

char TestFile::getDataFileType() const
{
    return 't';
}

unsigned TestFile::getNumberOfElements() const
{
    return 0;
}
