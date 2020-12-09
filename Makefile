include Make.include

DISTFILES = \
COPYING \
CREDITS \
ChangeLog \
INSTALL \
Make.include.in \
Makefile \
NOTES \
TODO \
config.h.in \
configure \
configure.ac \
data/ \
icons/ \
install-sh \
src/ \
win32/

DISTDIR = blingbling-$(VERSION)

all:
	make -C src/

install: all
	mkdir -p $(BINDIR)
	install -m 755 src/$(PROGRAM) $(BINDIR)
	strip $(BINDIR)/$(PROGRAM)
	mkdir -p $(DATADIR)
	install -m 644 data/bling.zip $(DATADIR)
	install -m 644 data/background.mp3 $(DATADIR)

clean:
	make -C src/ clean
	make -C win32/ clean
	make -C data/ clean

distclean:
	make -C src/ distclean
	make -C win32/ distclean
	make -C data/ distclean

dist:
	rm -rf $(DISTDIR)
	mkdir $(DISTDIR)
	cp -a $(DISTFILES) $(DISTDIR)

	# need Make.include temporarily to run make
	cp Make.include $(DISTDIR)
	make -C $(DISTDIR) distclean
	rm -f $(DISTDIR)/Make.include

	tar czvf $(DISTDIR).tar.gz $(DISTDIR)
	rm -rf $(DISTDIR)
