#include "TextFile.h"
#include <string.h>

TextFile::TextFile(string fileName)
{
    textStream.open(fileName, ios::in | ios::binary);
    if (!textStream.is_open()) {
        throw exception("Text file does not exist!\n");
    }
    std::string s;
    //for performance
    s.reserve(100);
    while (textStream) {
        lineBegins.push_back(textStream.tellg());
        getline(textStream, s);
    }
    // Last line is empty
    lineBegins.pop_back();
    textStream.clear();
}

DataFile::Entry TextFile::getEntry(Offset off)
{
    // Read line based on where the 
    std::string s;
    //for performance
    s.reserve(100);
    textStream.seekg(lineBegins[off.get()]);
    getline(textStream, s);
    
    // Tokenize string
    static const string delimiter = ", ";
    vector<string> tokResult;
    size_t pos = 0;
    
    while ((pos = s.find(delimiter)) != std::string::npos) {
        tokResult.push_back(s.substr(0, pos));
        s.erase(0, pos + delimiter.length());
    }
    tokResult.push_back(s);

    // Make sure format is correct
    if (tokResult.size() < 4) {
        throw exception("Text file format incorrect!\n");
    }

    // Latitude then longitude
    vector<string> nameComponent(tokResult.begin(), tokResult.end() - 3);
    vector<string> otherComponent(tokResult.end() - 3, tokResult.end());
    double y = stod(otherComponent[1]);
    double x = stod(otherComponent[2]);
    string finalName;
    // Rebuild name in case name contains delimiter
    for (string str : nameComponent) {
        finalName += str;
        finalName += delimiter;
    }
    finalName.erase(finalName.size() - 2, finalName.size());
    Rect entRect = Rect({ x, y }, { x, y });
    Entry ent = { finalName, otherComponent[0], entRect };

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
