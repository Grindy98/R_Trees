
#include <iostream>
#include <chrono>
#include "Node.h"
#include <random>
#include <ctime>


int main(){
    using namespace std;

    Node n = Node(3, true);
    uniform_real_distribution<double> unif(-10.0, 10.0);
    default_random_engine re;
    re.seed(time(0));
    for (int i = 0; i < 7; i++) {
        Point p1, p2;
        p1 = { unif(re), unif(re) };
        p2 = { unif(re), unif(re) };
        if (p1.x > p2.x) {
            swap(p1.x, p2.x);
        }
        if (p1.y > p2.y) {
            swap(p1.y, p2.y);
        }
        n.insert(Rect(p1, p2), Offset(0x0));
    }

    for (int i = 0; i < n.getArrSize(); i++) {
        cout << "[";
        cout << n.childrenBB[i].getDownLeft().x << ", ";
        cout << n.childrenBB[i].getDownLeft().y << " , ";
        cout << n.childrenBB[i].getUpRight().x << ", ";
        cout << n.childrenBB[i].getUpRight().y << "]," << endl;
    }

    auto res = n.split();

    cout << endl;
    for (int i = 0; i < res[0].first->getArrSize(); i++) {
        cout << "[";
        cout << res[0].first->childrenBB[i].getDownLeft().x << ", ";
        cout << res[0].first->childrenBB[i].getDownLeft().y << " , ";
        cout << res[0].first->childrenBB[i].getUpRight().x << ", ";
        cout << res[0].first->childrenBB[i].getUpRight().y << "]," << endl;
    }
    cout << endl;
    for (int i = 0; i < res[1].first->getArrSize(); i++) {
        cout << "[";
        cout << res[1].first->childrenBB[i].getDownLeft().x << ", ";
        cout << res[1].first->childrenBB[i].getDownLeft().y << " , ";
        cout << res[1].first->childrenBB[i].getUpRight().x << ", ";
        cout << res[1].first->childrenBB[i].getUpRight().y << "]," << endl;
    }
    cout << endl;
    cout << "[";
    cout << res[0].second.getDownLeft().x << ", ";
    cout << res[0].second.getDownLeft().y << " , ";
    cout << res[0].second.getUpRight().x << ", ";
    cout << res[0].second.getUpRight().y << "];" << endl;   
    cout << "[";
    cout << res[1].second.getDownLeft().x << ", ";
    cout << res[1].second.getDownLeft().y << " , ";
    cout << res[1].second.getUpRight().x << ", ";
    cout << res[1].second.getUpRight().y << "];" << endl;

    system("pause");
    return 0;
}

