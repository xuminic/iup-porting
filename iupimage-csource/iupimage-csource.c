
/* This file is a small utility to convert PNG/JPG/GIF/BMP/... image files 
   to C code, which is useful for compiling images directly into programs, 
   especially for IupImage pixel structure.

   This file is based on STB single-file public domain libraries for C/C++.
   see <https://github.com/nothings/stb> to find the latest stb_image.h.

   IUP is a multi-platform toolkit for building graphical user interfaces. 
   It offers APIs in three basic languages: C, Lua and LED. See
   <http://webserver2.tecgraf.puc-rio.br/iup/> for details

   The octree source code of color quantization is from the rosettacode.org.
   According to its webpage it's under GNU Free Documentation License 1.2:
   https://rosettacode.org/wiki/Color_quantization/C

   NEUQUANT Neural-Net quantization algorithm by Anthony Dekker, 1994.
   See also http://members.ozemail.com.au/~dekker/NEUQUANT.HTML
   NEUQUANT32 was modified to process 32bit RGBA images by Stuart Coyle.
   See also https://github.com/stuart/pngnq

   This file is under GNU GENERAL PUBLIC LICENSE Version 3.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   This file is programmed by "Andy Xuming" <xuming@users.sourceforge.net>

   Compile:
     gcc -Wall -O3 -o iupimage-csource iupimage-csource.c -lm

   Example:
     If wish to load a image called my_dialog_icon.png which has a size of
     128x128 and RGBA pixel structure, in the command line do

     iupimage-csource --name my_picture my_dialog_icon.png > icon.h

     then in your IUP based program

     #include "icon.h"
     Ihandle *image = IupImageRGBA(128, 128, my_picture);

     The image can be loaded as an indexed picture such as 256 color picture.
     You need to load the palette in your IUP program. For example too:

     iupimage-csource --chan 1 --name my_picture my_dialog_icon.png > icon.h

     then in your IUP based program

     #include "icon.h"
     Ihandle *image = IupImage(my_picture_width, \
                        my_picture_height, my_picture);
     for (i = 0; i < 256; i++) {
         IupSetAttribute(image, my_picture_palette[i*2],
                                my_picture_palette[i*2+1]);
     }

     If an image was originally of 256-color, such as a GIF file. Apparently
     even it has been convert to RGBA, it's actually still 256 colors in its
     RGBA space. It is probably not worth of quantization again. You can use

     iupimage-csource --chan 2 --name my_picture my_dialog_icon.png > icon.h

     to revert the process to get a indexed picture.
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "octree_quant.h"
#include "neuquant32.h"

#define	LINE_PREFIX	"        "
#define	LINE_LIMIT	80
#define PALETTE_SIZE	256

typedef	struct	{
	int	width, height, chan;
} pix_t;

typedef	int (*F_Quant)(uint8_t *, uint8_t *, pix_t *);

static	const	char	*prefix1 = "\
/* IupImage/IupImageRGB/IupImageRGBA C-Source image dump */\n\
/* %s: Width=%d Height=%d Pixel-Channel=%d */\n";

static	const	char	*prefix2 = "\
#ifdef __SUNPRO_C\n\
#pragma align 4 (%s)\n\
#endif\n\
#ifdef __GNUC__\n\
const unsigned char	%s[] __attribute__ ((__aligned__ (4))) =\n\
#else\n\
const unsigned char	%s[] =\n\
#endif\n\
{\n";

static	const	char	*prefix3 = "\
#ifdef __SUNPRO_C\n\
#pragma align 4 (%s_palette)\n\
#endif\n\
#ifdef __GNUC__\n\
const char *%s_palette[] __attribute__ ((__aligned__ (4))) =\n\
#else\n\
const char *%s_palette[] =\n\
#endif\n\
{\n";

static	int	climit_in_line = LINE_LIMIT;
static	char	cindent_in_line[64];
static	int	content_in_line = 0;	/* default by climit_in_line */
static	int	palette_in_line = 0;	/* default by climit_in_line */

static	char	*usage = "\
Usage: iupimage-csource [OPTIONS] image_file\n\
OPTIONS:\n\
  -n, --name IDENTIFIER specify the identifier of the array in C style\n\
      --line-limit N    specify the maximum display length in a line\n\
      --line-content N  specify the number of data dumpped in a line\n\
      --line-palette N  specify the number of palette pair dumpped in a line\n\
      --cindent N       specify the space of the line indent (<63)\n\
  -c, --chan CHANNEL    override the default color channels by this option.\n\
\n\
The CHANNEL parameter can be '0', '1_reduce', '1_revert', '3' and '4'.\n\
'0' means automatic; '3' means RGB 3 bytes a pixel and '4' means RGBA 4 bytes.\n\
'1_reduce', or '1', means to reduce the RGBA to the indexed color (256 color).\n\
'1_revert', or '2', means to revert the RGBA to indexed color if the source \n\
  image was indexed.\n";

