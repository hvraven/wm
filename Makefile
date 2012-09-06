TARGET = wm

SCRS = main.cpp windowmanager.cpp init.cpp keybind.cpp window.cpp
OBJS = ${SCRS:.cpp=.o}

PREFIX ?= /usr
CXX = clang++
PKGCONFIG = xcb xcb-aux xcb-keysyms
CPPFLAGS += `pkg-config --cflags ${PKGCONFIG}`
CXXFLAGS += -Wall -Wextra -std=c++11 -g
LDFLAGS  += `pkg-config --libs ${PKGCONFIG}`

all: ${TARGET}

clean:
	rm -rf ${OBJS} ${TARGET}

${TARGET}: ${OBJS}
	${CXX} ${OBJS} ${LDFLAGS} -o ${TARGET}

.cpp.o:
	${CXX} -c ${CXXFLAGS} ${CPPFLAGS} $<
