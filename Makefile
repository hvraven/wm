TARGET = wm

SCRS = main.cpp windowmanager.cpp atom.cpp basic_window.cpp conf.cpp event.cpp init.cpp keybind.cpp xwindow.cpp
OBJS = ${SCRS:.cpp=.o}

PREFIX ?= /usr
CXX = clang++

PKGCONFIG = xcb xcb-aux xcb-keysyms xcb-icccm
CPPFLAGS += `pkg-config --cflags ${PKGCONFIG}`
CXXFLAGS += -Wall -Wextra -std=c++11 -g
LDFLAGS  += `pkg-config --libs ${PKGCONFIG}`

all: ${TARGET}

clean:
	rm -f ${OBJS} ${TARGET}

distclean: clean
	rm -f core vgcore.*

${TARGET}: ${OBJS}
	${CXX} ${OBJS} ${LDFLAGS} -o ${TARGET}

.cpp.o:
	${CXX} -c ${CXXFLAGS} ${CPPFLAGS} $<

init: all
	xinit ./wm -- :5

vinit: all
	xinit /usr/bin/valgrind --leak-check=full ./wm -- :5

.PHONY: clean distclean init vinit