#define RGB_SET(d,s)	((d)[0]=(s)[0],(d)[1]=(s)[1],(d)[2]=(s)[2],(d)[3]=0)
#define RGB_CMP(d,s)	(((s)[0]==(d)[0])&&((s)[1]==(d)[1])&&((s)[2]==(d)[2]))
#define LF_RDY(i,n)	(((i) % (n)) == (n) - 1)

#define NEXT_PARAM	\
	if (++argv, --argc == 0) {\
		fprintf(stderr, "Parameter is required!\n");\
		return 1;\
	}

static void *load_image_quantization(char *fname, pix_t *info, F_Quant);
static int quant_reverse(uint8_t *input, uint8_t *output, pix_t *info);
static int quant_octree(uint8_t *input, uint8_t *output, pix_t *info);
static int quant_neuquant(uint8_t *input, uint8_t *output, pix_t *info);
static void dump_image_data(char *arname, uint8_t *data, pix_t *info);
static void dump_image_palette(char *arname, uint8_t *palette);

int main(int argc, char **argv)
{
	uint8_t	*data = NULL;
	pix_t	info;
	char	*mcname, *arname = NULL;
	int	i, rchan = 0;

	strcpy(cindent_in_line, LINE_PREFIX);
	while (--argc && **++argv == '-') {
		if (!strcmp(*argv, "-h") || !strcmp(*argv, "--help")) {
			puts(usage);
			return 0;
		} else if (!strcmp(*argv, "-n") || !strcmp(*argv, "--name")) {
			NEXT_PARAM;
			arname = strdup(*argv);
		} else if (!strcmp(*argv, "--line-limit")) {
			NEXT_PARAM;
			climit_in_line = (int)strtol(*argv, NULL, 0);
		} else if (!strcmp(*argv, "--line-content")) {
			NEXT_PARAM;
			content_in_line = (int)strtol(*argv, NULL, 0);
		} else if (!strcmp(*argv, "--line-palette")) {
			NEXT_PARAM;
			palette_in_line = (int)strtol(*argv, NULL, 0);
		} else if (!strcmp(*argv, "--cindent")) {
			int	tmp;
			NEXT_PARAM;
			tmp = (int)strtol(*argv, NULL, 0);
			if ((tmp > 1) && (tmp < 64)) {
				for (i = 0; i < tmp; cindent_in_line[i++] = ' ');
				cindent_in_line[i] = 0;
			}
		} else if (!strcmp(*argv, "-c") || !strcmp(*argv, "--chan")) {
			NEXT_PARAM;
			if (!strcmp(*argv, "1_revert")) {
				rchan = 2;
			} else if (!strcmp(*argv, "1_reduce")) {
				rchan = 1;
			} else if (!strcmp(*argv, "1_octree")) {
				rchan = 5;
			} else {
				rchan = (int)strtol(*argv, NULL, 0);
			}
		} else {
			fprintf(stderr, "Unknown option: [%s]\n", *argv);
			free(arname);
			return 1;
		}
	}
	if (climit_in_line <= strlen(cindent_in_line)) {
		climit_in_line = LINE_LIMIT;
		strcpy(cindent_in_line, LINE_PREFIX);
	}

	if (argc < 1) {
		fprintf(stderr, "Image file not found\n");
		free(arname);
		return 2;
	}

	switch (rchan) {
	case 0:
		data = stbi_load(*argv, &info.width, 
				&info.height, &info.chan, 0);
		break;
	case 1:
		data = load_image_quantization(*argv, &info, quant_neuquant);
		break;
	case 2:
		data = load_image_quantization(*argv, &info, quant_reverse);
		break;
	case 3:
	case 4:
		data = stbi_load(*argv, &info.width, &info.height, NULL, rchan);
		info.chan = rchan;
		break;
	case 5:
		data = load_image_quantization(*argv, &info, quant_octree);
		break;
	}
	if (data == NULL) {
		fprintf(stderr, "%s: can not be loaded.\n", *argv);
		free(arname);
		return 3;
	}

	/* format the default identifier from the file name */
	if (arname == NULL) {
		char	*tmp;
		arname = strdup(*argv);
		for (tmp = arname; *tmp; tmp++) {
			if (!isalnum(*tmp)) {
				*tmp = '_';
			}
		}
	}

	/* calculate the column number by the screen width */
	if (content_in_line == 0) {
		content_in_line = climit_in_line+2 - strlen(cindent_in_line);
		if (info.chan > 2) {
			content_in_line = content_in_line / 
				(info.chan * 4 + 2) * info.chan;
		} else {
			content_in_line = (content_in_line / 4) & 0xfe;
		}
	}
	if (palette_in_line == 0) {
		palette_in_line = climit_in_line+4 - strlen(cindent_in_line);
		palette_in_line = palette_in_line / 24;
	}
	/*fprintf(stderr, "Column=%d Palette=%d\n", 
				content_in_line, palette_in_line);*/

	/* output the comments of the image and dimention varibles */
	printf(prefix1, *argv, info.width, info.height, info.chan);

	/* output the begin macro of the header file */
	mcname = strdup(arname);
	for (i = 0; mcname[i]; i++) mcname[i] = toupper(mcname[i]);
	printf("\n#ifndef _%s_H_\n", mcname);
	printf("#define _%s_H_\n\n", mcname);

	/* output the image parameters */
	printf("const	int	%s_width = %d;\n", arname, info.width);
	printf("const	int	%s_height = %d;\n", arname, info.height);
	printf("const	int	%s_channel = %d;\n\n", arname, info.chan);
	
	/* output the image pixel data */
	dump_image_data(arname, data, &info);

	/* output the paletter structure, whether it's existed or not.
	 * if the paletter doesn't exist, ie RGB or RGBA, it still will
	 * output a dummy palette to support the test program */
	if (info.chan == 1) {
		dump_image_palette(arname, data + info.width * info.height);
	} else { 
		printf(prefix3, arname, arname, arname);
		printf("};\n\n");
	}

	/* output the matching end of the macro of the header file */
	printf("#endif	/* _%s_H_ */\n\n", mcname);

	stbi_image_free(data);
	free(mcname);
	free(arname);
	return 0;
}

