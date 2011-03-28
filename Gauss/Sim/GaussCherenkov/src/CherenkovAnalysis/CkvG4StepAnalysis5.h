// $Id: RichG4StepAnalysis5.h,v 1.1 2004-06-03 12:44:00 seaso Exp $
#ifndef RICHANALYSIS_CKVG4STEPANALYSIS5_H 
#define RICHANALYSIS_CKVG4STEPANALYSIS5_H 1

// Include files
#include "GiGa/GiGaStepActionBase.h"
template <class TYPE> class GiGaFactory;
class G4Step;

/** @class CkvG4StepAnalysis5 CkvG4StepAnalysis5.h RichAnalysis/CkvG4StepAnalysis5.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2004-05-27
 */
class CkvG4StepAnalysis5:virtual public  GiGaStepActionBase {
  friend class GiGaFactory<CkvG4StepAnalysis5>;
  
public: 
  /// Standard constructor
  CkvG4StepAnalysis5
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;

  virtual ~CkvG4StepAnalysis5( ); ///< Destructor
  virtual void UserSteppingAction( const G4Step* aStep );

protected:

private:
  CkvG4StepAnalysis5();
  CkvG4StepAnalysis5(const  CkvG4StepAnalysis5& );
  CkvG4StepAnalysis5& operator=(const  CkvG4StepAnalysis5& );

};
#endif // RICHANALYSIS_CKVG4STEPANALYSIS5_H
