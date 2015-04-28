#ifndef maxflow_GT_param_h
#define maxflow_GT_param_h

#include "hi_pr/hi_pr.h"
#include "dimacs_parser.h"
#include "maxflow_solver.h"
//#include "maxflow_GT.h"

//class maxflow_GT_param : public maxflow_GT{
class maxflow_GT_param : public maxflow_solver, public dimacs_parser_callback{
 public:
    hi_pr g;
    std::vector<float> weights; // node weights

 public:
    maxflow_GT_param();
    void construct(const dynamic::num_array<int, 2> &E, dynamic::num_array<int, 2> &cap, dynamic::num_array<int, 1> &excess, std::vector<float> & weights); 
    tflow maxflow();
    void construct(const char *filename, std::vector<float> & weights); 
    virtual void save_cut_to_file(const std::string & filename);
    void save_cut();
    virtual tflow cut_cost();
    virtual void get_cut(int * S);
    virtual void save_cut_to_vector(std::vector<int> & S);

 public:
    virtual void allocate1(int n, int m, int S, int T);
    virtual void allocate2(int loop);
    virtual void read_arc(int loop, int u, int v, float cap1, float cap2);

    // Compute energy function for a given selection (indicator)
    float energy_function(std::vector<int> & indicator);

    // Compute the indicator of the optimal set of features
    // for the given trs_weights vector of source->node or node->sink weights.
    int optimize_energy(std::vector<int> & indicator, std::vector<float> & trs_weights);
	
    // Compute the solution of a given size along the solution path
    int solve_for_fixed_size(int kval, float beta_min, float beta_max,  solutionSz & solution);
	

};

#endif /* maxflow_GT_param_h */
