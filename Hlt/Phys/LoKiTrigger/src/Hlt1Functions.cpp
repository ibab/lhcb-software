// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <algorithm>
// ============================================================================
// GauduKernel
// ============================================================================
#include "GaudiKernel/SystemOfUnits.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Algs.h"
#include "LoKi/Hlt1.h"
#include "LoKi/Hlt1Functions.h"
// ============================================================================
// KalmanFilter
// ============================================================================
#include "KalmanFilter/FastVertex.h"
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning( disable : 177 ) // variable ... was declared but never referenced
#pragma warning( push )
#endif
// ============================================================================
// Boost
// ============================================================================
#include "boost/lambda/bind.hpp"
// ============================================================================
#ifdef __INTEL_COMPILER
#pragma warning( pop )
#endif
// ============================================================================
/** @file
 *  implementation file for classed form the file LoKi/Hlt1Functions.h
 *
 *  This file is part of LoKi project:
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-03-30
 *
 *                    $Revision$
 *  Last Modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace
{
// ==========================================================================
/** the default cut on muon-delta p
 *  @see LoKi::Tracks::MuonDeltaP
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-03-30
 */
const double s_muonDeltaP_delta = -2; // the default cut on muon-delta p
// ==========================================================================
/// number of regions in muon system
const size_t s_nRegions = 4; // number of regions in muon system
                             /** the default p-cuts
 *  @see LoKi::Tracks::MuonDeltaP
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-03-30
 */
const double s_muonDeltaP_pCuts[s_nRegions] = {
    60 * Gaudi::Units::GeV, 30 * Gaudi::Units::GeV,
    15 * Gaudi::Units::GeV, 7.5 * Gaudi::Units::GeV}; // the default p-cuts
