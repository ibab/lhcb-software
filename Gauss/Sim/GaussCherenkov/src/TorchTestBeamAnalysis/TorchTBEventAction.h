// $Id: $
#ifndef TORCHTESTBEAMANALYSIS_TORCHTBEVENTACTION_H 
#define TORCHTESTBEAMANALYSIS_TORCHTBEVENTACTION_H 1

// Include files
#include <vector>
#include "GiGa/GiGaEventActionBase.h"
#include "TorchTBG4FillHistoSet6.h" 
#include "TorchTBHitCollName.h"
#include  "TorchTBHitAnalysis.h"

/** @class TorchTBEventAction TorchTBEventAction.h TorchTestBeamAnalysis/TorchTBEventAction.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-06-01
 */
class TorchTBEventAction :public GiGaEventActionBase {
public: 
  /// Standard constructor
  TorchTBEventAction(const std::string& type,
    const std::string& name,
    const IInterface*  parent   ); 

  virtual ~TorchTBEventAction( ); ///< Destructor
  /// initialize
  StatusCode initialize();

  /// finalize
  StatusCode finalize();

  virtual void BeginOfEventAction ( const G4Event* );
  virtual void EndOfEventAction   ( const G4Event* );
  TorchTBHitCollName* TorchTBCollName() {return m_TorchTBCollName; }  


protected:

private:
  TorchTBEventAction();
  TorchTBEventAction( const TorchTBEventAction& );
  TorchTBEventAction& operator = (const TorchTBEventAction& );
  template <typename TOOL> inline
  void delPointer( TOOL *& tool ) {
    if ( 0 != tool ) { delete tool; tool = 0;}
    
  }
  
  
private:
  

  TorchTBHitCollName* m_TorchTBCollName;
  int m_TorchTBG4CollectionID;
  

  TorchTBG4FillHistoSet6* m_TorchTBG4FillHistoSet6;

  TorchTBHitAnalysis* m_TorchTBHitAnalysis;
  

  bool m_ActivateTorchTBHistoFill6;
  

  };
#endif // TORCHTESTBEAMANALYSIS_TORCHTBEVENTACTION_H
