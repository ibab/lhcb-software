#include "ODINCodecBaseTool.h"

//=============================================================================
// IMPLEMENTATION of ODINCodecBaseTool
//=============================================================================

#include <sstream>
#include <memory>
#include <algorithm>
// from LHCb
#include "Event/ODIN.h"
#include "Event/RawBank.h"
#include "Event/RawEvent.h"
#include "DAQKernel/DecoderToolBase.h"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ODINCodecBaseTool::ODINCodecBaseTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : Decoder::ToolBase ( type, name , parent )
{
  declareInterface<IGenericTool>(this);

  declareProperty("Force", m_force = false,
                  "If already present, override the destination object.");
  declareProperty("IgnoreUnknownBankVersion", m_ignoreBankVersion = false,
                  "Do not stop in case of unknown bank version number, assuming"
                  " it is binary compatible with the latest known version.");
}
//=============================================================================
// Destructor
//=============================================================================
ODINCodecBaseTool::~ODINCodecBaseTool() {
}
//=============================================================================
// ODIN Bank constants
//=============================================================================
namespace {
  const unsigned int bank_version = 6;
  const size_t bank_size    = 10 * sizeof(int);
  const size_t bank_size_v2 =  9 * sizeof(int);

  namespace PreV6 {
    enum TriggerType {
        Reserve            = 0,
        PhysicsTrigger     = 1,
        AuxilliaryTrigger  = 2,
        RandomTrigger      = 3,
        PeriodicTrigger    = 4,
        NonZSupTrigger     = 5,
        TimingTrigger      = 6,
        CalibrationTrigger = 7
    };
  }
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
  std::unique_ptr<LHCb::ODIN> ptr;
  if (!odin) {
    odin = new LHCb::ODIN();
    ptr.reset(odin);
  }

  // Check bank type
  Assert(bank->type() == LHCb::RawBank::ODIN, "Wrong ODIN raw bank type");
  Assert(bank->magic() == LHCb::RawBank::MagicPattern, "Magic pattern mismatch in ODIN raw bank");

  // Validate bank version
  const unsigned int version = bank->version();
  if ((version > bank_version) && ! m_ignoreBankVersion) {
    std::ostringstream msg;
    msg << "Unknown ODIN bank version " << version << ", latest known is " << bank_version;
    //Exception(msg.str()); // throw
    Error(msg.str()).ignore();
    return nullptr;
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
    return nullptr;
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
  if (version <= 5) {
    switch ((temp32 & LHCb::ODIN::TriggerTypeMask) >> LHCb::ODIN::TriggerTypeBits) {
    case PreV6::Reserve            :
    case PreV6::PhysicsTrigger     : odin->setTriggerType(LHCb::ODIN::PhysicsTrigger);     break;
    case PreV6::AuxilliaryTrigger  : odin->setTriggerType(LHCb::ODIN::AuxiliaryTrigger);   break;
    case PreV6::RandomTrigger      : odin->setTriggerType(LHCb::ODIN::LumiTrigger);        break;
    case PreV6::PeriodicTrigger    : odin->setTriggerType(LHCb::ODIN::TechnicalTrigger);   break;
    case PreV6::NonZSupTrigger     : odin->setTriggerType(LHCb::ODIN::NonZSupTrigger);     break;
    case PreV6::TimingTrigger      : odin->setTriggerType(LHCb::ODIN::TimingTrigger);      break;
    case PreV6::CalibrationTrigger : odin->setTriggerType(LHCb::ODIN::CalibrationTrigger); break;
    default                        : odin->setTriggerType(LHCb::ODIN::PhysicsTrigger);     break;
    }
  } else {
    switch ((temp32 & LHCb::ODIN::TriggerTypeMask) >> LHCb::ODIN::TriggerTypeBits) {
    case 0 : odin->setTriggerType(LHCb::ODIN::PhysicsTrigger);     break;
    case 1 : odin->setTriggerType(LHCb::ODIN::BeamGasTrigger);     break;
    case 2 : odin->setTriggerType(LHCb::ODIN::LumiTrigger);        break;
    case 3 : odin->setTriggerType(LHCb::ODIN::TechnicalTrigger);   break;
    case 4 : odin->setTriggerType(LHCb::ODIN::AuxiliaryTrigger);   break;
    case 5 : odin->setTriggerType(LHCb::ODIN::NonZSupTrigger);     break;
    case 6 : odin->setTriggerType(LHCb::ODIN::TimingTrigger);      break;
    case 7 : odin->setTriggerType(LHCb::ODIN::CalibrationTrigger); break;
    default: odin->setTriggerType(LHCb::ODIN::PhysicsTrigger);     break;
    }
  }

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
    case 1  : odin->setBunchCrossingType(LHCb::ODIN::Beam1); break;
    case 2  : odin->setBunchCrossingType(LHCb::ODIN::Beam2); break;
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

  // This conversion is needed to be able to disentangle the enum internal (C) representation
  // from the hardware ODIN bit codes.
  int triggerType = 0;
  switch (odin->triggerType()) {
  case LHCb::ODIN::PhysicsTrigger     : triggerType = 0; break;
  case LHCb::ODIN::BeamGasTrigger     : triggerType = 1; break;
  case LHCb::ODIN::LumiTrigger        : triggerType = 2; break;
  case LHCb::ODIN::TechnicalTrigger   : triggerType = 3; break;
  case LHCb::ODIN::AuxiliaryTrigger   : triggerType = 4; break;
  case LHCb::ODIN::NonZSupTrigger     : triggerType = 5; break;
  case LHCb::ODIN::TimingTrigger      : triggerType = 6; break;
  case LHCb::ODIN::CalibrationTrigger : triggerType = 7; break;
  }

  data[LHCb::ODIN::Word8] = ((odin->bunchId() << LHCb::ODIN::BunchIDBits) & LHCb::ODIN::BunchIDMask) |
                            ((odin->timeAlignmentEventWindow() << LHCb::ODIN::TAEWindowBits) & LHCb::ODIN::TAEWindowMask) |
                            ((triggerType << LHCb::ODIN::TriggerTypeBits) & LHCb::ODIN::TriggerTypeMask) |
                            ((odin->calibrationType() << LHCb::ODIN::CalibrationTypeBits) & LHCb::ODIN::CalibrationTypeMask) |
                            ((odin->forceBit() << LHCb::ODIN::ForceBits) & LHCb::ODIN::ForceMask) |
                            ((odin->bunchCrossingType() << LHCb::ODIN::BXTypeBits) & LHCb::ODIN::BXTypeMask) |
                            ((odin->bunchCurrent() << LHCb::ODIN::BunchCurrentBits) & LHCb::ODIN::BunchCurrentMask);

  data[LHCb::ODIN::TriggerConfigurationKey] = odin->triggerConfigurationKey();

  return bank; // pass the ownership to the caller
}
//=============================================================================

