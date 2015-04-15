PRG = param_GT05
OBJDIR  =../../bin/obj
LIBDIR  = ../../bin/lib
BINDIR  =../../bin/
SRCROOT = ../../code
LIBS = exttype d_maxflow maxflow_GT

CC = g++ 
CXX = $(CC)
CFLAGS = $(shell cat ./../cflags)
CFLAGS += -I$(SRCROOT)

LDFLAGS = $(shell cat ./../lflags)

CPPFILES = d_maxflow/param_GT.cpp

RANLIB = ranlib
AR = ar
ARFLAGS = cr

DEPEND = makedepend -fdeps -- $(CFLAGS) $(SRCROOT) -- 

LLIBS = $(patsubst %,-l%,$(LIBS))
PLIBS = $(patsubst %,$(LIBDIR)/lib%.a,$(LIBS))
SRC = $(patsubst %.cpp,$(SRCROOT)/%.cpp,$(CPPFILES))
OBJ = $(patsubst %.cpp,$(OBJDIR)/%.o,$(CPPFILES))


.PHONY: all
all: DIRS $(BINDIR)/$(PRG)

DIRS:

$(BINDIR)/$(PRG): $(OBJ) $(PLIBS)
	@if [ ! -d $(BINDIR) ]; then mkdir -p $(BINDIR); fi
	@echo linking $(BINDIR)/$(PRG)
	@$(CXX) -o $(BINDIR)/$(PRG) $(OBJ) $(LDFLAGS) -L$(LIBDIR) $(LLIBS)
	@echo Done

${OBJDIR}/%.o : $(SRCROOT)/%.cpp ${HEADERS}
	@mkdir -p `dirname $@`
	@echo $*.cpp
	@$(CXX) ${CFLAGS} -o $@ -c $(SRCROOT)/$*.cpp

# generate source dependencies
deps: $(CPPFILES)
	touch deps
	$(DEPEND) $(CPPFILES)                             
	echo -e "\nPlease ignore errors above.\n"

GARBAGE = $(OBJ) core *~ $(BINDIR)/$(PRG)

.PHONY:clean

clean:
	@$(RM) $(OBJ) $(GARBAGE)
	@$(RM) $(BINDIR)/$(PRG)

.PHONY:tags

tags:
	ctags -R    	

#include deps

