#
# Makefile for ADAGE project, src/ folder.
#
# This folder contains executables that are generic to all problems.
#
# This uses GLPK for the linear programming portion.
#

OBJECTS = LinearProgram.o		\
	  constraintset.o


CC 	= g++
CXX = g++

OPT = -O0 -g
# -O7
DEBUG =
# -g
# -static
WARNINGS = -Wall
DEFINES = -D GLPK

CFLAGS 		= $(OPT) $(DEBUG) $(WARNINGS) $(DEFINES)
CXXFLAGS 	= $(OPT) $(DEBUG) $(WARNINGS) $(DEFINES)
LFLAGS 		= $(OPT) $(DEBUG) $(WARNINGS) $(DEFINES) -lm -lglpk

TREESEARCHDIR = ../../TreeSearch
UTILSDIR = ../../Utilities/src
GLPKDIR = ../../glpk/src
NAUTYDIR = ../../nauty


INCLUDES =  -I$(TREESEARCHDIR) 	\
			-I$(NAUTYDIR)		\
			-I$(GLPKDIR)		\
			-I$(UTILSDIR)		\

NAUTYOBJS     	= $(NAUTYDIR)/nauty.o 			\
				  $(NAUTYDIR)/nausparse.o		\
				  $(NAUTYDIR)/gtools.o			\
				  $(NAUTYDIR)/nautil.o			\
				  $(NAUTYDIR)/naugraph.o

TREESEARCHOBJS	= $(TREESEARCHDIR)/SearchManager.o

#UTILSOBJS	= 	$(UTILSDIR)/Set.o				\
				$(UTILSDIR)/translation.o				\
				$(UTILSDIR)/TreeSet.o



LIBOBJS			= $(TREESEARCHOBJS) $(CLIQUEROBJS) $(UTILSOBJS) $(NAUTYOBJS)


.SUFFIXES: .c .cpp .o .obj .exe

all: $(OBJECTS) $(TESTS) $(TARGETS)


# The default object compiler
.c.o: $<
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) -c $< -o $@

.cpp.o: $<
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $< -o $@

.cpp.exe: $< $(OBJECTS)
	$(CXX) $(LFLAGS)			\
        	$(INCLUDES)	$(DEBUG)			\
        	$(LIBOBJS) $(LIBS)				\
        	`cat $@.objs`           		\
            $< -o $@

.c.exe: $< $(COBJECTS)
	$(CC) 	$(LFLAGS)			    \
        	$(INCLUDES)				\
        	$(NAUTYOBJS)  $(COBJECTS) $(LIBS)		\
            $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGETS) $(TESTS)

cleanexe:
	rm -f $(TARGETS)

cleantest:
	rm -f $(TESTS)

clexe:
	rm -f $(TARGETS)
