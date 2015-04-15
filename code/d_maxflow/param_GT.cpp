#include "d_maxflow/maxflow_solver.h"
#include "maxflow_GT.h"
#include "files/xfs.h"

using namespace debug;
// using namespace dynamic;

debug::PerformanceCounter c1;

#include <sys/time.h>


void solve(const char * file){
  try{
    maxflow_GT * solver;
    dimacs_parser_callback * constructor;

    struct timeval tbeginalgo, tendalgo, tbeginread, tendread;
    double texecalgo(0), texecread(0);

    solver = new maxflow_GT;
    ((maxflow_GT*)solver)->g.globUpdtFreq = 0.5;
    constructor = (maxflow_GT*)solver;
    
    gettimeofday(&tbeginread, NULL);
    dimacs_parser(file,*constructor,2);
    gettimeofday(&tendread, NULL);
    texecread = (double)(1000 * (tendread.tv_sec - tbeginread.tv_sec) + ((tendread.tv_usec - tbeginread.tv_usec)/1000));
    debug::stream << "exec time for building graph = " << texecread << " ms\n";
    
    gettimeofday(&tbeginalgo, NULL);    
    double F = solver->maxflow();
    gettimeofday(&tendalgo, NULL);
    texecalgo = (double)(1000 * (tendalgo.tv_sec - tbeginalgo.tv_sec) + ((tendalgo.tv_usec - tbeginalgo.tv_usec)/1000));
    debug::stream << "exec time for maxflow = " << texecalgo << " ms\n";
    solver->print_info(); // for some reason if you comment this out there's a compilation error

    solver->save_cut();
    
    delete solver;
  } catch(...){
    debug::stream << "ERROR\n";
    exit(1);
  };
}

int main(int argc, char *argv[]){
  if (argc != 2)
    {
      std::cout << "ERROR Wrong number of arguments in main" << std::endl;
    }
  else
    {
      const char * file;
      file = argv[1];
      solve(file);
      
      return 0;
    };
}
