#ifndef TrackEvent_StateKeys_H
#define TrackEvent_StateKeys_H 1

//--------------------------------------------------------------------------
/** @namespace TrackKeys
 *
 *  Namespace for State enumerations
 *
 *  @author J.A. Hernando
 *  @date 11/05/2005
 */
//--------------------------------------------------------------------------
namespace StateKeys 
{
  //------------------------------------------------------------------------
  /** @enum Location
   *
   *  State location enumerations
   *
   *  @author J.A. Hernando
   *  @date 11/05/2005
   */
  //------------------------------------------------------------------------  
  enum Location {LocationUnknown,     ///< State at undefined location
                 ClosestToBeam,       ///< State closest to the beam-line
                 FirstMeasurement,    ///< State at the first measurement
                 EndVelo,             ///< State at the end of the VELO
                 AtTT,                ///< State at the TT stations
                 AtT,                 ///< State at the T seeding stations
                 BegRich1,            ///< State at the beginning of RICH1
                 EndRich1,            ///< State at the end of RICH1
                 BegRich2,            ///< State at the beginning of RICH2
                 EndRich2,            ///< State at the end of RICH2
                 Calo,                ///< State in the calorimeter
                 Muon                 ///< State in the muon stations
  };

};
#endif   ///TrackEvent_StateKeys_H
