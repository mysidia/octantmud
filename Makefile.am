AM_YFLAGS = -d -p yymp
#AM_LFLAGS = -olex.yy.c -Pyymp -d
#AM_LFLAGS = -olex.yy.c -Pyymp -d
AM_LFLAGS = -f -s -olex.yy.c -Pyymp

bin_PROGRAMS = mud
#include_HEADERS = bit.h dataf.h pone.h parse.h md5.h

mud_SOURCES = md5_iface.c md5.c mud.cc Game.cc Config.cc GamePort.cc \
	      GamePoll.cc Descriptor.cc TextQueue.cc GameString.cc \
	      Account.cc Database.cc DataHandle.cc QueryResult.cc \
	      Character.cc Colour.cc Object.cc Location.cc \
	      pack.cc antibot.cc misc.cc interp.cc info.cc

mud_LDADD = -ldbi

cmdlist.h: interp.cc
	perl cmdlist.pl
