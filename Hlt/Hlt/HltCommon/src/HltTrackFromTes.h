// $Id: HltTrackFromTes.h,v 1.2 2007-03-26 16:37:18 cattanem Exp $
#ifndef HLTTRACKFROMTES_H 
#define HLTTRACKFROMTES_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "Event/HltNames.h"

#include "Event/L0DUReport.h"

/** @class HltTrackFromTes HltTrackFromTes.h
 *  
 *  functionality (TODO):
 *         select a track from a list of track container
 *         plots efficiency of a list of selection summary and L0 
 *    as a function of Pt of the selected track.
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-09-01
 */
class HltTrackFromTes : public HltAlgorithm {
public: 
  /// Standard constructor
  HltTrackFromTes( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltTrackFromTes( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  StringProperty m_tracksLocation;
  StringArrayProperty m_types;
  
  bool m_orderByPt;
  
  LHCb::Tracks* m_tracks;
};
#endif // HLTTRACKFROMTES_H
