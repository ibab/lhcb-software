// $Id: $
#ifndef TORCHTESTBEAMANALYSIS_TORCHTBRUNACTION_H 
#define TORCHTESTBEAMANALYSIS_TORCHTBRUNACTION_H 1

// Include files
#include "GiGa/GiGaRunActionBase.h"

/** @class TorchTBRunAction TorchTBRunAction.h TorchTestBeamAnalysis/TorchTBRunAction.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-06-01
 */
class TorchTBG4DefineHistSet6;

class TorchTBRunAction: public virtual GiGaRunActionBase {
public: 
  /// Standard constructor
  TorchTBRunAction(const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent  ); 

  virtual ~TorchTBRunAction( ); ///< Destructor
  virtual void BeginOfRunAction ( const G4Run* run );


protected:

private:
  TorchTBRunAction();
  TorchTBRunAction(const TorchTBRunAction&);
  TorchTBRunAction& operator = (const  TorchTBRunAction& );
private:
  bool m_ActivateDefineTorchTBHistoSet6;
  TorchTBG4DefineHistSet6* m_TorchTBG4DefineHistSet6;
  
  
 

};
#endif // TORCHTESTBEAMANALYSIS_TORCHTBRUNACTION_H
