// $Id: ODINDefinitions.h,v 1.1 2006-03-15 15:00:44 cattanem Exp $
#ifndef ODINDEFINITIONS_H 
#define ODINDEFINITIONS_H 1

// Include files

/** @file ODINDefinitions.h
 *  Parameters of ODIN data
 *
 *  @author Marco Cattaneo
 *  @date   2006-03-15
 */
namespace LHCb 
{
  /** @namespace LHCb::ODIN
   *  Parameters describing structure of ODIN bank as defined in EDMS 704084 v1.2
   *  @author Marco Cattaneo
   *  @date   2006-03-15
   */
  namespace ODIN 
  {
    enum Data {
      RunNumber = 0, 
      OrbitNumber,
      L0EventIDLo,
      L0EventIDHi,
      GPSTimeLo,
      GPSTimeHi,
      Word6,
      Word7
    }; // enum ODIN::Data

    enum Word6Bits {
      DetectorStatusBits = 0,
      ErrorBits          = 24,
      SynchErrorBits     = 24,
      ErrorForcedBits    = 25
    }; // enum ODIN::Word6Bits
     
    enum Word6Masks {
      DetectorStatusMask = 0xFFFFFFL,
      ErrorMask          = 0xFF000000L,
      SynchErrorMask     = 0x01000000L,
      ErrorForcedMask    = 0x02000000L
    }; // enum ODIN::Word6Masks

    enum Word7Bits {
      BunchIDBits      = 0,
      TriggerTypeBits  = 16,
      ReadoutTypeBits  = 19,
      ForceBits        = 21,
      BXTypeBits       = 22,
      BunchCurrentBits = 24
    }; // enum ODIN::Word7Bits

    enum Word7Masks {
      BunchIDMask      = 0xFFFFL,
      TriggerTypeMask  = 0x70000L,
      ReadoutTypeMask  = 0x180000L,
      ForceMask        = 0x200000L,
      BXTypeMask       = 0xC00000L,
      BunchCurrentMask = 0xFF000000L
    }; // enum ODIN::Word7Masks

    enum ReadoutTypes {
      ZeroSuppressed    = 0,
      NonZeroSuppressed = 1
    }; // enum ODIN::ReadoutTypes

    enum BXTypes {
      NoBeam          = 0,
      SingleBeamLeft  = 1,
      SingleBeamRight = 2,
      BeamCrossing    = 3
    }; // enum ODIN::BXTypes

  }; // namespace ODIN
} // namespace LHCb
#endif // ODINDEFINITIONS_H
