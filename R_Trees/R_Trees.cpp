#include <iostream>
#include <chrono>
#include <random>
#include <ctime>
#include "Tree.h"
#include "TestFile.h"


int main() {
    using namespace std;
    try
    {
        auto test = make_unique<TestFile>();
        Tree t(move(test), "index", 3);

        uniform_real_distribution<double> unif(-10.0, 10.0);
        default_random_engine re;
        re.seed(time(0));
        for (int i = 0; i < 1000; i++) {
            Point p1, p2;
            p1 = { unif(re), unif(re) };
            //p2 = { unif(re), unif(re) };
            //if (p1.x > p2.x) {
            //    swap(p1.x, p2.x);
            //}
            //if (p1.y > p2.y) {
            //    swap(p1.y, p2.y);
            //}
            t.insert(make_pair(Rect(p1, p1), Offset(-1)));
        }

        auto res = t.search({ 0.0, 0.0 }, 1);
        cout << res.size();
    }
    catch (const exception& e)
    {
        cout << e.what() << endl;
    }
}