NSIS_PATH=/c/Program Files/NSIS

all: blingbling blinglibs

blingbling:
	PATH="$$PATH:$(NSIS_PATH)" makensis.exe blingbling.nsi

blinglibs:
	PATH="$$PATH:$(NSIS_PATH)" makensis.exe blinglibs.nsi

clean:
	rm -f *.exe

distclean: clean
	rm -f *.dll
	rm -rf CVS
