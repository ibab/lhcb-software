// $Id: HltCosmicsOT.cpp,v 1.2 2009-09-09 14:22:12 jpalac Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"
//#include "GaudiKernel/AlgFactory.h"

#include "HltCosmicsOT.h"
#include "Event/OTLiteTime.h"

#include <boost/foreach.hpp>

namespace LHCb
{
class RawEvent;
}

//-----------------------------------------------------------------------------
// Implementation file for class : HltCosmicsOT
//
// 2009-07-16 : Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
//
// 2009-09-08 : adapted by Stephan Nies <snies@cern.ch>
//                     and Jesko Merkel <jmerkel@cern.ch>
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( HltCosmicsOT )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltCosmicsOT::HltCosmicsOT( const std::string& name, ISvcLocator* pSvcLocator )
    : HltAlgorithm( name, pSvcLocator ), m_selections( *this )
{
    declareProperty( "OTDecoderName", m_otdecodername = "OTRawBankDecoder" );
    declareProperty( "MinNumberOfHits", m_minhits = 6 );
    declareProperty( "TolStraws", m_tolstraws = 3 );
    m_selections.declareProperties();
}

//=============================================================================
// Destructor
//=============================================================================
HltCosmicsOT::~HltCosmicsOT()
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltCosmicsOT::initialize()
{
    StatusCode sc = HltAlgorithm::initialize();
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm
    // m_selections.retrieveSelections();
    m_selections.registerSelection();

    debug() << "==> Initialize" << endmsg;

    m_otdecoder = tool<IOTRawBankDecoder>( m_otdecodername );

    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltCosmicsOT::execute()
{
    if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Execute" << endmsg;

    if ( m_otdecoder->totalNumberOfHits() >= m_minhits ) {
        // loop over all modules, decode them, and return early if we find
        // hits in two neighbouring straws of a module
        // (reasoning: decoding modules without hits is cheap, and we stop
        // at the first one with hits which satisfies the neighbouring
        // straws criterion... if we have lots of noise, we may have to
        // reconsider the strategy)
        for ( unsigned i = 0; i < 432; ++i ) {
            // covert i to quadruple (station, layer, quadrant, module):
            // 1st 2 bits of i are lay, 2nd 2 bits are qua, remaining bits
            // are sta + 3 * module (starting at LSB)
            // numbering conventions are such that sta and mod start at 1
            LHCb::OTLiteTimeRange hits = m_otdecoder->decodeModule(
                LHCb::OTChannelID( 1 + ( ( i >> 4 ) % 3 ), i & 3, ( i >> 2 ) & 3,
                                   1 + ( i / ( 4 * 4 * 3 ) ), 0 ) );
            // check for at least 2 hits
            if ( hits.begin() + 1 >= hits.end() ) continue;
            // calculate sum of straw numbers
            unsigned long sum =
                std::accumulate( std::begin( hits ), std::end( hits ), 0ul,
                                 []( unsigned long x, const LHCb::OTLiteTime& hit ) {
                    return x + hit.channel().straw();
                } );
            sum &= 0x7ful; // remainder when dividing by 128
            if ( sum < m_tolstraws || 0x80ul - sum < m_tolstraws ) {
                // we have hits in two neighbouring straws, return early
                setFilterPassed( true );
                m_selections.output()->setDecision( true );
                if ( msgLevel( MSG::DEBUG ) )
                    debug() << "OT activity found!" << endmsg;
                return StatusCode::SUCCESS;
            }
        }
    }

    // default: we don't pass the filter
    setFilterPassed( false );
    m_selections.output()->setDecision( false );
    return StatusCode::SUCCESS;
}

