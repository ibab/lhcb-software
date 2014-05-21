#ifndef PRFTHITMANAGER_H 
#define PRFTHITMANAGER_H 1

// Include files
// from Gaudi
#include "PrKernel/PrHitManager.h"
#include "FTDet/DeFTDetector.h"
#include "GaudiKernel/RndmGenerators.h"

static const InterfaceID IID_PrFTHitManager ( "PrFTHitManager", 1, 0 );

/** @class PrFTHitManager PrFTHitManager.h
 *  Tool that transforms clusters into 'hits' (spatial positions) which are then used by the pattern
 *  recognition algorithms involving the FT.
 *  
 *  Parameters:
 *  - XSmearing: Amount of gaussian smearing in x
 *  - ZSmearing: Switch on displacement in z (not used at the moment)
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

  /** @brief Setup geometry of FT with hit zones for top / bottom / x-u-v-x layers
   */
  void buildGeometry();
  
  /** @brief Construct the hits and apply smearing to hit position (if enabled)
   */
  void decodeData();

  bool m_geometryBuilt; ///< Flag to check if geometry was already built
  

protected:

private:
  DeFTDetector* m_ftDet;
  Rndm::Numbers m_gauss; ///< Random number generator for gaussian smearing
  float m_xSmearing;  ///< Amount of smearing in x
  float m_zSmearing; 
};
#endif // PRFTHITMANAGER_H
