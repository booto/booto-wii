/* build: gcc main.c -o tpl2png -std=c99 -lgd -lpng -lz -ljpeg -lfreetype -lm */
#include <gd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <arpa/inet.h>

char *strdup(const char *s);

typedef struct
{
	uint32_t magic;
	uint32_t count;
	uint32_t size;
} tpl_header;

typedef struct
{
	uint32_t th_off;
	uint32_t tp_off;
} texture_offsets;

typedef struct
{
	uint16_t height;
	uint16_t width;
	uint32_t format;
	uint32_t data_off;
	uint32_t wrap_s;
	uint32_t wrap_t;
	uint32_t min_filter;
	uint32_t mag_filter;
	float lod_bias;
	uint8_t edge_lod;
	uint8_t min_lod;
	uint8_t max_lod;
	uint8_t unpacked;
} __attribute__((packed)) texture_header;


#define RGB5_FLAG 0x8000
typedef struct
{
	union
	{
		uint16_t val;
		struct
		{
			unsigned r :5;
			unsigned g :5;
			unsigned b :5;
		} __attribute__((packed)) RGB5;
		struct
		{
			unsigned r :4;
			unsigned g :4;
			unsigned b :4;
			unsigned a :3;
		} __attribute__((packed)) RGB4A3;
	} pixels[16];
} rgb5a3_tile;
int convertRGB5A3(texture_header *th, FILE *in, char *out_fn)
{
	gdImagePtr im = gdImageCreateTrueColor(th->width, th->height);

	if(th == NULL) return -1;

	gdImageAlphaBlending(im, 0);
	gdImageSaveAlpha(im, 1);

	FILE *out = fopen(out_fn, "wb");
	if(out == NULL)
	{
		gdImageDestroy(im);
		return -2;
	}

	fseek(in, th->data_off, SEEK_SET);

	rgb5a3_tile rt;
	for(size_t y=0; y<th->height; y+=4)
	{
		for(size_t x=0; x<th->width; x+=4)
		{
			fread(&rt,sizeof(rt), 1, in); 
			for(size_t i=0; i<16; i++)
			{
				rt.pixels[i].val = ntohs(rt.pixels[i].val);
				if(rt.pixels[i].val & RGB5_FLAG)
				{
					// RGB5
					int colour = gdImageColorResolve(im,
						rt.pixels[i].RGB5.b*255/31,
						rt.pixels[i].RGB5.g*255/31,
						rt.pixels[i].RGB5.r*255/31);
					gdImageSetPixel(im, x+i%4, y+i/4, colour); 
					
				}
				else
				{
					// RGB4A3
					int colour = gdImageColorResolveAlpha(im,
						rt.pixels[i].RGB4A3.b*255/15,
						rt.pixels[i].RGB4A3.g*255/15,
						rt.pixels[i].RGB4A3.r*255/15,
						255-(rt.pixels[i].RGB4A3.a*255/7));
					gdImageSetPixel(im, x+i%4, y+i/4, colour); 
				}
			}
		}
	}

	gdImagePng(im, out);

	fclose(out);

	gdImageDestroy(im);
}

const char *tpl_format(int i)
{
	const char *rv="unknown";
	switch(i)
	{
		case 0: rv = "I4"; break;
		case 1: rv = "I8"; break;
		case 2: rv = "IA4"; break;
		case 3: rv = "IA8"; break;
		case 4: rv = "RGB565"; break;
		case 5: rv = "RGB5A3"; break;
		case 6: rv = "RGBA8"; break;
		case 8: rv = "CI4"; break;
		case 9: rv = "CI8"; break;
		case 10: rv = "CI14X2"; break;
		case 14: rv = "CMP"; break;
	}
	return rv;
}

int png2tpl(const char *in_fn, const char *out_fn)
{
	return -31415;	
}

