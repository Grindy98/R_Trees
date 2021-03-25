#include "TextFile.h"
#include <string.h>

TextFile::TextFile(string fileName)
{
    textStream.open(fileName, ios::in);
    if (!textStream.is_open()) {
        throw exception("Text file does not exist!\n");
    }
    std::string s;
    //for performance
    s.reserve(100);
    while (getline(textStream, s)) {
        lineBegins.push_back(Offset(textStream.tellg()));
    }
    textStream.clear();
}

DataFile::Entry TextFile::getEntry(Offset off)
{
    // Read line based on where the 
    std::string s;
    //for performance
    s.reserve(100);
    textStream.seekg(lineBegins[off.get()].get());
    getline(textStream, s);
    // Tokenize string
    char* cString = strdup(s.c_str());
   
    vector<string> tokResult;
    char* p = strtok(cString, ", ");
    tokResult.push_back(p);
    while (p = strtok(nullptr, ", ")) {
        tokResult.push_back(p);
    }
    free(cString);

    // Make sure format is correct
    if (tokResult.size() != 4) {
        throw exception("Text file format incorrect!\n");
    }
    // Latitude then longitude
    double y = stod(tokResult[2]);
    double x = stod(tokResult[3]);
    Rect entRect = Rect({ x, y }, { x, y });
    Entry ent = { tokResult[0], tokResult[1], entRect };
    return ent;
}

char TextFile::getDataFileType() const
{
    return 'x';
}

unsigned TextFile::getNumberOfElements() const
{
    return lineBegins.size();
}
