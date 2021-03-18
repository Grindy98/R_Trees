#pragma once
struct Point
{
public:
	Point(double x, double y);
	Point operator+(const Point& other) const;
	Point operator-(const Point& other) const;
	double x, y;
};