static void *load_image_quantization(char *fname, pix_t *info, F_Quant quantize)
{
	uint8_t	*data, *outdata;
	int	rc;

	/* load the image as RGBA only */
	data = stbi_load(fname, &info->width, &info->height, NULL, 4);
	if (data == NULL) {
		return NULL;
	}
	info->chan = 4;

	outdata = malloc(info->width * info->height + PALETTE_SIZE * 4);
	if (outdata == NULL) {
		free(data);
		return NULL;
	}

	if ((rc = quantize(data, outdata, info)) != 0) {
		fprintf(stderr, "Warning: palette overflow! [%d]\n", rc);
	}

	/* the channel has been squeezed to 8-bit after quantization */
	info->chan = 1;	
	stbi_image_free(data);
	return outdata;
}

static int quant_reverse(uint8_t *input, uint8_t *output, pix_t *info)
{
	int	i, k, warn = 0;
	uint8_t	*p, *palette;

	palette = output + info->width * info->height;
	memset(palette, 0xff, PALETTE_SIZE * 4);
	
	for (i = 0; i < info->width * info->height; i++,input += info->chan) {
		for (k = 0, p = palette; k < PALETTE_SIZE; k++, p += 4) {
			output[i] = (uint8_t) k;
			if (p[3] == 0xff) {
				RGB_SET(p, input);
				break;
			} else if (RGB_CMP(p, input)) {
				break;
			}
		}
		if (k == PALETTE_SIZE) {
			warn++;
		}
	}
	return warn;
}

/* re-build the palette while indexing the RGB pixels from the octree, or use
 * the palette inside the octree directly. The former is a bit conservative */
//#define REBUILD_PALETTE

