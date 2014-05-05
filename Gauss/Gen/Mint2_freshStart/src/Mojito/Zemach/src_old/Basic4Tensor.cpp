#include "Basic4Tensor.h"
#include <cmath>
#include <iostream>

using namespace std;

Basic4Tensor::Basic4Tensor(int rank)
  : _rank(rank)
  , _dim(4)
  , _theArray((int) (pow(4, rank)+0.5), 0.0)
{
}

int Basic4Tensor::arrayPosition(const std::vector<int>& indices)const{
  if(indices.size() != _rank){
    cout << "WARNING in Basic4Tensor::arrayPosition"
	 << " dimensions of index array " << indices.size()
	 << " != rank = " << _rank 
	 << ". Will treat missing indices as 0."
	 << endl;
  }
  unsigned int to=_rank;
  if(to > indices.size()) to = indices.size();

  int pos=0;

  int d=1;
  for(unsigned int i = _rank -1 ; i >= 0; i--){
    if( i < to) pos += indices[i] * d;
    d *= _dim;
  }
  if(pos < 0 || pos > _theArray.size()){
    cout << "WARNING in Basic4Tensor::arrayPosition for"
	 << " indexList = " << indices
	 << ". Calculated position out of bounds: " << pos
	 << " is not in [0, " << _theArray.size() << "[ ."
	 << endl;
  }
  return pos;
}
int Basic4Tensor::arrayPosition(int i) const{
  if(i < 0 || i >= _theArray.size()){
    cout << "Basic4Tensor::arrayPosition(" << i
	 << ") index out of bounds [ 0, " << _theArray.size() << " [."
	 << endl;
    throw "index out of bounds";
  }
  return _theArray[i];
}
int Basic4Tensor::arrayPosition(int i, int j) const{
  if(_rank < 2){
    cout << "ERROR in Basic4Tensor::arrayPosition(" <<  i << ", " << j <<") "
	 << " _rank = " << _rank << " < 2." << endl;
    throw "out of bounds";
  }
  int pos = i*_dim + j;
  if(pos < 0 || pos >= _theArray.size()){
    cout << "Basic4Tensor::arrayPosition(" << i << ", " << j
	 << ") index " << pos 
	 << " out of bounds [ 0, " << _theArray.size() << " [."
	 << endl;
    throw "index out of bounds";
  }
  return _theArray[pos];
}
int Basic4Tensor::arrayPosition(int i, int j, int k) const{
  if(_rank < 3){
    cout << "ERROR in Basic4Tensor::arrayPosition(" 
	 << i << ", " << j << ", " <<  k << ") "
	 << " _rank = " << _rank << " < 3." << endl;
    throw "out of bounds";
  }
  int pos = i*_dim*_dim + j*_dim + k;
  if(pos < 0 || pos >= _theArray.size()){
    cout << "Basic4Tensor::arrayPosition(" 
	 << i << ", " << j << ", " << k
	 << ") index " << pos 
	 << " out of bounds [ 0, " << _theArray.size() << " [."
	 << endl;
    throw "index out of bounds";
  }
  return _theArray[pos];
}
double& Basic4Tensor::operator()(const std::vector<int>& indices){
  return _theArray[arrayPosition(indices)];
}
const double& Basic4Tensor::operator()(const std::vector<int>& indices) const{
  return _theArray[arrayPosition(indices)];
}

double& Basic4Tensor::operator()(){
  return _theArray[arrayPosition(0)];
}
const double& Basic4Tensor::operator()() const{
  return _theArray[arrayPosition(0)];
}
double& Basic4Tensor::operator()(int i){
  return _theArray[arrayPosition(i)];
}
const double& Basic4Tensor::operator()(int i) const{
  return _theArray[arrayPosition(i)];
}
double& Basic4Tensor::operator()(int i, int j){
  return _theArray[arrayPosition(i,j)];
}
const double& Basic4Tensor::operator()(int i, int j) const{
  return _theArray[arrayPosition(i,j)];
}
double& Basic4Tensor::operator()(int i, int j, int k){
  return _theArray[arrayPosition(i,j,k)];
}
const double& Basic4Tensor::operator()(int i, int j, int k) const{
  return _theArray[arrayPosition(i,j,k)];
}



