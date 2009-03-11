#ifndef be16_hpp
#define be16_hpp
#include <iostream>
#include <stdint.h>
#include <arpa/inet.h>

class be16
{
	uint16_t value; // in big endian;
public:
	explicit be16(uint16_t v): value(htons(v)) {}
	be16(): value(0) {}

	inline uint16_t operator*() const
	{
		return to_uint16_t();
	}

	inline uint16_t to_uint16_t() const
	{
		return ntohs(value);
	}

	inline be16 &operator = (const uint16_t &v)
	{
		value = htons(v);
		return *this;
	}

	inline be16 &from_be(const uint16_t &v)
	{
		value = v;
		return *this;
	}

	inline void unserialise_bin(std::istream &is) throw (std::ios_base::failure)
	{
		is.read(reinterpret_cast<char*>(&value), sizeof(value));
	}
};

#endif
