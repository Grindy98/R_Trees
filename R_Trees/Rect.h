#pragma once
#include "Point.h"
class Rect
{
public:
	Rect(Point downLeft, Point upRight);
	Point getDownLeft() const;
	Point getUpRight() const;

	bool contains(const Point& inPoint) const;
	bool contains(const Rect& inRect) const;

	Rect extend(const Point& outPoint) const;
	Rect extend(const Rect& outRect) const;

	double getArea() const;
	double getMinExtraAreaToCover(const Point& point) const;
	double getMinExtraAreaToCover(const Rect& rect) const;
private:
	Point downLeft, upRight;
};

