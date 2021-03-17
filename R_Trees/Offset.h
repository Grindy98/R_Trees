#pragma once
#include <cstdint>
#include <ios>

class Offset
{
public:
	Offset();
	Offset(std::streamoff offset);

	Offset operator+(int other) const;
	int64_t getVal() const;
	bool notNull() const;
private:
	int64_t offset;
};

