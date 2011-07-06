#ifndef SIMPLE_PROTECTED_ARRAY_HH
#define SIMPLE_PROTECTED_ARRAY_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT

template<typename T> 
class simpleProtectedArray{
 protected:
  T* _array;
  unsigned int _size;
 public:
  simpleProtectedArray(int size=1){
    _array = new T[size+1];
    _size = size;
  }
  ~simpleProtectedArray(){
    delete[] _array;
  }
  T& operator[](unsigned int i){
    if(i >= _size){
      return _array[size];
    }else{
      return _array[i];
    }
  }
  const T& operator[](unsigned int i) const{
    if(i >= _size){
      return _array[size];
    }else{
      return _array[i];
    }
  }
};

#endif
//
