#ifndef PATVTTHYBRIDTRACK_H
#define PATVTTHYBRIDTRACK_H 1

// Include files
#include "Event/Track.h"

#include "PatKernel/PatTTHit.h"


  /** @class PatVTTHybridTrack PatVTTHybridTrack.h
   *  Holds the candidate track while building it.
   *  Based on the original implementation by Olivier Callot.
   *
   *  @author Luis Fernandez
   *  @date   2005-07-27
   *  Taken over by Mariusz Witek and modified to have single version on-line off-line
   *  @date   2006-10-07
   */

  class PatVTTHybridTrack {

  public:
    /// Standard constructor
    PatVTTHybridTrack(const LHCb::Track* tr ) : m_track(tr), 
                                                m_chi2PerDoF( 0 ),
                                                m_qOverP( 0.0 )
    {
      const LHCb::State& trState = tr->hasStateAt(LHCb::State::LastMeasurement) ?
        *(tr->stateAt(LHCb::State::LastMeasurement)) :
        (tr->closestState(LHCb::State::EndVelo)) ;
      m_origin = trState.position();
      m_slope = trState.slopes();
      m_clusters.reserve(4); // max of 1 per layer
    };
    
    virtual ~PatVTTHybridTrack( ) {}; ///< Destructor
    
    /// Copy constructor
    PatVTTHybridTrack(const PatVTTHybridTrack& ) = default;
    /// Move constructor
    PatVTTHybridTrack(PatVTTHybridTrack&& ) = default;
    
    /// Copy assignment operator
    PatVTTHybridTrack &operator=(const PatVTTHybridTrack& ) = default;
    /// Move assignment operator
    PatVTTHybridTrack &operator=(PatVTTHybridTrack&& ) = default;
    
    
    const LHCb::Track* track()       const { return m_track; }

    float xAtZ( float z ) const {
      return m_origin.x() + m_slope.x() * ( z - m_origin.z() ) ;
    }

    float yAtZ( float z ) const {
      return m_origin.y() + m_slope.y() * ( z - m_origin.z() ) ;
    }
    
    float slopeY() const { return m_slope.y(); }

    void storeHit(PatTTHit* hit){
        m_clusters.push_back(hit);
    }

    PatTTHits& clusters() { return m_clusters;}

    void setChi2PerDoF(float chi2) { m_chi2PerDoF = chi2; }
    float chi2PerDoF() const { return m_chi2PerDoF; }

    void setQOverP(float qp) { m_qOverP = qp; }
    float qOverP() const { return m_qOverP; }

    void setXTT(float x) { m_xTT = x; }
    float xTT() const { return m_xTT; }

    void setXSlopeTT(float tx) { m_txTT = tx; }
    float xSlopeTT() const { return m_txTT; }

  protected:
    
  private:
    const LHCb::Track*  m_track;
    Gaudi::XYZPoint m_origin;
    Gaudi::XYZVector m_slope;
    PatTTHits m_clusters;
    float m_chi2PerDoF;
    float m_qOverP;
    float m_xTT;
    float m_txTT;
  };

#endif // PATVTTTRACK_H

