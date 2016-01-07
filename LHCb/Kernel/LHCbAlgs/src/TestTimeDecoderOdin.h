// $Id: TestTimeDecoderOdin.h,v 1.1 2009-02-03 18:31:03 marcocle Exp $
#ifndef TESTTIMEDECODERODIN_H
#define TESTTIMEDECODERODIN_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IEventTimeDecoder.h"            // Interface

// forward declarations
class IDataProviderSvc;
class IIncidentSvc;
namespace LHCb
{
  class ODIN;
}

namespace LHCbAlgsTests {
/** @class TestTimeDecoderOdin TestTimeDecoderOdin.h
 *
 *  Fake EventTimeDecoder that generates an ODIN object with incremented run
 *  number at each call of getTime().
 *
 *  Used in the test "lhcbalgs.runchangehandler".
 *
 *  @author Marco Clemencic
 *  @date   2006-09-21
 */
class TestTimeDecoderOdin : public GaudiTool, virtual public IEventTimeDecoder {
public:
  /// Standard constructor
  TestTimeDecoderOdin( const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

  virtual ~TestTimeDecoderOdin(); ///< Destructor

  // --- implementation of IEventTimeDecoder ---
  /// Get the time of the current event from the ODIN object.
  /// @return The time of current event.
  Gaudi::Time getTime() const;

private:

  // --- local data ---
  /// Used to remember the run number and spot a change of run number.
  mutable unsigned int m_currentRun;

  /// How much to increase the run number at every call (default 1).
  bool m_runNumberStep;
};
}
#endif // ODINTIMEDECODER_H
