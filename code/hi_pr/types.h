#ifndef hi_pr_types_h
#define hi_pr_types_h
/* defs.h */

/* #ifdef EXCESS_TYPE_LONG */
/* typedef unsigned long excessType; */
/* typedef double excessType; */
/* #else */
/* typedef long long int excessType; /\* change to double if not supported *\/ */
/* #endif */

/* typedef unsigned long cType; */
/* typedef double cType; */

#include <vector>


typedef  /* arc */
   struct arcSt
{
    //float           totCap;          /* total capacity */
    int             gCapIdx;       /* index in g.cap */
    float           resCap;          /* residual capacity */
    struct nodeSt   *head;           /* arc head */
    struct arcSt    *rev;            /* reverse arc */
}
  arc;

typedef  /* node */
struct nodeSt
{
    int index;            /* index in the original list of nodes */
    arc *first;           /* first outgoing arc */
    arc *current;         /* current outgoing arc */
    double excess;        /* excess at the node
			     change to double if needed */
    /* double excess; */
    long d;                /* distance label */
    struct nodeSt *bNext;  /* next node in bucket */
    struct nodeSt *bPrev;  /* previous node in bucket */
} node;


typedef /* bucket */
  struct bucketSt
{
  node *firstActive;      /* first node with positive excess */
  node *firstInactive;    /* first node with zero excess */
} bucket;


typedef /* segment of a solution path */
  struct solutionSgt
{
  std::vector<int> * indicator_vector; /* indicator vector of selected features */
  float beta1; /* left side of the segment */
  float beta2; /* right side of the segment */
} solutionSgt;


typedef /* solution of a given size along the solution path */
  struct solutionSz
{
  bool true_solution; /* true if this is an actual solution; false if no solution of this size was found */
  float brkpt_or_h; /* if true_solution then value of the energy function, else value of the breakpoint beta */
  std::vector<int> indicator1; /* if true_solution then selected features, else solution of smaller size */
  std::vector<int> indicator2; /* if true_solution then empty, else solution of larger size */
} solutionSz;

#endif
