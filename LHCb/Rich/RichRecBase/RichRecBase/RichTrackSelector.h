
//-----------------------------------------------------------------------------
/** @file RichTrackSelector.h
 *
 * Header file for utility class : RichTrackSelector
 *
 * CVS Log :-
 * $Id: RichTrackSelector.h,v 1.14 2006-01-23 14:08:55 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2003-06-20
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHTRACKSELECTOR_H
#define RICHRECBASE_RICHTRACKSELECTOR_H 1

// from Gaudi
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/MsgStream.h"

// Event model
#include "Event/Track.h"
#include "Event/RichRecTrack.h"

// Kernel
#include "RichKernel/BoostArray.h"

//--------------------------------------------------------------------------------
/** @class RichTrackSelector RichTrackSelector.h RichRecBase/RichTrackSelector.h
 *
 *  A utility class for the RICH reconstruction providing generic track selection
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-06-20
 */
//--------------------------------------------------------------------------------

class RichTrackSelector 
{

public: // definitions

  /// Container type for momentum data for each track type
  typedef std::vector<std::string> MomentumCutData;

public:

  /// Standard constructor
  RichTrackSelector();

  /// Destructor
  ~RichTrackSelector() {}

  /** Method used to set the momentum cut data
   *  @return Write access to the momentum cut data
   */
  MomentumCutData & setMomentumCuts();

  /** Returns the minimum momentum cut value for given track type
   *
   *  @param type Track type enumeration
   *
   *  @return Minimum momentum cut value for that track
   */
  const double minMomentum( const Rich::Track::Type type ) const;

  /** Returns the maximum momentum cut value for given track type
   *
   *  @param type Track type enumeration
   *
   *  @return Maximum momentum cut value for that track
   */
  const double maxMomentum( const Rich::Track::Type type ) const;

  /** Test if the given Track is selected under the current criteria
   *
   *  @param track Pointer to a Track
   *
   *  @return Boolean indicating if the track is selected
   *  @retval true  Track is selected
   *  @retval false Track is rejected
   */
  bool trackSelected( const LHCb::Track * track ) const;

  /** Test it the given RichRecTrack is selected
   *
   *  @param track Pointer to a RichRecTrack
   *
   *  @return Boolean indicating if the track is selected
   *  @retval true  Track is selected
   *  @retval false Track is rejected
   */
  bool trackSelected( const LHCb::RichRecTrack * track ) const;

  /** Access to selected track type name (non-const)
   *
   *  @return Reference to vector of track type names
   */
  std::vector<std::string> & selectedTrackTypes();

  /** Access to selected track type name (const)
   *
   *  @return Const reference to vector of track type names
   */
  const std::vector<std::string> & selectedTrackTypes() const;

  /** Is a given track type configured to be selected
   *
   *  @param track Track type enueration
   *
   *  @return Boolean indicating if the track type is selected
   *  @retval true  Track is selected
   *  @retval false Track is rejected
   */
  bool typeSelected( const Rich::Track::Type track ) const;

  /** Configure the track selection
   *
   *  @return Boolean indicating status of the selection configuration
   *  @retval true  Selection is configured successfully
   *  @retval false Selection configuration failed
   */
  bool configureTrackTypes();

  /// Print to the requested MsgStream the track selection criteria
  void printTrackSelection( MsgStream & stream ) const;

  /// Returns selected track types as a single string
  const std::string selectedTracksAsString() const;

private: // methods

  /// Configure the momentum cuts
  bool configureMomentumCuts();

private: // private data

  /// Track types to accept
  typedef std::vector<std::string> TrackNames;
  TrackNames m_trNames;

  /// Unique tracks only ?
  bool m_uniqueTrOnly;

  /// Track type selection array
  typedef boost::array<bool,Rich::Track::NTrTypes> TrackTypesSel;
  /// Track types to select
  TrackTypesSel m_tkTypeSel;

  /// Track charge selection -1 = negative only, +1 = positive only, 0 = all tracks
  int m_chargeSel;

  /// Minimum momentum cut
  std::vector<double> m_minP;

  /// Maximum momentum cut
  std::vector<double> m_maxP;

  /// Momentum cut data. Set externally and used to configure the momentum cuts
  MomentumCutData m_pCutData;

};

inline RichTrackSelector::MomentumCutData & RichTrackSelector::setMomentumCuts()
{
  return m_pCutData;
}

inline const double RichTrackSelector::minMomentum( const Rich::Track::Type type ) const
{
  return m_minP[type];
}

inline const double RichTrackSelector::maxMomentum( const Rich::Track::Type type ) const
{
  return m_maxP[type];
}

inline bool RichTrackSelector::trackSelected( const LHCb::Track * track ) const
{
  const Rich::Track::Type type = Rich::Track::type(track);
  return ( type != Rich::Track::Unknown &&          // track type is known
           type != Rich::Track::Unusable &&         // track type is usable
           track &&                                 // Track info OK
           (!m_uniqueTrOnly || track->checkFlag(LHCb::Track::Unique)) &&  // Unique tracks
           m_tkTypeSel[type] &&                     // tracking algorithm type
           ( m_chargeSel*track->charge() >= 0 ) &&  // track charge
           ( track->p()/GeV > minMomentum(type) ) &&  // Momentum cuts
           ( track->p()/GeV < maxMomentum(type) )     // Momentum cuts
           );
}

inline bool RichTrackSelector::trackSelected( const LHCb::RichRecTrack * track ) const
{
  const Rich::Track::Type type = track->trackID().trackType();
  return ( type != Rich::Track::Unknown &&          // track type is known
           type != Rich::Track::Unusable &&         // track type is usable
           track &&                                           // Track pointer OK
           (!m_uniqueTrOnly || track->trackID().unique()) &&  // Unique tracks
           m_tkTypeSel[type] &&       // tracking algorithm type
           ( m_chargeSel*track->charge() >= 0 )  &&           // track charge
           ( track->vertexMomentum()/GeV > minMomentum(type) ) &&  // Momentum cut
           ( track->vertexMomentum()/GeV < maxMomentum(type) )     // Momentum cut
           );
}

inline std::vector<std::string> & RichTrackSelector::selectedTrackTypes()
{
  return m_trNames;
}

inline const std::vector<std::string> & RichTrackSelector::selectedTrackTypes() const
{
  return m_trNames;
}

inline bool RichTrackSelector::typeSelected( const Rich::Track::Type track ) const
{
  return ( m_trNames.end() != std::find( m_trNames.begin(),
                                         m_trNames.end(),
                                         Rich::text(track) ) );
}

#endif // RICHRECBASE_RICHTRACKSELECTOR_H
