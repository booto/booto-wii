#ifndef brlyt_header_hpp
#define brlyt_header_hpp

#include <stdint.h>
#include <stdexcept>
#include <iostream>
#include <istream>
#include <boost/format.hpp>
#include "be32.hpp"
#include "be16.hpp"


const uint32_t brlyt_magic = 0x54594c52L; 
struct brlyt_header
{
	uint32_t magic;
	uint32_t _unk1;
	be32 size;
	be16 entry_offset;
	be16 num_root_chunks;

	void unserialise_bin(std::istream &is) throw (std::ios_base::failure)
	{
		is.read(reinterpret_cast<char*>(&magic), sizeof(magic));
		is.read(reinterpret_cast<char*>(&_unk1), sizeof(_unk1));
		size.unserialise_bin(is);
		entry_offset.unserialise_bin(is);
		num_root_chunks.unserialise_bin(is);
	}
 
};

std::ostream &operator<<(std::ostream &os, brlyt_header &bh)
{
	using boost::format;
	os << "brlyt_header:\n";
	os << format("magic: 0x%08x\n") % bh.magic;
	os << format("_unk1: 0x%08x\n") % bh._unk1;
	os << format("size: 0x%08x\n") % *bh.size;
	os << format("entry_offset: 0x%04x\n") % *bh.entry_offset;
	os << format("num_root_chunks: 0x%04x\n") % *bh.num_root_chunks;
	return os;
}

#endif
