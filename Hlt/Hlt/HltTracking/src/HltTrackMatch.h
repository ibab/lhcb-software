// $Id: HltTrackMatch.h,v 1.3 2008-06-27 16:34:08 hernando Exp $
#ifndef HLTTRACKING_HLTTRACKMATCH_H 
#define HLTTRACKING_HLTTRACKMATCH_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "TrackInterfaces/ITrackMatch.h"
#include "Event/HltEnums.h"
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
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void recoConfiguration();

  StatusCode setReco(const std::string& recoName);
  

protected:

  // they are in HltAlgorithm
  // Hlt::TrackSelection* m_inputTracks;
  // Hlt::TrackSelection* m_inputTracks2;
  // Hlt::TrackSelection* m_outputTracks;

  std::string m_matchName;
  ITrackMatch* m_tool;

  std::string m_TESOutput;
  
  double m_maxQuality;
  double m_maxQuality2;

protected:
  
  template <class T>
  void recoregister(const std::string& name, const std::string& prop,
                    const T& t) {
    std::string key = name+"/"+prop;m_recoConf.add(key,t);
    info() << " reco ["<<key<<"] = " << t <<endreq;
  }

  zen::dictionary m_recoConf;

protected:

  std::string m_qualityName;
  std::string m_quality2Name;
  
  int m_qualityID;
  int m_quality2ID;

  int m_recoID;
  bool m_owner;
  int m_trackType;

  bool m_transferInfo;
  bool m_transferIDs;
  bool m_transferAncestor;

};
#endif // HLTTRACKMATCH_H
