include $(ORACLE_HOME)/precomp/lib/env_precomp.mk

# This Makefile builds the sample programs in 
# $(ORACLE_HOME)/precomp/demo/proc, and can serve as a template for linking
# customer applications.

# SAMPLES is a list of the c proc sample programs.
# CPPSAMPLES is a list of the c++ proc sample programs.
# OBJECT_SAMPLES is a list of the proc sample programs using the new
# type features.  You must install ott in addition to proc to run some
# of those samples.
SAMPLES=sample1 sample2 sample3 sample4 sample6 sample7 sample8 \
	sample9 sample10 sample11 sample12 oraca sqlvcp cv_demo \
	ansidyn1 ansidyn2

# These targets build all of a class of samples in one call to make.
samples: $(SAMPLES)

# The target 'build' puts together an executable $(EXE) from the .o files
# in $(OBJS) and the libraries in $(PROLDLIBS).  It is used to build the
# c sample programs.
# The rules to make .o files from .c and .pc files are later in this file.
# $(PROLDLIBS) uses the client shared library; $(STATICPROLDLIBS) does not.
build: $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(LDPATHFLAG)$(LIBHOME) $(LDPATHFLAG)$(LIBHOME)/stubs/ $(PROLDLIBS)
build_static: $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(LDPATHFLAG)$(LIBHOME) $(LDPATHFLAG)$(LIBHOME)/stubs/ $(STATICPROLDLIBS)
$(SAMPLES) $(OBJECT_SAMPLES):
	$(MAKE) -f $(MAKEFILE) OBJS=$@.o EXE=$@ build

# The c++ samples are built using the 'cppbuild' target.  It precompiles to
# get a .c file, compiles to get a .o file and then builds the executable.
cppbuild: 
	$(PROC) $(PROCPPFLAGS) iname=$(EXE)
	$(CPLUSPLUS) -c $(INCLUDE) $(EXE).c
	$(CPLUSPLUS) -o $(EXE) $(OBJS) $(LDPATHFLAG)$(LIBHOME) $(CPPLDLIBS)
cppbuild_static: 
	$(PROC) $(PROCPPFLAGS) iname=$(EXE)
	$(CPLUSPLUS) -c $(INCLUDE) $(EXE).c
	$(CPLUSPLUS) -o $(EXE) $(OBJS) $(LDPATHFLAG)$(LIBHOME) $(STATICCPPLDLIBS)
$(CPPSAMPLES): cppdemo2
	$(MAKE) -f $(MAKEFILE) OBJS=$@.o EXE=$@ cppbuild


# Here are some rules for converting .pc -> .c -> .o and for .typ -> .h.
#
# If proc needs to find .h files, it should find the same .h files that the 
# c compiler finds.  We use a macro named INCLUDE to hadle that.  The general 
# format of the INCLUDE macro is 
#   INCLUDE= $(I_SYM)dir1 $(I_SYM)dir2 ...
#
# Normally, I_SYM=-I, for the c compiler.  However, we have a special target,
# pc1, which calls $(PROC) with various arguments, include $(INCLUDE).  It
# is used like this:
#	$(MAKE) -f $(MAKEFILE) <more args to make> I_SYM=include= pc1
# This is used for some of $(SAMPLES) and for $(OBJECT_SAMPLE).
.SUFFIXES: .pc .c .o .typ .h

pc1:
	$(PROC) $(PROCFLAGS) iname=$(PCCSRC) $(INCLUDE)

.pc.c:
	$(PROC) $(PROCFLAGS) iname=$*

.pc.o:
	$(PROC) $(PROCFLAGS) iname=$*
	$(CC) $(CFLAGS) -c $*.c

.c.o:
	$(CC) $(CFLAGS) -c $*.c

.typ.h:
	$(OTT) intype=$*.typ hfile=$*.h outtype=$*o.typ $(OTTFLAGS) code=c user=scott/tiger

