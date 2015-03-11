// $Id: $
#ifndef CANDIDATE_H
#define CANDIDATE_H 1

// STL
#include <vector>
#include <algorithm>

// Gaudi
#include <GaudiKernel/boost_allocator.h>

// LHCb
#include <Event/Track.h>
#include <Event/State.h>

// Include files
#include "Hlt1MuonHit.h"

/** @class Candidate Candidate.h
 *  Hlt1Muons candidate.
 *
 *  @author Roel Aaij
 *  @date   2010-12-02
 */
class Candidate final
{
  public:
    /// Standard constructor
     Candidate( const LHCb::Track* track, Hlt1ConstMuonHits hits, LHCb::State::Location stateLocation )
        : m_track(track)
        , m_hits( std::move(hits) )
    {
      const LHCb::State* state = track->stateAt( stateLocation );
      if ( !state ) state = &( track->closestState( 5000 ) );
      m_params = {{ state->x(),  state->y(), state->z(), state->tx(), state->ty(),
		    state->errTx2(), state->errTy2() }};
    }

     Candidate( const LHCb::Track* track )
       : Candidate( track, Hlt1ConstMuonHits(), LHCb::State::EndVelo )
    {
        m_hits.reserve( 5 );
    }

     Candidate( const LHCb::Track* track, LHCb::State::Location stateLocation )
       : Candidate( track, Hlt1ConstMuonHits(), stateLocation )
    {
        m_hits.reserve( 5 );
    }

    const LHCb::Track* track() const
    {
        return m_track;
    }

    void xStraight( const double z, double& x, double& errX ) const
    {
        double dz = z - m_params[2];
        x = m_params[0] + dz * m_params[3];
        errX = dz * sqrt( m_params[5] );
    }

    void yStraight( const double z, double& y, double& errY ) const
    {
        double dz = z - m_params[2];
        y = m_params[1] + dz * m_params[4];
        errY = dz * sqrt( m_params[6] );
    }

    double tx() const
    {
        return m_params[3];
    }
    double ty() const
    {
        return m_params[4];
    }
    double errTx2() const
    {
        return m_params[5];
    }
    double errTy2() const
    {
        return m_params[6];
    }
    double tx2() const
    {
        return m_params[3] * m_params[3];
    }
    double ty2() const
    {
        return m_params[4] * m_params[4];
    }

    double sinTrack() const
    {
        return sqrt( 1. - 1. / ( 1. + tx2() + ty2() ) );
    }

    double cosTy() const
    {
        return 1. / sqrt( 1 + ty2() );
    }

    Hlt1ConstMuonHits::iterator hitsBegin()
    {
        return m_hits.begin();
    }
    Hlt1ConstMuonHits::iterator hitsEnd()
    {
        return m_hits.end();
    }

    void addHit( const Hlt1MuonHit* hit )
    {
        m_hits.push_back( hit );
    }
    Hlt1ConstMuonHits& hits()
    {
        return m_hits;
    }
    const Hlt1ConstMuonHits& hits() const
    {
        return m_hits;
    }

    double sin() const
    {
        return sqrt( 1 - 1 / ( 1 + tx2() + ty2() ) );
    }

    double& slope()
    {
        return m_slope;
    }
    double slope() const
    {
        return m_slope;
    }
    double& slopeY()
    {
        return m_slopeY;
    }
    double slopeY() const
    {
        return m_slopeY;
    }
    double& p()
    {
        return m_p;
    }
    double p() const
    {
        return m_p;
    }
    bool& fitted()
    {
        return m_fitted;
    }
    bool fitted() const
    {
        return m_fitted;
    }
    double& chi2()
    {
        return m_chi2;
    }
    double chi2() const
    {
        return m_chi2;
    }
    double chi2DoF() const
    {
        return m_chi2 / m_nDoF;
    }
    unsigned int& nDoF()
    {
        return m_nDoF;
    }
    unsigned int nDoF() const
    {
        return m_nDoF;
    }

  private:
    const LHCb::Track* m_track;
    std::array<double,7> m_params;

    double m_slope  = 0;
    double m_slopeY = 0;
    double m_p = 0;

    Hlt1ConstMuonHits m_hits;

    double m_chi2 = 0;
    unsigned int m_nDoF = 0;
    bool m_fitted = false;
};


#endif // CANDIDATE_H
