#ifndef FILTERMATCHTRACKS_H 
#define FILTERMATCHTRACKS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class FilterMatchTracks FilterMatchTracks.h
 *  Fileter Match tracks identical to Forward tracks
 *
 *  @author Olivier Callot
 *  @date   2010-06-14
 */
class FilterMatchTracks : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FilterMatchTracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FilterMatchTracks( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  bool m_filter;
};
#endif // FILTERMATCHTRACKS_H
