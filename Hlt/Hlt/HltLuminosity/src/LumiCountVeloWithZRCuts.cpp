#include <functional>
#include "GaudiKernel/AlgFactory.h"

#include "Event/Track.h"
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"

#include "LumiCountVeloWithZRCuts.h"
#include "boost/format.hpp"

using namespace LHCb;

// composition implements, given f and g, their composition  f( g(x1), x2, x3, .. )
template< class F, class G > struct Composition {
    F f;
    G g;

    template< class _F, class _G >
    constexpr Composition( _F&& f, _G&& g ) : f( std::forward<_F>(f) ), g( std::forward<_G>(g) ) { }

    template< class X, class ...Y >
    decltype( f( g(std::declval<X>()), std::declval<Y>()... ) )
    operator() ( X&& x, Y&& ...y ) {
        return f( g(std::forward<X>(x)), std::forward<Y>(y)... );
    }
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

    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Initialize" << endmsg;
    if ( msgLevel( MSG::DEBUG ) )
        debug() << "TrackCounterName: "
                << boost::format( "%20s" ) % m_TrackCounterName << " "
                << "VertexCounterName: "
                << boost::format( "%20s" ) % m_VertexCounterName << " "
                << "TrackInputSelection: "
                << boost::format( "%20s" ) % m_TrackInputSelectionName << " "
                << "VertexInputSelection: "
                << boost::format( "%20s" ) % m_VertexInputSelectionName << " "
                << "OutputContainer: "
                << boost::format( "%20s" ) % m_OutputContainerName << " "
                << "AbsZCut: " << boost::format( "%20g" ) % m_AbsZCut << " "
                << "RCut: " << boost::format( "%20g" ) % m_RCut << endmsg;
    m_TrackCounter = LHCb::LumiCounters::counterKeyToType( m_TrackCounterName );
    m_VertexCounter = LHCb::LumiCounters::counterKeyToType( m_VertexCounterName );

    if ( m_TrackCounter == LHCb::LumiCounters::Unknown ) {
        info() << "LumiCounter not found with name: " << m_TrackCounterName
               << endmsg;
    } else {
        if ( msgLevel( MSG::DEBUG ) )
            debug() << m_TrackCounterName << " value: " << m_TrackCounter << endmsg;
    }
    if ( m_VertexCounter == LHCb::LumiCounters::Unknown ) {
        info() << "LumiCounter not found with name: " << m_VertexCounterName
               << endmsg;
    } else {
        if ( msgLevel( MSG::DEBUG ) )
            debug() << m_VertexCounterName << " value: " << m_VertexCounter
                    << endmsg;
    }
    return StatusCode::SUCCESS;
}

//=============================================================================
constexpr struct  position_ {
    template <typename T> Gaudi::XYZPoint operator()(const T* t) const { return  t->position(); }
} position {} ;

//=============================================================================
StatusCode LumiCountVeloWithZRCuts::execute()
{
    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Execute" << endmsg;

    LHCb::HltLumiSummary* sums =
        getOrCreate<HltLumiSummary, HltLumiSummary>( m_OutputContainerName );
    setFilterPassed( true );

    auto predicate = compose( std::bind(&LumiCountVeloWithZRCuts::fiducial, this, std::placeholders::_1) // point -> bool
                            , position     // object -> point
                            ); 

    LHCb::Tracks* trCands = getIfExists<LHCb::Tracks>( m_TrackInputSelectionName );
    int nTr = trCands ? std::count_if( std::begin( *trCands ), std::end( *trCands ), predicate )
                      : -1;
    sums->addInfo( m_TrackCounter, nTr ); // add track  counter

    LHCb::RecVertices* vxCands =
        getIfExists<LHCb::RecVertices>( m_VertexInputSelectionName );
    int nVx = vxCands ? std::count_if( std::begin( *vxCands ), std::end( *vxCands ), predicate )
                      : -1;
    sums->addInfo( m_VertexCounter, nVx ); // add vertex counter

    if ( msgLevel( MSG::DEBUG ) ) {
        debug() << "There are " << nTr << " VELO tracks   inside abs(z)<"
                << m_AbsZCut << " and R<" << m_RCut << " in "
                << m_TrackInputSelectionName << endmsg;
        debug() << "There are " << nVx << " VELO vertices inside abs(z)<"
                << m_AbsZCut << " and R<" << m_RCut << " in "
                << m_VertexInputSelectionName << endmsg;
    }

    return StatusCode::SUCCESS;
}
