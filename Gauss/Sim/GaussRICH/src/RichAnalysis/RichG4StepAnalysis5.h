// $Id: RichG4StepAnalysis5.h,v 1.1 2004-06-03 12:44:00 seaso Exp $
#ifndef RICHANALYSIS_RICHG4STEPANALYSIS5_H 
#define RICHANALYSIS_RICHG4STEPANALYSIS5_H 1

// Include files
#include "GiGa/GiGaStepActionBase.h"
template <class TYPE> class GiGaFactory;
class G4Step;

/** @class RichG4StepAnalysis5 RichG4StepAnalysis5.h RichAnalysis/RichG4StepAnalysis5.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2004-05-27
 */
class RichG4StepAnalysis5:virtual public  GiGaStepActionBase {
  friend class GiGaFactory<RichG4StepAnalysis5>;
  
public: 
  /// Standard constructor
  RichG4StepAnalysis5
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;

  virtual ~RichG4StepAnalysis5( ); ///< Destructor
  virtual void UserSteppingAction( const G4Step* aStep );

protected:

private:
  RichG4StepAnalysis5();
  RichG4StepAnalysis5(const  RichG4StepAnalysis5& );
  RichG4StepAnalysis5& operator=(const  RichG4StepAnalysis5& );

};
#endif // RICHANALYSIS_RICHG4STEPANALYSIS5_H
