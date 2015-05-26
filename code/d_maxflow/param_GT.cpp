#include "d_maxflow/maxflow_solver.h"
#include "maxflow_GT_param.h"
#include "files/xfs.h"

using namespace debug;
// using namespace dynamic;

debug::PerformanceCounter c1;

#include <sys/time.h>

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>


// void solve(const char * file){
//   try{
//     maxflow_GT * solver;
//     dimacs_parser_callback * constructor;

//     struct timeval tbeginalgo, tendalgo, tbeginread, tendread;
//     double texecalgo(0), texecread(0);

//     solver = new maxflow_GT;
//     ((maxflow_GT*)solver)->g.globUpdtFreq = 0.5;
//     constructor = (maxflow_GT*)solver;
    
//     gettimeofday(&tbeginread, NULL);
//     dimacs_parser(file,*constructor,2);
//     gettimeofday(&tendread, NULL);
//     texecread = (double)(1000 * (tendread.tv_sec - tbeginread.tv_sec) + ((tendread.tv_usec - tbeginread.tv_usec)/1000));
//     debug::stream << "exec time for building graph = " << texecread << " ms\n";
    
//     gettimeofday(&tbeginalgo, NULL);    
//     double F = solver->maxflow();
//     gettimeofday(&tendalgo, NULL);
//     texecalgo = (double)(1000 * (tendalgo.tv_sec - tbeginalgo.tv_sec) + ((tendalgo.tv_usec - tbeginalgo.tv_usec)/1000));
//     debug::stream << "exec time for maxflow = " << texecalgo << " ms\n";
//     solver->print_info(); // for some reason if you comment this out there's a compilation error

//     solver->save_cut();
    
//     delete solver;
//   } catch(...){
//     debug::stream << "ERROR\n";
//     exit(1);
//   };
// };


void test_energy_computation(maxflow_GT_param * solver){
  std::vector<int> test_indicator;
  test_indicator.push_back(0);
  test_indicator.push_back(1);
  test_indicator.push_back(3);
  
  // std::cout << "test indicator[0]: " << test_indicator[0] << std::endl;

  // If solver was constructed from 
  // ../test/W.dimacs and ../test/weights.txt
  // then the value of the energy function should be 11.4223
  debug::stream << "Expected value of the energy function: 11.4223\n";

  double energy = solver->energy_function(test_indicator);
  debug::stream << "energy = " << energy << " \n";
}


void test_optimize_h(maxflow_GT_param * solver){
  // If solver was constructed from 
  // ../test/W.dimacs and ../test/weights.txt
  // and W was multiplied by 0.01

  solver->g.multiply(0.01);

  std::vector<int> test_indicator;
  std::vector<float> test_weights;
  test_weights.resize(solver->g.n-2);
  
  // // then the solution here should be [0, 1, 2, 3]
  // // std::cout << "test_weights: ";
  // for (int i=0; i<solver->g.n-2; i++){
  //   test_weights[i] = solver->weights[i] - 9.;
  //   // std::cout << test_weights[i] << " ";
  // }
  // // std::cout << std::endl;

  // solver->optimize_energy(test_indicator, test_weights);

  // std::cout << "test_indicator: ";
  // for (i=0; i<test_indicator.size(); i++){
  //   std::cout << test_indicator[i] << " ";
  // }
  // std::cout << std::endl;

  // // the solution here should be [0, 1, 2, 3, 4, 32]
  // for (int i=0; i<solver->g.n-2; i++){
  //   test_weights[i] = solver->weights[i] - 8.6;
  // }
  // solver->optimize_energy(test_indicator, test_weights) ;
  // std::cout << "test_indicator: ";
  // for (i=0; i<test_indicator.size(); i++){
  //   std::cout << test_indicator[i] << " ";
  // }
  // std::cout << std::endl;
  // std::cout << test_indicator.size() << " nodes selected" << std::endl;


  // the solution here should be 
  //  [0, 1, 2, 3, 4, 10, 12, 13, 20, 21, 22, 24, 29, 32, 33, 40, 42, 46, 48, 49] 
  for (int i=0; i<solver->g.n-2; i++){
    test_weights[i] = solver->weights[i] - 7.6;
  }
  solver->optimize_energy(test_indicator, test_weights) ;
  std::cout << "test_indicator: ";
  for (i=0; i<test_indicator.size(); i++){
    std::cout << test_indicator[i] << " ";
  }
  std::cout << std::endl;
  std::cout << test_indicator.size() << " nodes selected" << std::endl;

  test_weights.clear();
  test_indicator.clear();
}


