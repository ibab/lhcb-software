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
class Candidate
{
  public:
    /// Standard constructor
    Candidate( const LHCb::Track* track, const Hlt1MuonHits& hits )
        : m_hits( hits.begin(), hits.end() )
    {
        init( track );
    }

    Candidate( const LHCb::Track* track, const Hlt1ConstMuonHits& hits )
        : m_hits( hits )
    {
        init( track );
    }

    Candidate( const LHCb::Track* track )
    {
        m_hits.reserve( 5 );
        init( track );
    }

    void init( const LHCb::Track* track )
    {
        m_track = track;
        const LHCb::State* state = track->stateAt( LHCb::State::EndVelo );
        if ( !state ) state = &( track->closestState( 5000 ) );
        m_x = state->x();
        m_y = state->y();
        m_z = state->z();
        m_tx = state->tx();
        m_ty = state->ty();

        m_errTx2 = state->errTx2();
        m_errTy2 = state->errTy2();

        m_tx2 = m_tx * m_tx;
        m_ty2 = m_ty * m_ty;

        m_slope = 0.;
        m_p = 0.;

        m_fitted = false;
        m_chi2 = 0.;
        m_nDoF = 0;
    }

    virtual ~Candidate() {}; ///< Destructor

    const LHCb::Track* track() const
    {
        return m_track;
    }

    void xStraight( const double z, double& x, double& errX ) const
    {
        double dz = z - m_z;
        x = m_x + dz * m_tx;
        errX = dz * sqrt( m_errTx2 );
    }

    void yStraight( const double z, double& y, double& errY ) const
    {
        double dz = z - m_z;
        y = m_y + dz * m_ty;
        errY = dz * sqrt( m_errTy2 );
    }

    double tx() const
    {
        return m_tx;
    }
    double ty() const
    {
        return m_ty;
    }
    double errTx2() const
    {
        return m_errTx2;
    }
    double errTy2() const
    {
        return m_errTy2;
    }
    double tx2() const
    {
        return m_tx2;
    }
    double ty2() const
    {
        return m_ty2;
    }

    double sinTrack() const
    {
        return sqrt( 1. - 1. / ( 1. + m_tx2 + m_ty2 ) );
    }

    double cosTy() const
    {
        return 1. / sqrt( 1 + m_ty2 );
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
        return sqrt( 1 - 1 / ( 1 + m_tx2 + m_ty2 ) );
    }

    double& slope()
    {
        return m_slope;
    }
    double slope() const
    {
        return m_slope;
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

    /// operator new
    static void* operator new( size_t size )
    {
        return (
            sizeof( Candidate ) == size
                ? boost::singleton_pool<Candidate, sizeof( Candidate )>::malloc()
                : ::operator new( size ) );
    }

    /// operator delete
    static void operator delete( void* p )
    {
        boost::singleton_pool<Candidate, sizeof( Candidate )>::is_from( p )
            ? boost::singleton_pool<Candidate, sizeof( Candidate )>::free( p )
            : ::operator delete( p );
    }

  private:
    const LHCb::Track* m_track;
    double m_x;
    double m_y;
    double m_z;
    double m_tx;
    double m_ty;
    double m_tx2;
    double m_ty2;

    double m_errTx2;
    double m_errTy2;

    double m_slope;
    double m_p;

    Hlt1ConstMuonHits m_hits;

    bool m_fitted;
    double m_chi2;
    unsigned int m_nDoF;
};

typedef std::vector<Candidate*> Candidates;
typedef std::vector<const Candidate*> ConstCandidates;

#endif // CANDIDATE_H
