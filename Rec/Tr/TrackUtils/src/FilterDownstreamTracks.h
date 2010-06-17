// $Id: $
#ifndef FILTERDOWNSTREAMTRACKS_H 
#define FILTERDOWNSTREAMTRACKS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class FilterDownstreamTracks FilterDownstreamTracks.h
 *  Filter Downstream tracks that share the T station part with a Forward
 *
 *  @author Olivier Callot
 *  @date   2010-06-15
 */
class FilterDownstreamTracks : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FilterDownstreamTracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FilterDownstreamTracks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  bool m_filter;
};
#endif // FILTERDOWNSTREAMTRACKS_H
