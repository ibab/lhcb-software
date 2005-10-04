#ifndef TrackEvent_TrackKeys_H
#define TrackEvent_TrackKeys_H 1

//--------------------------------------------------------------------------
/** @namespace TrackKeys
 *
 *  Namespace for Track enumerations
 *
 *  @author J.A. Hernando
 *  @date 13/04/2005
 */
//--------------------------------------------------------------------------
namespace TrackKeys 
{
  //------------------------------------------------------------------------
  /** @enum History
   *
   *  Track history enumerations.
   *  Each entry corresponds to the name of the pattern recognition
   *  algorithm that produced the track
   *
   *  @author J.A. Hernando
   *  @date 13/04/2005
   */
  //------------------------------------------------------------------------
  enum History {HistoryUnknown=0,
                TrackIdealPR,
                TrackSeeding,
                PatVelo,
                PatVeloTT,
                TrgVeloTT,
                PatLong,
                TrgForward,
                TrackMatching,
                TrackKShort,
                TsaTrack,
                CnvSeed,       ///< Seed tracks converted from old Track Event Model tracks
                CnvVelo,       ///< Velo tracks converted from old Track Event Model tracks
                CnvVeloBack,   ///< Velo Backwards tracks converted from old Track Event Model tracks
                CnvVeloTT,     ///< VeloTT tracks converted from old Track Event Model tracks
                CnvForward,    ///< Forward tracks converted from old Track Event Model tracks
                CnvMatch,      ///< match tracks converted from old Track Event Model tracks
                CnvKsTrack     ///< KsTrack tracks converted from old Track Event Model tracks
               };
  
  //------------------------------------------------------------------------
  /** @enum HistoryFit
   *
   *  Track fit history enumerations
   *
   *  @author J.A. Hernando
   *  @date 13/04/2005
   */
  //------------------------------------------------------------------------
  enum HistoryFit {FitUnknown=0,  ///< Track has not been fitted yet
                   Kalman};       ///< Track fitted with the Kalman fitter


  //------------------------------------------------------------------------
  /** @enum Types
   *
   *  Track type enumerations
   *
   *  @author J.A. Hernando
   *  @date 13/04/2005
   */
  //------------------------------------------------------------------------  
  enum Types {TypeUnknown=0,  ///< Track of undefined type
              Velo,           ///< Velo track
              VeloR,          ///< 2D Velo track
              Long,           ///< Forward track
              Upstream,       ///< Upstream track
              Downstream,     ///< Downstream track
              Ttrack};        ///< Seed track (hits in the seeding stations)
  //------------------------------------------------------------------------
  /** @enum Status
   *
   *  Track status flag enumerations.
   *  The flag specifies in which state of the reconstruction phase the
   *  track is. The status flag is set by the relevant algorithms
   *
   *  @author J.A. Hernando
   *  @date 13/04/2005
   */
  //------------------------------------------------------------------------
  enum Status {StatusUnknown=0,  ///< Track in an undefined status
               PatRecIDs,        ///< Pattern recognition track with LHCbIDs
               PatRecMeas,       ///< Pattern recognition track with Measurements added
               Fitted,           ///< Fitted track
               FitFailed};       ///< Track for which the track fit failed

  //------------------------------------------------------------------------
  /** @enum Flags
   *
   *  Track general flags enumerations
   *
   *  @author J.A. Hernando
   *  @date 13/04/2005
   */
  //------------------------------------------------------------------------
  enum Flags {Backward=1,  ///< Velo track in the backward direction
              Invalid=2,   ///< Invalid track for physics
              Unique=4,    ///< Unique track, i.e. with no (known) clones
              IPSelected=8,
              PIDSelected=16};
};
#endif   ///TrackEvent_TrackKeys_H
