#include "Point.h"

Point::Point(double x, double y)
	:
	x(x),
	y(y)
{
}

Point Point::operator+(const Point& other) const
{
	return Point(x + other.x, y + other.y);
}

Point Point::operator-(const Point& other) const
{
	return Point(x - other.x, y - other.y);
}
