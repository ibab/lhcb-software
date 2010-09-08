// $Id: HltTrackMatch.h,v 1.9 2010-09-08 12:55:25 graven Exp $
#ifndef HLTTRACKING_HLTTRACKMATCH_H 
#define HLTTRACKING_HLTTRACKMATCH_H 1

// Include files
// from Gaudi
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"
#include "TrackInterfaces/ITrackMatch.h"
#include "Event/Track.h"


/** @class HltTrackMatch HltTrackMatch.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-12-07
 */
class HltTrackMatch : public HltAlgorithm {
public: 
  /// Standard constructor
  HltTrackMatch( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltTrackMatch( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  // 1 output: track
  // 2 inputs: track, track
  // hence: container3<track,track,track> (output, first input, second input)
  Hlt::SelectionContainer3<LHCb::Track,LHCb::Track,LHCb::Track> m_selections;

  ITrackMatch* m_tool;
  AIDA::IHistogram1D* m_qualityHisto;
  AIDA::IHistogram1D* m_quality2Histo;
  AIDA::IHistogram1D* m_qualityHistoBest;
  AIDA::IHistogram1D* m_quality2HistoBest;

  std::string m_TESOutput;
  std::string m_toolName;
  std::string m_qualityName;
  std::string m_quality2Name;
  
  double m_maxQuality;
  double m_maxQuality2;

  int m_qualityID;
  int m_quality2ID;
  int m_recoID;
  int m_trackType;

  bool m_transferInfo;
  bool m_transferIDs;
  bool m_transferAncestor;

};
#endif // HLTTRACKMATCH_H
