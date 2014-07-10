// $Id: OdinTimeDecoder.h,v 1.1 2009-02-03 18:31:03 marcocle Exp $
#ifndef ODINTIMEDECODER_H
#define ODINTIMEDECODER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiAlg/IGenericTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IEventTimeDecoder.h"            // Interface

// forward declarations
class IDataProviderSvc;
class IIncidentSvc;
namespace LHCb
{
  class ODIN;
}

/** @class OdinTimeDecoder OdinTimeDecoder.h
 *
 *
 *  @author Marco Clemencic
 *  @date   2006-09-21
 */
class OdinTimeDecoder : public GaudiTool, virtual public IEventTimeDecoder {
public:
  /// Standard constructor
  OdinTimeDecoder( const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

  virtual ~OdinTimeDecoder( ); ///< Destructor

  /// Initialize the tool
  StatusCode initialize();

  // --- implementation of IEventTimeDecoder ---
  /// Get the time of the current event from the ODIN object.
  /// @return The time of current event.
  Gaudi::Time getTime() const;

private:
  // --- local methods ---
  /// Get the ODIN object from the T.E.S. or create it.
  /// @return Pointer to the ODIN object or NULL if it cannot be found.
  LHCb::ODIN *getODIN() const;

  /// Tool to decode the ODIN bank
  ToolHandle<IGenericTool> m_odinDecoder;

  // --- local data ---
  /// Used to remember the run number and spot a change of run number.
  mutable unsigned int m_currentRun;
  /// Used to detect if we switch from flagging to filtering mode (which implies
  /// a "run change" without actually changing the run number).
  mutable bool m_flaggingMode;

};
#endif // ODINTIMEDECODER_H
