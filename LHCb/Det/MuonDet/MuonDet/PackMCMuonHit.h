#ifndef MuonEvent_PackMCMuonHit_H
#define MuonEvent_PackMCMuonHit_H 1

// Include files

/** @namespace PackMCMuonHit 
 *
 * bit pattern for hit address (gap,chamber,region,station,quadrant) 
 *
 * @author unknown
 * created Fri Jan 13 10:32:31 2006
 * 
 */


namespace PackMCMuonHit
{
  static const unsigned int bitGapID         = 2;                                                                      ///< number of bit to define the gap
  static const unsigned int bitChamberID     = 8;                                                                      ///< number of bit to define the chamber
  static const unsigned int bitRegionID      = 2;                                                                      ///< number of bit to define the region
  static const unsigned int bitStationID     = 3;                                                                      ///< number of bit to define the station
  static const unsigned int bitQuadrantID    = 2;                                                                      ///< number of bit to define the quadrant
  static const unsigned int shiftGapID       = 0;                                                                      ///< 
  static const unsigned int shiftChamberID   = shiftGapID+ bitGapID;                                                   ///< 
  static const unsigned int shiftRegionID    = shiftChamberID+ bitChamberID;                                           ///< 
  static const unsigned int shiftStationID   = shiftRegionID+ bitRegionID;                                             ///< 
  static const unsigned int shiftQuadrantID  = shiftStationID+ bitStationID;                                           ///< 
  static const unsigned int maskGapID        =  ( ( ( (unsigned int) 1 ) << bitGapID  ) - 1  ) << shiftGapID;          ///< 
  static const unsigned int maskChamberID    =  ( ( ( (unsigned int) 1 ) << bitChamberID  ) - 1 ) <<shiftChamberID ;   ///< 
  static const unsigned int maskRegionID     =  ( ( ( (unsigned int) 1 ) << bitRegionID  ) - 1 ) <<shiftRegionID ;     ///< 
  static const unsigned int maskStationID    =  ( ( ( (unsigned int) 1 ) << bitStationID  ) - 1 ) <<shiftStationID ;   ///< 
  static const unsigned int maskQuadrantID   =  ( ( ( (unsigned int) 1 ) << bitQuadrantID  ) - 1 ) <<shiftQuadrantID ; ///< 

}

// -----------------------------------------------------------------------------
//   end of namespace
// -----------------------------------------------------------------------------

// Including forward declarations


#endif ///MuonEvent_PackMCMuonHit_H
