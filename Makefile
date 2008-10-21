PLSLIBDIR=../Plasma/core/lib

CXX = g++
CXXFLAGS = -fPIC -Wall -g -DGLX_GLXEXT_LEGACY -I../Plasma/core `sdl-config --cflags`
CXXLIBS = -L../Plasma/core -lPlasma -lm -lGL -lGLU `sdl-config --libs`

CC = gcc
CCFLAGS = -fPIC -Wall -g -I3rdp
CCLIBS =

VIEWER_CPP =\
	./src/camera.cpp \
	./src/PlasmaViewer.cpp\
	./src/prpengine.cpp\
	./src/viewer_fni.cpp

all: $(PLSLIBDIR)/libPlasma.so \
	bin/plViewer

install:
	(cd ../Plasma/core ; make install)

clean-all:
	rm -f bin/plViewer

clean:
	rm -f bin/plViewer


# Libs
$(PLSLIBDIR)/libPlasma.so:
	(cd ../Plasma/core ; make)

# plViewer
bin/plViewer: $(VIEWER_CPP)
	$(CXX) $(CXXFLAGS) $(VIEWER_CPP) $(CXXLIBS) -o $@
