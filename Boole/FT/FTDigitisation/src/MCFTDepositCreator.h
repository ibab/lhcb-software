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
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/RndmGenerators.h"

// from Event
#include "Event/MCHit.h"
#include "Event/MCFTDeposit.h"
// LHCbKernel
#include "Kernel/FTChannelID.h"


class MCFTDepositCreator : public GaudiHistoAlg {

  typedef std::pair<LHCb::FTChannelID, double> FTDoublePair;
  typedef std::vector< FTDoublePair > FTDoublePairs;

public: 
  /// Standard constructor
  MCFTDepositCreator( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCFTDepositCreator( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

  StatusCode HitToChannelConversion_OldGeometry(LHCb::MCHit* ftHit, LHCb::MCFTDeposits *depositCont, unsigned int iSpill);
  StatusCode HitToChannelConversion_NewGeometry(LHCb::MCHit* ftHit, LHCb::MCFTDeposits *depositCont, unsigned int iSpill);
private:
  // Locations
  std::string m_inputLocation;     ///< FT MCHits Location
  std::string m_outputLocation;    ///< FT energy deposit Location

  // Spill properties     
  std::vector<std::string> m_spillNames;  ///< Vector of spill names and mc hit path
  std::vector<std::string> m_spillVector; ///< Vector of spill names
  std::vector<double> m_spillTimes;       ///< Vector of spill arrival times       

  // FT Geometry version
  int m_ftGeomVersion;
  
  // Fibre properties
  float       m_fiberPropagationTime;   ///< Light propagation time in fiber
  float       m_scintillationDecayTime; ///< Decay time of scintillation light release
  float       m_shortAttenuationLength; ///< Attenuation lengh of the light along the fibre : short component
  float       m_longAttenuationLength;  ///< Attenuation lengh of the light along the fibre : long component
  float       m_fractionShort;          ///< Fraction of short attenuation length at SiPM
  float       m_xMaxIrradiatedZone;     ///< Size in x of the zone where fibres are irradiated
  float       m_yMaxIrradiatedZone;     ///< Size in y of the zone where fibres are irradiated
  std::vector<float> m_irradiatedAttenuationLength;    ///< Attenuation length by steps in the zone.
  float       m_reflectionCoefficient;   ///< reflection coefficient of the mirror at the y=0 side of the fibre
  float       m_beginReflectionLossY;    ///< begin zone where reflectio is too late and lost
  float       m_endReflectionLossY;      ///< end of this zone

  float       m_xStepOfMap;               ///< Step  along X-axis of the FullAttenuationMap(in mm)
  float       m_yStepOfMap;               ///< Step  along Y-axis of the FullAttenuationMap(in mm)
  int         m_nXSteps;
  int         m_nYSteps;
  std::vector<float> m_transmissionMap;    ///< Maps hits to transmitted energy from the direct pulse
  std::vector<float> m_transmissionRefMap; ///< Maps hits to transmitted energy from the reflected pulse

  DeFTDetector* m_deFT; ///< pointer to FT detector description
  Rndm::Numbers m_flatDist; ///< random number generator 

};
#endif // MCFTDEPOSITCREATOR_H
