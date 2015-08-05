#ifndef CACHED_BY_EVENT_HH
#define CACHED_BY_EVENT_HH

#include "Mint/FitParRef.h"
#include "Mint/IFitParDependent.h"
//#include "Mint/IDalitzEvent.h"
#include "Mint/DalitzEvent.h"

#include <iostream>

template <typename T>
class CachedByEvent : virtual public MINT::IFitParDependent{
  
 public:
  virtual T getNewVal(IDalitzEvent& evt)=0; // <<< user needs to implement this

 protected:
  long int _rememberNumber; // for caching
  std::vector<long int> _rememberNumberPermutation;
  long int _configNumber;

  long int rememberNumber() {
    if(_rememberNumber < 0){
      _rememberNumber = DalitzEvent::assignUniqueRememberNumber();
      std::cout << "just assigned _rememberNumber "<< _rememberNumber << std::endl;
    }
    return _rememberNumber;
  }
    
  long int rememberNumberPermutation(IDalitzEvent& evt) {
    if(_rememberNumberPermutation.size() < evt.numPermutations()){
      _rememberNumberPermutation.resize(evt.numPermutations());
      for(unsigned int i=0; i < evt.numPermutations(); i++)
	_rememberNumberPermutation[i] = DalitzEvent::assignUniqueRememberNumber();
      
    }
    return _rememberNumberPermutation[evt.permutationIndex()];
  }
  
  long int configNumber(){ 
    if(_configNumber <= 0) _configNumber=1;
    return _configNumber;
  }

  T recalculate(IDalitzEvent& evt){
    T result(getNewVal(evt));
    rememberFitParValues();
    evt.setValue(rememberNumber(), result, configNumber());
    return result;
  }
    
  T recalculatePermutation(IDalitzEvent& evt){
        T result(getNewVal(evt));
        rememberFitParValues();
        evt.setValue(rememberNumberPermutation(evt), result, configNumber());
        return result;
  }  
 public:
 CachedByEvent() : _rememberNumber(-9999), _configNumber(0){}
 CachedByEvent(const CachedByEvent& ) : _rememberNumber(-9999), _configNumber(0) {}
  
  T getValWithCaching(IDalitzEvent& evt){
    if(changedSinceLastCall()){
      _configNumber++;
      return recalculate(evt);
    }

    T result;
    if(! evt.retrieveValue(rememberNumber(), result, configNumber())){
      return recalculate(evt);
    }
    //std::cout << "using cached result" << std::endl;

    /*
    // debugging stuff:
    if(result != getNewVal(evt)){
      std::cout << "=============================================================" << std::endl;
      std::cout << "Nothing has changed? Hah! Check: " << result << " == " << getNewVal(evt) 
		<< " ?? (" << rememberNumber() << ")" << std::endl;
      listFitParDependencies(std::cout);
      std::cout << "=============================================================" << std::endl;
    }
    */  
    
    return result;
  }
    
  T getValWithCachingPermutation(IDalitzEvent& evt){
        if(changedSinceLastCall()){
           if(evt.permutationIndex()==0) _configNumber++;
           return recalculatePermutation(evt);
        }
        T result;
        if(! evt.retrieveValue(rememberNumberPermutation(evt), result, configNumber())){
            return recalculatePermutation(evt);
        }
        //std::cout << "using cached result" << std::endl;
        
        /*
        // debugging stuff:
        if(result != getNewVal(evt)){
        std::cout << "=============================================================" << std::endl;
        std::cout << "Nothing has changed? Hah! Check: " << result << " == " << getNewVal(evt) 
		<< " ?? (" << rememberNumberPermutation(evt) << ")" << std::endl;
        listFitParDependencies(std::cout);
        std::cout << "=============================================================" << std::endl;
        }
        */
      
        return result;
  }  

};

#endif
//
