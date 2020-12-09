include ../Make.include

ZIP_CONTENTS = \
bling.theme .blingscores blingbling.conf \
bling/ bling/* \
help/ help/* \
sounds/ sounds/* \
tiles/ tiles/*

all:

zip:
	rm -f bling.zip
	zip bling.zip $(ZIP_CONTENTS)
	zip -d bling.zip '*CVS*'
	
distclean: zip
	rm -rf $(ZIP_CONTENTS)
	rm -rf CVS
