// $Id: HltTrackIPSelection.h,v 1.3 2006-09-22 11:36:11 hernando Exp $
#ifndef HLTTRACKIPSELECTION_H 
#define HLTTRACKIPSELECTION_H 1

// Include files
// from Gaudi
#include "HltCommon/HltAlgorithm.h"
#include "HltCommon/HltFunctions.h"

/** @class HltTrackIPSelection HltTrackIPSelection.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2005-11-17
 */

class HltTrackIPSelection : public HltAlgorithm {
public: 
  /// Standard constructor
  HltTrackIPSelection( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltTrackIPSelection( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void computeTracksMinIP();
  
  void selectVertex();

  void debugOutput(const Hlt::TrackContainer& tracks) const;

protected:

  std::string m_iptype;
  double m_ipmin;
  double m_ipmax;
  bool m_ipabs;
  bool m_selectPrimaryVertex;

protected:  
  
  int m_IPKey;
  int m_PVKey;

  Hlt::track_filter* _nobackwards;
  Hlt::trackvertex_bifunction* _ipFun;
  Hlt::track_filter* _ipWindow;
  Hlt::track_filter* _kvertex;


protected:

  HltHisto m_histoIP;

protected:

  Hlt::TrackContainer m_otracks;
  Hlt::TrackContainer m_otracks2;
  std::vector<int> m_kver;

};
#endif // TRGTRACKIPSELECTION_H
