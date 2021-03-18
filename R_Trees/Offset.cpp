#include "Offset.h"

Offset::Offset()
	:
	offset(0x0)
{
}

Offset::Offset(int64_t off)
	:
	offset(off)
{
}

Offset Offset::operator+(int other) const
{
	return Offset(offset + other);
}

Offset Offset::operator-(int other) const
{
	return offset - other;
}

bool Offset::operator==(const Offset& other) const
{
	return offset == other.offset;
}

bool Offset::operator<(const Offset& other) const
{
	return offset < other.offset;
}

int64_t Offset::get() const
{
	return offset;
}

bool Offset::notNull() const
{
	return offset != 0x0;
}
