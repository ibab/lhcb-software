#ifndef PRFTHITMANAGER_H 
#define PRFTHITMANAGER_H 1

// Include files
// from Gaudi
#include "PrKernel/PrHitManager.h"
#include "FTDet/DeFTDetector.h"
#include "GaudiKernel/RndmGenerators.h"

static const InterfaceID IID_PrFTHitManager ( "PrFTHitManager", 1, 0 );

/** @class PrFTHitManager PrFTHitManager.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2012-03-13
 */
class PrFTHitManager : public PrHitManager {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PrFTHitManager; }

  /// Standard constructor
  PrFTHitManager( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~PrFTHitManager( ); ///< Destructor

  void buildGeometry();
  
  void decodeData();

protected:

private:
  DeFTDetector* m_ftDet;
  Rndm::Numbers m_gauss;
  float m_xSmearing;
  float m_zSmearing;
};
#endif // PRFTHITMANAGER_H
