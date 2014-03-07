// $Id: $
#ifndef OTYALIGNMAGOFF_H 
#define OTYALIGNMAGOFF_H 1

// Include files
#include "TrackMonitorBase.h"

class DeOTDetector;
class DeOTModule;

namespace LHCb{
  class State;
}

/** @class OTYAlignMagOff OTYAlignMagOff.h
 *  ...
 *
 *  @author Frederic Guillaume Dupertuis
 *  @date   2010-05-31
 */
class OTYAlignMagOff : public TrackMonitorBase {
public: 
  /// Standard constructor
  OTYAlignMagOff( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OTYAlignMagOff( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  bool inModule(const LHCb::State& state, const DeOTModule* module) const;
  
protected:

private:
  DeOTDetector* m_tracker;
  double m_xTol;
  double m_yTol;
  bool m_global;
  bool m_expected;
  bool m_printmoduleinfo;
};
#endif // OTYALIGNMAGOFF_H
