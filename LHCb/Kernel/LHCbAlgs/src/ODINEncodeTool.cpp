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

  /// Initialize the tool
  virtual inline StatusCode initialize();

  /// Do the conversion
  virtual void execute();

private:
  /// Location in the transient store of the ODIN object.
  std::string m_odinLocation;

  /// Location in the transient store of the RawEvent object.
  std::string m_rawEventLocation;
};

//=============================================================================
// IMPLEMENTATION
//=============================================================================

#include <vector>
#include <memory>
// from LHCb
#include "Event/ODIN.h"
#include "Event/RawBank.h"
#include "Event/RawEvent.h"

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ODINEncodeTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ODINEncodeTool::ODINEncodeTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : ODINCodecBaseTool(type, name, parent) {
  declareProperty("ODINLocation", m_odinLocation = "",
                  "Location of the ODIN object in the transient store. By "
                  "default is the content of LHCb::ODINLocation::Default.");
  declareProperty("RawEventLocation", m_rawEventLocation = "",
                  "Location of the RawEvent object in the transient store. By "
                  "default is the content of LHCb::RawEventLocation::Default.");
}
//=============================================================================
// Destructor
//=============================================================================
ODINEncodeTool::~ODINEncodeTool() {
}
//=============================================================================
// Initialize
//=============================================================================
StatusCode ODINEncodeTool::initialize() {
  StatusCode sc = ODINCodecBaseTool::initialize(); // always first
  if (sc.isFailure()) return sc; // error message already printed

  if (m_odinLocation.empty()) {
    // use the default
    m_odinLocation = LHCb::ODINLocation::Default;
  } else {
    info() << "Using '" << m_odinLocation << "' as location of the ODIN object" << endmsg;
  }

  if (m_rawEventLocation.empty()) {
    // use the default
    m_rawEventLocation = LHCb::RawEventLocation::Default;
  } else {
    info() << "Using '" << m_rawEventLocation << "' as location of the RawEvent object" << endmsg;
  }

  return sc;
}
//=============================================================================
// Main function
//=============================================================================
void ODINEncodeTool::execute() {
  // Check if there is an ODIN object
  LHCb::RawEvent* raw = this->getIfExists<LHCb::RawEvent>(m_rawEventLocation);
  if ( NULL != raw ) {
    // Add the raw bank to the raw event
    debug() << "Getting " << m_rawEventLocation << endmsg;

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