# These are specific targets to make the .o files for samples that require
# more careful handling.
sample9.o: sample9.pc calldemo-sql
	$(PROC) $(PROCPLSFLAGS) iname=$*
	$(CC) $(CFLAGS) -c $*.c

cv_demo.o: cv_demo.pc cv_demo-sql
	$(PROC) $(PROCPLSFLAGS) iname=$*
	$(CC) $(CFLAGS) -c $*.c

sample11.o: sample11.pc sample11-sql
	$(PROC) $(PROCPLSFLAGS) iname=$*
	$(CC) $(CFLAGS) -c $*.c

sample4.o: sample4.pc
	$(MAKE) -f $(MAKEFILE) PCCSRC=$* I_SYM=include= pc1
	$(CC) $(CFLAGS) -c $*.c

ansidyn1.o: ansidyn1.pc
	$(PROC) mode=ansi iname=$*
	$(CC) $(CFLAGS) -c $*.c

ansidyn2.o: ansidyn2.pc
	$(PROC) dynamic=ansi iname=$*
	$(CC) $(CFLAGS) -c $*.c
	
empclass.o: empclass.pc cppdemo2-sql
	$(MAKE) -f $(MAKEFILE) PROCFLAGS="sqlcheck=full user=scott/tiger $(PROCPPFLAGS)" PCCSRC=$* I_SYM=include= pc1
	$(CPLUSPLUS) -c $(INCLUDE) $*.c

lobdemo1.o: lobdemo1.pc lobdemo1-sql
	$(MAKE) -f $(MAKEFILE) PROCFLAGS="$(PROCPLSFLAGS)" PCCSRC=$* I_SYM=include= pc1
	$(CC) $(CFLAGS) -c $*.c

objdemo1.o: objdemo1.pc objdemo1.typ objdemo1-sql
	$(OTT) intype=$*.typ hfile=$*.h outtype=$*o.typ $(OTTFLAGS) code=c user=scott/tiger
	$(MAKE) -f $(MAKEFILE) PROCFLAGS=intype=$*o.typ PCCSRC=$* I_SYM=include= pc1
	$(CC) $(CFLAGS) -c $*.c

coldemo1.o: coldemo1.pc coldemo1.typ coldemo1-sql
	$(OTT) intype=$*.typ hfile=$*.h outtype=$*o.typ $(OTTFLAGS) code=c user=scott/tiger
	$(MAKE) -f $(MAKEFILE) PROCFLAGS=intype=$*o.typ PCCSRC=$* I_SYM=include= pc1
	$(CC) $(CFLAGS) -c $*.c

navdemo1.o: navdemo1.pc navdemo1.typ navdemo1-sql
	$(OTT) intype=$*.typ hfile=$*.h outtype=$*o.typ $(OTTFLAGS) code=c user=scott/tiger
	$(MAKE) -f $(MAKEFILE) PROCFLAGS=intype=$*o.typ PCCSRC=$* I_SYM=include= pc1
	$(CC) $(CFLAGS) -c $*.c

# These macro definitions fill in some details or override some defaults
OTTFLAGS=$(PCCFLAGS)
PRODUCT_LIBHOME=
MAKEFILE=$(ORACLE_HOME)/precomp/demo/proc/demo_proc.mk
PROCPLSFLAGS= sqlcheck=full userid=$(USERID)
PROCPPFLAGS= code=cpp $(CPLUS_SYS_INCLUDE)
USERID=scott/tiger
NETWORKHOME=$(ORACLE_HOME)/network/
PLSQLHOME=$(ORACLE_HOME)/plsql/
INCLUDE=$(I_SYM). $(I_SYM)$(PRECOMPHOME)public $(I_SYM)$(RDBMSHOME)public $(I_SYM)$(RDBMSHOME)demo $(I_SYM)$(PLSQLHOME)public $(I_SYM)$(NETWORKHOME)public
