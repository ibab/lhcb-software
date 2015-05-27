#ifndef REMEMBER_ANYTHINGFAST_HH
#define REMEMBER_ANYTHINGFAST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include <vector>
#include <utility>

#include <iostream>

// new: configNumber. The idea: It numbers the configuration/set of fit parameters. If it changed, you need to re-calculate the value. "Not set" corresponds to  zero configNumber.

template<typename T>
class RememberAnythingFast{
  std::vector< std::pair< T, long int> > _anyVector;
 public:

  unsigned int size(){
    return _anyVector.size();
  }
  
  void resize(unsigned int newSize){
    if(newSize < this->size()) return;
    std::pair< T, long int> empty(0, 0);
    _anyVector.resize(newSize, empty);
  }

  void set(unsigned int i, const T& value, long int configNumber=1){
    //std::cout << "RememberAnythingFast called with "
    //          << value << ", " << configNumber << std::endl;
    if(i >= _anyVector.size()){
      resize(i+1);
    }
    std::pair< T, long int> trueValue(value, configNumber);
    //std::cout << "trueValue = " << trueValue.first 
    //	      << ", " << trueValue.second << std::endl;
    _anyVector[i] = trueValue;

    /*
    std::cout << "my contents, now " << std::endl;
    for(unsigned int i=0; i < _anyVector.size(); i++){
      std::cout << _anyVector[i].first 
		<< ", " << _anyVector[i].second << std::endl;
    }
    */
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
  bool get(int i, T& value, long int configNumber=1){
    /*std::cout << "my contents, now " << std::endl;
    for(unsigned int i=0; i < _anyVector.size(); i++){
      std::cout << _anyVector[i].first 
		<< ", " << _anyVector[i].second << std::endl;

    }
    std::cout << "called with: " << i << ", " << configNumber << std::endl;
    */
    if(! valid(i)) return false;
    //std::cout << "I'm valid! " << std::endl;
    if(_anyVector[i].second != configNumber) return false;
    value = _anyVector[i].first;
    //std::cout << "set value to " << value <<std::endl;
    return true;
  }

};


#endif
//




