#include "dimacs_parser.h"
#include <stdio.h>
#include "dynamic/dynamic.h"
#include <string.h>

class buff_read{
public:
	static const int max_size = 1024*1024*32;//8Mb
	dynamic::fixed_array1<char> buff;
	//char * buff;
	int sz;
	int pos;
	FILE *f;
private:
	void readahead(){
		sz = fread(buff.begin(),1,buff.size(),f);
		pos = 0;
	};
	__forceinline bool ready(){
		if(pos<sz)return true;
		if(feof(f))return false;
		readahead();
		return true;
	};
public:
	bool eof()const{return pos==sz && feof(f);};
	//
	buff_read(FILE * _f):f(_f){
		pos = 0;
		sz = 0;
		//buff = new char[max_size];
		buff.resize(max_size);
	};
	~buff_read(){
		//delete[] buff;
	};
	//
	__forceinline char getc(){
		ready();
		return buff[pos++];
	};
	//
	__forceinline void skip_line(){
		while(ready()){
			while(pos<sz){
				if(buff[pos]=='\n')return;
				++pos;
			};
		};
	};
	//
	__forceinline int read_line(char * s,int max_count){
		int k = 0;
		while(ready() && k<max_count-1){
			while(pos<sz && k<max_count-1){
				s[k] = buff[pos];
				if(buff[pos]=='\n'){
					s[k+1] = 0;
					return k;
				};
				++pos;
				++k;
			};
		};
		s[k+1] = 0;
		return k;
	};
};

__forceinline char * eatspace(char * ps){
	do{
		char c = *ps;
		if(c==' ' || c== '\t' || c== '\n' || c=='\r'){
			++ps;
			continue;
		};
		return ps;
	}while(1);
};
__forceinline int strtol10(char *& ps){
	int r = 0;
	bool isnumber = false;
	ps = eatspace(ps);
	do{
		char c = *ps;
		if(c<'0' || c>'9'){
			if(!isnumber)
				std::cout << "number expected" << std::endl;
			return r;
		};
		isnumber = true;
		r = r*10+(c-'0');
		++ps;
	}while(1);
};

void dimacs_parser(const char * filename, dimacs_parser_callback & A, int loops)
{
  FILE * f = fopen(filename,"rb");
  if (!f)
    std::cout << "cant read file: " << filename << std::endl;
  char s[1024];
  char s1[1024];
  long int pos;// remember position where arcs start in the stream
  char c = fgetc(f);
  int n, m;
  int S, T;
  struct tlast_arc
  {
    int u;
    int v;
    float cap;
    bool parsed;
    tlast_arc():parsed(true){};
  } last_arc;

  // read header
  for ( ; !feof(f) && c != 'a'; c = fgetc(f)) { 
    switch (c) {
    case 'p'://problem specification
      fscanf(f, " %*s %i %i ", &n, &m);// number of nodes and arcs
      break ;
    case 'c'://comment line, look for regulargreed, complexgrid
      fgets(s, 1024, f); //read comment line until the end
      sscanf(s, " %s ", s1);
      break ;
    case 'n'://source or sink nodes
      int v;
      fscanf(f, " %i %c ", &v, s);
      --v; //zero-based index
      if (s[0] == 's')
	S = v;
      else
	T = v;
      break ;
    }
  }

  pos = ftell(f) - 1;
  //A.allocate1(n, m, S, T);
  A.allocate1(n, (m + 2*(n-2)), S, T); // add source-to-node and node-to-sink edges


  // now a double loop over edges - count and read
  for (int loop = 0; loop < loops; ++loop) {

    // create the source-to-node and node-to-sink edges
    for (int node_index = 0; node_index < n-2; node_index++){
      A.read_arc(loop, S, node_index, 0., 0);
      A.read_arc(loop, node_index, T, 0., 0);
    }

    fseek(f, pos, SEEK_SET);//rewind to where arcs begin
    buff_read ff(f);
    while (!ff.eof()) {
      c = ff.getc();
      switch (c) {
      case 'c':
	ff.skip_line();
	break ;
      case 'a':
	int head;
	int tail;
	float cap;
	ff.read_line(s, 1024);
	char *ps = s;
	head = strtol10(ps);
	tail = strtol10(ps);
	cap = atof(ps);
	// std::cout << "cap: " << cap << std::endl;
	if (cap == 0.) {
	  //std::cout << "ignore arc " << head << ", " << tail << "(" << cap << ")" << std::endl;
	  // skip zero arcs
	  continue ; 
	}
	--head; // to zero-based index
	--tail;
	// break;
	// if u is source or v is sink, assign this capacity to the excess
	// A.read_arc(loop,u,v,cap,0);//next arc was a reverse one
	// continue;
	if (head == S || tail == T){
	  A.read_arc(loop, head, tail, cap, 0);
	}
	else {
	  if (!last_arc.parsed) {
	    if (last_arc.u == tail && last_arc.v == head) {
	      // next arc was the reverse of that one
	      A.read_arc(loop, head, tail, cap, last_arc.cap);
	      last_arc.parsed = true;
	    }
	    else {
	      // parse arc as unpaired
	      A.read_arc(loop, last_arc.u, last_arc.v, last_arc.cap, 0);
	      last_arc.u = head;
	      last_arc.v = tail;
	      last_arc.cap = cap;
	      last_arc.parsed = false;
	    }
	  }
	  else {
	    // std::cout << head << ", " << tail << "(" << cap << ")" << std::endl;	   
	    last_arc.u = head;
	    last_arc.v = tail;
	    last_arc.cap = cap;
	    last_arc.parsed = false;
	  }
	}
	break ; // case
      }
    }
    if(!last_arc.parsed) {
      // std::cout << last_arc.u << ", " << last_arc.v << std::endl; 
      A.read_arc(loop, last_arc.u, last_arc.v, last_arc.cap, 0);//parse as unpaired
      last_arc.parsed = true;
    }
    A.allocate2(loop);
  }
  fclose(f);
}
