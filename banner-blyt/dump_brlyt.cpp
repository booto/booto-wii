#include <fstream>
#include <iostream>
#include "brlyt_header.hpp"
#include "chunk.hpp"

int main(int argc, char *argv[])
{
	if(argc != 2) return 0;
	std::ifstream input(argv[1], std::ios_base::binary | std::ios_base::in);
	input.exceptions(std::ifstream::badbit |
	                 std::ifstream::eofbit |
	                 std::ifstream::failbit);
	input.seekg(0);

	
	

	brlyt_header bh;

	try
	{
		bh.unserialise_bin(input);
		std::cout << bh;
	}
	catch(std::exception &se)
	{
		std::cerr << se.what() << std::endl;
		return -1;
	}


	std::vector<chunk_ptr> chunks;
	while(input.tellg() < *bh.size)
	{
		chunks.push_back(chunk::read_chunk(input));
	}


	for(std::vector<chunk_ptr>::const_iterator svci= chunks.begin();
	    svci != chunks.end();
	    svci++)
	{
		(*svci)->pretty_print("",std::cout);
	}

}
