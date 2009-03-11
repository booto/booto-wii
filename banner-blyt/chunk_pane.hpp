#ifndef chunk_pan1_hpp
#define chunk_pan1_hpp

#include <stdint.h>
#include <vector>
#include "chunk.hpp"
#include "bf32.hpp"

class chunk_pan1: public chunk
{
	uint32_t m_unk1;
	std::string m_name;
	/*
	float m_funk1;
	float m_funk2;
	float m_funk3;
	float m_funk4;
	*/
	bf32 m_funk1;
	bf32 m_funk2;
	bf32 m_funk3;
	bf32 m_funk4;
	

	std::vector<chunk_ptr> m_children;

public:
	static const uint32_t id = 0x316e6170L;
	static const uint32_t pas1_id = 0x31736170L;
	static const uint32_t pae1_id = 0x31656170L;
	chunk_pan1(uint32_t p_length, std::streampos p_offset): chunk(id, p_length, p_offset),
	    m_unk1(0), m_name(""), m_funk1(0.0f), m_funk2(0.0f), m_funk3(0.0f), m_funk4(0.0f)  {};
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
