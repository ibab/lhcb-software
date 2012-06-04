// $Id: $
#ifndef TORCHTESTBEAMANALYSIS_TORCHTBBASICSTEPANALYSIS_H 
#define TORCHTESTBEAMANALYSIS_TORCHTBBASICSTEPANALYSIS_H 1

// Include files
#include "GiGa/GiGaStepActionBase.h"
template <class TYPE> class GiGaFactory;
class G4Step;


/** @class TorchTBBasicStepAnalysis TorchTBBasicStepAnalysis.h TorchTestBeamAnalysis/TorchTBBasicStepAnalysis.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-05-30
 */
class TorchTBBasicStepAnalysis:virtual public  GiGaStepActionBase {
  friend class GiGaFactory<TorchTBBasicStepAnalysis>;

public: 
  /// Standard constructor
  TorchTBBasicStepAnalysis(const std::string& type   ,   
                           const std::string& name   ,
                          const IInterface*  parent  ); 

  virtual ~TorchTBBasicStepAnalysis( ); ///< Destructor
  virtual void UserSteppingAction( const G4Step* aStep );

protected:

private:
  TorchTBBasicStepAnalysis();
  TorchTBBasicStepAnalysis(const TorchTBBasicStepAnalysis&);
  TorchTBBasicStepAnalysis& operator=(const TorchTBBasicStepAnalysis& );
  
};
#endif // TORCHTESTBEAMANALYSIS_TORCHTBBASICSTEPANALYSIS_H
