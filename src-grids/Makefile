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
				IdentifyingCodeConfiguration.o	\
				LinearConstraint.o 		\
				RuleShape.o 						\
				SquareGrid.o   	\
				TriangularGrid.o

CC 	= gcc
CXX = g++

OPT = -O3 
DEBUG =  
# -stdlib=libstdc++
WARNINGS = -Wall

CFLAGS 		= $(OPT) $(DEBUG) $(WARNINGS) $(CPLEX_INCLUDE) $(DEFINES)
CXXFLAGS 	= $(OPT) $(DEBUG) $(WARNINGS) $(CPLEX_INCLUDE) $(DEFINES) 
LFLAGS 		= $(OPT) $(DEBUG) $(WARNINGS) $(CPLEX_FLAGS) $(DEFINES) 

TREESEARCHDIR = ../../TreeSearch
UTILSDIR = ../../Utilities/src

INCLUDES = -I.   \
			-I$(TREESEARCHDIR) 	\
			-I$(UTILSDIR)		\

TREESEARCHOBJS	= $(TREESEARCHDIR)/SearchManager.o

UTILSOBJS = $(UTILSDIR)/fraction.o  \
	$(UTILSDIR)/Trie.o
				  
LIBOBJS			= $(TREESEARCHOBJS) $(UTILSOBJS)
			
			
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
	cp $@ ../bin/
        
.c.exe: $< $(COBJECTS)
	$(CC) 	$(LFLAGS)	$(DEBUG)		    						\
        	$(INCLUDES)										\
        	$(NAUTYOBJS)  $(COBJECTS) $(LIBS)		\
            $< -o $@
	cp $@ ../bin/
        
clean:
	-@rm $(OBJECTS) $(TARGETS) $(TESTS)
	
cleanexe:
	-@rm $(TARGETS)

cleantest:
	-@rm $(TESTS)

clest:
	-@rm $(TESTS)

clexe:
	-@rm $(TARGETS)
