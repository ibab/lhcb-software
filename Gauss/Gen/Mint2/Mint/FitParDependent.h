#ifndef FIT_PAR_DEPENDENT_HH
#define FIT_PAR_DEPENDENT_HH

#include "Mint/IFitParDependent.h"
#include "Mint/IFitParRegister.h"
#include "Mint/FitParRef.h"

#include <vector>
#include <iostream>

namespace MINT{
  class FitParDependent 
    : virtual public IFitParRegister
    , public std::vector<FitParRef>
    {
      IFitParRegister* _daddy;

      bool ignoreFitParRef(const FitParRef& fpr)const;

    public:
      //bool changedSinceLastCall() const;
      //void rememberFitParValues() const;
      
      virtual unsigned int size() const{return std::vector<FitParRef>::size();}
      //const FitParRef& operator[](unsigned int i) const;

      virtual const FitParRef& operator[](unsigned int i) const{
	return ((static_cast<const std::vector<FitParRef>& >(*this))[i]);
      } 

      virtual FitParRef& operator[](unsigned int i) {
	return ((static_cast<std::vector<FitParRef>& >(*this))[i]);
      } 
   
      virtual bool changedSinceLastCall() const{
	for(unsigned int i=0; i < this->size(); i++){
	  if( ((*this)[i]).changedSinceLastCall() ) return true;
	}
	return false;
      }

      virtual void rememberFitParValues(){
	for(unsigned int i=0; i < this->size(); i++){
	  ((*this)[i]).rememberFitParValues();
	}
      }

      virtual bool registerFitParDependence(const IFitParDependent& fpd);
      bool registerFitParDependence(const FitParRef& fpr);
      void removeAllFitParDependencies(){this->clear();}
      // I would one day like to make this saver, but for
      // now it is possible to call this, although in 
      // fact the dependencies still persist.

      FitParDependent(IFitParRegister* daddy=0);
      FitParDependent(const FitParDependent& other, IFitParRegister* newDaddy=0);

      void listFitParDependencies(std::ostream& os=std::cout) const;
    }; // class

} // namespace MINT

#endif

//

