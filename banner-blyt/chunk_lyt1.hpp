#ifndef chunk_lyt1_hpp
#define chunk_lyt1_hpp

#include "chunk.hpp"
#include "be32.hpp"
#include "bf32.hpp"

class chunk_lyt1: public chunk
{

	be32 m_unk1;
	bf32 m_width;
	bf32 m_height;

public:
	static const uint32_t id = 0x3174796cL;
	chunk_lyt1(uint32_t p_length, std::streampos p_offset): chunk(id, p_length, p_offset), m_unk1(0), m_width(0.0), m_height(0.0) {};
	static chunk_ptr create_chunk(uint32_t id, std::streampos start, std::istream &is);
	virtual void pretty_print(std::string prefix, std::ostream &os) const throw(std::ios_base::failure);

private:
	class reg
	{
		public:
		reg()
		{
			chunk::register_chunk(chunk_lyt1::id, "lyt1", &chunk_lyt1::create_chunk);
		}
	};
	static reg _r;
};

chunk_lyt1::reg chunk_lyt1::_r;



#endif
