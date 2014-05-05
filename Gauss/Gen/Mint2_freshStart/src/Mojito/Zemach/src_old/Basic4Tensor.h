#ifndef MINT_DALITZ_BASICTENSOR
#define MINT_DALITZ_BASICTENSOR

#include "ITensor.h"
#include "IndexList.h"
#include "counted_ptr.h"

#include <vector>

class Basic4Tensor : public virtual ITensor {

  int _rank;
  int _dim;
  std::vector<double> _theArray;

  int arrayPosition(const std::vector<int>& indices)const;
  int arrayPosition(int i)const;
  int arrayPosition(int i, int j)const;
  int arrayPosition(int i, int j, int k)const;

 public:
  Basic4Tensor(int rank);

  int rank() const{return _rank;}

  double& operator()(const std::vector<int>& indices);
  const double& operator()(const std::vector<int>& indices) const;

  double& operator()();
  const double& operator()() const;

  double& operator()(int i);
  const double& operator()(int i) const;

  double& operator()(int i, int j);
  const double& operator()(int i, int j) const;

  double& operator()(int i, int j, int k);
  const double& operator()(int i, int j, int k) const;

  MINT::counted_ptr<ITensor> contract(MINT::counted_ptr<ITensor>&t
				      , const IndexList& myIndices
				      , const IndexList& hisIndices
				      ) const;
};
#endif
//
