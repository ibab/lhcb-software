#ifndef CHERENKOVANALYSIS_CHERENKOVSTEPANALYSIS8_H 
#define CHERENKOVANALYSIS_CHERENKOVSTEPANALYSIS8_H 1

// Include files

/** @class CherenkovStepAnalysis8 CherenkovStepAnalysis8.h CherenkovAnalysis/CherenkovStepAnalysis8.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-10-08
 */
/// GiGa
#include "GiGa/GiGaStepActionBase.h"

template <class TYPE> class GiGaFactory;
class G4Step;



class CherenkovStepAnalysis8: virtual public  GiGaStepActionBase {
  friend class GiGaFactory<CherenkovStepAnalysis8>;


public: 
  ///  Standard constructor
  CherenkovStepAnalysis8(const std::string& type   ,
                        const std::string& name   ,
                        const IInterface*  parent  ); 

  virtual ~CherenkovStepAnalysis8( ); ///< Destructor
  virtual void UserSteppingAction( const G4Step* aStep );

  

protected:

private:
  CherenkovStepAnalysis8();
  CherenkovStepAnalysis8(const CherenkovStepAnalysis8& );
  CherenkovStepAnalysis8& operator=( const CherenkovStepAnalysis8&);

   
  

};
#endif // CHERENKOVANALYSIS_CHERENKOVSTEPANALYSIS7_H
