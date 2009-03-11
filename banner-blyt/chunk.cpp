#include <boost/format.hpp>
#include "chunk.hpp"
#include "be32.hpp"



chunk::chunk_info_table_type &chunk::get_table()
{
	static chunk_info_table_type chunk_info_table; 
	return chunk_info_table;
}

void chunk::register_chunk(uint32_t id, std::string name, chunk_create_function create_function)
{
	chunk_info_ptr cip(new chunk_info(id, name, create_function));
	std::cout << "registering " << name << std::endl;
	get_table().push_back(cip);
}

chunk_ptr chunk::read_chunk(std::istream &is) throw(std::ios_base::failure)
{
	uint32_t id;
	be32 length;

	std::istream::pos_type chunk_start = is.tellg();
	is.read(reinterpret_cast<char*>(&id), sizeof(id));

	
	chunk_create_function ccf = &chunk::create_chunk;

	for(chunk_info_table_type::const_iterator cittci = get_table().begin();
	    cittci != get_table().end();
		cittci++)
	{
		if((*cittci)->m_id == id)
		{
			ccf = (*cittci)->m_create_function;

			break;
		}
	}
	chunk_ptr rv = ccf(id,chunk_start, is);

	return rv;
}

chunk_ptr chunk::create_chunk(uint32_t id, std::istream::pos_type start, std::istream &is)
{
	using boost::format;
	be32 length;
	length.unserialise_bin(is);
	chunk_ptr rv(new chunk(id, *length, start));
	is.seekg(*length + start);
	return rv;
}

void chunk::pretty_print(std::string prefix, std::ostream &os) const throw(std::ios_base::failure)
{
	using boost::format;
	os << prefix << format("chunk(0x%08x)=%.4s\n") % m_id % reinterpret_cast<const char *>(&m_id);
	os << prefix << format(" length: 0x%08x\n") % *m_length;
	os << prefix << format(" offset: 0x%08x\n") % m_offset;
	os << prefix << "chunk_end\n";
}

std::ostream &operator<<(std::ostream &os, chunk_ptr &c)
{
	c->pretty_print("", os);
	return os;
}
