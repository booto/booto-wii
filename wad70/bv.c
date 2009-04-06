#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>

#include <arpa/inet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "tools.h"

static uint8_t key[16];
static uint8_t iv[16];


uint64_t ntohll(uint64_t in)
{
	uint32_t tmp[2];
	tmp[0] = ntohl(in & 0xfffffffful);
	tmp[1] = ntohl(in >> 32);
	
	uint64_t rv = tmp[0];
	rv <<= 32;
	rv |= tmp[1];
	return rv;
}

typedef struct 
{
	uint32_t header_size; // 0x00000070;
	uint32_t magic; // 0x426B0001
	uint32_t wii_id;
	uint32_t files_count;
	uint32_t files_size;
	uint32_t tmd_length; // 0x880
	uint32_t payload_length;
	uint32_t file_length; // 0x80 + tmd_length + payload_length
	uint8_t content_flags[0x40]; // which contents are in here
	uint64_t title_id;
	uint32_t zeros_00000068[2];
} __attribute__((__packed__)) wad70_header_t;



typedef struct
{
	uint32_t sig_type;
	char sig[256];
	uint32_t zeros_00000104[15];
	char issuer[64];
	uint8_t version;
	uint8_t ca_crl_version;
	uint8_t signer_crl_version;
	uint8_t zeros_00000183[1];
	uint64_t ios_version;
	uint64_t title_id;
	uint32_t title_type;
	uint16_t group_id;
	uint8_t reserved[62];
	uint32_t access_rights;
	uint16_t title_version;
	uint16_t num_contents;
	uint16_t boot_index;
	uint16_t fill3;
} __attribute__((__packed__)) tmd_t ;

typedef struct
{
	uint32_t id;
	uint16_t index;
	uint16_t type;
	uint32_t unk;
	uint32_t size;
	uint8_t hash[20];
} __attribute__((__packed__)) content_record_t ;

#define assert_zeros(k) \
for(size_t i = 0; i < sizeof(k)/sizeof(*k); i++)\
{\
	assert(k[i] == 0);\
}


int32_t align(int num, int alignment)
{
	return ((num + alignment - 1) & ~(alignment-1));
}


int main(int argc, char *argv[])
{


	get_key("prng", key, 16);


	if(argc == 1)
	{
		return 1;
	}

	FILE *f = fopen(argv[1], "rb");
	if(f == NULL)
	{
		perror("fopen");
		return 1;
	}
	
	wad70_header_t wh;
	
	fread(&wh, sizeof(wh), 1, f);
	fseek(f, align(ftell(f), 0x40), SEEK_SET);

	printf("wad70 header:\n");
	printf(" header_size:    0x%08x\n", ntohl(wh.header_size));
	assert(ntohl(wh.header_size) == 0x00000070ul);
	printf(" magic:          0x%08x\n", ntohl(wh.magic));
	assert(ntohl(wh.magic) == 0x426B0001ul);
	printf(" wii_id:         0x%08x\n", ntohl(wh.wii_id));
	printf(" files_count:    0x%08x\n", ntohl(wh.files_count));
	printf(" files_size:     0x%08x\n", ntohl(wh.files_size));
	printf(" tmd_length:     0x%08x\n", ntohl(wh.tmd_length));
	printf(" payload_length: 0x%08x\n", ntohl(wh.payload_length));
	printf(" file_length:    0x%08x\n", ntohl(wh.file_length));
	printf(" contained content indexes:\n");

	short contained_content_id = -1;
	for(size_t i=0; i<512; i++)
	{
		if(wh.content_flags[i/8] & (1 << (i%8)))
		{
			printf("\t%04x\n", i);
			if(contained_content_id != -1)
			{
				printf("bailing due to more than one contained content?\n");
				return 0;
			}
			contained_content_id = i;
		}
		
	}
	printf(" title_id:        0x%016llx\n", ntohll(wh.title_id));
	assert_zeros(wh.zeros_00000068);
	
	tmd_t tmd;
	fread(&tmd, sizeof(tmd), 1, f);

	printf("tmd:\n");
	printf(" sig_type:       0x%08x\n", ntohl(tmd.sig_type));
	assert_zeros(tmd.zeros_00000104);
	printf(" issuer:         %-.64s\n", tmd.issuer);
	printf(" version:        0x%02hhx\n", tmd.version);
	printf(" ca_crl_version: 0x%02hhx\n", tmd.ca_crl_version);
	printf(" signer_crl_ver: 0x%02hhx\n", tmd.signer_crl_version);
	printf(" sys_version:    0x%016llx\n", ntohll(tmd.ios_version));
	printf(" title_id:       0x%016llx\n",ntohll(tmd.title_id)); 
	printf(" title_type:     0x%08x\n", ntohl(tmd.title_type));
	printf(" title_version:  0x%04hx\n", ntohs(tmd.title_version));
	printf(" num_contents:   0x%04hx\n", ntohs(tmd.num_contents));
	printf(" boot_index:     0x%04hx\n", ntohs(tmd.boot_index));
	
	content_record_t *content_records = malloc(sizeof(*content_records)*ntohs(tmd.num_contents));
	fread(content_records, sizeof(*content_records), ntohs(tmd.num_contents), f);
	
	printf("contents: 0x%08x * 0x%08x = 0x%08x\n", sizeof(*content_records), ntohs(tmd.num_contents), sizeof(*content_records) * ntohs(tmd.num_contents));	
	printf("id       ind  type size             hash\n");
	for(size_t i = 0; i < ntohs(tmd.num_contents); i++)
	{
		printf("%08x %04hx %04hx %08x ", ntohl(content_records[i].id), ntohs(content_records[i].index), ntohs(content_records[i].type), ntohl(content_records[i].size)); 
		for(size_t j = 0; j<20; j++)
		{
			printf("%02x", content_records[i].hash[j]);
		}
		printf("\n");
	}


	if(contained_content_id == -1)
	{
		printf("bailing due to wad70 header...\n");
		return 0;
	}

	printf("read up to 0x%08x\n", ftell(f));
	fseek(f, align(ftell(f), 0x40), SEEK_SET);
	printf("adjust to 0x%08x\n", ftell(f));

	uint8_t *data = malloc(ntohl(wh.payload_length));
	uint8_t *out_data = malloc(ntohl(wh.payload_length));

	printf("reading a payload of 0x%08x bytes\n", ntohl(wh.payload_length));

	size_t byte_count;
	byte_count = fread(data, 1, ntohl(wh.payload_length), f);
	printf("read 0x%08x bytes\n", byte_count);


	short be_contained_content_id = htons(contained_content_id);
	memset(iv, 0, 16);
	memcpy(iv, &be_contained_content_id, 2);

	aes_cbc_dec(key, iv, data, ntohl(wh.payload_length), out_data);

	char *fn_buffer;
	asprintf(&fn_buffer, "%s.%08x.app", argv[1], ntohl(content_records[contained_content_id].id));
	FILE *out = fopen(fn_buffer, "wb");


	if(out)
	{
		printf("writing 0x%08x of 0x%08x bytes out to %s\n", ntohl(content_records[contained_content_id].size), ntohl(wh.payload_length), fn_buffer);
		byte_count = fwrite(out_data, 1, ntohl(content_records[contained_content_id].size), out);
		printf("wrote 0x%08x (%d) bytes\n", byte_count, byte_count);
		fclose(out);
	}

	free(fn_buffer);
	free(out_data);
	free(data);
	
	free(content_records);
	
	fclose(f);
	return 0;
}
