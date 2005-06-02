// $Id: CompareTrStoredTrack.h,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
#ifndef COMPARETRSTOREDTRACK_H 
#define COMPARETRSTOREDTRACK_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class CompareTrStoredTrack CompareTrStoredTrack.h
 *  Compare two versions of the TrStoredTracks
 *
 *  @author Olivier Callot
 *  @date   2005-02-07
 */
class CompareTrStoredTrack : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CompareTrStoredTrack( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CompareTrStoredTrack( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_inputName;
  std::string m_testName;
};
#endif // COMPARETRSTOREDTRACK_H
