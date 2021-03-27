#include <iostream>
#include <chrono>
#include <random>
#include <ctime>
#include "Tree.h"
#include "ShapeFile.h"

int main() {

    ShapeFile shp("C:\\Users\\stefa\\Desktop\\ShapefileTest");
    auto no = shp.getNumberOfElements().get();
    for (int i = 0; i < 10; i++)
    {
        
    }
    system("pause");

    return 0;
}