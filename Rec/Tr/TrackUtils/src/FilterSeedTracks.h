#ifndef FILTERSEEDTRACKS_H 
#define FILTERSEEDTRACKS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class FilterSeedTracks FilterSeedTracks.h
 *  Filter the remaining Seed tracks.
 *
 *  @author Olivier Callot
 *  @date   2010-06-17
 */
class FilterSeedTracks : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FilterSeedTracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FilterSeedTracks( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  bool m_filter;
};
#endif // FILTERSEEDTRACKS_H
