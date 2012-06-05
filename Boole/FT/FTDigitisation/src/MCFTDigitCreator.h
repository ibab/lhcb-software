// $Id$
#ifndef MCFTDIGITCREATOR_H 
#define MCFTDIGITCREATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// LHCbKernel
#include "Kernel/FTChannelID.h"


//using namespace LHCb;

/** @class MCFTDigitCreator MCFTDigitCreator.h
 *  
 *
 *  @author COGNERAS Eric
 *  @date   2012-04-04
 */
class MCFTDigitCreator : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MCFTDigitCreator( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCFTDigitCreator( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  int deposit2ADC(LHCb::MCFTDeposit* ftdeposit); ///< This function converts deposited energy in ADC Count
  
protected:

private:

  std::string m_inputLocation;
  std::string m_outputLocation;

  int m_meanPhotoElectronPerMeV;

  
};
#endif // MCFTDIGITCREATOR_H
