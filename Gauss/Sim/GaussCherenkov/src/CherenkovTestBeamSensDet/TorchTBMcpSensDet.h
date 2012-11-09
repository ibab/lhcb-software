// $Id: $
#ifndef CHERENKOVTESTBEAMSENSDET_TORCHTBMCPSENSDET_H 
#define CHERENKOVTESTBEAMSENSDET_TORCHTBMCPSENSDET_H 1

// Include files
// from GiGa
#include "GiGa/GiGaSensDetBase.h"

#include "GaussCherenkov/CkvG4Hit.h"
#include "TorchTBHitCollName.h"
#include "G4Step.hh"

// forward declarations
class G4HCofThisEvent;



/** @class TorchTBMcpSensDet TorchTBMcpSensDet.h CherenkovTestBeamSensDet/TorchTBMcpSensDet.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-05-25
 */


class TorchTBMcpSensDet : virtual public GiGaSensDetBase{

public: 
  /// Standard constructor
  TorchTBMcpSensDet(const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ); 

  virtual ~TorchTBMcpSensDet( ); ///< Destructor

 /// initialize
  StatusCode initialize();

  /// finalize
  StatusCode finalize();

  virtual void Initialize(G4HCofThisEvent* HCE);
  virtual bool ProcessHits ( G4Step* step ,
    G4TouchableHistory* history ) ;
  virtual void clear();
  
protected:

private:
  TorchTBMcpSensDet();
  TorchTBMcpSensDet(const TorchTBMcpSensDet& );
  TorchTBMcpSensDet& operator = (const TorchTBMcpSensDet& );
  TorchTBHitCollName * m_TorchTBHitCollName;
  CkvG4HitsCollection* m_TorchHC;
  G4int m_torchHCID;
  
private:
 

};
#endif // CHERENKOVTESTBEAMSENSDET_TORCHTBMCPSENSDET_H
