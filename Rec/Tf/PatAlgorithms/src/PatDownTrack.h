// $Id: PatDownTrack.h,v 1.3 2009-10-08 10:09:46 sstahl Exp $
#ifndef PATDOWNTRACK_H 
#define PATDOWNTRACK_H 1

// Include files
#include "GaudiKernel/Point3DTypes.h"

#include "Event/Track.h"
#include "Event/State.h"

#include "PatKernel/PatTTHit.h"
#include "TfKernel/RecoFuncs.h"

/** @class PatDownTrack PatDownTrack.h
 *  Track helper for Downstream tarck search
 *  Adapted from Pat/PatKShort package
 *
 *  @author Olivier Callot
 *  @date   2007-10-18
 */

class PatDownTrack {
public:
  /// Standard constructor
  PatDownTrack( LHCb::Track* tr, 
                double zTT,
                const std::vector<double>& magnetParams,
                const std::vector<double>& momentumParams,
                const std::vector<double>& yParams,
                const double magnetScale ); 
  
  virtual ~PatDownTrack( ) {} ///< Destructor

  // getters
  LHCb::Track*      track()      const { return m_track;      }
  LHCb::State*      state()      const { return m_state;      }
  PatTTHits&        hits()             { return m_hits;       }
  const PatTTHits&  hits()       const { return m_hits;       }
  double            xMagnet()    const { return m_magnet.x(); }
  double            yMagnet()    const { return m_magnet.y(); }
  double            zMagnet()    const { return m_magnet.z(); }
  double            slopeX()     const { return m_slopeX;     }
  double            slopeY()     const { return m_slopeY;     }
  double            errXMag()    const { return m_errXMag;    }
  double            errYMag()    const { return m_errYMag;    }
  double            displX()     const { return m_displX;     }
  double            displY()     const { return m_displY;     }
  double            chi2()       const { return m_chi2;       }

  // setters
  void setSlopeX( const double slope ){ 
    m_slopeX = slope;
    m_slopeXCand = slope;
  }
  
  void setDisplX( const double displX ){ m_displX = displX; }
  void setDisplY( const double displY ){ m_displY = displY; }
  void setChi2( const double chi2 ){ m_chi2 = chi2; }
  
  double xAtZ( const double z ) const {
    return m_magnet.x() + (z-m_magnet.z() ) * m_slopeX + m_curvature * ( z-m_zTT) * (z-m_zTT);
  }

  double xAtZ( const double z, const double slopeX ) const {
    return m_magnet.x() + (z-m_magnet.z() ) * slopeX;
  }
 
  double yAtZ( const double z ) const {
    return m_magnet.y() + m_displY + (z-m_magnet.z() ) * slopeY();
  }

  void updateX( const double dx, const double dsl )  {
    m_displX += dx;
    m_magnet  = Gaudi::XYZPoint( m_magnet.x() + dx, m_magnet.y(), m_magnet.z() );
    m_slopeX += dsl;
  }

  double dxMagnet() const { return m_magnetSave.x() - m_magnet.x(); }

  double initialChi2() const {
    return m_displX * m_displX / ( m_errXMag * m_errXMag ) + 
      m_displY * m_displY / ( m_errYMag * m_errYMag );
  }
 
  double sagitta( const double z ) const { return m_curvature * ( z-m_zTT) * (z-m_zTT); }

  double distance( const PatTTHit* hit ) const {
    return hit->x() - xAtZ( hit->z() );
  }
 
  double momentum()  const { 
    return ( (*m_momPar)[0] +    
             (*m_momPar)[1] * m_state->tx() * m_state->tx() +
             (*m_momPar)[2] * m_state->ty() * m_state->ty() ) / 
      ( m_state->tx() - m_slopeX ) * m_magnetScale;
  }

  void startNewCandidate() {
    m_hits.clear();
    m_magnet = m_magnetSave;
    m_slopeX = m_slopeXSave;
    m_displY  = 0.;
    m_displX  = 0.;
  }

  void startNewXCandidate(PatTTHit* firstHit) {
    m_hits.clear();
    m_hits.push_back(firstHit);
    m_magnet = m_magnetSave;
    m_slopeX = m_slopeXCand;
    m_displY  = 0.;
    m_displX  = 0.;
  }

  void sortFinalHits() {
    std::sort( m_hits.begin(), m_hits.end(), Tf::increasingByZ<PatTTHit>() );
  }
  
 
protected:

private:
  
  
  const std::vector<double>* m_magPar;
  const std::vector<double>* m_momPar;
  
  LHCb::Track*        m_track;
  LHCb::State*        m_state;
  Gaudi::XYZPoint     m_magnet;
  Gaudi::XYZPoint     m_magnetSave;

  double      m_magnetScale;
  double      m_zTT;
  double      m_slopeX;
  double      m_slopeXSave;
  double      m_slopeXCand;
  double      m_slopeY;
  double      m_displX;
  double      m_displY;
  double      m_moment;
  double      m_errXMag;
  double      m_errYMag;
  double      m_chi2;
  double      m_curvature;
  
  PatTTHits m_hits;      /// working list of hits on this track
};
#endif // PATDOWNTRACK_H
