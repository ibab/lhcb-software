#include "HltBase/HltUtils.h"

#include <cassert>
#include <numeric>
#include "LHCbMath/MatrixTransforms.h"
#include "Math/MatrixFunctions.h"

using namespace Gaudi;
using namespace Gaudi::Math;
using namespace LHCb;

//! retun the closest point between the 2 tracks (first State)
XYZPoint closestPoint( const Track &track1, const Track &track2 ) {
    const State &state1 = track1.firstState();
    const State &state2 = track2.firstState();
    XYZPoint pos1{0., 0., 0.};
    XYZPoint pos2{0., 0., 0.};
    closestPoints( Line<XYZPoint, XYZVector>( state1.position(), state1.slopes() ),
                   Line<XYZPoint, XYZVector>( state2.position(), state2.slopes() ),
                   pos1, pos2 );

    return XYZPoint{0.5 * ( pos1.x() + pos2.x() ), 0.5 * ( pos1.y() + pos2.y() ),
                    0.5 * ( pos1.z() + pos2.z() )};
}

void Hlt::VertexCreator::operator()( const LHCb::Track &track1,
                                     const LHCb::Track &track2,
                                     LHCb::RecVertex &ver ) const {
    ver.setPosition( closestPoint( track1, track2 ) );
    ver.addToTracks( const_cast<Track *>( &track1 ) );
    ver.addToTracks( const_cast<Track *>( &track2 ) );
}

XYZVector HltUtils::closestDistance( const Track &track1, const Track &track2 ) {
    const State &state1 = track1.firstState();
    const State &state2 = track2.firstState();
    XYZPoint pos1{0., 0., 0.};
    XYZPoint pos2{0., 0., 0.};
    typedef Gaudi::Math::Line<Gaudi::XYZPoint, Gaudi::XYZVector> line_t;
    bool ok = Gaudi::Math::closestPoints( line_t{state1.position(), state1.slopes()},
                                          line_t{state2.position(), state2.slopes()},
                                          pos1, pos2 );
    return ok ? ( pos1 - pos2 ) : XYZVector{0., 0., 1.e6};
}

//! return the impact parameter significance
double HltUtils::impactParameterSignificance( const LHCb::RecVertex &vertex,
                                              const LHCb::Track &track ) {
    const LHCb::State *state = track.stateAt( LHCb::State::ClosestToBeam );
    if ( !state ) state = &track.firstState();
    XYZVector delta( state->position() - vertex.position() ); // vertex to ref point
    XYZVector dir(
        state->slopes().unit() ); // note: if backward, reverse direction???

    double dot = delta.Dot( dir );
    double ip2 = delta.Mag2() - dot * dot;

    bool positive = ( delta.z() > dot * dir.z() );

    XYZVector dip2ddelta = 2 * ( delta - dot * dir );
    delta *= -2 * dot * dir.z();

    // WARNING: this assumes the track is not used in the vertex...
    double err2 =
        Gaudi::Math::Similarity( dip2ddelta, vertex.covMatrix() ) +
        ROOT::Math::Similarity(
            Gaudi::Vector5(
                -dip2ddelta.x(), -dip2ddelta.y(),
                delta.Dot( XYZVector( 1.0 - dir.x() * dir.x(), -dir.x() * dir.y(),
                                      -dir.x() * dir.z() ) ),
                delta.Dot( XYZVector( -dir.y() * dir.x(), 1.0 - dir.y() * dir.y(),
                                      -dir.y() * dir.z() ) ),
                0.0 ),
            state->covariance() );
    err2 = sqrt( err2 );
    double ips = 2 * ip2 / err2;
    return positive ? ips : -ips;
}

double HltUtils::vertexMatchIDsFraction( const LHCb::RecVertex &vref,
                                         const LHCb::RecVertex &v ) {
    assert( vref.tracks().size() == 2 );
    const LHCb::Track &rtrack1 = *( vref.tracks()[0] );
    const LHCb::Track &rtrack2 = *( vref.tracks()[1] );

    assert( v.tracks().size() == 2 );
    const LHCb::Track &track1 = *( v.tracks()[0] );
    const LHCb::Track &track2 = *( v.tracks()[1] );

    double r11 = HltUtils::matchIDsFraction( rtrack1, track1 );
    double r22 = HltUtils::matchIDsFraction( rtrack2, track2 );
    double f11 = r11 + r22;

    double r12 = HltUtils::matchIDsFraction( rtrack1, track2 );
    double r21 = HltUtils::matchIDsFraction( rtrack2, track1 );
    double f12 = r12 + r21;

    return f11 > f12 ? f11 : f12;
}

namespace {
struct isMuonStation {
    unsigned int m_station;
  public:
    isMuonStation( unsigned int station ) : m_station( station ) {}
    bool operator()( const LHCbID &id ) const {
        return ( id.isMuon() && ( id.muonID().station() == m_station ) );
    }
};
}

bool HltUtils::doShareM3( const Track &t1, const Track &t2 ) {
    const std::vector<LHCbID> &l1 = t1.lhcbIDs();
    const std::vector<LHCbID> &l2 = t2.lhcbIDs();
    // TODO: check whether it makes sense to always do the
    //       shortest list first...
    // if (l1.size()>l2.size()) return doShareM3(t2,t1);
    // typically muonhits are at the back, so go in reverse order...
    // hope the CPU cache won't mind too much...
    auto i1 = find_if( l1.rbegin(), l1.rend(), isMuonStation( 2 ) );
    if ( i1 == l1.rend() ) return false;

    auto i2 = find_if( l2.rbegin(), l2.rend(), isMuonStation( 2 ) );
    if ( i2 == l2.rend() ) return false;

    if ( !( *i1 == *i2 ) ) return false;

    auto i3 = find_if( l1.rbegin(), l1.rend(), isMuonStation( 1 ) );
    if ( i3 == l1.rend() ) return false;

    auto i4 = find_if( l2.rbegin(), l2.rend(), isMuonStation( 1 ) );
    return i4 != l2.rend() && *i3 == *i4;
}
