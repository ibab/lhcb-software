// $Id: HltMonitorEffvsPt.h,v 1.2 2007-03-26 16:37:18 cattanem Exp $
#ifndef HLTMONITOREFFVSPT_H 
#define HLTMONITOREFFVSPT_H 1

// Include files
// from Gaudi
#include "HltBase/HltMonitorAlgorithm.h"
#include "Event/HltNames.h"

#include "Event/L0DUReport.h"

/** @class HltMonitorEffvsPt HltMonitorEffvsPt.h
 *  
 *  functionality (TODO):
 *         select a track from a list of track container
 *         plots efficiency of a list of selection summary and L0 
 *    as a function of Pt of the selected track.
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-09-01
 */
class HltMonitorEffvsPt : public HltMonitorAlgorithm {
public: 
  /// Standard constructor
  HltMonitorEffvsPt( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltMonitorEffvsPt( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
  virtual double referencePt();

protected:

  StringProperty m_l0Location;
  StringProperty m_tracksLocation;
  StringArrayProperty m_l0ChannelsName;

  float m_maxPt;
  int m_nBins;

  std::vector<int> m_l0IDs;
  std::vector<int> m_hltSelIDs;

  std::vector<HltCounter> m_counterL0Channel;
  std::vector<HltCounter> m_counterHltSel;

  HltHisto m_histoPt;
  HltHisto m_histoPtL0;

  std::vector<HltHisto> m_histoL0Channel;

  std::vector<HltHisto> m_histoHltSel;
  std::vector<HltHisto> m_histoHltSel1;

  LHCb::L0DUReport* m_l0;
  LHCb::Tracks* m_tracks;

};
#endif // HLTMONITOREFFVSPT_H
