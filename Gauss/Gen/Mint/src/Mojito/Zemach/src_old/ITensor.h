#ifndef MINT_DALITZ_ITENSOR_HH
#define MINT_DALITZ_ITENSOR_HH

#include "counted_ptr.h"
#include "IndexList.h"

class ITensor{
 public:
  int rank() const=0;
  MINT::counted_ptr<ITensor> contract(MINT::counted_ptr<ITensor>&t
				      , const IndexList& myIndices
				      , const IndexList& hisIndices
				      ) const=0;

};

#endif
//
