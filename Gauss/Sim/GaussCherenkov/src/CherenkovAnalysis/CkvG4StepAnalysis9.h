// $Id: $
#ifndef CHERENKOVANALYSIS_CKVG4STEPANALYSIS9_H 
#define CHERENKOVANALYSIS_CKVG4STEPANALYSIS9_H 1

// Include files
/// GiGa
#include "GiGa/GiGaStepActionBase.h"

template <class TYPE> class GiGaFactory;
class G4Step;


/** @class CkvG4StepAnalysis9 CkvG4StepAnalysis9.h CherenkovAnalysis/CkvG4StepAnalysis9.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2011-03-23
 */
class CkvG4StepAnalysis9 : virtual public  GiGaStepActionBase {
  friend class GiGaFactory<CkvG4StepAnalysis9>;


public: 
  /// Standard constructor
  CkvG4StepAnalysis9(const std::string& type   ,
                        const std::string& name   ,
                        const IInterface*  parent ); 

  virtual ~CkvG4StepAnalysis9( ); ///< Destructor
  virtual void UserSteppingAction( const G4Step* aStep );


protected:

private:
  CkvG4StepAnalysis9();
  CkvG4StepAnalysis9(const  CkvG4StepAnalysis9&);
  CkvG4StepAnalysis9&  operator=(const CkvG4StepAnalysis9& );
  
 

};
#endif // CHERENKOVANALYSIS_CKVG4STEPANALYSIS9_H
