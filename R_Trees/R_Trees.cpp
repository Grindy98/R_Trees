
#include <iostream>
#include <chrono>
#include "Node.h"


int main(){
    using namespace std;

    Node n = Node(2, true, 3);
    Rect a({0, 1}, {2, 3});
    Rect b({4, 5}, {6, 7});
    n.childrenBB[0] = a;
    n.childrenBB[1] = b;
    n.childrenBB[2] = a;
    
    n.shapeDataOffset[0] = Offset(0);
    n.shapeDataOffset[1] = Offset(1);
    n.shapeDataOffset[2] = Offset(2);

    auto res = Node::serialize(n);
    auto nodeRes = Node::deserialize(res, n.degree);
    
    for (int i = 0; i < nodeRes->arrSize; i++) {
        cout << nodeRes->childrenBB[i].getDownLeft().x << ", ";
        cout << nodeRes->childrenBB[i].getDownLeft().y << " | ";
        cout << nodeRes->childrenBB[i].getUpRight().x << ", ";
        cout << nodeRes->childrenBB[i].getUpRight().y << endl;
    } 
    cout << endl << "Shp" << endl;
    for (int i = 0; i < nodeRes->shapeDataOffset.size(); i++) {
        cout << hex << nodeRes->shapeDataOffset[i].get() << ", ";
    }   
    cout << endl << "children" << endl;
    for (int i = 0; i < nodeRes->childrenOffset.size(); i++) {
        cout << hex << nodeRes->childrenOffset[i].get() << ", ";
    }
    cout << endl;

    system("pause");

    return 0;
}

