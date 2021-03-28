#include <iostream>
#include <chrono>
#include <random>
#include <ctime>
#include "Tree.h"
#include "ShapeFile.h"

int main() {

    shared_ptr<ShapeFile> shpf = make_shared<ShapeFile>("C:\\Users\\stefa\\Desktop\\romania-latest-free");
    Tree t(shpf, "indexro");
    // Radius is NOT in Km, but in pseudo-degrees
    // We have to use some sort of function to convert to Km from this representation

    // Get a random point and search for data
    int64_t size = shpf->getNumberOfElements().get();

    uniform_int_distribution<long long> unif(0, (int64_t)size - 1);
    default_random_engine re;
    re.seed(time(0));

    auto entry = shpf->getEntry(Offset(unif(re)));

    // In km
    double radius = 1;
    Point timisoaraCoords = {21.226678, 45.766035 };
    Point resitaCoords = { 21.8821, 45.3050 };

    Point toSearch = timisoaraCoords;

    auto res = t.search(toSearch, radius, true, "pharmacy");

    std::cout << "Original point is: " <<
        toSearch.x << ", " << toSearch.y << endl;
    std::cout << "Radius of search is: " << radius << endl;
    std::cout << endl;

    for (const auto& entry : res) {
        if (entry.name.empty()) {
            continue;
        }
        std::cout << entry.name << ", " << entry.tag << " | " <<
            entry.BB.getDownLeft().x << ", " << entry.BB.getDownLeft().y << endl;
    }

    system("pause");

    return 0;
}