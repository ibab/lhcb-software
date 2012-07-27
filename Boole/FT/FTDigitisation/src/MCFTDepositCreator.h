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
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  /** This function apply attenuation factor according to the path of the light through the fibre.
      @param fibrelengh : full lengh of the fibre
      @param fibrefraction : relative position of the hit in the fibre wrt the SiPM position
      @param fibreenergy : energy deposited in the fibre, where the attenuation facor is applied
      @return true if successful
  */
  StatusCode ApplyAttenuationAlongFibre(const double fibrelengh, 
                                        const double fibrefraction, 
                                        FTDoublePairs& fibreenergy);

  /** This function apply attenuation factor according to the path of the light through the fibre.
      @param fibrelengh : full lengh of the fibre
      @param fibrefraction : relative position of the hit in the fibre wrt the SiPM position
      @param fibreenergy : energy deposited in the fibre, where the attenuation facor is applied
      @return true if successful
  */
  StatusCode ApplyAttenuationAlongFibre(const FTDoublePairs& fibrelengh, 
                                        const FTDoublePairs& fibrefraction, 
                                        FTDoublePairs& fibreenergy);
  

private:

  std::string m_inputLocation;  ///< FT MCHits Location
  std::string m_outputLocation; ///< FT energy deposit Location
  double m_attenuationLengh; ///< Attenuation lengh of the light along the fibre

  DeFTDetector* m_deFT; ///< pointer to FT detector description
  
};
#endif // MCFTDEPOSITCREATOR_H
