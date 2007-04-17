// $Id: OdinTimeDecoder.h,v 1.2 2007-04-17 15:39:59 cattanem Exp $
#ifndef ODINTIMEDECODER_H 
#define ODINTIMEDECODER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IEventTimeDecoder.h"            // Interface

// forward declarations
class IDataProviderSvc;
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


  /// Initialization
  //virtual StatusCode initialize();

  /// Finalization
  //virtual StatusCode finalize();  

  // --- implementation of IEventTimeDecoder ---

  /// Return the time of current event.
  Gaudi::Time getTime() const;

private:
  // --- local methods ---
  /// Get the ODIN object from the T.E.S. or create it.
  /// @return Pointer to the ODIN object or NULL if it cannot be found.
  LHCb::ODIN *getODIN() const;

  // --- properties ---
  bool m_forceRawEvent; ///< Flag to force unconditional decoding from RawEvent
  
};
#endif // ODINTIMEDECODER_H
