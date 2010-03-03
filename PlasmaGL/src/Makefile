#Makefile for PlasmaGL

CXX = g++
CXXFLAGS += -I../../../Plasma/core `sdl-config --cflags`
CXXFLAGS += -lm -lGL -lGLU -lHSPlasma `sdl-config --libs`
CXXFLAGS += -g -Wall

SOURCES = \
	main.cpp \
	plglCamera.cpp \
	plglCamera2.cpp \
	plglClient.cpp \
	plglGame.cpp \
	plglRenderer.cpp \
	plglSpawnPtMgr.cpp \
	plglUtils.cpp

all: ${SOURCES}
	${CXX} ${CXXFLAGS} ${SOURCES} -o plglClient
