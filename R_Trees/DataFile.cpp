#include "DataFile.h"
#include <sstream>

string DataFile::Entry::toString(int index) const
{
    stringstream ss;
    ss << "Entry";
    if (index == -1) {
        ss << ":" << endl;
    }
    else {
        ss << " " << index << ":" << endl;
    }
    ss << "\tName: " << name << endl;
    ss << "\tTag: " << tag << endl;
    ss << "\tCoords: " << (BB.getDownLeft().y + BB.getUpRight().y) / 2 << ", "
        << (BB.getDownLeft().x + BB.getUpRight().x) / 2 << endl;
    return ss.str();
}
