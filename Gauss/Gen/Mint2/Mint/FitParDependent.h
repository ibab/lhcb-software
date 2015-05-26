#ifndef FIT_PAR_DEPENDENT_HH
#define FIT_PAR_DEPENDENT_HH

#include "IFitParDependent.h"
#include "IFitParRegister.h"

#include <vector>

namespace MINT{
  class FitParDependent 
    : virtual public IFitParRegister
    , std::vector<const IFitParDependent*> // a bit unsave, but easy
    {
    public:
      virtual bool changedSinceLastCall() const;
      
      bool registerFitParDependence(const IFitParDependent& fpd){
	//if(0 != fpd) this->push_back(fpd);
	this->push_back(&fpd);
	return true;
      }
      void removeAllFitParDependencies(){this->clear();}
      // I would one day like to make this saver, but for
      // now it is possible to call this, although in 
      // fact the dependencies still persist.

      FitParDependent(IFitParRegister* daddy=0);
      FitParDependent(const FitParDependent& other);
    }; // class

} // namespace MINT

#endif

//

