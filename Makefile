CC = gcc
CFLAGS = -Wall -noixemul
INSTALL = /gg/bin/install
PREFIX = /gg

%.o: %.c
	$(CC) $(CFLAGS) -c $<

ilbmviewer: palette.o screen.o filepicker.o viewer.o main.o
	$(CC) $(CFLAGS) -o $@ $? -lilbm -liff -lamivideo

all: ilbmviewer

install: all
	$(INSTALL) -d $(PREFIX)/bin
	$(INSTALL) ilbmviewer $(PREFIX)/bin

clean:
	rm -f *.o
	rm -f ilbmviewer
