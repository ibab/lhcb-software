#ifndef REMEMBER_ANYTHING_HH
#define REMEMBER_ANYTHING_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include <map>

template<typename T>
class RememberAnything{
  std::map<void*, T > _anyMap;
 public:
  
  void set(void* key, const T& value){
    
    _anyMap[key] = value;
  }

  
  RememberAnything(){}
  RememberAnything(const RememberAnything& other)
    : _anyMap(other._anyMap){
  }

  ~RememberAnything(){
    _anyMap.clear();    
  }


  bool find(void* key, T& value){
    
    typename std::map<void*, T >::const_iterator it
      = _anyMap.find(key);
    
    if(it == _anyMap.end()) return false;
    
    value = it->second;
    return true;
  }

};


#endif
//




