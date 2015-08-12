// $Id: TrackBuildCloneTable.h,v 1.3 2007-09-17 09:25:03 jonrob Exp $
#ifndef _TrackBuildCloneTable_H_
#define _TrackBuildCloneTable_H_

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiAlg/GaudiAlgorithm.h"

#include <string>
#include <vector>

// track interfaces
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/TrackUnitsConverters.h"

// from Event/LinkerEvent
#include "Linker/LinkerWithKey.h"

#include "LHCbMath/MatrixUtils.h"

#include "TrackInterfaces/ITrackExtrapolator.h"

/** @class TrackBuildCloneTable TrackBuildCloneTable.h
 *
 *  Creates a summary linker object container clone information,
 *  using a Kullbeck Liebler Distance
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */

class TrackBuildCloneTable: public GaudiAlgorithm
{

public:

  /// Constructor
  TrackBuildCloneTable(const std::string& name,
                       ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~TrackBuildCloneTable();

  /// Initialisation
  virtual StatusCode initialize();

  /// Execution
  virtual StatusCode execute();

private:

  /** @class CloneState TrackBuildCloneTable.h
   *
   *  Working state object for clone finder
   *
   *  @author M.Needham
   *  @date   30/05/2006
   */
  class CloneState
  {
  public:
    /// Contructor from a state and parent track object
    CloneState( const LHCb::State & _state )
      : state  ( _state),
        invCov ( _state.covariance() )
    { invertCovMatrix(); }
  public:
    LHCb::State state;  ///< The state to use for the comparisions
    Gaudi::TrackSymMatrix invCov; ///< Cached inverted matrix
  private:
    /// Invert the covariance matrix
    void invertCovMatrix();
  };

  /** @class CloneTrack TrackBuildCloneTable.h
   *
   *  Working track object for clone finder
   *
   *  @author M.Needham
   *  @date   30/05/2006
   */
  class CloneTrack
  {
  public: 
    CloneTrack( LHCb::Track* _track = NULL ) : track(_track) { }
    const CloneState * findState( const double z ) const;
  public:
    typedef std::map<double,CloneState> CloneStates;
    CloneStates states;
    LHCb::Track* track; ///< Pointer to the parent track
  public:
    /// Container of working tracks
    typedef std::vector<CloneTrack> Vector;
  };

private:

  /// Computes the KL distance between the two tracks
  double kullbeckLieblerDist(const CloneState& c1,
                             const CloneState& c2) const;

  double m_klCut; ///< Maximum distance for the information to be stored in the linker
  std::string m_inputLocation;  ///< Input TES location for tracks
  std::string m_outputLocation; ///< Output TES location for the linker object

  std::vector<double> m_zStates; ///< The z position(s) to use for the comparision
  double m_maxDz;  ///< max tolerance on finding a state close to the comparison z position

  ITrackExtrapolator* m_extrapolator; ///< Pointer to the state extrapolator to use
  std::string m_extraType;            ///< The state extrapolator type

};

inline void TrackBuildCloneTable::CloneState::invertCovMatrix()
{
  TrackUnitsConverters::convertToG3( invCov );
  invCov.Invert();
  TrackUnitsConverters::convertToG4( invCov );
}

#endif
