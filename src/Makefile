include ../Make.include

OBJECTS = app.o block.o creditscreen.o field.o gamesingle.o helpscreen.o \
main.o menuscreen.o newgamescreen.o optionscreen.o parser.o prefs.o \
scorelist.o scorescreen.o soundset.o ticker.o tile.o spriteset.o utils.o \
wfield.o widgets.o wscreen.o $(OPT_OBJS)

all:	$(PROGRAM)

test:	$(PROGRAM)
	./$(PROGRAM)

$(PROGRAM):	$(OBJECTS)
	$(CXX) -o $(PROGRAM) $(OBJECTS) $(LIBS)

$(OBJECTS):

.SUFFIXES:
.SUFFIXES: .cc .rc .o

.cc.o:
	$(CXX) -c $(CFLAGS) -o $@ $<

.rc.o:
	windres -o $@ $<

clean: 
	rm -f *.o $(PROGRAM)

distclean: clean
	rm -rf CVS

dep:
	makedepend *.cc
# DO NOT DELETE

app.o: app.h main.h ../config.h /usr/include/assert.h /usr/include/features.h
app.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h spriteset.h tile.h
app.o: ticker.h parser.h utils.h soundset.h scorelist.h gamesingle.h
app.o: wscreen.h wfield.h block.h prefs.h field.h widgets.h menuscreen.h
app.o: helpscreen.h /usr/include/sys/time.h /usr/include/bits/types.h
app.o: /usr/include/bits/wordsize.h
app.o: /usr/lib/gcc-lib/i486-linux/3.3.2/include/stddef.h
app.o: /usr/include/bits/typesizes.h /usr/include/time.h
app.o: /usr/include/bits/time.h /usr/include/sys/select.h
app.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
app.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
app.o: /usr/include/bits/confname.h
block.o: block.h tile.h main.h ../config.h /usr/include/assert.h
block.o: /usr/include/features.h /usr/include/sys/cdefs.h
block.o: /usr/include/gnu/stubs.h ticker.h prefs.h parser.h utils.h app.h
block.o: spriteset.h soundset.h scorelist.h gamesingle.h wscreen.h wfield.h
block.o: field.h widgets.h /usr/include/math.h /usr/include/bits/huge_val.h
block.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
creditscreen.o: creditscreen.h main.h ../config.h /usr/include/assert.h
creditscreen.o: /usr/include/features.h /usr/include/sys/cdefs.h
creditscreen.o: /usr/include/gnu/stubs.h wscreen.h menuscreen.h app.h
creditscreen.o: spriteset.h tile.h ticker.h parser.h utils.h soundset.h
creditscreen.o: scorelist.h widgets.h block.h prefs.h field.h
field.o: field.h main.h ../config.h /usr/include/assert.h
field.o: /usr/include/features.h /usr/include/sys/cdefs.h
field.o: /usr/include/gnu/stubs.h block.h tile.h ticker.h prefs.h parser.h
field.o: utils.h app.h spriteset.h soundset.h scorelist.h gamesingle.h
field.o: wscreen.h wfield.h widgets.h
gamesingle.o: gamesingle.h main.h ../config.h /usr/include/assert.h
gamesingle.o: /usr/include/features.h /usr/include/sys/cdefs.h
gamesingle.o: /usr/include/gnu/stubs.h wscreen.h wfield.h block.h tile.h
gamesingle.o: ticker.h prefs.h parser.h utils.h field.h widgets.h
gamesingle.o: menuscreen.h app.h spriteset.h soundset.h scorelist.h
helpscreen.o: helpscreen.h main.h ../config.h /usr/include/assert.h
helpscreen.o: /usr/include/features.h /usr/include/sys/cdefs.h
helpscreen.o: /usr/include/gnu/stubs.h wscreen.h spriteset.h tile.h ticker.h
helpscreen.o: parser.h utils.h menuscreen.h app.h soundset.h scorelist.h
helpscreen.o: widgets.h block.h prefs.h field.h
main.o: app.h main.h ../config.h /usr/include/assert.h
main.o: /usr/include/features.h /usr/include/sys/cdefs.h
main.o: /usr/include/gnu/stubs.h spriteset.h tile.h ticker.h parser.h utils.h
main.o: soundset.h scorelist.h
menuscreen.o: menuscreen.h main.h ../config.h /usr/include/assert.h
menuscreen.o: /usr/include/features.h /usr/include/sys/cdefs.h
menuscreen.o: /usr/include/gnu/stubs.h wscreen.h newgamescreen.h widgets.h
menuscreen.o: block.h tile.h ticker.h prefs.h parser.h utils.h field.h
menuscreen.o: optionscreen.h scorescreen.h scorelist.h helpscreen.h
menuscreen.o: spriteset.h creditscreen.h app.h soundset.h
newgamescreen.o: newgamescreen.h main.h ../config.h /usr/include/assert.h
newgamescreen.o: /usr/include/features.h /usr/include/sys/cdefs.h
newgamescreen.o: /usr/include/gnu/stubs.h wscreen.h widgets.h block.h tile.h
newgamescreen.o: ticker.h prefs.h parser.h utils.h field.h menuscreen.h
newgamescreen.o: gamesingle.h wfield.h app.h spriteset.h soundset.h
newgamescreen.o: scorelist.h
optionscreen.o: optionscreen.h main.h ../config.h /usr/include/assert.h
optionscreen.o: /usr/include/features.h /usr/include/sys/cdefs.h
optionscreen.o: /usr/include/gnu/stubs.h wscreen.h widgets.h block.h tile.h
optionscreen.o: ticker.h prefs.h parser.h utils.h field.h menuscreen.h app.h
optionscreen.o: spriteset.h soundset.h scorelist.h
parser.o: parser.h main.h ../config.h /usr/include/assert.h
parser.o: /usr/include/features.h /usr/include/sys/cdefs.h
parser.o: /usr/include/gnu/stubs.h utils.h app.h spriteset.h tile.h ticker.h
parser.o: soundset.h scorelist.h /usr/include/string.h
parser.o: /usr/lib/gcc-lib/i486-linux/3.3.2/include/stddef.h
prefs.o: prefs.h main.h ../config.h /usr/include/assert.h
prefs.o: /usr/include/features.h /usr/include/sys/cdefs.h
prefs.o: /usr/include/gnu/stubs.h parser.h utils.h app.h spriteset.h tile.h
prefs.o: ticker.h soundset.h scorelist.h
scorelist.o: scorelist.h main.h ../config.h /usr/include/assert.h
scorelist.o: /usr/include/features.h /usr/include/sys/cdefs.h
scorelist.o: /usr/include/gnu/stubs.h parser.h utils.h prefs.h
scorescreen.o: scorescreen.h main.h ../config.h /usr/include/assert.h
scorescreen.o: /usr/include/features.h /usr/include/sys/cdefs.h
scorescreen.o: /usr/include/gnu/stubs.h wscreen.h scorelist.h parser.h
scorescreen.o: utils.h menuscreen.h app.h spriteset.h tile.h ticker.h
scorescreen.o: soundset.h widgets.h block.h prefs.h field.h
soundset.o: soundset.h main.h ../config.h /usr/include/assert.h
soundset.o: /usr/include/features.h /usr/include/sys/cdefs.h
soundset.o: /usr/include/gnu/stubs.h parser.h utils.h app.h spriteset.h
soundset.o: tile.h ticker.h scorelist.h prefs.h
spriteset.o: spriteset.h main.h ../config.h /usr/include/assert.h
spriteset.o: /usr/include/features.h /usr/include/sys/cdefs.h
spriteset.o: /usr/include/gnu/stubs.h tile.h ticker.h parser.h utils.h app.h
spriteset.o: soundset.h scorelist.h prefs.h
ticker.o: ticker.h main.h ../config.h /usr/include/assert.h
ticker.o: /usr/include/features.h /usr/include/sys/cdefs.h
ticker.o: /usr/include/gnu/stubs.h
tile.o: tile.h main.h ../config.h /usr/include/assert.h
tile.o: /usr/include/features.h /usr/include/sys/cdefs.h
tile.o: /usr/include/gnu/stubs.h ticker.h utils.h app.h spriteset.h parser.h
tile.o: soundset.h scorelist.h prefs.h gamesingle.h wscreen.h wfield.h
tile.o: block.h field.h widgets.h /usr/include/math.h
tile.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
tile.o: /usr/include/bits/mathcalls.h
utils.o: utils.h main.h ../config.h /usr/include/assert.h
utils.o: /usr/include/features.h /usr/include/sys/cdefs.h
utils.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h
utils.o: /usr/lib/gcc-lib/i486-linux/3.3.2/include/stddef.h
wfield.o: wscreen.h main.h ../config.h /usr/include/assert.h
wfield.o: /usr/include/features.h /usr/include/sys/cdefs.h
wfield.o: /usr/include/gnu/stubs.h app.h spriteset.h tile.h ticker.h parser.h
wfield.o: utils.h soundset.h scorelist.h scorescreen.h gamesingle.h wfield.h
wfield.o: block.h prefs.h field.h widgets.h
widgets.o: widgets.h main.h ../config.h /usr/include/assert.h
widgets.o: /usr/include/features.h /usr/include/sys/cdefs.h
widgets.o: /usr/include/gnu/stubs.h block.h tile.h ticker.h prefs.h parser.h
widgets.o: utils.h field.h app.h spriteset.h soundset.h scorelist.h
widgets.o: scorescreen.h wscreen.h gamesingle.h wfield.h
wscreen.o: wscreen.h main.h ../config.h /usr/include/assert.h
wscreen.o: /usr/include/features.h /usr/include/sys/cdefs.h
wscreen.o: /usr/include/gnu/stubs.h widgets.h block.h tile.h ticker.h prefs.h
wscreen.o: parser.h utils.h field.h app.h spriteset.h soundset.h scorelist.h
