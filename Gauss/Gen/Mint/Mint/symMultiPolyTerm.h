#ifndef MINT_SYM_MULTI_POLY_TERM_HH
#define MINT_SYM_MULTI_POLY_TERM_HH

#include "Mint/symPolyTerm.h"

#include <vector>
#include <iostream>

class symMultiPolyTerm : public std::vector< symPolyTerm >{

  bool createTerms(int dimension, int order);
  bool createTerms(int dimension, int order
		   , std::vector<std::vector<int> >& v);
  bool init(int dimension, int order);

 public:
  symMultiPolyTerm(int dimension, int order=0);
  symMultiPolyTerm(const symMultiPolyTerm& other);

  virtual ~symMultiPolyTerm(){}

  void print(std::ostream& os=std::cout) const;
};

std::ostream& operator<<(std::ostream& os, const symMultiPolyTerm& smpt);

#endif
//
