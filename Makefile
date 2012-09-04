TARGET = wm

PREFIX = /usr

SCRS = windowmanager.cpp
OBJS = ${SCRS:.cpp=.o}

CXX = clang++
PKGCONFIG = xcb xcb-aux
CPPFLAGS = `pkg-config --cflags ${PKGCONFIG}`
CXXFLAGS = -Wall -Werror -std=c++11 -g
LDFLAGS  = `pkg-config --libs ${PKGCONFIG}`

all: ${TARGET}

clean:
	rm -rf ${OBJS} ${TARGET}

${TARGET}: ${OBJS}
	${CXX} ${OBJS} ${LDFLAGS} -o ${TARGET}

.cpp.o:
	${CXX} -c ${CXXFLAGS} ${CPPFLAGS} $<
