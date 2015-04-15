BINDIR  =../../bin/
LIBDIR  =../../bin/lib

CFLAGS = $(shell cat ./../cflags) 

LDFLAGS = $(shell cat ./../lflags) 

targets = lib/libexttype.a lib/libd_maxflow.a lib/libmaxflow_GT param_GT05 #solve_dimacs

all:
	@make -f ../exttype/makefile
	@make -f d_maxflow.makefile
	@make -f maxflow_GT.makefile
	@make -f param_GT05.makefile
	# @make -f solve_dimacs.makefile
	# @g++ $(CFLAGS) ../../code/d_maxflow/cut_cost.cpp -o $(BINDIR)/cut_cost	

clean:
	@make -f ../exttype/makefile clean
	@make -f d_maxflow.makefile clean
	@make -f maxflow_GT.makefile clean
	@make -f param_GT05.makefile clean
	# @make -f solve_dimacs.makefile clean
