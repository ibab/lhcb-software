// Include files
#include "boost/format.hpp"
#include <algorithm>

#include "Event/RawEvent.h"

// local
#include "HltTrackReportsDecoder.h"
#include "HltTrackReportsWriter.h"
#include "HltTrackingCoder.h"

#include "Event/Track.h"

using namespace LHCb;

namespace
{
int sourceID( const RawBank& bank )
{
     return bank.sourceID() >> HltTrackReportsWriter::kSourceID_BitShift;
}
int seq( const RawBank& bank )
{
    return bank.sourceID() & HltTrackReportsWriter::kSourceID_MinorMask;
}
constexpr struct select2nd_ {
        template <typename U, typename V> 
        const V& operator()(const std::pair<U,V>& p) const { return p.second; }
} select2nd{};

}

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackReportsDecoder
//
// The actual decoding of tracks is delegated
// to the functions in HltTrackingCoder.
//
// 2014-01-16 : Sebastian Neubert
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackReportsDecoder )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackReportsDecoder::HltTrackReportsDecoder( const std::string& name,
                                                ISvcLocator* pSvcLocator )
    : Decoder::AlgBase( name, pSvcLocator ), m_callcount( 0 )
{
    // new for decoders, initialize search path, and then call the base method
    m_rawEventLocations = {LHCb::RawEventLocation::Trigger,
                           LHCb::RawEventLocation::Copied,
                           LHCb::RawEventLocation::Default};
    initRawEventSearch();

    declareProperty( "Output2SourceId", m_map = { { "Hlt/Track/Velo", 1 }, 
                                                  { "Hlt/Track/VeloTTHPT", 2 }, 
                                                  { "Hlt/Track/ForwardHPT", 4 } } );
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackReportsDecoder::initialize()
{
    StatusCode sc = Decoder::AlgBase::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

    // Check validity of source IDs: max is 7, must be unique...
    if ( std::any_of( std::begin(m_map), std::end(m_map), [](const std::pair<std::string,unsigned>& entry ) {
            return entry.second > HltTrackReportsWriter::kSourceID_Max;
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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
StatusCode HltTrackReportsDecoder::execute()
{
    ++m_callcount;
    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Execute" << endmsg;

    // get inputs
    LHCb::RawEvent* rawEvent = findFirstRawEvent();
    if ( !rawEvent ) return Error( " No RawEvent found. No HltTracks created." );

    // ----------------------------------------------------------
    // get the bank from RawEvent
    // ----------------------------------------------------------
    std::vector<RawBank*> hltTrackReportsRawBanks =
        rawEvent->banks( RawBank::HltTrackReports );

    //warning() << hltTrackReportsRawBanks.size() << " raw banks found" << endmsg;
    //for ( const auto & bank : hltTrackReportsRawBanks) {
    //  warning() << "raw sourceID: " << bank->sourceID() << endmsg;
    //  warning() << "SourceID: " << sourceID(*bank) << endmsg;
    //}

    // check for bad banks:
    hltTrackReportsRawBanks.erase(
        std::remove_if( std::begin( hltTrackReportsRawBanks ),
                        std::end( hltTrackReportsRawBanks ), [=]( const RawBank* bank ) {
            if ( bank->magic() != RawBank::MagicPattern ) {
                Warning( " HltTrackReports RawBank has wrong magic number. Skipped ",
                       StatusCode::SUCCESS, 20 );
                return true;
            }
            return false;
        } ),
        std::end( hltTrackReportsRawBanks ) );

    // sort by sourceID ( which is 'origin << somebits | sequential number' )
    std::sort( std::begin( hltTrackReportsRawBanks ),
               std::end( hltTrackReportsRawBanks ), [=]( const RawBank* lhs, const RawBank* rhs ) {
                    return lhs->sourceID() < rhs->sourceID();
               } );

    constexpr struct cmp_sourceID_t {
        bool operator()(int id, const RawBank* bank) const { return id < sourceID( *bank ); }
        bool operator()(const RawBank* bank, int id) const { return sourceID( *bank ) < id; }
    } cmp_sourceID{};


    // TODO: add some counters to track how many tracks per source ID per event...
    for ( const auto & entry : m_map ) {

        auto range = std::equal_range( std::begin( hltTrackReportsRawBanks ), 
                                       std::end( hltTrackReportsRawBanks ), entry.second, cmp_sourceID );
        // if there is a valid bank, create the output -- even if it is an empty bank...
        // (which results in an empty output ;-). If there is no bank, then do NOT
        // create the output...
        if ( range.first == range.second ) {
	  //warning() << "Empty bank " << entry.first << endmsg;
	  continue;
	}

        // create output container and put it on TES
        auto  outputTracks = new Tracks();
        put( outputTracks, entry.first );
        // verify all present in expected order...
        bool ok = true;
        std::accumulate( range.first, range.second, 0, [&ok]( int i, const RawBank* b ) {
            ok = ok && i == seq( *b );
            return ++i;
        } );
        if ( !ok )
            return Error( "Missing Sequential HltTrackReports RawBank part - quiting.",
                          StatusCode::SUCCESS, 100 );

        // figure out the total size -- zero length (i.e. empty) bank is NOT an error
        unsigned int bankSize = std::accumulate( range.first,
                                                 range.second, 0u,
                                                 []( unsigned int s, const RawBank* bank ) {
            return s +
                   std::distance( bank->begin<unsigned int>(), bank->end<unsigned int>() );
        } );
        if ( std::distance( range.first, range.second)  == 1 ) {
            // avoid allocating and copying the rawbank if there is only a single one...
            // do the actual decoding: see HltTrackingCoder.cpp
            decodeTracks( (*range.first)->data(), bankSize, *outputTracks );
        } else {
            // concatenate banks into a local array
            std::unique_ptr<unsigned int> completeBank{new unsigned int[bankSize]};
            auto p = std::accumulate( range.first, range.second, completeBank.get(),
                             []( unsigned int* p, const RawBank* bank ) {
                return std::copy( bank->begin<unsigned int>(), bank->end<unsigned int>(), p );
            } );
            assert( std::distance( completeBank.get(), p ) == bankSize ); 
            // do the actual decoding: see HltTrackingCoder.cpp
            decodeTracks( completeBank.get(), bankSize, *outputTracks );
        }

	counter( entry.first ) += outputTracks->size();
        // for debug purposes print the contents of the outputLocation
        if ( msgLevel( MSG::VERBOSE ) ) {
            verbose() << "----------------------------------------\n";
            verbose() << "Decoded event " << m_callcount << endmsg;
            verbose() << "Decoding source id " << entry.second << " into TES " << entry.first <<endmsg;
            verbose() << outputTracks->size() << " Resurrected tracks: \n";
            std::for_each( std::begin(*outputTracks), std::end(*outputTracks),
                [&](const LHCb::Track* track) {
                verbose() << *track << endmsg;
                verbose() << "LHCbIDs: [\n";
                std::for_each( std::begin(track->lhcbIDs()), std::end(track->lhcbIDs()),
                               [&](const LHCb::LHCbID& id) {  verbose() << id << ",\n";} );
                verbose() << "]" << endmsg;
            } );
        }
    }
    return StatusCode::SUCCESS;
}
#pragma GCC diagnostic pop
