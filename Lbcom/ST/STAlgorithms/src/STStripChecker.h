#ifndef STSTRIPCHECKER_H 
#define STSTRIPCHECKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Reading TELL1 data
#include "Event/STCluster.h"

#include "Kernel/STHistoAlgBase.h"

#include "Kernel/STChannelID.h"

#include "Kernel/ISTReadoutTool.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

#include <sstream>

/** @class STStripChecker STStripChecker.h
 *  
 *
 *  @author Johan Luisier
 *  @date   2009-10-22
 */
class STStripChecker : public ST::HistoAlgBase {
public: 
  /// Standard constructor
  STStripChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~STStripChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  /**
   * Initialise couting map
   */
  StatusCode initMap();
  
  /**
   * Prints out the strip
   */
  void printZeros(const std::pair< LHCb::STChannelID, unsigned int >& p);

private:
  
  /**
   * Map containing the number of hits above the S/N threshold.
   */
  std::map< LHCb::STChannelID, unsigned int > m_StripsMap;

  /**
   * Cut applied on the S/N, in order to determine whether the strip is
   * alive or not.
   */
  double m_signalToNoiseCut;

  /**
   * Location of the read data
   */
  std::string m_DataLoc;

  /**
   * Input data
   */
  LHCb::STClusters* clusterCont;

  /**
   * Last "printed" sector
   */
  std::string m_lastSector;

  /**
   * Counts how many strips didn't get a hit
   */
  unsigned int m_counter;

  /**
   * Number of wanted hits in order to tag the strip as alive
   */
  unsigned int m_hitsNumber;
};
#endif // STSTRIPCHECKER_H
