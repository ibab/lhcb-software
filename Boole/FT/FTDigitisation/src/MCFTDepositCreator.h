// $Id$
#ifndef MCFTDEPOSITCREATOR_H 
#define MCFTDEPOSITCREATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// LHCbKernel
#include "Kernel/FTChannelID.h"

// xml geometry
//#include "FTDet/Detector.h"
//#include "FTDet/Sector.h"
//#include "FTDet/Sensor.h"

//using namespace LHCb;

typedef std::vector< std::pair<LHCb::FTChannelID, double> > FTDoublePair;

/** @class MCFTDepositCreator MCFTDepositCreator.h
 *  
 *
 *  @author COGNERAS Eric
 *  @date   2012-04-04
 */
class MCFTDepositCreator : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MCFTDepositCreator( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCFTDepositCreator( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


protected:

private:

  std::string m_inputLocation;
  std::string m_outputLocation;
  
  DeFTDetector* m_deFT;
  

  double m_neighbourFrac;
  std::string m_energySharingMode;

  StatusCode relativeXFractionToEnergyFraction(const FTDoublePair positionPair, FTDoublePair& energyPair);
  
};
#endif // MCFTDEPOSITCREATOR_H
