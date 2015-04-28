#include <cmath>
#include "maxflow_GT_param.h"
#include "hi_pr/types.h"

maxflow_GT_param::maxflow_GT_param(){
  info.name = "GT param";
};


void maxflow_GT_param::construct(const dynamic::num_array<int, 2> &E, dynamic::num_array<int, 2> &cap, dynamic::num_array<int, 1> &excess, std::vector<float> & weights)
{
  using namespace exttype;
  g.construct(excess.size(), E.size()[1], E.begin(), cap.begin(), excess.begin());
}


void maxflow_GT_param::construct(const char *filename, std::vector<float> & weights)
{
  dimacs_parser(filename, *this, 2);
}


maxflow_GT_param::tflow maxflow_GT_param::maxflow()
{
  // std::cout << "num nodes in maxflow_GT_param::maxflow() " << g.n << std::endl;
  // std::cout << "num edges in maxflow_GT_param::maxflow() " << g.m << std::endl;
  info.solve_t.start();
  g.stageOne();
  info.flow = g.flow;
  g.globalUpdate(); // required to determine the minimum cut
  info.solve_t.stop();
  //info.name = txt::String::Format("GT%2.1f",g.globUpdtFreq);
  if(g.globUpdtFreq<0.001){
    info.name = "GT0";
  }else if(fabs(g.globUpdtFreq-0.5)<0.01){
    info.name = "GT05";
  }else{
    info.name = txt::String::Format("GT%2.1f",g.globUpdtFreq);
  };
  info.nV = g.n;
  info.nE = g.m;
  return info.flow;
  // g.stageOne();
  // return g.flow;
}


void maxflow_GT_param::save_cut(){
  float flow = 0.;
  int vertices = 0;
  std::cout << "cost = " << g.cut_cost() << std::endl;
  
  flow = g.flow;
  for (int v=0; v<g.n; ++v)
    {
      if (g.is_weak_source(&g.nodes[v]) && (v != (g.source - g.nodes)))
	{
	  // v attached to source and is not source
	  vertices++;
	  std::cout << (v+1) << " ";
	}
    }
  std::cout << std::endl;
  std::cout << "flow = " << flow << std::endl;
  std::cout << vertices << " vertices attached to the source." << std::endl;
}


void maxflow_GT_param::save_cut_to_file(const std::string & filename){
  FILE * f = fopen(filename.c_str(),"wt+");
  setvbuf(f,NULL,_IOFBF,1024*1024*2);
  fprintf(f,"p max %lli %lli\n",(long long)g.n, (long long)g.m);
  fprintf(f,"c minimum cut, generated by %s\n",info.name.c_str());
  fprintf(f,"f %f\n",info.flow);
  //fprintf(f,"n 1 1\n");//source has label 1
  //fprintf(f,"n 2 0\n");//sink has label 0
  for(int v=0;v<g.n;++v){
    fprintf(f,"n %i %i\n",v+1,g.is_weak_source(&g.nodes[v]));
  };
}


maxflow_GT_param::tflow maxflow_GT_param::cut_cost(){
  return g.cut_cost();
}


void maxflow_GT_param::get_cut(int * C){
  for(int v=0;v<g.n;++v){
    C[v] = g.is_weak_source(&g.nodes[v]);
  };
}


void maxflow_GT_param::save_cut_to_vector(std::vector<int> & indicator){
  for(int v=0;v<g.n;++v){
    if (g.is_weak_source(&g.nodes[v]) && (v != (g.source - g.nodes))) {
      // v attached to source and is not source
      indicator.push_back(v);
    };
  };
}


void maxflow_GT_param::allocate1(int n, int m, int S, int T){
  // std::cout << "[maxflow_GT_param::allocate1] n: " << n << "\tm: " << m;
  // std::cout << "\tS: " << S << "\tT: "<< T << std::endl;
  g.n = n;
  g.nodes = (node*)calloc((n + 2), sizeof(node));
  g.source = g.nodes + S;
  g.sink = g.nodes + T;
  g.m = 0;
  g.flow0 = 0.;
  g.flow = 0.;
  g.nMin = 0;
}



void maxflow_GT_param::allocate2(int loop){
  // std::cout << "[maxflow_GT_param::allocate2] loop: " << loop << "\tg.m: " << g.m << std::endl;
  if (loop == 0)
    {
      // std::cout << "loop=0" << std::endl;
      g.arcs = (arc*)calloc((2 * g.m + 1), sizeof(arc));
      g.cap = (float*)calloc((2 * g.m + 1), sizeof(float));
      size_t		accum_size = 0;
      int index = 0;
      for (node *v = g.nodes; v != g.nodes + g.n + 2; ++v)
	{
	  v->index = index;
	  size_t s = (size_t&)v->first;
	  v->first = g.arcs + accum_size;
	  v->current = v->first;
	  accum_size += s;
	  index++;
	}
      assert(accum_size <= 2 * g.m + 1);
      g.m = 0;
    }
  else
    {
      // std::cout << "loop=1" << std::endl;
      g.allocDS();
      g.init();
    }
  // std::cout << "[maxflow_GT_param::allocate2] n: " << g.n << "\tm: " << g.m << std::endl;
}


