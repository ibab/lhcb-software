// $Id$
// Include files

#include "Event/RawEvent.h"
#include "Event/Track.h"
#include "HltTrackingCoder.h"
#include <functional>
#include <algorithm>

// local
#include "HltTrackReportsWriter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackReportsWriter
//
// 2014-1-10 : Sebastian Neubert
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackReportsWriter )

using namespace LHCb;
namespace {

constexpr struct select2nd_ {
        template <typename U, typename V>
        const V& operator()(const std::pair<U,V>& p) const { return p.second; }
} select2nd{};

}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackReportsWriter::HltTrackReportsWriter( const std::string& name,
                                              ISvcLocator* pSvcLocator )
    : GaudiAlgorithm( name, pSvcLocator ), m_callcount{ 0u }
{
    declareProperty( "Input2SourceId", m_map = { { "Hlt/Track/Velo",          kSourceID_Hlt1_Velo }
                                               , { "Hlt/Track/ForwardHPT", kSourceID_Hlt1_ForwardHPT}
	                                             , { "Hlt/Track/VeloTTHPT", kSourceID_Hlt1_VeloTT }
                                               } );
    declareProperty( "OutputRawEventLocation",
                     m_outputRawEventLocation = LHCb::RawEventLocation::Default );
    declareProperty( "WriteStates", m_writeStates = true );
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackReportsWriter::initialize()
{
    StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Initialize" << endmsg;

    // Check validity of source IDs: max is 7, must be unique...
    if ( std::any_of( std::begin(m_map), std::end(m_map), [](const std::pair<std::string,int>& entry ) {
            return entry.second > kSourceID_Max || entry.second<0;
    }) ) {
        return Error( "Illegal SourceID specified; maximal allowed value is 7",
                      StatusCode::FAILURE );
    }

    std::vector<unsigned> ids; ids.reserve(m_map.size());
    std::transform( std::begin(m_map), std::end(m_map), std::back_inserter(ids), select2nd );
    std::sort(std::begin(ids), std::end(ids));
    if ( std::unique(std::begin(ids),std::end(ids)) != std::end(ids) ) {
        return Error( "Duplicate SourceID specified", StatusCode::FAILURE );
    }

    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTrackReportsWriter::execute()
{
    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Execute" << endmsg;
    ++m_callcount;

    // get output
    RawEvent* rawEvent = getIfExists<RawEvent>( m_outputRawEventLocation );
    if ( !rawEvent ) {
        return Error( " No RawEvent at " + m_outputRawEventLocation.value(),
                      StatusCode::SUCCESS, 20 );
    }

    for (const auto& entry : m_map) convert( entry.first, entry.second, rawEvent ) ;
    return StatusCode::SUCCESS;
}

void HltTrackReportsWriter::convert(const std::string& location, unsigned sourceID, RawEvent* rawEvent) const
{
        // get input
        const LHCb::Tracks* inputTracks = getIfExists<LHCb::Tracks>(location);

        if ( !inputTracks ) {
            Warning( " No container at " + location, StatusCode::SUCCESS, 0 ).ignore();
            return;
        }

        if ( msgLevel( MSG::VERBOSE ) ) {
            verbose() << "----------------------------------------\n";
            verbose() << " Written event " << m_callcount << endmsg;
            verbose() << " Input tracks at " << location << "  -> source ID " << sourceID <<"\n";
            for ( const auto& i : *inputTracks ) {
                verbose() << *i << endmsg;
                // also dump IDs
                verbose() << "LHCbIDs: [\n";
                for ( const auto& id : i->lhcbIDs() ) verbose() << id << ",\n";
                verbose() << "]" << endmsg;
            }
            verbose() << "\n----------------------------------------" << endmsg;
        }

        // compose the bank body
        std::vector<unsigned int> bankBody;

        // calling core encoder function see src/core/HltTrackingCoder.cxx
        encodeTracks( *inputTracks, bankBody, m_writeStates );

        // RawBank is limited in size to 65535 bytes i.e. 16383 words; be conservative cut it
        // off at a smaller limit.
        // Save in chunks if exceed this size.
        // make sure that for an empty bank we write 1 bank...
        int nBank = 1 + ( bankBody.empty() ? 0 : ( ( bankBody.size() - 1 ) / 16300 ) ) ;
        if ( nBank > kSourceID_MinorMask ) {
            Warning( "HltTrackReports too large to save", StatusCode::SUCCESS, 50 ).ignore();
            return;
        }
        for ( int iBank = 0; iBank < nBank; ++iBank ) {
            int ioff = iBank * 16300;
            auto isize = std::min( bankBody.size() - ioff, 16300ul );
            rawEvent->addBank( ( sourceID << kSourceID_BitShift ) | iBank,
                               RawBank::HltTrackReports, kVersionNumber,
                               { &bankBody[ioff], &bankBody[ioff + isize] } );
        }
        if ( UNLIKELY( msgLevel(MSG::DEBUG) ) &&  nBank > 1 ) {
                debug() << "HltTrackReports is huge. Saved in " << nBank
                        << " separate RawBanks " << endmsg;
        }
}
