#include "Rect.h"
#include <cassert>
Rect::Rect(Point downLeft, Point upRight)
	:
	downLeft(downLeft),
	upRight(upRight)
{
	assert(downLeft.x <= upRight.x && downLeft.y <= upRight.y);
}

Point Rect::getDownLeft() const
{
	return downLeft;
}

Point Rect::getUpRight() const
{
	return upRight;
}

bool Rect::contains(const Point& inPoint) const
{
	return downLeft.x <= inPoint.x && downLeft.y <= inPoint.y && 
		upRight.x >= inPoint.x && upRight.y >= inPoint.y;
}

bool Rect::contains(const Rect& inRect) const
{
	// If both extreme points are included then the rect is also included
	return contains(inRect.downLeft) && contains(inRect.upRight);
}

bool Rect::intersects(const Rect& xRect) const
{
	// If any extreme point is included, this intersects xRect
	bool dlCond = xRect.upRight.x >= downLeft.x && xRect.upRight.y >= downLeft.y;
	bool urCond = xRect.downLeft.x <= upRight.x && xRect.downLeft.y <= upRight.y;
	return dlCond || urCond;
}

Rect Rect::extend(const Point& outPoint) const
{
	Point newDL(downLeft), newUR(upRight);
	if (outPoint.x < downLeft.x) {
		newDL.x = outPoint.x;
	}
	if (outPoint.y < downLeft.y) {
		newDL.y = outPoint.y;
	}	
	if (upRight.x < outPoint.x) {
		newUR.x = outPoint.x;
	}
	if (upRight.y < outPoint.y) {
		newUR.y = outPoint.y;
	}
	return Rect(newDL, newUR);
}

Rect Rect::extend(const Rect& outRect) const
{
	// Rect extend is just an extend for each extreme point of the outRect 
	// checked against its appropriate limit
	Point newDL(downLeft), newUR(upRight);
	if (outRect.getDownLeft().x < downLeft.x) {
		newDL.x = outRect.getDownLeft().x;
	}
	if (outRect.getDownLeft().y < downLeft.y) {
		newDL.y = outRect.getDownLeft().y;
	}
	if (upRight.x < outRect.getUpRight().x) {
		newUR.x = outRect.getUpRight().x;
	}
	if (upRight.y < outRect.getUpRight().y) {
		newUR.y = outRect.getUpRight().y;
	}
	return Rect(newDL, newUR);
}

double Rect::getArea() const
{
	Point diff = upRight - downLeft;
	return diff.x * diff.y;
}

double Rect::getMinExtraAreaToCover(const Point& point) const
{
	Rect temp = extend(point);
	return temp.getArea() - getArea();
}

double Rect::getMinExtraAreaToCover(const Rect& rect) const
{
	Rect temp = extend(rect);
	return temp.getArea() - getArea();
}
