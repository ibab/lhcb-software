// $Id: HltTrackIPSelection.h,v 1.5 2007-02-05 09:09:08 hernando Exp $
#ifndef HLTTRACKIPSELECTION_H 
#define HLTTRACKIPSELECTION_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

/** @class HltTrackIPSelection HltTrackIPSelection.h
 *
 *  functionality:
 *         selects tracks within a given smallest IP windows respect Vertices
 *         can select the vertex with largest number of tracks in IP window
 *
 *  options:
 *        MinIP,MaxIP, range of the window of Impact Parameter IP
 *        IPAbs, take the abs value of the IP
 *        IPType: please indicate "2DIP" for RZ tracks or "3DIP",for 3D tracks
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
