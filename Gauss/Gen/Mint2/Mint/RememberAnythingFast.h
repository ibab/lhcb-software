#ifndef REMEMBER_ANYTHINGFAST_HH
#define REMEMBER_ANYTHINGFAST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include <vector>
#include <utility>

template<typename T>
class RememberAnythingFast{
  std::vector< std::pair< T, bool> > _anyVector;
 public:

  unsigned int size(){
    return _anyVector.size();
  }
  
  void resize(unsigned int newSize){
    if(newSize < this->size()) return;
    std::pair< T, bool> empty(0, false);
    _anyVector.resize(newSize, empty);
  }

  void set(unsigned int i, const T& value){
    if(i >= _anyVector.size()){
      resize(i+1);
    }
    std::pair< T, bool> trueValue(value, true);
    _anyVector[i] = trueValue;
  }


  
  RememberAnythingFast(){
    _anyVector.clear();
  }
  RememberAnythingFast(const RememberAnythingFast& other)
    : _anyVector(other._anyVector){
  }

  ~RememberAnythingFast(){
    _anyVector.clear();    
  }

  bool valid(long int i){
    if(i < 0) return false;
    if(i >= (long int) this->size()) return false;
    if(!(_anyVector[i].second)) return false;
    return true;
  }
  bool get(int i, T& value){
    if(! valid(i)) return false;
    value = _anyVector[i].first;
    return true;
  }

};


#endif
//




