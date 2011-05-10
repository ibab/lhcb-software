// $Id: $
#ifndef CHERENKOVANALYSIS_CHERENKOVG4STEPANALYSIS6_H 
#define CHERENKOVANALYSIS_CHERENKOVG4STEPANALYSIS6_H 1


// Include files
#include "GiGa/GiGaStepActionBase.h"
template <class TYPE> class GiGaFactory;
class G4Step;

/** @class CherenkovG4StepAnalysis6 CherenkovG4StepAnalysis6.h CherenkovAnalysis/CherenkovG4StepAnalysis6.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2011-04-19
 */
class CherenkovG4StepAnalysis6:virtual public  GiGaStepActionBase {
  friend class GiGaFactory<CherenkovG4StepAnalysis6>;
  

public: 
  /// Standard constructor
  CherenkovG4StepAnalysis6
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;

  virtual ~CherenkovG4StepAnalysis6( ); ///< Destructor
  virtual void UserSteppingAction( const G4Step* aStep );


protected:

private:
  CherenkovG4StepAnalysis6();
  CherenkovG4StepAnalysis6(const CherenkovG4StepAnalysis6& );
  CherenkovG4StepAnalysis6& operator=(const CherenkovG4StepAnalysis6& );

};
#endif // CHERENKOVANALYSIS_CHERENKOVG4STEPANALYSIS6_H
