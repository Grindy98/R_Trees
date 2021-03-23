#pragma once
#include <cstdint>
#include <ios>

class Offset
{
public:
	Offset();
	explicit Offset(int64_t off);

	Offset operator+(int other) const;
	Offset operator-(int other) const;
	bool operator==(const Offset& other) const;
	bool operator<(const Offset& other) const;
	bool operator<=(const Offset& other) const;
	int64_t get() const;
	bool notNull() const;
private:
	int64_t offset;
};

