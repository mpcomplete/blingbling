# Process this file with autoconf to produce a configure script.
AC_INIT(blingbling, 1.1, guy@somewhere.fscked.org)
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADER([config.h])

sys=`uname`

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL

case $sys in
CYGWIN*|cygwin*|MINGW*|mingw*)
    PROGRAM="blingbling.exe"
    CFLAGS="${CFLAGS} -DWIN32"
    LIBS="${LFLAGS} -lz"
    OPT_OBJS="resource.o"
    BINDIR='win32'
    DATADIR='win32'
    ;;
*)
    PROGRAM="blingbling"
    BINDIR="\$(prefix)/bin/"
    DATADIR="\$(prefix)/share/blingbling/"
    ;;
esac

# Checks for required libs
AC_CHECK_PROG([SDL_CONF], [sdl-config], [yes])
if test -z "$SDL_CONF"; then
    AC_MSG_ERROR([cannot find SDL library])
fi

AC_CHECK_PROG([PARAGUI_CONF], [paragui-config], [yes])
if test -z "$PARAGUI_CONF"; then
    AC_MSG_ERROR([cannot find ParaGUI library])
fi

SDL_LIBS="\`paragui-config --libs\`"
SDL_CFLAGS="\`paragui-config --cflags\`"

AC_CHECK_LIB(SDL_mixer, Mix_FreeChunk, \
    SDL_LIBS="${SDL_LIBS} -lSDL_mixer", \
    AC_MSG_ERROR([cannot find SDL_mixer library]))

AC_CHECK_LIB(SDL_image, IMG_Load, \
    SDL_LIBS="${SDL_LIBS} -lSDL_image", \
    AC_MSG_ERROR([cannot find SDL_image library]))

AC_SUBST(BINDIR)
AC_SUBST(DATADIR)
AC_SUBST(PROGRAM)

AC_SUBST(OPT_OBJS)
AC_SUBST(SDL_LIBS)
AC_SUBST(SDL_CFLAGS)

AC_OUTPUT(Make.include)
