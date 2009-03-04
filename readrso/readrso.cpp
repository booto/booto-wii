#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>


class be32
{
	uint32_t value; // in big endian;

public:
	explicit be32(uint32_t v): value(htonl(v)) {}
	be32(): value(0) {}

	uint32_t operator*()
	{
		return to_uint32_t();
	}

	uint32_t to_uint32_t() const
	{
		return ntohl(value);
	}


	be32 &operator = (const uint32_t &v)
	{
		value = htonl(v);
		return *this;
	}
};

unsigned long elf_hash(const unsigned char *name)
{
	unsigned long h = 0, g;
	while (*name)
	{
		h = (h << 4) + *name++;
		if (g = h & 0xf0000000)
			h ^= g >> 24;
		h &= ~g;
	}
	return h;
}

struct export_table_entry
{
	be32 name_offset;
	be32 section_offset;
	be32 section_number;
	be32 elf_hash;
};

struct import_table_entry
{
	be32 name_offset;
	be32 p_symbol; // address of symbol, resolved during load
	be32 unknown; // offset into  off_38 table
};

struct section_table_entry
{
	be32 file_offset;
	be32 length;
};

struct relocation_entry
{
	be32 r_offset;
	be32 r_info; // bits 31-8 are the index of associated import index
	               // if bits 7-0 == 0x6d then:
				   //     r28 =  (previous member%4) + 1;
				   //         
	be32 r_addend;
};

#define ELF32_R_SYM(i) ((i)>>8)
#define ELF32_R_TYPE(i) ((unsigned char)(i))

struct rso_header
{
	be32 p_next; // 0 pointer to next module in module linked list
	be32 p_prev; // 0 pointer to previous module in module linked list
	be32 f_section_count; // 0x17
	be32 f_section_table_offset; // 0x58

	be32 elf_path_offset;
	be32 elf_path_length;
	be32 off_00000018; // 1
	be32 off_0000001c; // 0 length of an extra block of mem allocated at load

	be32 off_00000020; // 0
	be32 off_00000024; // 0 replaced with function pointer in ext_ead::www::SurfaceManager::ResolveRsoModule((void))?
	be32 off_00000028; // 0
	be32 off_0000002c; // 0 // value of an unresolved import

	be32 internal_relocation_table_offset; 
	be32 internal_relocation_table_length; 
	be32 external_relocation_table_offset; 
	be32 external_relocation_table_length; 

	be32 export_table_offset; //40 meta start
	be32 export_table_length; //44 meta length [bytes]
	be32 export_name_tab; //48 string start

	be32 import_table_offset; //4c meta start
	be32 import_table_length; //50 meta length [bytes]
	be32 import_name_tab; //54 string start

};



int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("usage: %s sel-file\n", argv[0]);
		return 0;
	}

	struct stat s;
	FILE *f = fopen(argv[1], "rb");

	if(f==NULL)
	{
		perror("fopen");
		return 0;
	}

	if(fstat(fileno(f),&s)!=0)
	{
		perror("fstat");
		fclose(f);
		return 0;
	}

	char *sel_buffer = new char[s.st_size];

	if(sel_buffer == NULL)
	{
		perror("malloc");
		fclose(f);
		return 0;
	}

	if(fread(sel_buffer, 1, s.st_size, f) != s.st_size)
	{
		perror("fread");
		fclose(f);
		free(sel_buffer);
		return 0;
	}
	fclose(f);

	rso_header *rh = reinterpret_cast<rso_header*>(sel_buffer);

	printf("generated from: %.*s\n", sel_buffer + *rh->elf_path_length, sel_buffer + *rh->elf_path_offset);

	if(*rh->f_section_table_offset != 0)
	{
		section_table_entry *ste = reinterpret_cast<section_table_entry*>(sel_buffer + *rh->f_section_table_offset);
		printf("%d sections\n", *rh->f_section_count);
		for(uint32_t i = 0; i < *rh->f_section_count; i++)
		{
			if(*ste[i].length != 0)
			{
				printf("%02d: 0x%08x [%08x]\n", i,
				    *ste[i].file_offset,
					*ste[i].length
				);
			}
		}
	}
	else
	{
		printf("rh->f_section_table_offset=0x%08x\n",
			*rh->f_section_table_offset);
	}

	
	if(*rh->export_table_offset != 0 && *rh->export_table_length != 0)
	{
		export_table_entry *ete = reinterpret_cast<export_table_entry*>(sel_buffer + *rh->export_table_offset);
		printf("exports:\n");
		for(uint32_t i = 0; i < *rh->export_table_length/sizeof(*ete); i++)
		{
			unsigned char *name = reinterpret_cast<unsigned char*>(sel_buffer + *rh->export_name_tab + *ete[i].name_offset);
			printf("%08x:%02x:%08x:%s",
				i,
				*ete[i].section_number,
				*ete[i].section_offset,
				name);
			if(*ete[i].elf_hash != elf_hash(name))
			{
				printf(" HASH MISMATCH");
			}
			printf("\n");
		}
	}
	if(*rh->import_table_offset != 0 && *rh->import_table_length != 0)
	{
		import_table_entry *ite = reinterpret_cast<import_table_entry*>(sel_buffer + *rh->import_table_offset);
		printf("imports:\n");
		for(uint32_t i = 0; i < *rh->import_table_length/sizeof(*ite); i++)
		{
			unsigned char *name = reinterpret_cast<unsigned char*>(sel_buffer + *rh->import_name_tab + *ite[i].name_offset);
			printf("%08x:%08x:%08x:%s",
				i,
				*ite[i].p_symbol,
				*ite[i].unknown,
				name);
			printf("\n");
		}
	}

	if(*rh->internal_relocation_table_offset != 0)
	{
		printf("Internals relocation table:\n");
		relocation_entry *oe = reinterpret_cast<relocation_entry *>(sel_buffer + *rh->internal_relocation_table_offset);
		for(int i=0; i<*rh->internal_relocation_table_length/sizeof(*oe); i++)
		{
			printf("r_offset: 0x%08x r_info: (0x%06x, %02x) r_addend: 0x%08x\n",
				*oe[i].r_offset,
				ELF32_R_SYM(*oe[i].r_info),
				ELF32_R_TYPE(*oe[i].r_info),
				*oe[i].r_addend);
		}
	}

	if(*rh->external_relocation_table_offset != 0)
	{
		printf("Externals relocation table:\n");
		relocation_entry *oe = reinterpret_cast<relocation_entry *>(sel_buffer + *rh->external_relocation_table_offset);
		for(int i=0; i<*rh->external_relocation_table_length/sizeof(*oe); i++)
		{
			printf("r_offset: 0x%08x r_info: (0x%06x, %02x) r_addend: 0x%08x\n",
				*oe[i].r_offset,
				ELF32_R_SYM(*oe[i].r_info),
				ELF32_R_TYPE(*oe[i].r_info),
				*oe[i].r_addend);
		}
	}



	delete[] sel_buffer;

	return 0;
}
