// $Id: HltTrackMerge.h,v 1.1 2007-06-20 20:34:05 hernando Exp $
#ifndef HLTTRACKMERGE_H 
#define HLTTRACKMERGE_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"

/** @class HltTrackMerge HltTrackMerge.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-03-22
 */
class HltTrackMerge : public HltAlgorithm {
public: 
  /// Standard constructor
  HltTrackMerge( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltTrackMerge( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  template <class CONT>
  void copy(const CONT& c1, CONT& c2)
  {std::copy(c1.begin(),c1.end(),std::back_inserter(c2));}

protected:


};
#endif // HLTTRACKFILTER_H