void test_solve_for_fixed_size(maxflow_GT_param * solver){
    // If solver was constructed from 
    // ../test/W.dimacs and ../test/weights.txt
    // and W was multiplied by 0.05
    solver->g.multiply(0.05);
    
    solutionSz my_solution;
    float bmin = 1.;
    float bmax = 10.;
    int i;
    int kval = 9;
    std::vector<int> my_indicator;
    /* Solution for kval = 11, beta_min = 1, beta_max = 10
       There is no solution of size 11
       [-1,
       7.832765039515003,
       [0, 1, 2, 3, 4, 20, 32, 40, 48, 49],
       [0, 1, 2, 3, 4, 20, 21, 22, 32, 40, 48, 49]]
    */
    solver->solve_for_fixed_size(kval, bmin, bmax, my_solution);

    if (my_solution.true_solution) {
	std::cout << "energy: " << my_solution.brkpt_or_h << std::endl;
	
	my_indicator = my_solution.indicator1;
	std::cout << "indicator: ";
	for (i=0; i<my_indicator.size(); i++){
	    std::cout << my_indicator[i] << " ";
	}
	std::cout << std::endl;
    }
    else { 
	std::cout << "No solution of size " << kval << std::endl;
	std::cout << "breakpoint: " << my_solution.brkpt_or_h << std::endl;
	
	my_indicator = my_solution.indicator1;
	std::cout << "indicator (smaller): ";
	for (i=0; i<my_indicator.size(); i++){
	    std::cout << my_indicator[i] << " ";
	}
	std::cout << std::endl;

	my_indicator = my_solution.indicator2;
	std::cout << "indicator (bigger): ";
	for (i=0; i<my_indicator.size(); i++){
	    std::cout << my_indicator[i] << " ";
	}
	std::cout << std::endl;
    }


    // /* Solution for kval = 9, beta_min = 1, beta_max = 10
    //    [[0, 1, 2, 3, 4, 20, 32, 40, 49], -76.795475540122354]
    // */
    // solver->solve_for_fixed_size(9, bmin, bmax, my_solution);
    // std::cout << "energy: " << my_solution.brkpt_or_h << std::endl;

    // my_indicator = my_solution.indicator1;
    // std::cout << "indicator: ";
    // for (i=0; i<my_indicator.size(); i++){
    // 	std::cout << my_indicator[i] << " ";
    // }
    // std::cout << std::endl;
}


void solve(maxflow_GT_param * solver, float lbd, int kval, float bmin, float bmax){
    // If solver was constructed from 
    // ../test/W.dimacs and ../test/weights.txt
    // and W was multiplied by 0.05
    solver->g.multiply(lbd);
    
    solutionSz my_solution;
    my_solution.true_solution = false;
    int i;
    std::vector<int> my_indicator;
    solver->solve_for_fixed_size(kval, bmin, bmax, my_solution);

    if (my_solution.true_solution) {
	std::cout << "A solution of size " << kval << std::endl;
	std::cout << "energy: " << my_solution.brkpt_or_h << std::endl;
	
	my_indicator = my_solution.indicator1;
	std::cout << "indicator: ";
	for (i=0; i<my_indicator.size(); i++){
	    std::cout << my_indicator[i] << " ";
	}
	std::cout << std::endl;
    }
    else { 
	std::cout << "No solution of size " << kval << std::endl;
	std::cout << "breakpoint: " << my_solution.brkpt_or_h << std::endl;
	
	my_indicator = my_solution.indicator1;
	std::cout << "indicator (smaller): ";
	for (i=0; i<my_indicator.size(); i++){
	    std::cout << my_indicator[i] << " ";
	}
	std::cout << std::endl;

	my_indicator = my_solution.indicator2;
	std::cout << "indicator (bigger): ";
	for (i=0; i<my_indicator.size(); i++){
	    std::cout << my_indicator[i] << " ";
	}
	std::cout << std::endl;
    }

    // delete &my_solution;
}



int main(int argc, char *argv[]){
  if (argc != 7){    
    std::cout << "ERROR Wrong number of arguments in main" << std::endl;
  }
  else
    {
      const char * ntwk_file, * weights_file;
      float lbd; // multiplying coefficient for network weights
      int kval;  // solution size
      float bmin, bmax; // beta min and max
      ntwk_file = argv[1];
      weights_file = argv[2];
      lbd = atof(argv[3]);
      kval = atoi(argv[4]);
      bmin = atof(argv[5]);
      bmax = atof(argv[6]);

      std::cout << "lbd: " << lbd << std::endl;
      std::cout << "kval: " << kval << std::endl;
      std::cout << "bmin: " << bmin << std::endl;
      std::cout << "bmax: " << bmax << std::endl;


      // Read weights 
      std::vector<float> weights;

      std::fstream input(weights_file);
      std::string line;
      while (std::getline(input, line))
	{
	  float value;
	  std::stringstream ss(line);
	  ss >> value;
	  weights.push_back(value);
	}
      // std::cout << "weights[0]: " << weights[0] << std::endl;
      // std::cout << "weights size: " << weights.size() << std::endl;

      try {
	maxflow_GT_param * solver;
	dimacs_parser_callback * constructor;

	// struct timeval tbeginalgo, tendalgo, tbeginread, tendread;
	// double texecalgo(0), texecread(0);

	solver = new maxflow_GT_param;
	((maxflow_GT_param*)solver)->g.globUpdtFreq = 0.5;
	constructor = (maxflow_GT_param*)solver;
    
	// gettimeofday(&tbeginread, NULL);
	dimacs_parser(ntwk_file,*constructor,2);
	// gettimeofday(&tendread, NULL);
	// texecread = (double)(1000 * (tendread.tv_sec - tbeginread.tv_sec) + ((tendread.tv_usec - tbeginread.tv_usec)/1000));
	// debug::stream << "exec time for building graph = " << texecread << " ms\n";
    
	// std::cout << "num nodes: " << solver->g.n << std::endl;
	// std::cout << "num edges: " << solver->g.m << std::endl;

	// attach weights to solver
	solver->weights = weights;
	
	// test_energy_computation(solver);

	// test_optimize_h(solver);
	
	// test_solve_for_fixed_size(solver);

	solve(solver, lbd, kval, bmin, bmax);

	// gettimeofday(&tbeginalgo, NULL);    

	// gettimeofday(&tendalgo, NULL);
	// texecalgo = (double)(1000 * (tendalgo.tv_sec - tbeginalgo.tv_sec) + ((tendalgo.tv_usec - tbeginalgo.tv_usec)/1000));
	// debug::stream << "exec time for test = " << texecalgo << " ms\n";

	solver->print_info(); // for some reason if you comment this out there's a compilation error

	
	
	// solver->save_cut();
	
	delete solver;
      } catch(...){
	debug::stream << "ERROR\n";
	exit(1);
      };





      
      return 0;
    };
}


