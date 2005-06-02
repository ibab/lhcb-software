// $Id: UnpackTrStoredTrack.h,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
#ifndef UNPACKTRSTOREDTRACK_H 
#define UNPACKTRSTOREDTRACK_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class UnpackTrStoredTrack UnpackTrStoredTrack.h
 *  Unpack the TrStoredTrack
 *
 *  @author Olivier Callot
 *  @date   2005-03-17
 */
class UnpackTrStoredTrack : public GaudiAlgorithm {
public: 
  /// Standard constructor
  UnpackTrStoredTrack( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackTrStoredTrack( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // UNPACKTRSTOREDTRACK_H
