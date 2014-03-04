// $Id: $
#ifndef OTCOORDINATES_H 
#define OTCOORDINATES_H 1

// Include files
#include "TrackMonitorBase.h"

class DeOTDetector;

/** @class OTCoordinates OTCoordinates.h
 *  ...
 *
 *  @author Frederic Guillaume Dupertuis
 *  @date   2010-05-31
 */
class OTCoordinates : public TrackMonitorBase {
public: 
  /// Standard constructor
  OTCoordinates( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OTCoordinates( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  DeOTDetector* m_tracker;
  bool m_printmoduleinfo;
  std::string m_alignmenttag;
};
#endif // OTCOORDINATES_H
