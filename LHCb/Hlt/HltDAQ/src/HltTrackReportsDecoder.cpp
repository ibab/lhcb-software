// Include files
#include "boost/format.hpp"
#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

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

    declareProperty( "OutputLocation", m_outputLocation = "/Hlt/Track/Velo" );

    declareProperty( "SourceID", m_sourceID = HltTrackReportsWriter::kSourceID_Dummy );
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackReportsDecoder::initialize()
{
    StatusCode sc = Decoder::AlgBase::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Initialize" << endmsg;

    if ( m_sourceID > HltTrackReportsWriter::kSourceID_Max ) {
        m_sourceID = m_sourceID & HltTrackReportsWriter::kSourceID_Max;
        return Error( "Illegal SourceID specified (too large);", StatusCode::FAILURE,
                      50 );
    }
    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
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
    hltTrackReportsRawBanks.erase(
        std::remove_if( std::begin( hltTrackReportsRawBanks ),
                        std::end( hltTrackReportsRawBanks ), [=]( const RawBank* bank ) {
            if ( m_sourceID != sourceID( *bank ) )
                return true; // Not our major SourceID i.e. different tracks
            if ( bank->magic() != RawBank::MagicPattern ) {
                Error( " HltTrackReports RawBank has wrong magic number. Skipped ",
                       StatusCode::SUCCESS, 20 );
                return true;
            }
            return false;
        } ),
        std::end( hltTrackReportsRawBanks ) );
    // collect possibly split up banks -- first put them into the right order
    std::sort( std::begin( hltTrackReportsRawBanks ), std::end( hltTrackReportsRawBanks ),
               []( const RawBank* lhs,
                   const RawBank* rhs ) { return seq( *lhs ) < seq( *rhs ); } );
    // verify all present in expected order...
    bool ok = true;
    std::accumulate( std::begin( hltTrackReportsRawBanks ),
                     std::end( hltTrackReportsRawBanks ), 0,
                     [&ok]( int i, const RawBank* b ) {
        ok = ok && i == seq( *b );
        return ++i;
    } );
    if ( !ok )
        return Error( "Missing Sequential HltTrackReports RawBank part - quiting.",
                      StatusCode::SUCCESS, 100 );

    // if there is a valid bank, create the output -- even if it is an empty bank...
    // (which results in an empty output ;-)

    // create output container and put it on TES
    // FIXME:TODO: if there is already something, that SHOULD be a configuration error,
    // and hence we should abort!!!
    LHCb::Tracks* outputTracks = nullptr;
    try
    {
        outputTracks = getOrCreate<LHCb::Tracks, LHCb::Tracks>( m_outputLocation );
        // put( outputTracks, m_outputLocation );
    }
    catch ( GaudiException ex )
    {
        warning() << "Failed to create output location " << m_outputLocation << endmsg;
        warning() << ex.message() << endmsg;
        return StatusCode::FAILURE;
    }

    // if(!outputTracks->empty()) {
    // if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    //    debug() << "outputLocation already contains tracks. Skipping decoding." <<
    // endmsg;
    //  return StatusCode::SUCCESS;
    //}

    // figure out the total size -- zero length (i.e. empty) bank is NOT an error
    unsigned int bankSize = std::accumulate( std::begin( hltTrackReportsRawBanks ),
                                             std::end( hltTrackReportsRawBanks ), 0u,
                                             []( unsigned int s, const RawBank* bank ) {
        return s +
               std::distance( bank->begin<unsigned int>(), bank->end<unsigned int>() );
    } );
    if ( hltTrackReportsRawBanks.size() == 1 ) {
        // avoid allocating and copying the rawbank if there is only a single one...
        // do the actual decoding: see HltTrackingCoder.cpp
        decodeTracks( hltTrackReportsRawBanks.front()->data(), bankSize, outputTracks );
    } else {
        // concatenate banks into a local array
        std::unique_ptr<unsigned int> completeBank{new unsigned int[bankSize]};
        std::accumulate( std::begin( hltTrackReportsRawBanks ),
                         std::end( hltTrackReportsRawBanks ), completeBank.get(),
                         []( unsigned int* p, const RawBank* bank ) {
            return std::copy( bank->begin<unsigned int>(), bank->end<unsigned int>(), p );
        } );
        // do the actual decoding: see HltTrackingCoder.cpp
        decodeTracks( completeBank.get(), bankSize, outputTracks );
    }

    // for debug purposes print the contents of the outputLocation
    if ( msgLevel( MSG::VERBOSE ) ) {
        verbose() << "----------------------------------------\n";
        verbose() << "Decoded event " << m_callcount << endmsg;
        verbose() << "Decoding source id " << m_sourceID << " into TES " << m_outputLocation <<endmsg;
        verbose() << outputTracks->size() << " Resurrected tracks: \n";
        counter( "#Tracks" ) += outputTracks->size();
        for ( const auto& Tr : *outputTracks ) {
            verbose() << *Tr << endmsg;
            // also dump IDs
            verbose() << "LHCbIDs: [\n";
            for ( const auto& id : Tr->lhcbIDs() ) verbose() << id << ",\n";
            verbose() << "]" << endmsg;
        }
    }
    return StatusCode::SUCCESS;
}
