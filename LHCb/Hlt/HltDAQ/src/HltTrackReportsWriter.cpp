// $Id$
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

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

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackReportsWriter::HltTrackReportsWriter( const std::string& name,
                                              ISvcLocator* pSvcLocator )
    : GaudiAlgorithm( name, pSvcLocator ), m_callcount( 0 )
{

    declareProperty( "InputHltTrackLocation",
                     m_inputHltTrackLocation = "Hlt/Track/Velo" );
    declareProperty( "OutputRawEventLocation",
                     m_outputRawEventLocation = LHCb::RawEventLocation::Default );
    declareProperty( "SourceID", m_sourceID = kSourceID_Dummy );
}
//=============================================================================
// Destructor
//=============================================================================
HltTrackReportsWriter::~HltTrackReportsWriter()
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackReportsWriter::initialize()
{
    StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Initialize" << endmsg;

    // We need space for the bitshift to identify split up banks
    if ( m_sourceID > kSourceID_Max ) {
        m_sourceID = m_sourceID & kSourceID_Max;
        return Error( "Illegal SourceID specified; maximal allowed value is 7",
                      StatusCode::FAILURE, 50 );
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

    // get input
    const LHCb::Tracks* inputTracks =
        getIfExists<LHCb::Tracks>( m_inputHltTrackLocation );
    if ( !inputTracks ) {
        return Warning( " No tracks at " + m_inputHltTrackLocation.value(),
                        StatusCode::SUCCESS, 0 );
    }

    // get output
    RawEvent* rawEvent = getIfExists<RawEvent>( m_outputRawEventLocation );
    if ( !rawEvent ) {
        return Error( " No RawEvent at " + m_outputRawEventLocation.value(),
                      StatusCode::SUCCESS, 20 );
    }

    if ( msgLevel( MSG::VERBOSE ) ) {
        verbose() << "----------------------------------------\n";
        verbose() << " Written event " << m_callcount << endmsg;
        verbose() << " Input tracks: \n";
        // collect tracks pointers in local vector
        for ( const auto& Tr : *inputTracks ) {
            verbose() << *Tr << endmsg;
            // also dump IDs
            verbose() << "LHCbIDs: [\n";
            for ( const auto& id : Tr->lhcbIDs() ) {
                verbose() << id << ",\n";
            }
            verbose() << "]" << endmsg;
        }
        verbose() << "\n----------------------------------------";
        verbose() << endmsg;
    }

    // compose the bank body
    std::vector<unsigned int> bankBody;

    // calling core encoder function see src/core/HltTrackingCoder.cxx
    encodeTracks( inputTracks, bankBody );

    // RawBank is limited in size to 65535 bytes i.e. 16383 words; be conservative cut it
    // off at a smaller limit.
    // Save in chunks if exceed this size.
    int nBank = ( bankBody.size() - 1 ) / 16300 + 1;
    if ( nBank > kSourceID_MinorMask ) {
        return Error( "HltTrackReports too long to save", StatusCode::SUCCESS, 50 );
    }
    for ( int iBank = 0; iBank < nBank; ++iBank ) {
        int ioff = iBank * 16300;
        auto isize = std::min( bankBody.size() - ioff, 16300ul );
        rawEvent->addBank( iBank | ( m_sourceID << kSourceID_BitShift ),
                           RawBank::HltTrackReports, kVersionNumber,
                           { &bankBody[ioff], &bankBody[ioff + isize] } );
    }
    if ( nBank > 1 ) {
        if ( msgLevel( MSG::DEBUG ) )
            debug() << "HltTrackReports is huge. Saved in " << nBank
                    << " separate RawBanks " << endmsg;
        // std::ostringstream mess;
        // mess << "HltTrackReports is huge. Saved in " << nBank << " separate RawBanks ";
        // Warning( mess.str(), StatusCode::SUCCESS, 10 );
    }

    // if(m_callcount==251) {
    //  std::cout << "RAWBANK:" << std::endl;
    //  std::copy(bankBody.begin(), bankBody.end(), std::ostream_iterator<unsigned
    // int>(std::cout, "\n"));
    //  std::cout.flush();
    // }

    if ( msgLevel( MSG::VERBOSE ) ) {
        verbose() << " Output:  ";
        verbose() << " VersionNumber= " << kVersionNumber;
        verbose() << " SourceID= " << m_sourceID;
        // std::vector<unsigned int>::const_iterator i=bankBody.begin();
        // verbose() << " configuredTCK = " << *i++ << " " ;
        // verbose() << " taskID = " << *i++ << " " ;
        // for( ; i!=bankBody.end();++i) {
        //   verbose() << HltDecReport(*i).intDecisionID() << "-" <<
        // HltDecReport(*i).decision() << " ";
        // }
        verbose() << endmsg;
    }
    return StatusCode::SUCCESS;
}
