#include "ShapeFile.h"
#include <iostream>
#include <cassert>

constexpr auto NAME_FIELD = "name";
constexpr auto TAG_FIELD = "fclass";

ShapeFile::ShapeFile(string dirName)
{
    using namespace filesystem;
    if (!is_directory(dirName)) {
        throw exception("Path is not a directory!\n");
    }
    // Go through the dir and store each shape file in a vector
    vector<string> shpFilePaths;
    for (const auto& entry : directory_iterator(dirName)) {
        if (entry.is_regular_file()) {
            if (entry.path().extension() == ".shp") {
                // If file is shp type
                shpFilePaths.push_back(entry.path().generic_string());
            }
        }
    }
    // Open for each shp and dbx file
    dbfHandles.reserve(shpFilePaths.size());
    shpHandles.reserve(shpFilePaths.size());
    IShapeOffset.reserve(shpFilePaths.size() + 1);
    IShapeOffset.push_back(Offset(0));
    static const string accessSpec = "rb";
    for (string path : shpFilePaths) {
        shpHandles.push_back(SHPOpen(path.c_str(), accessSpec.c_str()));
        int nEntities = 0;
        int shapeType = 0;
        double min[4], max[4];
        SHPGetInfo(shpHandles.back(), &nEntities, &shapeType, min, max);
        if (nEntities <= 0) {
            throw exception(("Error opening shapefile: " + path).c_str());
        }

        dbfHandles.push_back(DBFOpen(path.c_str(), accessSpec.c_str()));
        if (DBFGetRecordCount(dbfHandles.back()) != nEntities) {
            throw exception(("Error opening dbf of shapefile: " + path).c_str());
        }
        
        // Keep files only if they fulfill certain characteristics
        // For instance drop polylines because lines are not represented well with a rectangular BB
        if (shapeType != SHPT_POINT && shapeType != SHPT_POLYGON) {
            SHPClose(shpHandles.back());
            shpHandles.pop_back();    
            DBFClose(dbfHandles.back());
            dbfHandles.pop_back();
        }
        else {
            // Add the number to the offset array
            IShapeOffset.push_back(IShapeOffset.back() + nEntities);
        }
    }
    cout << "Read from directory, opened " << shpHandles.size() << " files!" << endl;
}

DataFile::Entry ShapeFile::getEntry(Offset off)
{
    assert(Offset(0) <= off && off < IShapeOffset.back());
    // Find which file contains index off
    auto iter = lower_bound(IShapeOffset.begin(), IShapeOffset.end(), off);
    if ((*iter).get() != off.get()) {
        // Offset is not start of array -- decrement index
        iter--;
    }
    int index = iter - IShapeOffset.begin();
    // Read from the proper files with the correct displacement
    Entry entry;

    int fileIShape = (int)(off.get() - (*iter).get());
    auto obj = SHPReadObject(shpHandles[index], fileIShape);
    entry.BB = { Point(obj->dfXMin, obj->dfYMin), Point(obj->dfXMax, obj->dfYMax) };
    
    SHPDestroyObject(obj);

    int nameInd = DBFGetFieldIndex(dbfHandles[index], NAME_FIELD);
    if (nameInd < 0) {
        throw exception("Cannot find name field in dbf file\n");
    }
    int tagInd = DBFGetFieldIndex(dbfHandles[index], TAG_FIELD);
    if (tagInd < 0) {
        throw exception("Cannot find tag field in dbf file\n");
    }

    entry.name = string(DBFReadStringAttribute(dbfHandles[index], fileIShape, nameInd));
    entry.tag = string(DBFReadStringAttribute(dbfHandles[index], fileIShape, tagInd));

    return entry;
}

char ShapeFile::getDataFileType() const
{
    return 's';
}

Offset ShapeFile::getNumberOfElements() const
{
    return IShapeOffset.back();
}
