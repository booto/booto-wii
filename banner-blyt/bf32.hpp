#ifndef bf32_hpp
#define bf32_hpp
#include <iostream>
#include <stdint.h>
#include <arpa/inet.h>

class bf32
{
	union
	{
		float value; // in big endian;
		uint32_t value_back; // make maniuplations easier
	};

public:
	explicit bf32(float v)
	{
		union
		{
			uint32_t a;
			float b;
		};
		b = v;
		value_back = htonl(a);
	}

	bf32(): value_back(0) {}

	inline float operator*() const
	{
		return to_float();
	}

	inline float to_float() const
	{
		union
		{
			uint32_t a;
			float b;
		};
		a = ntohl(value_back);
		return b;
	}


	inline bf32 &operator = (const float &v)
	{
		union
		{
			uint32_t a;
			float b;
		};
		b = v;
		value_back = htonl(a);
		return *this;
	}

	inline bf32 &from_be(const float &v)
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
