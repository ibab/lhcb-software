// $Id: PatDownTrack.h,v 1.3 2009-10-08 10:09:46 sstahl Exp $
#ifndef PRDOWNTRACK_H 
#define PRDOWNTRACK_H 1

// Include files
#include "GaudiKernel/Point3DTypes.h"

#include "Event/Track.h"
#include "Event/State.h"

#include "PrKernel/PrUTHit.h"
#include "TfKernel/RecoFuncs.h"

/** @class PrDownTrack PrDownTrack.h
 *  Track helper for Downstream tarck search
 *  Adapted from Pat/PatKShort package
 *
 *  @author Olivier Callot
 *  @date   2007-10-18
 */

class PrDownTrack {
public:
  /// Standard constructor
  PrDownTrack( LHCb::Track* tr, 
                double zTT,
                const std::vector<double>& magnetParams,
                const std::vector<double>& momentumParams,
                const std::vector<double>& yParams,
                double errZMag,
                double magnetScale ); 
  
  virtual ~PrDownTrack( ) {} ///< Destructor

  LHCb::Track*      track()      const { return m_track; }
  LHCb::State*      state()      const { return m_state; }
  double      xMagnet()          const { return m_magnet.x(); }
  double      yMagnet()          const { return m_magnet.y(); }
  double      zMagnet()          const { return m_magnet.z(); }
  double      slopeX()           const { return m_slopeX;     }
  void setSlopeX( double slope )       { 
    m_slopeX = slope;
    m_slopeXCand = slope;
  }
  double      slopeY()           const { return m_slopeY; }

  double      moment()  const { 
    return ( (*m_momPar)[0] +    
             (*m_momPar)[1] * m_state->tx() * m_state->tx() +
             (*m_momPar)[2] * m_state->ty() * m_state->ty() ) / 
      ( m_state->tx() - m_slopeX ) * m_magnetScale;
  }

  double      errXMag() const { return m_errXMag; }
  double      errYMag() const { return m_errYMag; }

  double yAtZ( double z ) const {
    return m_magnet.y() + m_displY + (z-m_magnet.z() ) * slopeY();
  }
  double xAtZ( double z ) const {
    return m_magnet.x() + (z-m_magnet.z() ) * m_slopeX + m_curvature * ( z-m_zTT) * (z-m_zTT);
  }

  double xAtZ( double z, double slopeX ) const {
    return m_magnet.x() + (z-m_magnet.z() ) * slopeX;
  }

  double sagitta( double z ) const { return m_curvature * ( z-m_zTT) * (z-m_zTT); }

  double distance( const PrUTHit* hit ) const {
    return hit->x() - xAtZ( hit->z() );
  }

  PrUTHits& hits() { return m_hits; }
  const PrUTHits& hits() const { return m_hits; }

  void startNewCandidate() {
    m_hits.clear();
    m_magnet = m_magnetSave;
    m_slopeX = m_slopeXSave;
    m_displY  = 0.;
    m_displX  = 0.;
  }

  void startNewXCandidate(PrUTHit* firstHit) {
    m_hits.clear();
    m_hits.push_back(firstHit);
    m_magnet = m_magnetSave;
    m_slopeX = m_slopeXCand;
    m_displY  = 0.;
    m_displX  = 0.;
  }

  

  void setDisplY( double displY )       { m_displY = displY; }
  double displY( )                const { return m_displY; }

  void setDisplX( double displX )       { m_displX = displX; }
  double displX( )                const { return m_displX; }

  void setChisq( double chisq )         { m_chisq = chisq; }
  double chisq( )                 const { return m_chisq; }

  double initialChisq() const {
    return m_displX * m_displX / ( m_errXMag * m_errXMag ) + 
           m_displY * m_displY / ( m_errYMag * m_errYMag );
  }

  void updateX( double dx, double dsl )  {
    m_displX += dx;
    m_magnet  = Gaudi::XYZPoint( m_magnet.x() + dx, m_magnet.y(), m_magnet.z() );
    m_slopeX += dsl;
  }

  double dxMagnet() const { return m_magnetSave.x() - m_magnet.x(); }

  void sortFinalHits() {
    std::sort( m_hits.begin(), m_hits.end(), Tf::increasingByZ<PrUTHit>() );
  }
  
 
protected:

private:
  double      m_zTT;
  const std::vector<double>* m_magPar;
  const std::vector<double>* m_momPar;
  double              m_magnetScale;
  LHCb::Track*        m_track;
  LHCb::State*        m_state;
  Gaudi::XYZPoint     m_magnet;
  Gaudi::XYZPoint     m_magnetSave;

  double      m_slopeX;
  double      m_slopeXSave;
  double      m_slopeXCand;
  double      m_slopeY;
  double      m_displX;
  double      m_displY;
  double      m_moment;
  double      m_errXMag;
  double      m_errYMag;
  double      m_chisq;
  double      m_curvature;
  
  PrUTHits m_hits;      /// working list of hits on this track
};
#endif // PRDOWNTRACK_H
