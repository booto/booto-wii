#ifndef be32_hpp
#define be32_hpp
#include <iostream>
#include <stdint.h>
#include <arpa/inet.h>

class be32
{
	uint32_t value; // in big endian;

public:
	explicit be32(uint32_t v): value(htonl(v)) {}
	be32(): value(0) {}

	inline uint32_t operator*() const
	{
		return to_uint32_t();
	}

	inline uint32_t to_uint32_t() const
	{
		return ntohl(value);
	}


	inline be32 &operator = (const uint32_t &v)
	{
		value = htonl(v);
		return *this;
	}

	inline be32 &from_be(const uint32_t &v)
	{
		value = v;
		return *this;
	}

	inline void unserialise_bin(std::istream &is) throw(std::ios_base::failure)
	{
		is.read(reinterpret_cast<char*>(&value), sizeof(value));
	}
};

#endif
