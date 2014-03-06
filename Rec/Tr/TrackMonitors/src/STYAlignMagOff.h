// $Id: $
#ifndef STYSECTEFF_H 
#define STYSECTEFF_H 1

// Include files
#include "TrackMonitorBase.h"

class DeSTDetector;
class DeSTSector;
class DeITDetector;

/** @class STYAlignMagOff STYAlignMagOff.h
 *  ...
 *
 *  @author Frederic Guillaume Dupertuis
 *  @date   2010-05-31
 */
class STYAlignMagOff : public TrackMonitorBase {
public: 
  /// Standard constructor
  STYAlignMagOff( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~STYAlignMagOff( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  bool inSector( const LHCb::State& state, 
                 const DeSTSector* sector) const;

  std::string m_detType;
  DeSTDetector* m_tracker;
  double m_xTol;
  double m_yTol;
  bool m_global;
  bool m_xy;
  bool m_expected;
  bool m_printsectorinfo;
};
#endif // STYSECTEFF_H
