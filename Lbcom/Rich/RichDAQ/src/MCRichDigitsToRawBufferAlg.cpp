
/** @file MCRichDigitsToRawBufferAlg.cpp
 *
 *  Implementation file for RICH DAQ algorithm : MCRichDigitsToRawBufferAlg
 *
 *  CVS Log :-
 *  $Id: MCRichDigitsToRawBufferAlg.cpp,v 1.6 2004-10-13 09:16:27 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.5  2004/07/27 13:46:06  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "MCRichDigitsToRawBufferAlg.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MCRichDigitsToRawBufferAlg>          s_factory ;
const        IAlgFactory& MCRichDigitsToRawBufferAlgFactory = s_factory ;

// Standard constructor
MCRichDigitsToRawBufferAlg::MCRichDigitsToRawBufferAlg( const std::string& name,
                                                        ISvcLocator* pSvcLocator )
  : RichAlgBase ( name, pSvcLocator )
{

  declareProperty( "RawBufferLocation",
                   m_rawBuffLoc = RawBufferLocation::Default );
  declareProperty( "MCRichDigitsLocation",
                   m_digitsLoc = MCRichDigitLocation::Default );
  declareProperty( "ZeroSuppressHitCut", m_zeroSuppresCut = 96 );

}

// Destructor
MCRichDigitsToRawBufferAlg::~MCRichDigitsToRawBufferAlg() {};

// Initialisation.
StatusCode MCRichDigitsToRawBufferAlg::initialize() {

  // intialise base
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode MCRichDigitsToRawBufferAlg::execute() {

  // debug message
  debug() << "Execute" << endreq;

  // Retrieve the RawBuffer
  RawBuffer * rawBuffer = get<RawBuffer>( m_rawBuffLoc );

  // Retrieve MCRichDigits
  MCRichDigits * digits = get<MCRichDigits>( m_digitsLoc );

  // new rich data map
  RichDAQ::PDMap PDDigits;

  // Loop over digits and sort according to PD
  for ( MCRichDigits::const_iterator iDigit = digits->begin();
        iDigit != digits->end(); ++iDigit ) {
    PDDigits[(*iDigit)->key().pdID()].push_back( *iDigit );
  }

  // Loop over each photon detector and produce a digit bank for each
  for ( RichDAQ::PDMap::const_iterator iPD = PDDigits.begin();
        iPD != PDDigits.end(); ++iPD ) {

    // Make a new data bank
    RichDAQ::RAWBank dataBank;

    // Based on number of hits, decide whether to zero suppress or not
    if ( ((*iPD).second).size() < m_zeroSuppresCut ) {
      fillZeroSuppressed( (*iPD).first, dataBank, (*iPD).second );
    } else {
      fillNonZeroSuppressed( (*iPD).first, dataBank, (*iPD).second );
    }

    // Add this bank to the Raw buffer
    const RichDAQHeaderPD header ( dataBank[0] );
    rawBuffer->addBank( header.linkNumber(), RawBuffer::Rich, dataBank );

  } // end photon detector loop

  debug() << "Created " << PDDigits.size() << " Rich banks for "
          << digits->size() << " MCRichDigits in RawBuffer" << endreq;

  return StatusCode::SUCCESS;
};

void
MCRichDigitsToRawBufferAlg::fillZeroSuppressed( RichSmartID pdID,
                                                RichDAQ::RAWBank & dataBank,
                                                const MCRichDigitVector & pdHits ) const
{

  // Make a new header word for this PD and add to data bank
  const RichDAQLinkNumber linkNumber( pdID.rich(), pdID.panel(),
                                      pdID.PDRow(), pdID.PDCol() );
  const RichDAQHeaderPD pdHeader( true, linkNumber, pdHits.size() );
  dataBank.push_back( pdHeader );

  // Some printout
  if ( msgLevel(MSG::VERBOSE) ) {
    verbose() << "PD " << pdID << " : Creating " << pdHits.size()
              << " zero suppressed hits " << endreq
              << " Header : " << pdHeader << endreq;
  }

  // Loop over digits and form groups of three
  MCRichDigitVector::const_iterator iDigit = pdHits.begin();
  while ( iDigit != pdHits.end() ) {

    MCRichDigit * one   = (*iDigit);
    MCRichDigit * two   = NULL;
    MCRichDigit * three = NULL;
    ++iDigit;
    if ( iDigit != pdHits.end() ) {
      two = (*iDigit);
      ++iDigit;
      if ( iDigit != pdHits.end() ) {
        three = (*iDigit);
        ++iDigit;
      }
    }

    // make a new triplet object and add to data bank
    RichZSHitTriplet triplet( one, two, three );
    dataBank.push_back( triplet );
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " Created triplet " << triplet << endreq;

  } // end while loop

}

void
MCRichDigitsToRawBufferAlg::fillNonZeroSuppressed( RichSmartID pdID,
                                                   RichDAQ::RAWBank & dataBank,
                                                   const MCRichDigitVector & pdHits ) const 
{
  // Make a new header word for this PD and add to data bank
  const RichDAQLinkNumber linkNumber( pdID.rich(), pdID.panel(),
                                      pdID.PDRow(), pdID.PDCol() );
  const RichDAQHeaderPD pdHeader( false, linkNumber, pdHits.size() );
  dataBank.push_back( pdHeader );

  if ( msgLevel(MSG::VERBOSE) ) {
    verbose() << "PD " << pdID << " : Creating " << pdHits.size()
              << " non-zero suppressed hits " << endreq
              << " Header : " << pdHeader << endreq;
    for ( MCRichDigitVector::const_iterator iDig = pdHits.begin();
          iDig != pdHits.end(); ++iDig ) {
      verbose() << " Hit " << (*iDig)->key() << endreq;
    }
  }

  // Create the non zero suppressed data block and update RAWBank
  const RichNonZeroSuppData nonZSdata( pdHits );
  nonZSdata.fillRAW( dataBank );

  // Printout of data array
  if ( msgLevel(MSG::VERBOSE) ) verbose() << nonZSdata << endreq;

}

//  Finalize
StatusCode MCRichDigitsToRawBufferAlg::finalize()
{
  // finalise base
  return RichAlgBase::finalize();
}
