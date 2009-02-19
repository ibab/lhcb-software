#include "ODINCodecBaseTool.h"

//=============================================================================
// IMPLEMENTATION of ODINCodecBaseTool
//=============================================================================

#include <sstream>
#include <memory>
// from LHCb
#include "Event/ODIN.h"
#include "Event/RawBank.h"
#include "Event/RawEvent.h"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ODINCodecBaseTool::ODINCodecBaseTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IGenericTool>(this);

  declareProperty("Force", m_force = false,
                  "If already present, override the destination object.");
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
ODINCodecBaseTool::~ODINCodecBaseTool() {
}
//=============================================================================
// Destructor
//=============================================================================
StatusCode ODINCodecBaseTool::initialize() {
  StatusCode sc = GaudiTool::initialize(); // always first
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

  return sc; // SUCCESS
}
//=============================================================================
// ODIN Bank constants
//=============================================================================
namespace {
  const unsigned int bank_version = 5;
  const size_t bank_size    = 10 * sizeof(int);
  const size_t bank_size_v2 =  9 * sizeof(int);
}
//=============================================================================
// Decode
//=============================================================================
LHCb::ODIN* ODINCodecBaseTool::i_decode(const LHCb::RawBank* bank, LHCb::ODIN* odin) {
  unsigned long long temp64;
  unsigned int temp32;

  // Check the passed pointers
  Assert((bank!=NULL), "Called without a RawBank object (pointer NULL)");

  // ensure that the new object is deleted in case of failure
  std::auto_ptr<LHCb::ODIN> ptr;
  if (!odin) {
    odin = new LHCb::ODIN();
    ptr.reset(odin);
  }

  // Check bank type
  Assert(bank->type() == LHCb::RawBank::ODIN, "Wrong raw bank type");

  // Validate bank version
  const unsigned int version = bank->version();
  if (version > bank_version) {
    std::ostringstream msg;
    msg << "Unknown ODIN bank version " << version << ", latest known is " << bank_version;
    //Exception(msg.str()); // throw
    Error(msg.str()).ignore();
    return 0;
  }
  // Validate bank size
  const size_t size = bank->size();
  if ( ((version >= 3) && size != bank_size) ||
       ((version < 3) && size < bank_size_v2) ) { // relaxed check to take into account a bug in BooleInit
    std::ostringstream msg;
    msg << "Wrong ODIN bank size " << size << ", expected ";
    if (version >= 3) { msg << bank_size; }
    else { msg << bank_size_v2; }
    //Exception(msg.str()); // throw
    Error(msg.str()).ignore();
    return 0;
  }

  // pointer to the data
  const unsigned int* odinData = bank->data();

  // Fill the ODIN object
  odin->setVersion(version);
  odin->setRunNumber(odinData[LHCb::ODIN::RunNumber]);
  odin->setOrbitNumber(odinData[LHCb::ODIN::OrbitNumber]);

  temp64 = odinData[LHCb::ODIN::L0EventIDHi];
  odin->setEventNumber((temp64 << 32) + odinData[LHCb::ODIN::L0EventIDLo]);

  temp64 = odinData[LHCb::ODIN::GPSTimeHi];
  odin->setGpsTime ((temp64 << 32) + odinData[LHCb::ODIN::GPSTimeLo]);

  temp32 = odinData[LHCb::ODIN::Word7];
  odin->setDetectorStatus( (temp32 & LHCb::ODIN::DetectorStatusMask) >> LHCb::ODIN::DetectorStatusBits );
  odin->setErrorBits( (temp32 & LHCb::ODIN::ErrorMask) >> LHCb::ODIN::ErrorBits );

  temp32 = odinData[LHCb::ODIN::Word8];
  odin->setBunchId( (temp32 & LHCb::ODIN::BunchIDMask) >> LHCb::ODIN::BunchIDBits );
  odin->setTriggerType( (temp32 & LHCb::ODIN::TriggerTypeMask) >> LHCb::ODIN::TriggerTypeBits );

  if (version >= 5) {
    switch ( (temp32 & LHCb::ODIN::CalibrationTypeMask) >> LHCb::ODIN::CalibrationTypeBits ) {
      case 0  : odin->setCalibrationType(LHCb::ODIN::A); break;
      case 1  : odin->setCalibrationType(LHCb::ODIN::B); break;
      case 2  : odin->setCalibrationType(LHCb::ODIN::C); break;
      case 3  : odin->setCalibrationType(LHCb::ODIN::D); break;
      default : odin->setCalibrationType(LHCb::ODIN::A); break;
    }
  } else {
    odin->setReadoutType( ((temp32 & LHCb::ODIN::CalibrationTypeMask) >> LHCb::ODIN::CalibrationTypeBits ) == 0 ?
        LHCb::ODIN::ZeroSuppressed : LHCb::ODIN::NonZeroSuppressed );
  }

  if (version >= 4) {
    odin->setTimeAlignmentEventWindow( (temp32 & LHCb::ODIN::TAEWindowMask) >> LHCb::ODIN::TAEWindowBits );
  }

  odin->setForceBit( ((temp32 & LHCb::ODIN::ForceMask) >> LHCb::ODIN::ForceBits ) != 0 );

  switch ( (temp32 & LHCb::ODIN::BXTypeMask) >> LHCb::ODIN::BXTypeBits ) {
    case 0  : odin->setBunchCrossingType(LHCb::ODIN::NoBeam); break;
    case 1  : odin->setBunchCrossingType(LHCb::ODIN::SingleBeamLeft); break;
    case 2  : odin->setBunchCrossingType(LHCb::ODIN::SingleBeamRight); break;
    case 3  : odin->setBunchCrossingType(LHCb::ODIN::BeamCrossing); break;
    default : odin->setBunchCrossingType(LHCb::ODIN::NoBeam); break;
  }

  odin->setBunchCurrent( (temp32 & LHCb::ODIN::BunchCurrentMask) >> LHCb::ODIN::BunchCurrentBits );

  if (version < 3) {

    odin->setEventType(odinData[LHCb::ODIN::EventType]);

  } else {

    temp32 = odinData[LHCb::ODIN::EventType];
    odin->setEventType((temp32 & LHCb::ODIN::EventTypeMask) >> LHCb::ODIN::EventTypeBits);
    odin->setCalibrationStep((temp32 & LHCb::ODIN::CalibrationStepMask) >> LHCb::ODIN::CalibrationStepBits);

    odin->setTriggerConfigurationKey(odinData[LHCb::ODIN::TriggerConfigurationKey]);

  }

  ptr.release(); // pass the ownership to the caller
  return odin;
}
//=============================================================================
// Encode
//=============================================================================
LHCb::RawBank* ODINCodecBaseTool::i_encode(const LHCb::ODIN *odin) {
  // Check the passed pointer
  Assert((odin!=NULL), "Called without an ODIN object (pointer NULL)");

  const unsigned int version = odin->version();
  if (version && (version != bank_version)) {
    std::ostringstream msg;
    msg << "Trying to convert ODIN object of version " << version
        << " to bank version " << bank_version;
    Warning(msg.str()).ignore();
  }
  // Create the new bank
  // Note that we cannot delete it, so better that there is no failure after
  // this line.
  LHCb::RawBank* bank = LHCb::RawEvent::createBank(0,LHCb::RawBank::ODIN, bank_version, bank_size);
  unsigned int *data = bank->data();

  data[LHCb::ODIN::RunNumber] = odin->runNumber();

  data[LHCb::ODIN::EventType] = ((odin->eventType() << LHCb::ODIN::EventTypeBits) & LHCb::ODIN::EventTypeMask) |
                                ((odin->calibrationStep() << LHCb::ODIN::CalibrationStepBits) & LHCb::ODIN::CalibrationStepMask);

  data[LHCb::ODIN::OrbitNumber] = odin->orbitNumber();
  data[LHCb::ODIN::L0EventIDHi] = (unsigned int) ((odin->eventNumber() >> 32) & 0xFFFFFFFF );
  data[LHCb::ODIN::L0EventIDLo] = (unsigned int) ((odin->eventNumber()) & 0xFFFFFFFF );
  data[LHCb::ODIN::GPSTimeHi]   = (unsigned int) ((odin->gpsTime() >> 32) & 0xFFFFFFFF );
  data[LHCb::ODIN::GPSTimeLo]   = (unsigned int) ((odin->gpsTime()) & 0xFFFFFFFF );

  data[LHCb::ODIN::Word7] = (unsigned int) ( ((odin->detectorStatus() << LHCb::ODIN::DetectorStatusBits) & LHCb::ODIN::DetectorStatusMask) |
                                             ((odin->errorBits() << LHCb::ODIN::ErrorBits) & LHCb::ODIN::ErrorMask) );

  data[LHCb::ODIN::Word8] = ((odin->bunchId() << LHCb::ODIN::BunchIDBits) & LHCb::ODIN::BunchIDMask) |
                            ((odin->timeAlignmentEventWindow() << LHCb::ODIN::TAEWindowBits) & LHCb::ODIN::TAEWindowMask) |
                            ((odin->triggerType() << LHCb::ODIN::TriggerTypeBits) & LHCb::ODIN::TriggerTypeMask) |
                            ((odin->calibrationType() << LHCb::ODIN::CalibrationTypeBits) & LHCb::ODIN::CalibrationTypeMask) |
                            ((odin->forceBit() << LHCb::ODIN::ForceBits) & LHCb::ODIN::ForceMask) |
                            ((odin->bunchCrossingType() << LHCb::ODIN::BXTypeBits) & LHCb::ODIN::BXTypeMask) |
                            ((odin->bunchCurrent() << LHCb::ODIN::BunchCurrentBits) & LHCb::ODIN::BunchCurrentMask);

  data[LHCb::ODIN::TriggerConfigurationKey] = odin->triggerConfigurationKey();

  return bank; // pass the ownership to the caller
}
//=============================================================================
