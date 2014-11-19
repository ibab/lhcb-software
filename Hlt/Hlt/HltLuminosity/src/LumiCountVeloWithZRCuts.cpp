#include <functional>
#include "GaudiKernel/AlgFactory.h"

#include "Event/Track.h"
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"

#include "LumiCountVeloWithZRCuts.h"
#include "boost/format.hpp"

// hack to avoid typing some things twice
#define AUTO_RETURN(...) noexcept(noexcept(__VA_ARGS__)) -> decltype(__VA_ARGS__) {return (__VA_ARGS__);}

// composition implements, given f and g, their composition  f( g(x1), x2, x3, .. )
template< class F, class G > struct Composition {
    F f;
    G g;

    template< class _F, class _G >
    constexpr Composition( _F&& f, _G&& g ) : f( std::forward<_F>(f) ), g( std::forward<_G>(g) ) { }

    template< class X, class ...Y >
    auto operator() ( X&& x, Y&& ...y ) const
    AUTO_RETURN( f( g(std::forward<X>(x)), std::forward<Y>(y)... ) )
};

template< class F, class G >
constexpr Composition<F,G>  compose( F f, G g ) {
    return { std::move(f), std::move(g) };
}


//-----------------------------------------------------------------------------
// Implementation file for class : LumiCountVeloWithZRCuts
// Counts number of L0 muons above threshold.
// 2011-03-01 : Vladislav Balagura
// copied from LumiCountMuons
//-----------------------------------------------------------------------------
DECLARE_ALGORITHM_FACTORY( LumiCountVeloWithZRCuts )

LumiCountVeloWithZRCuts::LumiCountVeloWithZRCuts( std::string name,
                                                  ISvcLocator* pSvcLocator )
    : GaudiAlgorithm( std::move( name ), pSvcLocator )
{
    declareProperty( "TrackCounterName", m_TrackCounterName = "Velo" );
    declareProperty( "VertexCounterName", m_VertexCounterName = "Vertex" );
    declareProperty( "TrackInputSelection",
                     m_TrackInputSelectionName = "TES:Hlt/Track/Lumi" );
    declareProperty( "VertexInputSelection",
                     m_VertexInputSelectionName = "TES:Hlt/Vertex/Lumi" );
    declareProperty( "OutputContainer",
                     m_OutputContainerName = LHCb::HltLumiSummaryLocation::Default );
    declareProperty( "AbsZCut", m_AbsZCut = 300 ); // in mm
    declareProperty( "RCut", m_RCut = 4 );         // in mm
}

StatusCode LumiCountVeloWithZRCuts::initialize()
{
    StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

    if ( msgLevel( MSG::DEBUG ) ) {
        auto fmt_s = boost::format("%20s");
        auto fmt_g = boost::format("%20g");
        debug() << "==> Initialize\n"
                << "TrackCounterName: " << fmt_s % m_TrackCounterName 
                << " VertexCounterName: " << fmt_s % m_VertexCounterName 
                << " TrackInputSelection: " << fmt_s % m_TrackInputSelectionName 
                << " VertexInputSelection: " << fmt_s % m_VertexInputSelectionName 
                << " OutputContainer: " << fmt_s % m_OutputContainerName 
                << " AbsZCut: " << fmt_g % m_AbsZCut 
                << " RCut: " << fmt_g % m_RCut << endmsg;
    }
    m_TrackCounter  = LHCb::LumiCounters::counterKeyToType( m_TrackCounterName );
    m_VertexCounter = LHCb::LumiCounters::counterKeyToType( m_VertexCounterName );

    if ( m_TrackCounter == LHCb::LumiCounters::Unknown ) {
        info() << "LumiCounter not found with name: " << m_TrackCounterName << endmsg;
    } else if ( msgLevel( MSG::DEBUG ) ) {
        debug() << m_TrackCounterName << " value: " << m_TrackCounter << endmsg;
    }
    if ( m_VertexCounter == LHCb::LumiCounters::Unknown ) {
        info() << "LumiCounter not found with name: " << m_VertexCounterName
               << endmsg;
    } else if ( msgLevel( MSG::DEBUG ) ) {
        debug() << m_VertexCounterName << " value: " << m_VertexCounter
                << endmsg;
    }
    return StatusCode::SUCCESS;
}

//=============================================================================
constexpr struct  position_t {
    template <typename T> 
    Gaudi::XYZPoint operator()(const T* t) const 
    { return  t->position(); }
} position {};

//=============================================================================
StatusCode LumiCountVeloWithZRCuts::execute()
{
    static const auto predicate = compose( [&](const Gaudi::XYZPoint& p) { return fiducial(p); } // point -> bool
                                         , position     // object -> point
                                         ); 

    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Execute" << endmsg;

    auto* sums =
        getOrCreate<LHCb::HltLumiSummary, LHCb::HltLumiSummary>( m_OutputContainerName );
    setFilterPassed( true );

    sums->addInfo( m_TrackCounter,  count_if<LHCb::Tracks>(m_TrackInputSelectionName, std::cref(predicate) )); // add track  counter
    sums->addInfo( m_VertexCounter, count_if<LHCb::RecVertices>(m_VertexInputSelectionName, std::cref(predicate) )); // add vertex counter

    if ( msgLevel( MSG::DEBUG ) ) {
        debug() << "There are " << sums->info(m_TrackCounter,-1) << " VELO tracks   inside abs(z)<"
                << m_AbsZCut << " and R<" << m_RCut << " in "
                << m_TrackInputSelectionName << endmsg;
        debug() << "There are " << sums->info(m_VertexCounter,-1) << " VELO vertices inside abs(z)<"
                << m_AbsZCut << " and R<" << m_RCut << " in "
                << m_VertexInputSelectionName << endmsg;
    }

    return StatusCode::SUCCESS;
}
