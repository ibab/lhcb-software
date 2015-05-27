#ifndef FIT_PAR_DEPENDENT_HH
#define FIT_PAR_DEPENDENT_HH

#include "IFitParDependent.h"
#include "IFitParRegister.h"

#include <vector>
#include <iostream>

namespace MINT{
  class FitParDependent 
    : virtual public IFitParRegister
    , std::vector<FitParRef>
    {
      IFitParRegister* _daddy;
    public:
      virtual bool changedSinceLastCall() const;
      virtual void rememberFitParValues() const;
      
      virtual unsigned int size() const{return std::vector<FitParRef>::size();}
      virtual const FitParRef& operator[](unsigned int i) const;
    
      bool registerFitParDependence(const IFitParDependent& fpd){
	 for(unsigned int i=0; i < fpd.size(); i++){
	   this->push_back(fpd[i]);
	 }
	 if(0 != _daddy) _daddy->registerFitParDependence(fpd);
	 return true;
      }
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

