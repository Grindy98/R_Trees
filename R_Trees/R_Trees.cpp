
#include <iostream>
#include <chrono>
#include "Node.h"
#include <random>
#include "TextFile.h"
#include "Tree.h"


int main(){
    using namespace std;

    shared_ptr<DataFile> dataf = make_shared<TextFile>("locations.txt");
    Tree t(dataf, "indexf");
    // Radius is NOT in Km, but in pseudo-degrees
    // We have to use some sort of function to convert to Km from this representation

    // Get a random point and search for data
    unsigned size = dataf->getNumberOfElements();

    uniform_int_distribution unif(0, (int)size - 1);
    default_random_engine re;
    re.seed(time(0));

    auto entry = dataf->getEntry(Offset(unif(re)));

    double radius = 0.02;
    auto res = t.search(entry.BB.getDownLeft(), radius, "hospital-health-care-facility");

    cout << "Original point is: " << 
        entry.BB.getDownLeft().x << ", " << entry.BB.getDownLeft().y << endl;
    cout << "Radius of search is: " << radius << endl;
    cout << endl;

    for (const auto& entry : res) {
        cout << entry.name << ", " << entry.tag << " | " <<
            entry.BB.getDownLeft().x << ", " << entry.BB.getDownLeft().y << endl;
    }

    system("pause");

    return 0;
}

