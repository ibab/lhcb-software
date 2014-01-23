// $Id: HltL0MuonCandidates.cpp,v 1.17 2010-08-13 12:04:02 graven Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include <algorithm>
#include "Event/L0DUBase.h"
#include "Event/L0DUReport.h"
#include "Event/L0MuonCandidate.h"

// local
#include "HltL0MuonCandidates.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltL0MuonCandidates
//
// 2008-12-06 : Gerhard Raven
// based on HltL0MuonPrepare by Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltL0MuonCandidates );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltL0MuonCandidates::HltL0MuonCandidates( const std::string& name,
                                          ISvcLocator* pSvcLocator )
    : HltAlgorithm( name, pSvcLocator, false )
    , m_selection( *this )
    , m_maker( nullptr )
    , m_pt( nullptr )
    , m_ptMax( nullptr )
{
    declareProperty( "L0DULocation", m_l0Location = L0DUReportLocation::Default );
    declareProperty( "L0Channel", m_l0Channel );
    // (re)set property from HltAlgorithm
    setProperty( "HistoDescriptor", "{ 'Pt'    : ('Pt',   0.,6000.,100)"
                                    ", 'PtMax' : ('PtMax',0.,6000.,100)"
                                    "}" );
    m_selection.declareProperties( std::map<int, std::string>{
        {1, std::string{"TES:"} + L0MuonCandidateLocation::Default}} );
}
//=============================================================================
// Destructor
//=============================================================================
HltL0MuonCandidates::~HltL0MuonCandidates()
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltL0MuonCandidates::initialize()
{
    StatusCode sc = HltAlgorithm::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorith

    m_selection.retrieveSelections();
    m_selection.registerSelection();

    m_maker = tool<IMuonSeedTool>( "MuonSeedTool" );
    // make sure counters exist
    counter( "#input" );
    // declareInfo("#accept",counter("#accept"),std::string("Events accepted by ") +
    // name());
    // declareInfo("#input",&counter("#input"),std::string("Candidates seen by ") +
    // name());
    // declareInfo("#candidates accepted",&counter("#candidates
    // accepted"),std::string("Candidates accepted by ") + name());
    m_pt = book( "Pt" );
    m_ptMax = book( "PtMax" );

    return StatusCode::SUCCESS;
}

std::vector<int>
HltL0MuonCandidates::generateCutList( const LHCb::L0DUChannel& channel )
{
    static const std::vector<std::string> map{"Muont1(Pt)", "DiMuon(Pt)"};

    const auto& conditions = channel.elementaryConditions();
    std::vector<int> cuts;
    for ( const auto& condition : conditions ) {
        const auto& data = condition.second->data()->name();
        for ( const auto& i : map ) {
            if ( i == data ) cuts.push_back( condition.second->threshold() );
        }
    }
    return cuts;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode HltL0MuonCandidates::execute()
{

    std::vector<int> cuts;
    if ( m_l0Channel != "AllMuon" ) {
        //@TODO: only update cuts on L0 TCK change...
        LHCb::L0DUReport* l0 = get<L0DUReport>( m_l0Location );
        const auto& channels = l0->configuration()->channels();
        auto channel = channels.find( m_l0Channel );
        if ( channel == channels.end() ) {
            error() << "could not find requested l0 channel " << m_l0Channel
                    << endmsg;
            return StatusCode::FAILURE;
        }
        //@TODO: check if channel is enabled!!
        cuts = generateCutList( *( channel->second ) );
    }

    Tracks* muons = new Tracks();
    put( muons, "Hlt/Track/" + m_selection.output()->id().str() );

    if ( cuts.empty() && m_l0Channel != "AllMuon" ) {
        return Warning( " L0 channel " + m_l0Channel + " does not use any known "
                                                       "type of l0MuonCandidate?? "
                                                       "-- no candidates converted!",
                        StatusCode::SUCCESS, 1 );
    }

    assert( cuts.size() == 1 || ( cuts.empty() && m_l0Channel == "AllMuon" ) );

    double ptMax = -1.;
    for ( auto l0muon : *m_selection.input<1>() ) {
        bool pass = ( cuts.empty() || ( ( l0muon->encodedPt() & 0x7F ) >
                                        cuts[0] ) ); // encodedPt is signed
        if ( !pass ) continue;

        if ( msgLevel( MSG::DEBUG ) )
            debug() << "l0pt " << l0muon->pt() << " l0encodedPt "
                    << l0muon->encodedPt() << endmsg;
        if ( checkClone( l0muon ) ) {
            if ( msgLevel( MSG::DEBUG ) ) debug() << "is clone " << endmsg;
            continue;
        }
        fill( m_pt, l0muon->pt(), 1. );
        if ( l0muon->pt() > ptMax ) ptMax = l0muon->pt();
        std::unique_ptr<Track> track( new Track() );
        StatusCode sc = m_maker->makeTrack( *l0muon, *track );
        if ( sc.isFailure() ) return sc;
        // TODO: pushes both into IHltDataSvc and into TES (muons)
        //      this should be possible to do through the IHltDataSvc...
        m_selection.output()->push_back( track.get() );
        muons->insert( track.release() );
    }
    if ( !m_selection.output()->empty() ) fill( m_ptMax, ptMax, 1. );

    counter( "#input" ) += m_selection.input<1>()->size();

    if ( msgLevel( MSG::DEBUG ) ) {
        debug() << "# Input: " << m_selection.input<1>()->size()
                << " -> # Output: " << m_selection.output()->size() << endmsg;
        printInfo( " Muons ", *m_selection.output() ); //#($)*))@*)@ requires
                                                       //outputlevel debut, but prnts
                                                       //at INFO...
    }

    return StatusCode::SUCCESS;
}

namespace
{
class isInMuonStation : public std::unary_function<bool, LHCb::LHCbID>
{
  public:
    isInMuonStation( unsigned int station ) : m_station( station )
    {
    }
    bool operator()( const LHCb::LHCbID& id )
    {
        return id.isMuon() && id.muonID().station() == m_station;
    }

  private:
    unsigned int m_station;
};
}

//=============================================================================
bool HltL0MuonCandidates::checkClone( const L0MuonCandidate* muon )
{
    auto tileM1 = muon->muonTileIDs( 0 ).front();
    auto tileM2 = muon->muonTileIDs( 1 ).front();

    for ( auto t : *m_selection.output() ) {
        const auto& ids = t->lhcbIDs();

        auto oldTileM1 =
            std::find_if( ids.begin(), ids.end(), isInMuonStation( 0 ) );
        if ( oldTileM1 == ids.end() || !( *oldTileM1 == tileM1 ) )
            continue; // not found, or no match...

        auto oldTileM2 =
            std::find_if( ids.begin(), ids.end(), isInMuonStation( 1 ) );
        if ( oldTileM2 != ids.end() && *oldTileM2 == tileM2 )
            return true; // found, and match...
    }
    return false;
}
