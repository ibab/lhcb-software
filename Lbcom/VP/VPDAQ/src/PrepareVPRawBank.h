// $Id: PrepareVPRawBank.h,v 1.1.1.1 2010-01-21 13:16:41 marcin Exp $
#ifndef PREPAREVPRAWBANK_H 
#define PREPAREVPRAWBANK_H 1
#include <vector>
#include <algorithm>
#include <string>
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiAlgorithm.h"
// Event
#include "Event/VPCluster.h"
#include "Event/VPLiteCluster.h"
// VPelDet
#include "VPDet/DeVP.h"
// Si
#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiRawBufferWord.h"
#include "SiDAQ/SiADCBankTraits.h"

/** @class PrepareVPRawBank.h 
 *  VPAlgorithms/PrepareVPRawBank.h
 *
 *  @author Marcin Kucharczyk
 *  @date   2009/12/08
 */

bool sortLessBySensor(const LHCb::VPCluster* first,
                      const LHCb::VPCluster* second) {
     return first->key().sensor() < second->key().sensor();
}
bool sortLessByChannel(LHCb::VPChannelID first,
                       LHCb::VPChannelID second) {
     return first < second;
}

class PrepareVPRawBank : public GaudiAlgorithm {
public:
  /// Standard constructor
  PrepareVPRawBank(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~PrepareVPRawBank();     ///< Destructor
  virtual StatusCode initialize();      ///< Algorithm initialization
  virtual StatusCode execute   ();      ///< Algorithm execution
  virtual StatusCode finalize  ();      ///< Algorithm finalise

private:

  void makeBank(
           std::vector<const LHCb::VPCluster*>::const_iterator begin, 
	         std::vector<const LHCb::VPCluster*>::const_iterator end);
  void storeBank(int sensor,
	     std::vector<const LHCb::VPCluster*>::const_iterator begin, 
	     std::vector<const LHCb::VPCluster*>::const_iterator end,
	     LHCb::RawEvent* rawEvent);
  long findPattern(LHCb::VPChannelID centrChanID,
                   std::vector<LHCb::VPChannelID> activeChIDs);

private:

  std::string m_clusterLocation;
  std::string m_rawEventLocation;
  unsigned int m_bankVersion;
  unsigned int m_bankSizeInBytes;
  std::vector<const LHCb::VPCluster*> m_sortedClusters; 
  std::vector<SiDAQ::buffer_word> m_rawData;
  std::vector<SiDAQ::buffer_word> m_clusterLiteBuffer;
  std::vector<SiDAQ::buffer_word> m_clusterPattBuffer;
  std::vector<SiDAQ::buffer_word> m_clusterToTsBuffer;
  std::vector<unsigned int> m_sensorNumbers;
  bool m_isDebug; 
  bool m_isVerbose;
  DeVP* m_vPelDet;

};
#endif // PREPAREVPRAWBANK_H