int tpl2png(const char *in_fn, const char *out_fn)
{
	FILE *in = fopen(in_fn, "rb");
	if(in == NULL)
	{
		return -2;
	}

	tpl_header tpl_h;

	fread(&tpl_h, sizeof(tpl_h), 1, in);

	if(tpl_h.magic != 0x30af2000)
	{
		return -3;
	}

	tpl_h.count = ntohl(tpl_h.count);
	tpl_h.size = ntohl(tpl_h.size);

	printf("magic: 0x%08x\n", tpl_h.magic);
	printf("count: 0x%08x\n", tpl_h.count);
	printf("size:  0x%08x\n", tpl_h.size);

	texture_offsets *tex_off = malloc(sizeof(*tex_off)*tpl_h.size);
	texture_header *tex_h = malloc(sizeof(*tex_h)*tpl_h.size);

	fread(tex_off, sizeof(*tex_off), tpl_h.size, in);

	for(size_t i=0; i<tpl_h.count; i++)
	{
		char out_fn_buffer[512];

		sprintf(out_fn_buffer, "%s%02d.png", out_fn, i);
		tex_off[i].th_off = ntohl(tex_off[i].th_off);
		tex_off[i].tp_off = ntohl(tex_off[i].tp_off);

		printf("%d: [0x%08x, 0x%08x]\n", i, tex_off[i].th_off, tex_off[i].tp_off);

		if(tex_off[i].th_off)
		{
			fseek(in, tex_off[i].th_off, SEEK_SET);
			fread(&tex_h[i], sizeof(*tex_h), 1, in);

			tex_h[i].height = ntohs(tex_h[i].height);
			tex_h[i].width = ntohs(tex_h[i].width);
			tex_h[i].format = ntohl(tex_h[i].format);
			tex_h[i].data_off = ntohl(tex_h[i].data_off);
			tex_h[i].wrap_s = ntohl(tex_h[i].wrap_s);
			tex_h[i].wrap_t = ntohl(tex_h[i].wrap_t);
			tex_h[i].min_filter = ntohl(tex_h[i].min_filter);
			tex_h[i].mag_filter = ntohl(tex_h[i].mag_filter);
			*((uint32_t*)&tex_h[i].lod_bias) = ntohl(*(uint32_t*)&tex_h[i].lod_bias);


			printf("height:   0x%04hx (%d)\n", tex_h[i].height, tex_h[i].height);
			printf("width:    0x%04hx (%d)\n", tex_h[i].width, tex_h[i].width);
			printf("format:   0x%08x (%s)\n", tex_h[i].format, tpl_format(tex_h[i].format));
			printf("data_off: 0x%08x\n", tex_h[i].data_off);
			printf("wrap_s:   0x%08x\n", tex_h[i].wrap_s);
			printf("wrap_t:   0x%08x\n", tex_h[i].wrap_t);
			printf("min_filt: 0x%08x\n", tex_h[i].min_filter);
			printf("mag_filt: 0x%08x\n", tex_h[i].mag_filter);
			printf("lod_bias: %f\n", (double)tex_h[i].lod_bias);
			printf("edge_lod: 0x%02hhx\n", tex_h[i].edge_lod);
			printf("min_lod:  0x%02hhx\n", tex_h[i].min_lod);
			printf("max_lod:  0x%02hhx\n", tex_h[i].max_lod);
			printf("unpacked: 0x%02hhx\n", tex_h[i].unpacked);
		}

		switch(tex_h[i].format)
		{
			case 5:
				convertRGB5A3(&tex_h[i], in, out_fn_buffer);
				break;
		}
	}


	free(tex_h);
	free(tex_off);

}


int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		printf("usage: %s <input> <output>\n", argv[0]);
		return -1;
	}

	int rv = -1;

	char *caller = strdup(argv[0]);

	if(strcmp(basename(caller), "tpl2png")==0)
	{
		rv = tpl2png(argv[1], argv[2]);
	}
	else if(strcmp(basename(caller), "png2tpl")==0)
	{
		rv = png2tpl(argv[1], argv[2]);
	}
	else
	{
		printf("error: I don't know how to %s\n", argv[0]);
	}

	free(caller);

	return rv;
}
