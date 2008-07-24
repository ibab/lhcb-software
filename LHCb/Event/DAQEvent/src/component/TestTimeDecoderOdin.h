// $Id: TestTimeDecoderOdin.h,v 1.1 2008-07-24 19:22:09 marcocle Exp $
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

namespace DAQEventTests {
/** @class TestTimeDecoderOdin TestTimeDecoderOdin.h
 *  
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
  
};
}
#endif // ODINTIMEDECODER_H
