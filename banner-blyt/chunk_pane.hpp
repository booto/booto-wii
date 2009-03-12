#ifndef chunk_pan1_hpp
#define chunk_pan1_hpp

#include <stdint.h>
#include <vector>
#include "chunk.hpp"
#include "bf32.hpp"

class chunk_pan1: public chunk
{

	uint8_t m_flag1;
	uint8_t m_flag2;
	uint8_t m_alpha1;
	uint8_t m_alpha2;
	std::string m_name;
	bf32 m_x;
	bf32 m_y;
	bf32 m_z;
	bf32 m_flip_x;
	bf32 m_flip_y;
	bf32 m_angle;
	bf32 m_x_mag;
	bf32 m_y_mag;
	bf32 m_width;
	bf32 m_height;
	

	std::vector<chunk_ptr> m_children;

public:
	static const uint32_t id = 0x316e6170L;
	static const uint32_t pas1_id = 0x31736170L;
	static const uint32_t pae1_id = 0x31656170L;
	chunk_pan1(uint32_t p_length, std::streampos p_offset):
		chunk(id, p_length, p_offset),
		m_flag1(0), m_flag2(0), m_alpha1(0), m_alpha2(0), m_name(""),
		m_x(0.0f), m_y(0.0f), m_z(0.0f),
		m_flip_x(0.0f), m_flip_y(0.0f), m_angle(0.0f),
		m_x_mag(0.0f), m_y_mag(0.0f),
		m_width(0.0f), m_height(0.0f) {};
	static chunk_ptr create_chunk(uint32_t id, std::streampos start, std::istream &is);
	virtual void pretty_print(std::string prefix, std::ostream &os) const throw(std::ios_base::failure);

private:
	class reg
	{
		public:
		reg()
		{
			chunk::register_chunk(chunk_pan1::id, "pan1", &chunk_pan1::create_chunk);
		}
	};
	static reg _r;
};

chunk_pan1::reg chunk_pan1::_r;



#endif
