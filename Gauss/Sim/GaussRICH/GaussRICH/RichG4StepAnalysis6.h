// $Id: RichG4StepAnalysis6.h,v 1.1 2008-01-21 16:55:34 seaso Exp $
#ifndef RICHANALYSIS_RICHG4STEPANALYSIS6_H 
#define RICHANALYSIS_RICHG4STEPANALYSIS6_H 1

// Include files
// Include files
#include "GiGa/GiGaStepActionBase.h"
template <class TYPE> class GiGaFactory;
class G4Step;


/** @class RichG4StepAnalysis6 RichG4StepAnalysis6.h RichAnalysis/RichG4StepAnalysis6.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-10-30
 */
class RichG4StepAnalysis6:virtual public  GiGaStepActionBase  {
  friend class GiGaFactory<RichG4StepAnalysis6>;


public: 
  /// Standard constructor
  RichG4StepAnalysis6
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  virtual ~RichG4StepAnalysis6( ); ///< Destructor
  virtual void UserSteppingAction( const G4Step* aStep );


protected:

private:
  RichG4StepAnalysis6();
  RichG4StepAnalysis6(const  RichG4StepAnalysis6& );
  RichG4StepAnalysis6& operator=(const  RichG4StepAnalysis6& );


};
#endif // RICHANALYSIS_RICHG4STEPANALYSIS6_H
