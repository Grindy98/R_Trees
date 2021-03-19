
#include <iostream>
#include <chrono>
#include "Point.h"
#include "Rect.h"


int main(){
    using namespace std;

    Point x(3, 1), y(4, 2), z(5, 5);
    Point w(3.5, 4);
    Rect a(x, y), b(y, z);
    cout << a.getArea() << " " << a.contains(w) << " " << b.contains(w) << endl;
    cout << b.extend(x).getDownLeft().x << " " << b.extend(x).getDownLeft().y << " ";
    cout << b.extend(x).getUpRight().x << " " << b.extend(x).getUpRight().y << endl;
    cout << a.getMinExtraAreaToCover(z) << " " << a.getMinExtraAreaToCover(x) << " ";
    cout << a.getMinExtraAreaToCover(w) << endl;

    double myDoubles[4] = {1, 2, 4, 3};
    char* myArr = (char*)myDoubles;
    Point p1 = *(Point*)(myArr);
    Point p2 = *(Point*)(myArr + sizeof(Point));
    cout << p1.x << " " << p1.y << " " << p2.x << " " << p2.y << endl; 
    Point* myPoints = (Point*)myDoubles;
    cout << myPoints[0].x << " " << myPoints[0].y << " " << myPoints[1].x << " " << myPoints[1].y << endl;

    p1 = (*(Rect*)(myDoubles)).getDownLeft();
    p2 = (*(Rect*)(myDoubles)).getUpRight();
    cout << p1.x << " " << p1.y << " " << p2.x << " " << p2.y << endl;
    system("pause");

    return 0;
}

