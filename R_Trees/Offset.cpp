#include "Offset.h"

Offset::Offset()
	:
	offset(0x0)
{
}

Offset::Offset(std::streamoff offset)
	:
	offset(offset)
{
}

Offset Offset::operator+(int other) const
{
	return Offset(offset + other);
}

int64_t Offset::getVal() const
{
	return offset;
}

bool Offset::notNull() const
{
	return offset != 0x0;
}
