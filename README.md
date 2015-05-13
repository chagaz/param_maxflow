# param_maxflow
A fixed-size parametric maxflow implementation. 
If a solution (set of nodes attached to the source) 
of the given size is found along the solution path, return it.
Otherwise, return the solutions immediately smaller and larger.

This is based on the Shekhostov implementation [1] of the Goldberg-Tarjan-Cherkassky algorithm [2].

To compile:
   make -C projects/param_maxflow

To test:
   bin/param_GT05 test/W.dimacs test/weights.txt 0.05 9 1 10

To use:
   bin/param_GT05 <network dimacs file> <node weights file> <lambda> <solution size> <bmin> <bmax>
Where <lambda> is a multiplying factor for the node weights (use 1.0 to keep the original weights)
and <bmin>, <bmax> bound the values of the (beta) parameter (use min(weights) and max(weights) to guarantee spanning the whole space of possible values) 


[1] A. Shekhovtsov and V. Hlavac "A Distributed Mincut/Maxflow Algorithm Combining Path Augmentation and Push-Relabel" in EMMCVPR 2011 / Technical Report CTU--CMP--2011--03. http://cmp.felk.cvut.cz/~shekhovt/d_maxflow

[2] Cherkassky, Boris V., and Andrew V. Goldberg. “On Implementing Push-Relabel Method for the Maximum Flow Problem.” In Integer Programming and Combinatorial Optimization, edited by Egon Balas and Jens Clausen, 920:157–71. Berlin, Heidelberg: Springer Berlin Heidelberg, 1995. http://rd.springer.com/chapter/10.1007/3-540-59408-6_49.

