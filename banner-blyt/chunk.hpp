#ifndef chunk_hpp
#define chunk_hpp

#include <stdint.h>
#include <vector>
#include <iostream>
#include <string>
#include <boost/shared_ptr.hpp>
#include "be32.hpp"


class chunk;
typedef boost::shared_ptr<chunk> chunk_ptr;
typedef chunk_ptr (*chunk_create_function)(uint32_t id, std::streampos start, std::istream &is);

struct chunk_info
{
	uint32_t m_id;
	std::string m_name;
	chunk_create_function m_create_function;

	chunk_info(uint32_t id, std::string &name, chunk_create_function create_function): m_id(id), m_name(name), m_create_function(create_function) {};
};

typedef boost::shared_ptr<chunk_info> chunk_info_ptr;

class chunk
{
protected:
	uint32_t m_id;
	be32 m_length;
	std::streampos m_offset;
	
	typedef std::vector<chunk_info_ptr> chunk_info_table_type;

	chunk(uint32_t p_id = 0, size_t p_length = 0, std::streampos p_offset = 0): m_id(p_id), m_length(p_length), m_offset(p_offset) {}

	static chunk_info_table_type &get_table();
	static chunk_ptr create_chunk(uint32_t id, std::streampos start, std::istream &is);
	/*
	virtual void serialise(std::ostream &os) const throw(std::ios_base::failure); 
	virtual void unserialise(std::istream &is) throw(std::ios_base::failure);
	*/

	static void register_chunk(uint32_t id, std::string name, chunk_create_function create_function);
	friend std::ostream &operator<<(std::ostream &os, chunk_ptr &c);
public:
	virtual void pretty_print(std::string prefix, std::ostream &os) const throw(std::ios_base::failure);
	static chunk_ptr read_chunk(std::istream &is) throw (std::ios_base::failure);
};

std::ostream &operator<<(std::ostream &os, chunk_ptr &c);



#endif
