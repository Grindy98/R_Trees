#include <iostream>
#include <chrono>
#include <random>
#include <ctime>
#include "Tree.h"
#include "ShapeFile.h"

int main() {

    ShapeFile shp("C:\\Users\\stefa\\Desktop\\ShapefileTest");
    auto no = shp.getNumberOfElements().get();
    uniform_int_distribution unif(0, no - 1);
    default_random_engine re;
    re.seed(time(0));
    for (int i = 0; i < 10; i++)
    {
        
    }
    system("pause");

    return 0;
}