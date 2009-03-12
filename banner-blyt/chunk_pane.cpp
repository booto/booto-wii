#include <boost/format.hpp>
#include "chunk_pane.hpp"

chunk_ptr chunk_pan1::create_chunk(uint32_t id, std::streampos start, std::istream &is)
{
	be32 length;
	length.unserialise_bin(is);

	chunk_pan1 *int_rv = new chunk_pan1(*length, start);

	is.read(reinterpret_cast<char*>(&int_rv->m_flag1), sizeof(int_rv->m_flag1));
	is.read(reinterpret_cast<char*>(&int_rv->m_flag2), sizeof(int_rv->m_flag2));
	is.read(reinterpret_cast<char*>(&int_rv->m_alpha1), sizeof(int_rv->m_alpha1));
	is.read(reinterpret_cast<char*>(&int_rv->m_alpha2), sizeof(int_rv->m_alpha2));

	{
		char name_tmp[24];
		is.read(name_tmp, sizeof(name_tmp));
		int_rv->m_name = name_tmp;
	}

	int_rv->m_x.unserialise_bin(is);
	int_rv->m_y.unserialise_bin(is);
	int_rv->m_z.unserialise_bin(is);
	int_rv->m_flip_x.unserialise_bin(is);
	int_rv->m_flip_y.unserialise_bin(is);
	int_rv->m_angle.unserialise_bin(is);
	int_rv->m_x_mag.unserialise_bin(is);
	int_rv->m_y_mag.unserialise_bin(is);
	int_rv->m_width.unserialise_bin(is);
	int_rv->m_height.unserialise_bin(is);

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
	os << prefix << format(" flags: 0x%02x, 0x%02x\n") % (int)m_flag1 % (int)m_flag2;
	os << prefix << format(" alpha: 0x%02x, 0x%02x\n") % (int)m_alpha1 % (int)m_alpha2;
	os << prefix << format(" name: %s\n") % m_name.c_str();
	os << prefix << format(" (x,y,z): (%f, %f, %f)\n") % *m_x % *m_y % *m_z;
	os << prefix << format(" flip(x,y): (%f, %f)\n") % *m_flip_x % *m_flip_y;
	
	os << prefix << format(" angle: %f\n") % *m_angle;
	os << prefix << format(" mag(x,y): (%f, %f)\n") % *m_x_mag % *m_y_mag;
	os << prefix << format(" width: %f\n") % *m_width;
	os << prefix << format(" height: %f\n") % *m_height;
	os << prefix << format(" num_children: %d\n") % m_children.size();

	for(std::vector<chunk_ptr>::const_iterator svci = m_children.begin();
	    svci != m_children.end();
		svci++)
	{
		(*svci)->pretty_print(prefix + "  ", os);
	}
	os << prefix << "chunk_pan1_end\n";
}