static int quant_octree(uint8_t *input, uint8_t *output, pix_t *info)
{
	node_heap	*heap;
	uint8_t	*palette;
	int	i, warn = 0;
#ifdef	REBUILD_PALETTE
	uint8_t	tmp[4], *p;
	int	k;
#endif

	heap = color_quant_octree_alloc(input, 
			info->width * info->height, info->chan);
	if (heap == NULL) {
		return -1;
	}
	//fprintf(stderr, "OCTREE: %d %d\n", heap->alloc, heap->n);
 
	palette = output + info->width * info->height;
	memset(palette, 0xff, PALETTE_SIZE * 4);

#ifdef	REBUILD_PALETTE
	for (i = 0; i < info->width*info->height; i++, input += info->chan) {
		bcopy(input, tmp, 4);
		color_replace(heap->root, tmp);

		for (k = 0, p = palette; k < PALETTE_SIZE; k++, p+=4) {
			output[i] = (uint8_t) k;
			if (p[3] == 0xff) {
				RGB_SET(p, input);
				break;
			} else if (RGB_CMP(p, input)) {
				break;
			}
		}
		if (k == PALETTE_SIZE) {
			warn++;
		}
	}
#else
	for (i = 1; i < PALETTE_SIZE + 1; i++, palette += 4) {
		if (i < heap->n) {
			palette[0] = heap->buf[i]->r;
			palette[1] = heap->buf[i]->g;
			palette[2] = heap->buf[i]->b;
			palette[3] = 0;
		}
	}

	for (i = 0; i < info->width * info->height; i++,input += info->chan) {
		output[i] = color_quant_octree_indexing(heap, input);
	}
#endif	/* ! REBUILD_PALETTE */

	color_quant_octree_free(heap);
	return warn;
}

#if 0	/* it doesn't work well! */
static int quant_dither(uint8_t *input, uint8_t *output, pix_t *info)
{
	node_heap	*heap;
	uint8_t	*palette;
	int	i, warn = 0;

	heap = color_quant_octree_alloc(input, 
			info->width * info->height, info->chan);
	if (heap == NULL) {
		return -1;
	}
	fprintf(stderr, "OCTREE: %d %d\n", heap->alloc, heap->n);
 
	palette = output + info->width * info->height;
	memset(palette, 0xff, PALETTE_SIZE * 4);

	for (i = 1; i < PALETTE_SIZE + 1; i++, palette += 4) {
		if (i < heap->n) {
			palette[0] = heap->buf[i]->r;
			palette[1] = heap->buf[i]->g;
			palette[2] = heap->buf[i]->b;
			palette[3] = 0;
		}
	}
	warn = color_quant_dither(heap, input, output, 
			info->width, info->height, info->chan);

	color_quant_octree_free(heap);
	return warn;
}
#endif

static int quant_neuquant(uint8_t *input, uint8_t *output, pix_t *info)
{
	int	i;

	initnet(input, info->width * info->height * 4, 3, PALETTE_SIZE);
	learn(0);
	unbiasnet();

	getcolormap(output + info->width * info->height);

	inxbuild();
	for (i = 0; i < info->width * info->height; i++, input += 4) {
		output[i] = inxsearch(input[3], input[2], input[1], input[0]);
		//output[i] = inxsearch(input[0], input[1], input[2]);
	}
	return 0;
}

static void dump_image_data(char *arname, uint8_t *data, pix_t *info)
{
	int	x, y, width;

	printf(prefix2, arname, arname, arname);

	/* expand the line width by color channels */
	width = info->width * info->chan;

	for (y = 0; y < info->height; y++) {
		printf("%s/* This is line %d */\n%s", 
				cindent_in_line, y, cindent_in_line);
		for (x = 0; x < width; x++) {
			/* is it the end of the array? */
			if ((y == info->height - 1) && (x == width - 1)) {
				printf("%3d", *data++);
			} else {
				printf("%3d,", *data++);
			}
			if (x == width - 1) {	/* end of the scan line */
				printf("\n");
			} else if ((info->chan > 1) && (info->chan <= 4)) {
				if (LF_RDY(x, content_in_line)) {
					printf("\n%s", cindent_in_line);
				} else if (LF_RDY(x, info->chan)) {
					printf("  ");
				}
			} else if (LF_RDY(x, content_in_line)) {
				printf("\n%s", cindent_in_line);
			}
		}
	}
	printf("};\n\n");
}

static void dump_image_palette(char *arname, uint8_t *palette)
{
	char	tmp[32];
	int	i;

	printf(prefix3, arname, arname, arname);
	printf(cindent_in_line);
	for (i = 0; i < PALETTE_SIZE; i++, palette += 4) {
		sprintf(tmp, "\"%d\",", i);
		printf("%6s", tmp);
		sprintf(tmp, "\"%d %d %d\"", 
				palette[0], palette[1], palette[2]);
		if (i == PALETTE_SIZE - 1) {
			printf("%14s\n", tmp);
			break;
		} else {
			printf("%14s,   ", tmp);
		}
		if (LF_RDY(i, palette_in_line)) {
			printf("\n%s", cindent_in_line);
		}
	}
	printf("};\n\n");
}


