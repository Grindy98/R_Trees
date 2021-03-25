#include <iostream>
#include <chrono>
#include <random>
#include <ctime>
#include "Tree.h"
#include "TextFile.h"


int main() {
    using namespace std;
    TextFile txt = TextFile("locations.txt");

    for (unsigned i = 0; i < txt.getNumberOfElements(); i++)
    {
        auto entry = txt.getEntry(Offset(i));
        cout << entry.name << ", " << entry.tag << " | " <<
            entry.BB.getDownLeft().x << ", " << entry.BB.getDownLeft().y << endl;
    }

    system("pause");

    return 0;
}