// $Id: $
#ifndef CHERENKOVTESTBEAMSENSDET_TORCHTBHITCOLLNAME_H 
#define CHERENKOVTESTBEAMSENSDET_TORCHTBHITCOLLNAME_H 1

// Include files
#include "globals.hh"

/** @class TorchTBHitCollName TorchTBHitCollName.h CherenkovTestBeamSensDet/TorchTBHitCollName.h *  
 *
 *  @author Sajan Easo
 *  @date   2012-05-28
 */
class TorchTBHitCollName {
public: 
  /// Standard constructor
  TorchTBHitCollName( ); 

  virtual ~TorchTBHitCollName( ); ///< Destructor
  G4String TorchTBHCName()
  {  return m_TorchTBHCName;}
  
protected:

private:
  G4String  m_TorchTBHCName;
  
};
#endif // CHERENKOVTESTBEAMSENSDET_TORCHTBHITCOLLNAME_H
