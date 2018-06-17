SYSTEM     = x86-64_osx
LIBFORMAT  = static_pic

#------------------------------------------------------------
#
# When you adapt this makefile to compile your CPLEX programs
# please copy this makefile and set CPLEXDIR and CONCERTDIR to
# the directories where CPLEX and CONCERT are installed.
#
#------------------------------------------------------------

CPLEXDIR      = /Applications/CPLEX_Studio128/cplex
CONCERTDIR    = /Applications/CPLEX_Studio128/concert

# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

CCC = clang++ -O0

# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

CCOPT = -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD -stdlib=libc++

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR)
CCLNFLAGS = -lconcert -lilocplex -lcplex -m64 -lm -lpthread -framework CoreFoundation -framework IOKit -stdlib=libc++


run: 
	make lp_DCMST

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

EXDIR         = $(CPLEXDIR)/examples
EXDATA        = $(EXDIR)/data
EXSRCCPP      = $(EXDIR)/src/cpp

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 
	 
	 
# ------------------------------------------------------------

clean :
	/bin/rm -rf *.o *~
	/bin/rm -rf lp_DCMST
	/bin/rm -rf *.mps *.ord *.sos *.lp *.sav *.net *.msg *.log *.clp

# ------------------------------------------------------------
#
# The examples
#
lp_DCMST: lp_DCMST.o
	$(CCC) $(CCFLAGS) $(CCLNDIRS) -o lp_DCMST lp_DCMST.o $(CCLNFLAGS)
lp_DCMST.o: ./lp_DCMST.cpp
	$(CCC) -c $(CCFLAGS) ./lp_DCMST.cpp -o lp_DCMST.o



# Local Variables:
# mode: makefile
# End:
