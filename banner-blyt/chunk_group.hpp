#ifndef chunk_grp1_hpp
#define chunk_grp1_hpp

#include <stdint.h>
#include <vector>
#include "chunk.hpp"
#include "bf32.hpp"

class chunk_grp1: public chunk
{
	std::string m_name;
	/*
	float m_funk1;
	float m_funk2;
	float m_funk3;
	float m_funk4;
	*/

	std::vector<chunk_ptr> m_children;
	std::vector<std::string> m_members;

public:
	static const uint32_t id = 0x31707267L;
	static const uint32_t grs1_id = 0x31737267L;
	static const uint32_t gre1_id = 0x31657267L;
	chunk_grp1(uint32_t p_length, std::streampos p_offset): chunk(id, p_length, p_offset),
	    m_name("") {};
	static chunk_ptr create_chunk(uint32_t id, std::streampos start, std::istream &is);
	virtual void pretty_print(std::string prefix, std::ostream &os) const throw(std::ios_base::failure);

private:
	class reg
	{
		public:
		reg()
		{
			chunk::register_chunk(chunk_grp1::id, "grp1", &chunk_grp1::create_chunk);
		}
	};
	static reg _r;
};

chunk_grp1::reg chunk_grp1::_r;



#endif
