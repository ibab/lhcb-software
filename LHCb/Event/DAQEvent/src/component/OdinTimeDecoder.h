// $Id: OdinTimeDecoder.h,v 1.5 2008-07-24 19:18:09 marcocle Exp $
#ifndef ODINTIMEDECODER_H 
#define ODINTIMEDECODER_H 1

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
  
  // --- implementation of IEventTimeDecoder ---
  /// Get the time of the current event from the ODIN object.
  /// @return The time of current event.
  Gaudi::Time getTime() const;

private:
  // --- local methods ---
  /// Get the ODIN object from the T.E.S. or create it.
  /// @return Pointer to the ODIN object or NULL if it cannot be found.
  LHCb::ODIN *getODIN() const;

  /// Create ODIN object from ODIN RawBank.
  /// @return Pointer to the ODIN object or NULL if bank cannot be found.
  LHCb::ODIN *decodeODIN() const;

  // --- local data ---
  /// Used to remember the run number and spot a change of run number.
  mutable unsigned int m_currentRun;
  
  // --- properties ---
  bool m_forceRawEvent; ///< Flag to force unconditional decoding from RawEvent
  
};
#endif // ODINTIMEDECODER_H