void maxflow_GT_param::read_arc(int loop, int u, int v, float cap1, float cap2){
  if (loop == 0)
    {
      ++g.m;
      ++(size_t&)g.nodes[u].first;
      ++(size_t&)g.nodes[v].first;
    }
  else
    {
      arc *&uv = g.nodes[u].current;
      arc *&vu = g.nodes[v].current;

      uv->head = g.nodes + v;
      vu->head = g.nodes + u;
      uv->resCap = cap1;
      uv->gCapIdx = (uv - g.arcs);
      g.cap[uv - g.arcs] = cap1;
      vu->resCap = cap2;
      vu->gCapIdx = (vu - g.arcs);
      g.cap[vu - g.arcs] = cap2;
      uv->rev = vu;
      vu->rev = uv;
      ++uv;
      ++vu;
      ++g.m;
    }
}


float maxflow_GT_param::energy_function(std::vector<int> & indicator){
  /* Compute the energy function
        H(f) = \sum_{p} (- w_p) f_p + \sum_{p,q} f_p (1-f_q) A_{pq}
  */
  float energy = 0.;
  int i, j;
  node * node_i, * node_j;
  arc * a, *stopA;
  // std::cout << "weights[0]: " << weights[0] << std::endl;
  // std::cout << "weights size: " << weights.size() << std::endl;
  // std::cout << "indicator size: " << indicator.size() << std::endl;

  if(indicator.size() == 0){
    return energy;
  }
  else {
    // Compute linear term 
    for (i=0; i<indicator.size(); i++){
      // std::cout << "i: " << i << std::endl;
      // std::cout << "indicator[i]: " << indicator[i] << std::endl;
      // std::cout << "adding " << weights[indicator[i]] << std::endl;
      energy -= weights[indicator[i]];
    }
  } 
  // std::cout << "linear term: " << energy << std::endl;
  // std::cout << "number of nodes: " << g.n << std::endl;

  if(indicator.size() != (g.n-2)){
    // Add quadratic term 
    for (i=0; i<indicator.size(); i++){
      // std::cout << "i: " << i << std::endl;
      // std::cout << "indicator[i]: " << indicator[i] << ": ";
      node_i = &g.nodes[indicator[i]];
      // std::cout << "node_i: "<< node_i << std::endl;
      for (a = node_i->first, stopA = (node_i+1)->first; a != stopA; a++){  
	node_j = a->head;
	// std::cout << "node_j: "<< node_j << std::endl;
	j = node_j->index;
	// is node j NOT selected?
	if (! std::binary_search(indicator.begin(), indicator.end(), j)){
	  // if (g.cap[a->gCapIdx] != 0){
	  //   std::cout << j << "\t";
	  //   std::cout << "["<< g.cap[a->gCapIdx] << "]\t";
	  // }
	  energy += g.cap[a->gCapIdx];
	}
      }
      //std::cout << std::endl;
    }
  }
return energy;
};


int maxflow_GT_param::optimize_energy(std::vector<int> & indicator, std::vector<float> & trs_weights){
    std::cout << "maxflow_GT_param::optimize_energy" << std::endl;
  /* Compute the indicator of the optimal set of features 
     Optimize (maxflow) the energy function
     H'(f) = \sum_{p} (- w'_p) f_p + \sum_{p,q} f_p (1-f_q) A_{pq}
     Where, typically, w'p = w_p - beta

     indicator:    the vector of indices such that f_p = 1
     trs_weights:  w'
   */
  arc * a, * stopA;
  int i, j;
  node * node_i;


  // Update the network with the appropriate capacities on the node-to-sink edges
  for (i=0; i<g.n-2; i++){
    // std::cout << "node " << i << "\t";
    node_i = &g.nodes[i];
    // loop over the edges leaving from node_i
    for (a = node_i->first, stopA = (node_i+1)->first; a != stopA; a++){  
      // if one of them links node_i to the sink, update its capacity
      if (a->head == g.sink){
	if (trs_weights[i] < 0){
	  // std::cout << "weight " << - trs_weights[i] << std::endl;
	  g.cap[a->gCapIdx] = - trs_weights[i];
	  a->resCap = - trs_weights[i];

	} 
	else {
	  // std::cout << "weight 0" << std::endl;
	  g.cap[a->gCapIdx] = 0.;
	  a->resCap = 0.;
	}
      }
    }
  }

  // Update the network with the appropriate capacities on the source-to-node edges
  a = g.source->first;
  for (i=0; i<g.n-2; i++){
    // std::cout << "node " << i << "\t";
    j = a->head->index; // index of the node connected to the source we're looking at
    if (trs_weights[j] > 0){
      // std::cout << "weight " << trs_weights[j] << std::endl;
      g.cap[a->gCapIdx] = trs_weights[j];
      a->resCap = trs_weights[j];
    } 
    else {
      // std::cout << "weight 0" << std::endl;
      g.cap[a->gCapIdx] = 0.;
      a->resCap = 0.;
    }
    a++; // move on to the next edge leaving from the source
  }

  // update the network
  g.init();

  // get the maxflow on the updated network
  double F = maxflow();

  // Save the cut
  save_cut_to_vector(indicator);

  return 0;
  
};
	

