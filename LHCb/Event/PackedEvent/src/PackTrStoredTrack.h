// $Id: PackTrStoredTrack.h,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
#ifndef PACKTRSTOREDTRACK_H 
#define PACKTRSTOREDTRACK_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class PackTrStoredTrack PackTrStoredTrack.h
 *  Pack the TrStoredTrack
 *
 *  @author Olivier Callot
 *  @date   2005-03-16
 */
class PackTrStoredTrack : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PackTrStoredTrack( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackTrStoredTrack( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
  bool m_storeMeasurements;
};
#endif // PACKTRSTOREDTRACK_H
