#ifndef TRACKTOOLS_SelectTrackInVertex_H
#define TRACKTOOLS_SelectTrackInVertex_H

/** @class SelectTrackInVertex SelectTrackInVertex.h
 *
 *  Selector that returns true if a track is found in a collection of tracks belonging to a vertex (LHCb::RecVertex, e.g. the PV)
 *  
 *  Parameters:
 *
 *  - VertexContainer: Container of vertices
 *   
 *  @author Michel De Cian
 *  @date   2015-09-18
 */

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
// base class
#include "TrackSelectorBase.h"

class SelectTrackInVertex : public TrackSelectorBase, public IIncidentListener {

public:

  /// constructor
  SelectTrackInVertex( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );
  
  virtual ~SelectTrackInVertex();
  
  virtual StatusCode initialize();
  
  
  /** Returns if the given track is selected or not
   *
   *  @param aTrack Reference to the Track to test
   *
   *  @return boolean indicating if the track is selected or not
   *  @retval true  Track is selected
   *  @retval false Track is rejected
   */
  virtual bool accept ( const LHCb::Track & aTrack ) const;
  
  void handle ( const Incident& incident );
  
private:
  
  /// Get all tracks belonging to all vertices and put them in a container
  void getTracksFromVertices() const ;
  
private:
  
  std::string                             m_vertexContainerName;
  mutable bool                            m_newEvent;
  mutable std::vector<const LHCb::Track*> m_tracks;
  
};

#endif // TRACKTOOLS_SelectTrackInVertex_H
