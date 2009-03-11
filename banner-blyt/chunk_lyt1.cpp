#include <boost/format.hpp>
#include "chunk_lyt1.hpp"

chunk_ptr chunk_lyt1::create_chunk(uint32_t id, std::streampos start, std::istream &is)
{
	be32 length;
	length.unserialise_bin(is);

	chunk_lyt1 *int_rv = new chunk_lyt1(*length, start);

	int_rv->m_unk1.unserialise_bin(is);
	int_rv->m_width.unserialise_bin(is);
	int_rv->m_height.unserialise_bin(is);

	chunk_ptr rv(int_rv);

	is.seekg(*length + start);

	return rv;
}

void chunk_lyt1::pretty_print(std::string prefix, std::ostream &os) const throw(std::ios_base::failure)
{
	using boost::format;
	os << prefix << format("chunk_lyt1(0x%08x)\n") % m_id;
	os << prefix << format(" length: 0x%08x\n") % *m_length;
	os << prefix << format(" offset: 0x%08x\n") % m_offset;
	os << prefix << format(" unk1: 0x%08x\n") % *m_unk1;
	os << prefix << format(" width: %f\n") % *m_width;
	os << prefix << format(" height: %f\n") % *m_height;
	os << prefix << "chunk_end\n";
}
