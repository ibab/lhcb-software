// $Id: $
#ifndef STCOORDINATES_H 
#define STCOORDINATES_H 1

// Include files
#include "TrackMonitorBase.h"

class DeSTDetector;

/** @class STCoordinates STCoordinates.h
 *  ...
 *
 *  @author Frederic Guillaume Dupertuis
 *  @date   2010-05-31
 */
class STCoordinates : public TrackMonitorBase {
public: 
  /// Standard constructor
  STCoordinates( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~STCoordinates( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_detType;
  DeSTDetector* m_tracker;
  bool m_printsectorinfo;
  std::string m_alignmenttag;
};
#endif // STCOORDINATES_H
