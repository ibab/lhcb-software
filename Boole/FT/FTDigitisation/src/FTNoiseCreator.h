// $Id: FTClusterCreator.h 155770 2013-04-29 08:09:25Z cogneras $
#ifndef FTNOISECREATOR_H 
#define FTNOISECREATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/RndmGenerators.h"

// LHCbKernel
#include "Kernel/FTChannelID.h"

#include "Event/MCFTDigit.h"

#include <iostream>
#include <iomanip>

//using namespace LHCb;

/** @class FTNoiseCreator FTNoiseCreator.h
 *  
 *
 *  @author Jacco de Vries
 *  @date   2013-09-25
 */


class FTNoiseCreator : public GaudiHistoAlg {
public: 
  /// Standard constructor
  FTNoiseCreator( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FTNoiseCreator( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  // Location
  std::string m_inputLocation;       ///< MCFTDigit input Container
  std::string m_outputLocation;      ///< FTCluster output Container

  float m_sipmGain, m_sipmGainVariation, m_pedestal, m_adcNoise, m_clusterMinCharge;
  float m_temperature, m_irradiation; 
  float m_thermalNoiseRateBase, m_crossTalkProbability, m_afterpulseProbability, m_nu;

  Rndm::Numbers m_flatDist, m_gauss, m_rndmLandau;


};
#endif // FTCLUSTERCREATOR_H


