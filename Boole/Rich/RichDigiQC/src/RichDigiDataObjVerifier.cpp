// $Id: RichDigiDataObjVerifier.cpp,v 1.7 2005-01-14 17:29:58 jonrob Exp $

// local
#include "RichDigiDataObjVerifier.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichDigiDataObjVerifier
//
// 2002-11-20 : Andy Presland   (Andrew.Presland@cern.ch)
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichDigiDataObjVerifier>          s_factory ;
const        IAlgFactory& RichDigiDataObjVerifierFactory = s_factory ;

// Standard constructor, initializes variables
RichDigiDataObjVerifier::RichDigiDataObjVerifier( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : RichAlgBase ( name, pSvcLocator ) 
{

  // Declare job options
  declareProperty( "CheckRichDigits", m_bdDigits = true );
  declareProperty( "CheckMCRichDigits", m_bdMcDigits = true );
  declareProperty( "CheckMCRichDeposits", m_bdMCDeposits = true );
  declareProperty( "CheckMCRichSummedDeposits", m_bdMCSumDeposits = true );
  declareProperty( "CheckMCRichHits", m_bdMCHits = true );

}

// Destructor
RichDigiDataObjVerifier::~RichDigiDataObjVerifier() {};

// Initialisation
StatusCode RichDigiDataObjVerifier::initialize() 
{
  debug() << "Initialize" << endreq;

  // Initialize base class
  return RichAlgBase::initialize();
};

void RichDigiDataObjVerifier::printHitsAt( const std::string & location ) const
{
  // MCRichHits
  if ( !exist<MCRichHits>(location) ) {
    debug() << "Cannot locate MCRichHits at " << location << endreq;
  } else {
    MCRichHits * mcHits = get<MCRichHits>(location);
    debug() << "Successfully located " << mcHits->size()
            << " MCRichHits at " << location << endreq;
    for ( MCRichHits::const_iterator iHit = mcHits->begin();
          iHit != mcHits->end(); ++iHit ) {
      debug() << "MCRichHit key= " << (*iHit)->key() << endreq;
      MCParticle * mcPart = (*iHit)->mcParticle();
      if ( mcPart ) {
        debug() << "  Parent MCParticle key= " << mcPart->key() << endreq;
      } else {
        debug() << "  No parent MCParticle found !!" << endreq;
      }
      std::cout << "   Data members " << **iHit << std::endl;
    }
  }

}

// Main execution
StatusCode RichDigiDataObjVerifier::execute() {

  debug() << "Execute" << endreq;

  // MCRichHits
  if ( m_bdMCHits ) {
    printHitsAt (               MCRichHitLocation::Default );
    printHitsAt ( "Prev/"     + MCRichHitLocation::Default );
    printHitsAt ( "PrevPrev/" + MCRichHitLocation::Default );
    printHitsAt ( "Next/"     + MCRichHitLocation::Default );
    printHitsAt ( "NexNext/"  + MCRichHitLocation::Default );
  }

  // MCRichDeposits
  if ( m_bdMCDeposits ) {
    SmartDataPtr<MCRichDeposits> mcDeps( eventSvc(), MCRichDepositLocation::Default );
    if ( !mcDeps ) {
      Warning("Cannot locate MCRichDeposits at "+MCRichDepositLocation::Default);
    } else {
      debug() << "Successfully located " << mcDeps->size()
              << " MCRichDeposits at " << MCRichDepositLocation::Default << endreq;
      for ( MCRichDeposits::const_iterator iDep = mcDeps->begin();
            iDep != mcDeps->end(); ++iDep ) {
        debug() << "MCRichDeposit key= " << (long)(*iDep)->key() << " : "
                << (*iDep)->key() << endreq;
        debug() << "  Parent MCRichHit key= "
                << (long)(*iDep)->parentHit()->key() << endreq;
        std::cout << "   Data members " << **iDep << std::endl;
      }
    }
  } // end MCRichDeposits print

    // MCRichSummedDeposits
  if ( m_bdMCSumDeposits ) {
    if ( !exist<MCRichSummedDeposits>(MCRichSummedDepositLocation::Default) ) {
      Warning("Cannot locate MCRichSummedDeposits at "+MCRichSummedDepositLocation::Default);
    } else {
      MCRichSummedDeposits * mcSumDeps = get<MCRichSummedDeposits>(MCRichSummedDepositLocation::Default);
      debug() << "Successfully located " << mcSumDeps->size()
              << " MCRichSummedDeposits at " << MCRichSummedDepositLocation::Default << endreq;
      for ( MCRichSummedDeposits::const_iterator iSumDep = mcSumDeps->begin();
            iSumDep != mcSumDeps->end(); ++iSumDep ) {
        debug() << "MCRichSummedDeposit key= " << (long)(*iSumDep)->key() << " : "
                << (*iSumDep)->key() << endreq;
        debug() << "  MCRichDeposits(" << (*iSumDep)->deposits().size() << ") keys= ";
        for ( SmartRefVector<MCRichDeposit>::const_iterator iDep = (*iSumDep)->deposits().begin();
              iDep != (*iSumDep)->deposits().end(); ++iDep ) {
          debug() << (long)(*iDep)->key() << " ";
        }
        debug() << endreq;
        std::cout << "   Data members " << **iSumDep << std::endl;
      }
    }
  } // end MCRichSummedDeposits print

    // MCRichDigits
  MCRichDigits * richMcDigits( 0 );
  if ( m_bdMcDigits ) {
    if ( !exist<MCRichDigits>(MCRichDigitLocation::Default) ) {
      Warning("Cannot locate MCRichDigits at "+MCRichDigitLocation::Default);
    } else {
      richMcDigits = get<MCRichDigits>(MCRichDigitLocation::Default);
      debug() << "Successfully located " << richMcDigits->size()
              << " MCRichDigits at " << MCRichDigitLocation::Default << endreq;
      for ( MCRichDigits::const_iterator imcDigit = richMcDigits->begin();
            imcDigit != richMcDigits->end(); ++imcDigit ) {
        debug() << "MCRichDigit " << (*imcDigit)->key() << endreq;
        debug() << "  MCRichHits(" << (*imcDigit)->hits().size() << ") keys= ";
        for ( SmartRefVector<MCRichHit>::const_iterator iHit = (*imcDigit)->hits().begin();
              iHit != (*imcDigit)->hits().end(); ++iHit ) {
          debug() << (long)(*iHit)->key() << " ";
        }
        debug() << endreq;
      }
    }
  } // end MCRichDigit print

    // RichDigits
  if ( m_bdDigits ) {
    if ( !exist<RichDigits>(RichDigitLocation::Default) ) {
      Warning("Cannot locate RichDigits at "+RichDigitLocation::Default);
    } else {
      RichDigits * richDigits = get<RichDigits>( RichDigitLocation::Default );
      debug() << "Successfully located " << richDigits->size()
              << " RichDigits at " << RichDigitLocation::Default << endreq;

      if ( richMcDigits && richDigits->size() != richMcDigits->size() ) {
        warning() << "Found " << richDigits->size() << " RichDigits and " 
                  << richMcDigits->size() << " MCRichDigits" << endreq;
      }

      for ( RichDigits::const_iterator iDigit = richDigits->begin();
            iDigit != richDigits->end(); ++iDigit ) {
        debug() << "RichDigit " << (*iDigit)->key() << endreq;
        // locate associated MCRichDigit
        MCRichDigit * mcDig = ( richMcDigits ? richMcDigits->object( (*iDigit)->key() ) : 0 );
        if ( !mcDig ) { 
          warning() << " -> Failed to locate associated MCRichDigit for RichDigit " << (*iDigit)->key() << endreq; 
        } else {
          debug() << " -> Found MCRichDigit " << mcDig->key() << endreq;
        }

      }

    }
    
    // Compare to MCRichDigits

  } // end RichDigit print

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichDigiDataObjVerifier::finalize() {

  debug() << "Finalize" << endreq;

  // finalize base class
  return RichAlgBase::finalize();
}
