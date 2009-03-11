#include <boost/format.hpp>
#include "chunk_pane.hpp"

chunk_ptr chunk_pan1::create_chunk(uint32_t id, std::streampos start, std::istream &is)
{
	be32 length;
	length.unserialise_bin(is);

	chunk_pan1 *int_rv = new chunk_pan1(*length, start);

	is.read(reinterpret_cast<char*>(&int_rv->m_unk1), sizeof(int_rv->m_unk1));

	{
		char name_tmp[0x30];
		is.read(name_tmp, sizeof(name_tmp));
		int_rv->m_name = name_tmp;
	}

	int_rv->m_funk1.unserialise_bin(is);
	int_rv->m_funk2.unserialise_bin(is);
	int_rv->m_funk3.unserialise_bin(is);
	int_rv->m_funk4.unserialise_bin(is);

/*
	is.read(reinterpret_cast<char*>(&int_rv->m_funk1), sizeof(int_rv->m_funk1));
	is.read(reinterpret_cast<char*>(&int_rv->m_funk2), sizeof(int_rv->m_funk2));
	is.read(reinterpret_cast<char*>(&int_rv->m_funk3), sizeof(int_rv->m_funk3));
	is.read(reinterpret_cast<char*>(&int_rv->m_funk4), sizeof(int_rv->m_funk4));
*/

	is.seekg(*length + start);

	uint32_t next_id;

	std::streampos tmp_pos;
	
	tmp_pos = is.tellg();
	is.read(reinterpret_cast<char*>(&next_id), sizeof(next_id));
	is.seekg(tmp_pos);

	if(next_id==chunk_pan1::pas1_id)
	{
		{
			chunk::read_chunk(is);
			tmp_pos = is.tellg();
			is.read(reinterpret_cast<char*>(&next_id), sizeof(next_id));
			is.seekg(tmp_pos);
		}
		while(next_id != chunk_pan1::pae1_id)
		{
			using boost::format;
			int_rv->m_children.push_back(chunk::read_chunk(is));
			tmp_pos = is.tellg();
			is.read(reinterpret_cast<char*>(&next_id), sizeof(next_id));
			is.seekg(tmp_pos);
		}

		if(next_id == chunk_pan1::pae1_id)
		{
			chunk::read_chunk(is);
		}
	}

	chunk_ptr rv(int_rv);
	return rv;
}

void chunk_pan1::pretty_print(std::string prefix, std::ostream &os) const throw(std::ios_base::failure)
{
	using boost::format;
	os << prefix << format("chunk_pan1(0x%08x)\n") % m_id;
	os << prefix << format(" length: 0x%08x\n") % *m_length;
	os << prefix << format(" offset: 0x%08x\n") % m_offset;
	os << prefix << format(" unk1: 0x%08x\n") % m_unk1;
	os << prefix << format(" name: %s\n") % m_name.c_str();
	
	os << prefix << format(" funk1: %f\n") % *m_funk1;
	os << prefix << format(" funk2: %f\n") % *m_funk2;
	os << prefix << format(" funk3: %f\n") % *m_funk3;
	os << prefix << format(" funk4: %f\n") % *m_funk4;
	/*
	os << prefix << format(" funk1: %f\n") % m_funk1;
	os << prefix << format(" funk2: %f\n") % m_funk2;
	os << prefix << format(" funk3: %f\n") % m_funk3;
	os << prefix << format(" funk4: %f\n") % m_funk4;
	*/
	os << prefix << format(" num_children: %d\n") % m_children.size();
	for(std::vector<chunk_ptr>::const_iterator svci = m_children.begin();
	    svci != m_children.end();
		svci++)
	{
		(*svci)->pretty_print(prefix + "  ", os);
	}
	os << prefix << "chunk_pan1_end\n";
}
