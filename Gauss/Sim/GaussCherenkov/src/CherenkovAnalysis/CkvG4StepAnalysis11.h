#ifndef CHERENKOVANALYSIS_CKVG4STEPANALYSIS11_H 
#define CHERENKOVANALYSIS_CKVG4STEPANALYSIS11_H 1

// Include files
#include "GiGa/GiGaStepActionBase.h"

template <class TYPE> class GiGaFactory;
class G4Step;



/** @class CkvG4StepAnalysis11 CkvG4StepAnalysis11.h CherenkovAnalysis/CkvG4StepAnalysis11.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2013-02-15
 */
class CkvG4StepAnalysis11: virtual public  GiGaStepActionBase  {
  friend class GiGaFactory<CkvG4StepAnalysis11>;


public: 
  /// Standard constructor
  CkvG4StepAnalysis11(const std::string& type   ,
                        const std::string& name   ,
                        const IInterface*  parent  ); 

  virtual ~CkvG4StepAnalysis11( ); ///< Destructor

  virtual void UserSteppingAction( const G4Step* aStep );

protected:

private:

  CkvG4StepAnalysis11();
  CkvG4StepAnalysis11(const  CkvG4StepAnalysis11&);
  CkvG4StepAnalysis11&  operator=(const CkvG4StepAnalysis11& );

};
#endif // CHERENKOVANALYSIS_CKVG4STEPANALYSIS11_H


