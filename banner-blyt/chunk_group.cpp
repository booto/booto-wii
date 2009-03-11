#include <boost/format.hpp>
#include "chunk_group.hpp"
#include "be16.hpp"

chunk_ptr chunk_grp1::create_chunk(uint32_t id, std::streampos start, std::istream &is)
{
	be32 length;
	length.unserialise_bin(is);

	chunk_grp1 *int_rv = new chunk_grp1(*length, start);

	char name_tmp[0x10];
	is.read(name_tmp, sizeof(name_tmp));
	int_rv->m_name = name_tmp;

	be16 num_members;
	num_members.unserialise_bin(is);
	be16 dummy;
	dummy.unserialise_bin(is);



	for(size_t i=0; i<*num_members; i++)
	{
		is.read(name_tmp, sizeof(name_tmp));
		int_rv->m_members.push_back(name_tmp);
	}

	is.seekg(*length + start);

	uint32_t next_id;

	std::streampos tmp_pos;
	
	tmp_pos = is.tellg();
	try
	{
		is.read(reinterpret_cast<char*>(&next_id), sizeof(next_id));
	}
	catch(std::ios_base::failure &e)
	{
		next_id = 0;
		std::cout << "what?: " << e.what() << std::endl;
		is.clear();
	}
	is.seekg(tmp_pos);

	if(next_id==chunk_grp1::grs1_id)
	{
		{
			chunk::read_chunk(is);
			tmp_pos = is.tellg();
			is.read(reinterpret_cast<char*>(&next_id), sizeof(next_id));
			is.seekg(tmp_pos);
		}
		while(next_id != chunk_grp1::gre1_id)
		{
			using boost::format;
			int_rv->m_children.push_back(chunk::read_chunk(is));
			tmp_pos = is.tellg();
			is.read(reinterpret_cast<char*>(&next_id), sizeof(next_id));
			is.seekg(tmp_pos);
		}

		if(next_id == chunk_grp1::gre1_id)
		{
			chunk::read_chunk(is);
		}
	}

	chunk_ptr rv(int_rv);
	return rv;
}

void chunk_grp1::pretty_print(std::string prefix, std::ostream &os) const throw(std::ios_base::failure)
{
	using boost::format;
	os << prefix << format("chunk_grp1(0x%08x)\n") % m_id;
	os << prefix << format(" length: 0x%08x\n") % *m_length;
	os << prefix << format(" offset: 0x%08x\n") % m_offset;
	os << prefix << format(" name: %s\n") % m_name.c_str();
	
	os << prefix << format(" num_members: %d\n") % m_members.size();
	for(std::vector<std::string>::const_iterator svci = m_members.begin();
	    svci != m_members.end();
		svci++)
	{
		os << prefix << "  " << *svci << "\n";
	}

	os << prefix << format(" num_children: %d\n") % m_children.size();
	for(std::vector<chunk_ptr>::const_iterator svci = m_children.begin();
	    svci != m_children.end();
		svci++)
	{
		(*svci)->pretty_print(prefix+"  ", os);
	}
	os << prefix << "chunk_grp1_end\n";
}
