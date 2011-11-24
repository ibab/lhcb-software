// Include files
#include "ODINCodecBaseTool.h"
#include "GaudiKernel/SerializeSTL.h"

/** @class ODINDecodeTool ODINDecodeTool.h
 *
 *  Tool to decode the ODIN raw bank, fill a ODIN object with its data and
 *  register it to the transient store.
 *
 *  @see ODINCodecBaseTool for the properties.
 *
 *  @author Marco Clemencic
 *  @date   2009-02-02
 */
class ODINDecodeTool : public ODINCodecBaseTool {
public:
  /// Standard constructor
  ODINDecodeTool(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

  virtual ~ODINDecodeTool(); ///< Destructor

  /// Initialize the tool
  virtual inline StatusCode initialize();

  /// Do the conversion
  virtual void execute();

private:
  /// Location in the transient store of the ODIN object.
  std::string m_odinLocation;

  /// Location in the transient store of the RawEvent object.
  /// @warning Obsolete: use m_rawEventLocations
  std::string m_rawEventLocation;

  /// List of locations in the transient store to search the RawEvent object.
  std::vector<std::string> m_rawEventLocations;
};

//=============================================================================
// IMPLEMENTATION
//=============================================================================

#include <sstream>
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// from LHCb
#include "Event/ODIN.h"
#include "Event/RawBank.h"
#include "Event/RawEvent.h"

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ODINDecodeTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ODINDecodeTool::ODINDecodeTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : ODINCodecBaseTool(type, name, parent) {
  declareProperty("ODINLocation", m_odinLocation = "",
                  "Location of the ODIN object in the transient store. By "
                  "default is the content of LHCb::ODINLocation::Default.");
  declareProperty("RawEventLocation", m_rawEventLocation = "",
                  "Location of the RawEvent object in the transient store. By "
                  "default is the content of LHCb::RawEventLocation::Default.");
  declareProperty("RawEventLocations", m_rawEventLocations,
                  "List of possible locations of the RawEvent object in the"
                  " transient store. By default it is LHCb::RawEventLocation::Copied,"
                  " LHCb::RawEventLocation::Default.");
}
//=============================================================================
// Destructor
//=============================================================================
ODINDecodeTool::~ODINDecodeTool() {
}
//=============================================================================
// Initialize
//=============================================================================
StatusCode ODINDecodeTool::initialize() {
  StatusCode sc = ODINCodecBaseTool::initialize(); // always first
  if (sc.isFailure()) return sc; // error message already printed

  if (m_odinLocation.empty()) {
    // use the default
    m_odinLocation = LHCb::ODINLocation::Default;
  } else {
    info() << "Using '" << m_odinLocation << "' as location of the ODIN object" << endmsg;
  }

  bool usingDefaultLocation = m_rawEventLocations.empty() && m_rawEventLocation.empty();
  if (! m_rawEventLocation.empty()) {
    warning() << "The RawEventLocation property is obsolete, use RawEventLocations instead" << endmsg;
    m_rawEventLocations.insert(m_rawEventLocations.begin(), m_rawEventLocation);
  }

  if (std::find(m_rawEventLocations.begin(), m_rawEventLocations.end(), LHCb::RawEventLocation::Default)
      == m_rawEventLocations.end()) {
    // append the defaults to the search path
    m_rawEventLocations.push_back(LHCb::RawEventLocation::Trigger);
    m_rawEventLocations.push_back(LHCb::RawEventLocation::Default);
  }

  if (!usingDefaultLocation) {
    info() << "Using '" << m_rawEventLocations << "' as search path for the RawEvent object" << endmsg;
  }
  return sc;
}
//=============================================================================
// Main function
//=============================================================================
void ODINDecodeTool::execute() {

  LHCb::ODIN* odin = 0;

  // Check if there is already an ODIN object
  if (exist<LHCb::ODIN>(m_odinLocation)) {
    if (m_force) {
      // Use the already registered object
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << "Modify existing ODIN object" << endmsg;
      odin = get<LHCb::ODIN>(m_odinLocation);
    } else {
      // ODIN already there and we are not supposed to touch it
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << "Keep existing ODIN object" << endmsg;
      return;
    }
  }

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "Getting RawEvent" << endmsg;
  LHCb::RawEvent* rawEvent = NULL;
  for (std::vector<std::string>::const_iterator p = m_rawEventLocations.begin(); p != m_rawEventLocations.end(); ++p) {
    if (exist<LHCb::RawEvent>(*p)){
      rawEvent = get<LHCb::RawEvent>(*p);
      break;
    }
  }
  if (!rawEvent) {
    using namespace GaudiUtils;
    // Throw a meaningful exception it the bank is not found;
    std::ostringstream out;
    out << "Cannot find RawEvent in " << m_rawEventLocations;
    Exception(out.str(), StatusCode(StatusCode::FAILURE, true));
  }

  // Check if have an ODIN bank...
  const std::vector<LHCb::RawBank*>& odinBanks = rawEvent->banks(LHCb::RawBank::ODIN);
  if(odinBanks.size()){ // ... good, we can decode it
    odin = this->i_decode(*odinBanks.begin(), odin);
    if (odin && (!odin->registry())){ // register ODIN object if valid and not yet registered
      put(odin, m_odinLocation);
    }
  }
  else {
    Warning("Cannot find ODIN bank in RawEvent");
  }
}

//=============================================================================
