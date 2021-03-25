
#include <iostream>
#include <chrono>
#include "Node.h"
#include <random>
#include "TextFile.h"
#include "Tree.h"


int main(){
    using namespace std;

    unique_ptr<DataFile> dataf = make_unique<TextFile>("locations.txt");
    Tree t(move(dataf), "indexf", 5);
    // Radius is NOT in Km, but in pseudo-degrees
    // We have to use some sort of function to convert to Km from this representation

    // Get a random point and search for data
    unsigned size = dataf->getNumberOfElements();

    uniform_int_distribution unif(0, (int)size - 1);
    default_random_engine re;
    re.seed(time(0));

    auto entry = dataf->getEntry(Offset(unif(re)));

    auto res = t.search(entry.BB.getDownLeft(), 0.02);

    for (auto entry : res) {

    }

    system("pause");

    return 0;
}