int maxflow_GT_param::solve_for_fixed_size(int kval, float beta_min, float beta_max, solutionSz & solution){
    /* Compute the solution of a given size along the solution path
       For the range of beta values, solve
       argmax \sum_{p} (w_p - beta) f_p - \sum_{p,q} f_p (1-f_q) A_{pq}
       eq.
       argmin \sum_{p} (beta - w_p) f_p + \sum_{p,q} f_p (1-f_q) A_{pq}
       H(f) = \sum_{p} (- w_p) f_p + \sum_{p,q} f_p (1-f_q) A_{pq}
       H^{beta}(f) = H(f) + beta \sum_{p} f_p
       
       Return THE solution of size kval (if any).
       Also return H(f). 
    */

    std::vector<int> f_min;
    std::vector<float> weights_tmp;
    std::vector<int> f_max;

    // Compute f_min (solution for beta_min)
    weights_tmp.resize(g.n-2);   
    for (int i=0; i<g.n-2; i++){
	weights_tmp[i] = weights[i] - beta_min;
    }
    maxflow_GT_param::optimize_energy(f_min, weights_tmp);

    if (f_min.size() == kval){
	solution.true_solution = true;
	solution.brkpt_or_h = maxflow_GT_param::energy_function(f_min);
	solution.indicator1 = f_min;
	return 0;
    }

    // Compute f_max (solution for beta_max)
    for (int i=0; i<g.n-2; i++){
	weights_tmp[i] = weights[i] - beta_max;
    }
    optimize_energy(f_max, weights_tmp);
    
    if (f_max.size() == kval){
	solution.true_solution = true;
	solution.brkpt_or_h = maxflow_GT_param::energy_function(f_max);
	solution.indicator1 = f_max;
	return 0;
    }
    
    if (f_min.size() == f_max.size()){
	// The solution path is only one segment
	solution.true_solution == false;
	solution.brkpt_or_h = beta_max;

	std::vector<int> empty_solution;
	solution.indicator1 = empty_solution;

	std::vector<int> full_solution;
	full_solution.resize(g.n-2);
	for (int i=0; i<g.n-2; i++){
	    full_solution[i] = i;
	}
	solution.indicator2 = full_solution;
	return 0;
    }

    // Construct solution path as a list of solution segments
    std::list<solutionSgt> solution_path; 

    solutionSgt segment;
    segment.indicator_vector = &f_min;
    segment.beta1 = beta_min;
    segment.beta2 = beta_min;
    solution_path.push_back(segment);

    segment.indicator_vector = &f_max;
    segment.beta1 = beta_max;
    segment.beta2 = beta_max;
    solution_path.push_back(segment);

    bool keep_going = true; 
    bool srch_brkpt = false;
    float beta_i, beta_j, beta_0;
    int f_i_size, f_j_size;
    std::vector<int> f_0;
    std::vector<int> f_i; 
    std::vector<int> f_j; 
    float denominator;
    std::list<solutionSgt>::iterator cur_sgt_it; // i
    std::list<solutionSgt>::iterator nxt_sgt_it; // j
    solutionSgt *cur_sgt;
    solutionSgt *nxt_sgt;

    int kill_me_now = 0;

    while (keep_going){
	kill_me_now ++;
	if (kill_me_now == 1000){
	    return -1;
	}
	for (cur_sgt_it=solution_path.begin(); cur_sgt_it != solution_path.end(); cur_sgt_it++){
	    nxt_sgt_it = cur_sgt_it;
	    nxt_sgt_it++;
	    if (nxt_sgt_it == solution_path.end()){
		break;
	    }
	    cur_sgt = &*cur_sgt_it; // address of the object referenced by the iterator
	    nxt_sgt = &*nxt_sgt_it;
	    // std::cout << cur_sgt->beta2 << " ?< " << nxt_sgt->beta1 << std::endl;
	    if (cur_sgt->beta2 < nxt_sgt->beta1){
		srch_brkpt = true;
		break;
	    }
	}
	if (! srch_brkpt){
	    // no more breakpoints to insert
	    keep_going = false;
	}
	
	// search for a breakpoint between beta_i and beta_j
	beta_i = cur_sgt->beta2;
	beta_j = nxt_sgt->beta1;
	std::cout << "Search for bkpt btw " << beta_i << " and " << beta_j << std::endl;
	
	f_i_size = cur_sgt->indicator_vector->size();
	f_j_size = nxt_sgt->indicator_vector->size();

	// Compute beta_0, solution of H^{beta}(f_i) = H^{beta}(f_j)
	denominator = f_i_size - f_j_size;
	// std::cout << "denominator: " << denominator << std::endl;
	if (denominator == 0){
	    // any beta_0 is solution
	    beta_0 = beta_i;
	}
	else {
	    f_i = *(cur_sgt->indicator_vector);
	    f_j = *(nxt_sgt->indicator_vector);
	    beta_0 = (energy_function(f_j) - energy_function(f_i)) / (float)(denominator);
	    // std::cout << "beta_0: " << beta_0 << std::endl;
	}
	
	// std::cout << "\t" << beta_i << " < " << beta_0 << " < " << beta_j << std::endl;
	assert((("beta_i: %.3f\tbeta_0: %.3f\tbeta_j: %.3f", beta_i, beta_0, beta_j), 
		((beta_i <= beta_0) && (beta_0 <= beta_j))));

	if (beta_0 == beta_i){
	    /* update I_j to be the union of I_j and [beta_i, beta_j]
	       ie. I_j from [beta_j, ...] becomes [beta_i, ...] */
	    // std::cout << beta_0 << " = " << beta_i << std::endl;
	    nxt_sgt->beta1 = beta_i;
	}
	else if (beta_0 == beta_j){
	    /* update I_i to be the union of I_i and [beta_i, beta_j]
	       ie. I_i from [..., beta_i] becomes [..., beta_j] */
	    cur_sgt->beta2 = beta_j;
	}
	else {
	    // Compute f_0 which minimizes H^{beta_0}(f)
	    f_0.erase(f_0.begin(), f_0.begin()+f_0.size()); // empty f_0
	    for (int i=0; i<g.n-2; i++){
		weights_tmp[i] = weights[i] - beta_0;
	    }
	    optimize_energy(f_0, weights_tmp);
	    // std::cout << "\tf_0.size(): " << f_0.size() << std::endl;
	    // for (int i=0; i<f_0.size(); i++){
	    // 	std::cout << f_0[i] << " ";
	    // }


	    if (f_0.size() == kval){
		solution.true_solution = true;
		solution.brkpt_or_h = maxflow_GT_param::energy_function(f_0);
		solution.indicator1 = f_0;
		return 0;
	    }

	    if ((f_0.size() == f_i_size) || (f_0.size() == f_j_size)){
		/* I_i becomes the union of I_i and [beta_i, beta_0]
		   i.e. I_i from [..., beta_i] becomes [..., beta_0]
		*/
		cur_sgt->beta2 = beta_0;
		/* I_j becomes the union of I_j and [beta_0, beta_j]
                   ie. I_j from [beta_j, ...] becomes [beta_0, ...]
		*/	
		nxt_sgt->beta1 = beta_0;
	    }
	    else {
		// insert [f_0, [beta_0, beta_0]] in between (i) and (j)
		segment.indicator_vector = &f_0;
		segment.beta1 = beta_0;
		segment.beta2 = beta_0;
		solution_path.insert(nxt_sgt_it, segment);		
	    }
	}
    } // end keep_going
    
    // No solution of the desired size was found
    beta_0 = (float)(-2^16);
    solution.true_solution = false;
    for (cur_sgt_it=solution_path.begin(); cur_sgt_it != solution_path.end(); cur_sgt_it++){
	cur_sgt = &*cur_sgt_it; // address of the object referenced by the iterator
	if (cur_sgt->indicator_vector->size() > kval){
	    solution.brkpt_or_h = cur_sgt->beta2;
	    solution.indicator1 = *cur_sgt->indicator_vector;
	}
	else if (cur_sgt->indicator_vector->size() < kval){
	    assert(beta_0 == cur_sgt->beta1);
	    solution.indicator2 = *cur_sgt->indicator_vector;
	    return 0;
	}
    }
};
