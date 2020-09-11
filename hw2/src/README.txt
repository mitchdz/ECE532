This package includes the following files:
	blur.c
	dip.h
	error.c
	matalloc.c
	matfree.c
	negative.c
	pngClose.c
	pngErrorHandler.c
	pngOpen.c
	pngReadHdr.c
	pngReadRow.c
	pngWarningHandler.c
	pngWriteHdr.c
	pngWriteRow.c
	pngio.h
	padarray.c
	timediff.c

Edit the EXE & LIBDIR definitions in Makefile to match your operating system.
Then build the library and executables by typing "make".

Try running the negative and blur programs:

	./negative cman.png out1.png
	./blur -n 9 cman.png out2.png

Then display the output images using your favorite image viewer.
