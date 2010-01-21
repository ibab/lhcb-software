// $Id: PrepareVeloPixRawBank.h,v 1.1.1.1 2010-01-21 13:16:41 marcin Exp $
#ifndef PREPAREVELOPIXRAWBANK_H 
#define PREPAREVELOPIXRAWBANK_H 1
#include <vector>
#include <algorithm>
#include <string>
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiAlgorithm.h"
// Event
#include "Event/VeloPixCluster.h"
#include "Event/VeloPixLiteCluster.h"
// VeloPixelDet
#include "VeloPixDet/DeVeloPix.h"
// Si
#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiRawBufferWord.h"
#include "SiDAQ/SiADCBankTraits.h"

/** @class PrepareVeloPixRawBank.h 
 *  VeloPixAlgorithms/PrepareVeloPixRawBank.h
 *
 *  @author Marcin Kucharczyk
 *  @date   2009/12/08
 */

bool sortLessBySensor(const LHCb::VeloPixCluster* first,
                      const LHCb::VeloPixCluster* second) {
     return first->key().sensor() < second->key().sensor();
}
bool sortLessByChannel(LHCb::VeloPixChannelID first,
                       LHCb::VeloPixChannelID second) {
     return first < second;
}

class PrepareVeloPixRawBank : public GaudiAlgorithm {
public:
  /// Standard constructor
  PrepareVeloPixRawBank(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~PrepareVeloPixRawBank();     ///< Destructor
  virtual StatusCode initialize();      ///< Algorithm initialization
  virtual StatusCode execute   ();      ///< Algorithm execution
  virtual StatusCode finalize  ();      ///< Algorithm finalise

private:

  void makeBank(
           std::vector<const LHCb::VeloPixCluster*>::const_iterator begin, 
	         std::vector<const LHCb::VeloPixCluster*>::const_iterator end);
  void storeBank(int sensor,
	     std::vector<const LHCb::VeloPixCluster*>::const_iterator begin, 
	     std::vector<const LHCb::VeloPixCluster*>::const_iterator end,
	     LHCb::RawEvent* rawEvent);
  long findPattern(LHCb::VeloPixChannelID centrChanID,
                   std::vector<LHCb::VeloPixChannelID> activeChIDs);

private:

  std::string m_clusterLocation;
  std::string m_rawEventLocation;
  unsigned int m_bankVersion;
  unsigned int m_bankSizeInBytes;
  std::vector<const LHCb::VeloPixCluster*> m_sortedClusters; 
  std::vector<SiDAQ::buffer_word> m_rawData;
  std::vector<SiDAQ::buffer_word> m_clusterLiteBuffer;
  std::vector<SiDAQ::buffer_word> m_clusterPattBuffer;
  std::vector<SiDAQ::buffer_word> m_clusterToTsBuffer;
  std::vector<unsigned int> m_sensorNumbers;
  bool m_isDebug; 
  bool m_isVerbose;
  DeVeloPix* m_veloPixelDet;

};
#endif // PREPAREVELOPIXRAWBANK_H