// ==========================================================================
}
// ============================================================================
// constructor from delta-cut  and list of p-cuts
// ============================================================================
LoKi::Tracks::MuonDeltaP::MuonDeltaP( double delta, std::vector<double> cuts )
    : LoKi::TrackTypes::TrCuts(), m_deltaP{delta}, m_pCuts{std::move( cuts )}
{
    Assert( m_pCuts.size() == s_nRegions, "Invalid numbef of Muon-regions" );
}
// ============================================================================
// constructor from delta-cut
// ============================================================================
LoKi::Tracks::MuonDeltaP::MuonDeltaP( double delta )
    : LoKi::TrackTypes::TrCuts()
    , m_deltaP{delta}
    , m_pCuts{s_muonDeltaP_pCuts, s_muonDeltaP_pCuts + s_nRegions}
{
    Assert( m_pCuts.size() == s_nRegions, "Invalid numbef of Muon-regions" );
}
// ============================================================================
// constructor from the list of p-cuts
// ============================================================================
LoKi::Tracks::MuonDeltaP::MuonDeltaP( std::vector<double> cuts )
    : LoKi::TrackTypes::TrCuts()
    , m_deltaP{s_muonDeltaP_delta}
    , m_pCuts{std::move( cuts )}
{
    Assert( m_pCuts.size() == s_nRegions, "Invalid numbef of Muon-regions" );
}
// ============================================================================
// constructor
// ============================================================================
LoKi::Tracks::MuonDeltaP::MuonDeltaP()
    : LoKi::TrackTypes::TrCuts()
    , m_deltaP{s_muonDeltaP_delta}
    , m_pCuts{s_muonDeltaP_pCuts, s_muonDeltaP_pCuts + s_nRegions}
{
    Assert( m_pCuts.size() == s_nRegions, "Invalid numbef of Muon-regions" );
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Tracks::MuonDeltaP* LoKi::Tracks::MuonDeltaP::clone() const
{
    return new MuonDeltaP( *this );
}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Tracks::MuonDeltaP::result_type LoKi::Tracks::MuonDeltaP::
operator()( LoKi::Tracks::MuonDeltaP::argument t ) const
{
    if ( !t ) {
        Error( "LHCb::Track* points to NULL, return 'false'" );
        return false;
    }

    // check ancestors :
    if ( t->ancestors().empty() ) return false; // RETURN
    // get the oldest ancestor
    const LHCb::Track* old = t->ancestors().front();
    if ( !old ) return false; // RETURN
    // get the momentum of the oldest ancestor:
    const double pNew = t->p();
    if ( 0 >= pNew ) return false; // RETURN
    const double pOld = old->p();
    if ( 0 >= pOld ) return false; // RETURN

    // get the momentum difference:
    const double delta = ( pNew - pOld ) / pNew;

    // for "bad" matching apply the region-dependent cut on P:
    if ( delta < m_deltaP ) {
        // try to find a region for muon
        typedef std::vector<LHCb::LHCbID> LHCbIDs;

        // get all IDs from the track
        const LHCbIDs& ids = t->lhcbIDs();

        // loop over all IDs in backward direction
        // (why??? - because muon hits are typically at the back,
        //         - so we can early abort the search in case of a positive
        //         - and in case of negative, it doesn't matter which direction
        //         - on uses, as everything needs to be checked...
        // and find muon tile
        auto itile =
            std::find_if( ids.rbegin(), ids.rend(),
                          []( const LHCb::LHCbID& id ) { return id.isMuon(); } );
        if ( itile == ids.rend() ) return false; // RETURN

        const LHCb::MuonTileID tile = itile->muonID();
        const unsigned int muReg = tile.region();
        Assert( muReg < s_nRegions, "Illegal muon region!" );

        if ( pNew < m_pCuts[muReg] ) return false; // RETURN
    }
    // and finally :
    return true; // RETURN
}
// ============================================================================
// OPTIONAL: the nice printput
// ============================================================================
std::ostream& LoKi::Tracks::MuonDeltaP::fillStream( std::ostream& s ) const
{
    if ( s_muonDeltaP_delta == m_deltaP && s_nRegions == m_pCuts.size() &&
         std::equal( m_pCuts.begin(), m_pCuts.end(), s_muonDeltaP_pCuts ) ) {
        return s << "TrDPMU";
    } // RETURN
    if ( s_muonDeltaP_delta == m_deltaP ) {
        return s << "TrDPMU_(" << Gaudi::Utils::toString( m_pCuts ) << ")";
    }
    if ( s_nRegions == m_pCuts.size() &&
                std::equal( m_pCuts.begin(), m_pCuts.end(), s_muonDeltaP_pCuts ) ) {
        return s << "TrDPMU_(" << m_deltaP << ")";
    } // RETURN
    //
    return s << "TrDPMU_(" << m_deltaP << "," << Gaudi::Utils::toString( m_pCuts )
             << ")";
}
// ============================================================================

// ============================================================================
// constructor from vertex selection
// ============================================================================
LoKi::Tracks::Hlt1TrackMinIp::Hlt1TrackMinIp(
    const Hlt::TSelection<LHCb::RecVertex>* selection )
    : LoKi::TrackTypes::TrFunc()
    , m_vertices{selection}
    , m_vxcut{LoKi::Constant<const LHCb::VertexBase*, bool>{true}}
    , m_vxcut_trivial{true}
{
}
// ============================================================================
// constructor from vertex selection & cuts
// ============================================================================
LoKi::Tracks::Hlt1TrackMinIp::Hlt1TrackMinIp(
    const Hlt::TSelection<LHCb::RecVertex>* selection,
    const LoKi::Functor<const LHCb::VertexBase*, bool>& cuts )
    : LoKi::TrackTypes::TrFunc()
    , m_vertices{selection}
    , m_vxcut{cuts}
    , m_vxcut_trivial{false}
{
}
// ============================================================================
// constructor from vertex selection
// ============================================================================
LoKi::Tracks::Hlt1TrackMinIp::Hlt1TrackMinIp( std::string selection )
    : LoKi::TrackTypes::TrFunc()
    , m_vertices{std::move( selection )}
    , m_vxcut{LoKi::Constant<const LHCb::VertexBase*, bool>{true}}
    , m_vxcut_trivial{true}
{
}
// ============================================================================
// constructor from vertex selection & cuts
// ============================================================================
LoKi::Tracks::Hlt1TrackMinIp::Hlt1TrackMinIp(
    std::string selection, const LoKi::Functor<const LHCb::VertexBase*, bool>& cuts )
    : LoKi::TrackTypes::TrFunc()
    , m_vertices{std::move( selection )}
    , m_vxcut{cuts}
    , m_vxcut_trivial{false}
{
}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Tracks::Hlt1TrackMinIp::~Hlt1TrackMinIp()
{
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Tracks::Hlt1TrackMinIp* LoKi::Tracks::Hlt1TrackMinIp::clone() const
{
    return new LoKi::Tracks::Hlt1TrackMinIp{ *this };
}
// ============================================================================
namespace
{
// ==========================================================================
/** @struct _IP
 *  helper structure to get IP for track with respect of the vertex
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-12-05
 */
struct _IP : public std::unary_function<const LHCb::VertexBase*, double>
{
  public:
    // ========================================================================
    _IP( const LHCb::Track* t ) : m_track( t )
    {
    }
    // ========================================================================
    inline double operator()( const LHCb::VertexBase* vertex ) const
    {
        if ( !vertex ) return LoKi::Constants::PositiveInfinity;
        //
        StatusCode sc =
            LoKi::FastVertex::distance( m_track, vertex, m_impact, true );
        if ( sc.isFailure() ) return LoKi::Constants::PositiveInfinity;
        //
        return m_impact.R();
    }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constuctor is disabled
    _IP(); // the default constructor is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the stored track
    const LHCb::Track* m_track; // the stored track
    /// the impact parameter itself
    mutable Gaudi::XYZVector m_impact; // the impact parameter itself
    // ========================================================================
};
// ==========================================================================
/** @struct_IPchi2
 *  helper structure to get chi2(IP) for track with respect of the vertex
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-12-05
 */
struct _IPchi2 : public std::unary_function<const LHCb::VertexBase*, double>
{
  public:
    // ========================================================================
    _IPchi2( const LHCb::Track* t ) : m_track( t )
    {
    }
    // ========================================================================
    inline double operator()( const LHCb::VertexBase* vertex ) const
    {
        if ( !vertex ) return LoKi::Constants::PositiveInfinity;
        //
        double impact = 0;
        double ipchi2 = 0;
        StatusCode sc =
            LoKi::FastVertex::distance( m_track, vertex, impact, ipchi2, true );
        if ( sc.isFailure() ) return LoKi::Constants::PositiveInfinity;
        //
        return ipchi2;
    }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constuctor is disabled
    _IPchi2(); // the default constructor is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the stored track
    const LHCb::Track* m_track; // the stored track
    // ========================================================================
};
// ==========================================================================
} //                                                 end of anonymous namespace
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Tracks::Hlt1TrackMinIp::result_type LoKi::Tracks::Hlt1TrackMinIp::
operator()( LoKi::Tracks::Hlt1TrackMinIp::argument t ) const
{
    if ( !t ) {
        Error( "LHCb::Track* points to NULL, return negative infinity " );
        return LoKi::Constants::NegativeInfinity;
    }
    //
    typedef Hlt::TSelection<LHCb::RecVertex> Selection;
    //
    const Selection* s = selection();
    //
    Assert( s, "Invalid Hlt::Selection!" );
    //
    if ( s->empty() ) return LoKi::Constants::PositiveInfinity;
    //
    std::pair<Selection::const_iterator, double> r =
        ( m_vxcut_trivial ) ? LoKi::Algs::extremum( s->begin(), s->end(), _IP( t ),
                                                    std::less<double>() )
                            : LoKi::Algs::extremum( s->begin(), s->end(), _IP( t ),
                                                    m_vxcut, std::less<double>() );
    //
    if ( s->end() == r.first ) return LoKi::Constants::PositiveInfinity;
    //
    return r.second;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Tracks::Hlt1TrackMinIp::fillStream( std::ostream& s ) const
{
    s << "Tr_HLTMIP(' " << m_vertices.selName() << "'";
    if ( !m_vxcut_trivial ) s << "," << m_vxcut;
    return s << ")";
}
// ============================================================================

// ============================================================================
// constructor from vertex selection
// ============================================================================
LoKi::Tracks::Hlt1TrackMinIpChi2::Hlt1TrackMinIpChi2(
    const Hlt::TSelection<LHCb::RecVertex>* selection )
    : LoKi::Tracks::Hlt1TrackMinIp( selection )
{
}
// ============================================================================
// constructor from vertex selection & cuts
// ============================================================================
LoKi::Tracks::Hlt1TrackMinIpChi2::Hlt1TrackMinIpChi2(
    const Hlt::TSelection<LHCb::RecVertex>* selection,
    const LoKi::Functor<const LHCb::VertexBase*, bool>& cuts )
    : LoKi::Tracks::Hlt1TrackMinIp( selection, cuts )
{
}
// ============================================================================
// constructor from vertex selection
// ============================================================================
LoKi::Tracks::Hlt1TrackMinIpChi2::Hlt1TrackMinIpChi2( std::string selection )
    : LoKi::Tracks::Hlt1TrackMinIp( std::move( selection ) )
{
}
// ============================================================================
// constructor from vertex selection & cuts
// ============================================================================
LoKi::Tracks::Hlt1TrackMinIpChi2::Hlt1TrackMinIpChi2(
    std::string selection, const LoKi::Functor<const LHCb::VertexBase*, bool>& cuts )
    : LoKi::Tracks::Hlt1TrackMinIp( std::move( selection ), cuts )
{
}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::Tracks::Hlt1TrackMinIpChi2::~Hlt1TrackMinIpChi2()
{
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Tracks::Hlt1TrackMinIpChi2* LoKi::Tracks::Hlt1TrackMinIpChi2::clone() const
{
    return new LoKi::Tracks::Hlt1TrackMinIpChi2{ *this };
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& LoKi::Tracks::Hlt1TrackMinIpChi2::fillStream( std::ostream& s ) const
{
    s << "Tr_HLTMIPCHI2(' " << m_vertices.selName() << "'";
    if ( !m_vxcut_trivial ) s << "," << m_vxcut;
    return s << ")";
}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Tracks::Hlt1TrackMinIpChi2::result_type LoKi::Tracks::Hlt1TrackMinIpChi2::
operator()( LoKi::Tracks::Hlt1TrackMinIpChi2::argument t ) const
{
    if ( !t ) {
        Error( "LHCb::Track* points to NULL, return negative infinity " );
        return LoKi::Constants::NegativeInfinity;
    }
    //
    typedef Hlt::TSelection<LHCb::RecVertex> Selection;
    //
    const Selection* s = selection();
    //
    Assert( s, "Invalid Hlt::Selection!" );
    //
    if ( s->empty() ) return LoKi::Constants::PositiveInfinity;
    //
    std::pair<Selection::const_iterator, double> r =
        ( m_vxcut_trivial )
            ? LoKi::Algs::extremum( s->begin(), s->end(), _IPchi2( t ),
                                    std::less<double>() )
            : LoKi::Algs::extremum( s->begin(), s->end(), _IPchi2( t ), m_vxcut,
                                    std::less<double>() );
    //
    if ( s->end() == r.first ) return LoKi::Constants::PositiveInfinity;
    //
    return r.second;
}
// ============================================================================
// The END
// ============================================================================
