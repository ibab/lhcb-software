/** @class MCFTDepositCreator MCFTDepositCreator.h
 *
 *  From the list of MCHits, this algorithm fill a container of FTChannelID 
 *  with the total energy deposited in each of them, and store it in the 
 *  transient data store.
 *
 *
 *  @author COGNERAS Eric
 *  @date   2012-06-05
 */

#ifndef MCFTDEPOSITCREATOR_H 
#define MCFTDEPOSITCREATOR_H 1

// Include files
/// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from Event
#include "Event/MCHit.h"

// LHCbKernel
#include "Kernel/FTChannelID.h"





class MCFTDepositCreator : public GaudiAlgorithm {

  typedef std::pair<LHCb::FTChannelID, double> FTDoublePair;
  typedef std::vector< FTDoublePair > FTDoublePairs;

public: 
  /// Standard constructor
  MCFTDepositCreator( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCFTDepositCreator( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  // Locations
  std::string m_inputLocation;     ///< FT MCHits Location
  std::string m_outputLocation;    ///< FT energy deposit Location

  // Fibre properties
  float       m_shortAttenuationLength; ///< Attenuation lengh of the light along the fibre : short component
  float       m_longAttenuationLength;  ///< Attenuation lengh of the light along the fibre : long component
  float       m_fractionShort;          ///< Fraction of short attenuation length at SiPM
  float       m_xMaxIrradiatedZone;     ///< Size in x of the zone where fibres are irradiated
  float       m_yMaxIrradiatedZone;     ///< Size in y of the zone where fibres are irradiated
  float       m_irradiatedAttenuationLength;    ///< Attenuation length at 0,0 or irradiated fibres
  float       m_reflexionCoefficient;   ///< reflexion coefficient of the mirror at the y=0 side of the fibre
  float       m_beginReflexionLossY;    ///< begin zone where reflexio is too late and lost
  float       m_endReflexionLossY;      ///< end of this zone

  float       m_xStepMap;
  float       m_yStepMap;
  int         m_nXSteps;
  int         m_nYSteps;
  std::vector<float> m_transmissionMap;

  DeFTDetector* m_deFT; ///< pointer to FT detector description  
};
#endif // MCFTDEPOSITCREATOR_H
