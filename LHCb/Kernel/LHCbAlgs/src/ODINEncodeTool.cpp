// $Id: ODINEncodeTool.cpp,v 1.1 2009-02-03 18:31:03 marcocle Exp $
// Include files
#include "ODINCodecBaseTool.h"

/** @class ODINEncodeTool ODINEncodeTool.h
 *
 *  Tool to encode the ODIN object into a RawBank. The raw bank is then added to
 *  the RawEvent, which must exist.
 *
 *  @see ODINCodecBaseTool for the properties.
 *
 *  @author Marco Clemencic
 *  @date   2009-02-02
 */
class ODINEncodeTool : public ODINCodecBaseTool {
public:
  /// Standard constructor
  ODINEncodeTool(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

  virtual ~ODINEncodeTool(); ///< Destructor

  /// Do the conversion
  virtual void execute();
};

//=============================================================================
// IMPLEMENTATION
//=============================================================================

#include <vector>
#include <memory>
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// from LHCb
#include "Event/ODIN.h"
#include "Event/RawBank.h"
#include "Event/RawEvent.h"

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ODINEncodeTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ODINEncodeTool::ODINEncodeTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : ODINCodecBaseTool(type, name, parent) {
}
//=============================================================================
// Destructor
//=============================================================================
ODINEncodeTool::~ODINEncodeTool() {
}
//=============================================================================
// Main function
//=============================================================================
void ODINEncodeTool::execute() {
  // Check if there is an ODIN object
  if (exist<LHCb::ODIN>(m_odinLocation)) {
    // Add the raw bank to the raw event
    debug() << "Getting " << m_rawEventLocation << endmsg;
    LHCb::RawEvent* raw = this->get<LHCb::RawEvent>(m_rawEventLocation);

    // pointer for a pre-existing bank
    LHCb::RawBank *old_bank = 0;
    // Check if have an ODIN bank already
    const std::vector<LHCb::RawBank*>& odinBanks = raw->banks(LHCb::RawBank::ODIN);
    if (!odinBanks.empty() ) {
      if (m_force) {
        // we have to replace it... remember which it is, so we can do it if the
        // encoding is successful, just before adding the new bank
        old_bank = *odinBanks.begin();
      } else {
        // keep the existing bank (the new one is automatically disposed)
        return;
      }
    }
    // Encode ODIN object
    LHCb::RawBank *bank = i_encode(get<LHCb::ODIN>(m_odinLocation));
    if (bank) {
      if (old_bank) raw->removeBank(old_bank);
      // add the bank
      raw->adoptBank(bank, true);
    }
  } else {
    Warning("Cannot find ODIN object to encode").ignore();
  }
}

//=============================================================================
