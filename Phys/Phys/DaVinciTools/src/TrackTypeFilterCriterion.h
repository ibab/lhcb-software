// $Id: TrackTypeFilterCriterion.h,v 1.2 2004-08-12 12:33:53 pkoppenb Exp $
#ifndef TRACKTYPEFILTERCRITERION_H 
#define TRACKTYPEFILTERCRITERION_H 1

// Include files
// from Gaudi
#include "GaudiTools/GaudiTool.h"
#include "DaVinciTools/IFilterCriterion.h"            // Interface


/** @class TrackTypeFilterCriterion TrackTypeFilterCriterion.h
 * Returns yes/no depending on track type. 
 * Relevant options are RequireLong, RequireDownstream, RequireUpstream
 * that allow only Long, Downstream, Upstream tracks to pass the filter
 * Veto* options allow to veto certain track types keeping the others.
 * The IgnoreNonTracks allows to ignore neutral and composite particles
 * when this option is not set all such particles are filtered out.
 * When the input particle has been made with a trigger track, it is assumed
 * to be a long track.
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-07-07
 */
class TrackTypeFilterCriterion : public GaudiTool, 
                                 virtual public IFilterCriterion {
public: 
  /// Standard constructor
  TrackTypeFilterCriterion( const std::string& type, 
                            const std::string& name,
                            const IInterface* parent);

  virtual ~TrackTypeFilterCriterion( ); ///< Destructor
  inline bool isSatisfied( const Particle* const & part ); ///< Filter output
  inline bool operator()( const Particle* const & part ); ///< Operator
  StatusCode initialize( );///< Initialization
  

protected:

private:

  // Inspired from TrStoredTrack functions
  bool m_reqLong;          ///< Require to be long tracks
  bool m_vetoLong;         ///< Require not to be long tracks     
  bool m_reqDownstream;    ///< Require to be dowstream tracks
  bool m_vetoDownstream;   ///< Require not to be dowstream tracks
  bool m_reqUpstream;      ///< Require to be upstream tracks
  bool m_vetoUpstream;     ///< Require not to be upstream tracks
  bool m_reqVeloTrack;     ///< Require to be Velo tracks (useless)
  bool m_vetoVeloTrack;    ///< Require not to be Velo tracks (useless)
  bool m_reqBackward;      ///< Require to be backward tracks (useless)
  bool m_vetoBackward;     ///< Require not to be backward tracks (useless)
  bool m_reqTtrack;        ///< Require to be Ttracks (useless)
  bool m_vetoTtrack;       ///< Require not to be Ttracks (useless)

  bool m_ignoreNonTracks; ///< Ignore particles not made from a track
};
#endif // TRACKTYPEFILTERCRITERION_H
