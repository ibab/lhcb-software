// $Id: HltTrackFromTes.h,v 1.3 2007-11-20 10:26:27 graven Exp $
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
  
};
#endif // HLTTRACKFROMTES_H
