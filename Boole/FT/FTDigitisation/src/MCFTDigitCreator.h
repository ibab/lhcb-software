/** @class MCFTDigitCreator MCFTDigitCreator.h
 *  
 *  From the list of MCFTDeposit (deposited energy in each FTChannel), 
 *  this algorithm converts the deposited energy in ADC charge according
 *  to the mean number of photoelectron per MIP 
 *  + the mean ADC count per photoelectron.
 *  Created digits are put in transient data store.
 *
 *  TO DO :
 *  - add noise
 *
 *  THINK ABOUT :
 *  - dead channels
 *
 *  @author COGNERAS Eric
 *  @date   2012-04-04
 */

#ifndef MCFTDIGITCREATOR_H 
#define MCFTDIGITCREATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/RndmGenerators.h"

// LHCbKernel
#include "Kernel/FTChannelID.h"

// local
#include "SiPMResponse.h"

#include <iostream>
#include <iomanip>

class MCFTDigitCreator : public GaudiHistoAlg {
public: 
  /// Standard constructor
  MCFTDigitCreator( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCFTDigitCreator( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

  int deposit2ADC(const LHCb::MCFTDeposit* ftdeposit); ///< This function converts deposited energy in ADC Count
  std::pair<double,double> integrateResponse(const LHCb::MCFTDeposit* ftdeposit);
  int averagePhotoElectrons(double energy, LHCb::MCHit* noiseBypass);
  
  SiPMResponse* m_SiPMResponse; ///< pointer to SiPM integrated response function
  
protected:

private:

  std::string m_inputLocation;///< FT energy deposit Location
  std::string m_outputLocation;///< FT digit Location

  double m_photoElectronsPerMeV; ///< mean number of photoelectrons produced per deposited MeV
  double m_sipmGain;             ///< SiPM gain, in adccount per photoelectron
  double m_sipmGainVariation;    ///< SiPM gain relative fluctuation
  double m_adcNoise;             ///< Sigma of the noise in the ADC.
  //bool m_channelNoiseEnabled;    ///< Boolean to en/disable channel noise 
  std::vector<double> m_integrationOffset; ///< Vector of the integration offsets for T1,T2,T3
  std::vector<double> m_stationsZ; ///< Vector of the Z position for T1,T2,T3

  bool m_force2bitADC;  ///< force 3-valued charges at the end

  Rndm::Numbers m_gauss;
  Rndm::Numbers m_flat;
  
  
  // Set geometry
  int Nlayers   = 4*3;  // layers * stations
  int NModule   = 12;   // per up/down
  int NMat      = 2;    // up and down
  int Nsipms    = 16;   // per module
  int Nchannels = 128;  // per sipm array
  int Ntotchannels = Nlayers * NModule * NMat * Nsipms * Nchannels;

  int m_clusterLowThreshold;
  int m_clusterMidThreshold;
  int m_clusterHighThreshold;

  // tools
  
  // detectors
  DeFTDetector* m_deFT; ///< pointer to FT detector description
};
#endif // MCFTDIGITCREATOR_H
