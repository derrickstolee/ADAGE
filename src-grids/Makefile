#
# Makefile for ADAGE project, src-grids/ folder.
#
# This has no linear programming portion, as that is compiled
# specific to which LP library is used.
#

TESTS = test_Configuration.exe 	\
			test_HexagonalGrid.exe \
			test_GridTransform.exe \
			test_IdentifyingCodes.exe \
			test_SquareGrid.exe \
			test_TriangularGrid.exe

TARGETS = 	grid-constraints.exe

OBJECTS = Configuration.o 						\
				ConstraintGenerator.o				\
				Grid.o										\
				HexagonalGrid.o						\
				HexagonalGridNoRotate.o						\
				IdentifyingCodeConfiguration.o	\
				LinearConstraint.o 		\
				LinearProgram.o 		\
				RuleShape.o 						\
				SquareGrid.o   	\
				TriangularGrid.o \
				fraction.o \
				SearchManager.o

CC 	= gcc
CXX = g++

INCLUDES = -I./
INCLUDES +=  -I../../Utilities/src

OPT = -O3
DEBUG =
WARNINGS = -Wall

CFLAGS 		= $(OPT) $(DEBUG) $(WARNINGS) $(DEFINES)
CXXFLAGS 	= $(OPT) $(DEBUG) $(WARNINGS) $(DEFINES)
LFLAGS 		= $(OPT) $(DEBUG) $(WARNINGS) $(DEFINES)

.SUFFIXES: .c .cpp .o .obj .exe

all: $(OBJECTS) $(TESTS) $(TARGETS)

# The default object compiler
.c.o: $<
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) -c $< -o $@

.cpp.o: $<
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $< -o $@

.cpp.exe: $< $(OBJECTS)
	$(CXX) $(LFLAGS)						\
        	$(INCLUDES)	$(DEBUG)			\
        	$(LIBOBJS) $(LIBS)					\
        $(OBJECTS)				    	   			\
            $< -o $@

.c.exe: $< $(COBJECTS)
	$(CC) 	$(LFLAGS)	$(DEBUG)		    						\
        	$(INCLUDES)										\
        	$(NAUTYOBJS)  $(COBJECTS) $(LIBS)		\
            $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGETS) $(TESTS)

cleanexe:
	rm -f $(TARGETS)

cleantest:
	rm -f $(TESTS)

clest:
	rm -f $(TESTS)

clexe:
	rm -f $(TARGETS)
