#include <iostream>
#include <chrono>
#include <random>
#include <ctime>
#include "Tree.h"
#include "ShapeFile.h"

int main() {

    ShapeFile shp("C:\\Users\\stefa\\Desktop\\ShapefileTest");
    auto no = shp.getNumberOfElements().get();
    uniform_int_distribution<long long> unif(0, no - 1);
    default_random_engine re;
    re.seed(time(0));
    for (int i = 0; i < 100;)
    {
        auto entry = shp.getEntry(Offset(unif(re)));
        if (entry.tag == "village") {
            cout << entry.name << ", " << entry.tag << " | " <<
                entry.BB.getDownLeft().x << ", " << entry.BB.getDownLeft().y << endl;
            i++;
        }
    }
    system("pause");

    return 0;
}