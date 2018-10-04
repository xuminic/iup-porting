
/* This file is a test utility for iupimage-csource.

   This file is based on IUP, a multi-platform toolkit for building 
   graphical user interfaces. Details can be found at:
   http://webserver2.tecgraf.puc-rio.br/iup

   The test image, as well as the icon of iupimage-test, 
   RGB_24bits_palette_sample_image.jpg, is from the Wikimedia Commons:
   https://en.wikipedia.org/wiki/File:RGB_24bits_palette_sample_image.jpg

   This file is under GNU GENERAL PUBLIC LICENSE Version 3.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   This file is programmed by "Andy Xuming" <xuming@users.sourceforge.net>

   Build:
     iupimage-csource -n iupimage_test Embedded_Image > iupimage_test.h
     gcc -Wall -O3 `pkg-config gtk+-2.0 --cflags` -o iupimage-test \
            iupimage-test.c -liup `pkg-config gtk+-2.0 --libs` -lX11
   
   The Makefile has a rule to help the build and test. First you need to
   copy the include files of IUP to 'include' directory and the library
   of IUP, libiup.a, to 'lib' directory. Then in command line type:
     make test
     
   Example:
     If wish to load a image called my_image.png instead of the default
     test picture, you could try
     
     make test IMAGE=my_image.png
     
     The image will be loaded as RGBA. It can be overrided to a RGB by

     make test CHAN=3 IMAGE=my_image.png

     or be quantized to an 8-bit/256 color indexed picture by

     make test CHAN=1_reduce IMAGE=my_image.png

     If you knew the image was a 256 color indexed picture in original,
     it maybe reverse the process to the original palette by

     make test CHAN=1_reverse IMAGE=my_image.png
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "iup.h"
#include "iupimage-test.h"
#include "iupimage-icon.h"

int main(int argc, char **argv)
{
	Ihandle	*image, *icon, *lable, *dwin;
	int	i;

	IupOpen(&argc, &argv);

	switch (iupimage_test_channel) {
	case 4:
		image = IupImageRGBA(iupimage_test_width, 
				iupimage_test_height, iupimage_test);
		break;
	case 3:
		image = IupImageRGB(iupimage_test_width, 
				iupimage_test_height, iupimage_test);
		break;
	case 1:
		image = IupImage(iupimage_test_width, 
				iupimage_test_height, iupimage_test);
		for (i = 0; i < 256; i++) {
			IupSetAttribute(image, iupimage_test_palette[i*2],
					iupimage_test_palette[i*2+1]);
		}
		break;
	default:
		image = NULL;
		break;
	}
	if (image == NULL) {
		IupClose();
		return 1;
	}

	lable = IupLabel(NULL);
	IupSetAttributeHandle(lable, "IMAGE", image);
	IupSetAttribute(lable, "EXPAND", "YES");
	IupSetAttribute(lable, "ALIGNMENT", "ACENTER");

	icon = IupImageRGB(iupimage_icon_width, 
			iupimage_icon_height, iupimage_icon);
	dwin = IupDialog(lable);
	if ((iupimage_test_width < 320) || (iupimage_test_height < 240)) {
		IupSetAttribute(dwin, "RASTERSIZE", "320x240");
	}
	IupSetHandle("DLG_ICON", icon);
	IupSetAttribute(dwin, "TITLE", "iupimage-test");
	IupSetAttribute(dwin, "ICON", "DLG_ICON");
	
	IupShow(dwin);
	IupMainLoop();
	IupClose();
	return 0;
}

