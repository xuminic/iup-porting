
CC	= gcc
AR	= ar
CP	= cp
RM	= rm -f

IUPDIR	= ../iup

INCS	= -I. -I$(IUPDIR)/include `pkg-config gtk+-2.0 --cflags`
LIBS	= -L$(IUPDIR)/lib -liup `pkg-config gtk+-2.0 --libs`
CFLAGS	= -Wall -O3

CHAN	= 0
IMAGE	= RGB_24bits_palette_sample_image.jpg

RELDIR	= iupimage-csource-1.0
RELSRCS	= iupimage-csource.c stb_image.h octree_quant.h neuquant32.c neuquant32.h\
	  iupimage-test.c iupimage-icon.h RGB_24bits_palette_sample_image.jpg\
	  Makefile

#.PHONY: iupimage-csource iupimage-test
.PHONY: iupimage-test.h
all: iupimage-csource

iupimage-csource: iupimage-csource.c neuquant32.c stb_image.h octree_quant.h
	$(CC) $(CFLAGS) -o $@ iupimage-csource.c neuquant32.c -lm

test: iupimage-test
	@echo
	@echo Note that the channel setting and the image file can be overrided by, for example, 
	@echo $$ make CHAN=3 IMAGE=mypicture.jpg
	@echo
	@iupimage-test

iupimage-test: iupimage-test.c iupimage-test.h
	$(CC) $(CFLAGS) $(INCS) -o $@ $< $(LIBS) -lX11

iupimage-test.h: iupimage-csource
	iupimage-csource -n iupimage_test -c $(CHAN) $(IMAGE) > $@

clean:
	$(RM) iupimage-csource iupimage-test iupimage-test.h

release:
	-if [ -d $(RELDIR) ]; then $(RM) -r $(RELDIR); fi
	-mkdir $(RELDIR)
	-$(CP) $(RELSRCS) $(RELDIR)
	-mkdir $(RELDIR)/include
	-mkdir $(RELDIR)/lib
	-tar czf $(RELDIR).tar.gz $(RELDIR)
	-$(RM) -r $(RELDIR)


