/** @class MCFTDepositCreator MCFTDepositCreator.h
 *
 *  From the list of MCHits, this algorithm fill a container of FTChannelID 
 *  with the total energy deposited in each of them, and store it in the 
 *  transient data store.
 *
 *  TO DO :
 *  - include signal attenuation along the fibre as a function of the z-position of the Hit
 *  - light sharing between adjacent fibres
 *
 *  @author COGNERAS Eric
 *  @date   2012-06-05
 */

#ifndef MCFTDEPOSITCREATOR_H 
#define MCFTDEPOSITCREATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

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
  virtual StatusCode finalize  ();    ///< Algorithm finalization


protected:

private:

  std::string m_inputLocation;  ///< FT MCHits Location
  std::string m_outputLocation; ///< FT energy deposit Location
  
  DeFTDetector* m_deFT; ///< pointer to FT detector description
  

  //double m_neighbourFrac;
  //std::string m_energySharingMode;

  /// Conversion from 
  StatusCode relativeXFractionToEnergyFraction(const FTDoublePairs& positionPair, FTDoublePairs& energyPair);
  
};
#endif // MCFTDEPOSITCREATOR_H
