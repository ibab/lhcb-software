// $Id: RichDigiDataObjVerifier.cpp,v 1.4 2004-04-13 14:43:07 jonesc Exp $

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
  declareProperty( "PrintRichDigits", m_bdDigits = true );
  declareProperty( "PrintMCRichDigits", m_bdMcDigits = true );
  declareProperty( "PrintMCRichDeposits", m_bdMCDeposits = true );
  declareProperty( "PrintMCRichSummedDeposits", m_bdMCSumDeposits = true );
  declareProperty( "PrintMCRichHits", m_bdMCHits = true );

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
  SmartDataPtr<MCRichHits> mcHits( eventSvc(), location );
  if ( !mcHits ) {
    debug() << "Cannot locate MCRichHits at " << location << endreq;
  } else {
    debug() << "Successfully located " << mcHits->size()
            << " MCRichHits at " << location << endreq;
    for ( MCRichHits::const_iterator iHit = mcHits->begin();
          iHit != mcHits->end(); ++iHit ) {
      debug() << "MCRichHit key= " << (*iHit)->key() << endreq;
      MCParticle * mcPart = (*iHit)->mcParticle();
      int mcPartKey = ( mcPart ? mcPart->key() : -1 );
      debug() << "  Parent MCParticle key= "
              << mcPartKey << endreq;
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
    SmartDataPtr<MCRichSummedDeposits> mcSumDeps( eventSvc(), MCRichSummedDepositLocation::Default );
    if ( !mcSumDeps ) {
      Warning("Cannot locate MCRichSummedDeposits at "+MCRichSummedDepositLocation::Default);
    } else {
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
  if ( m_bdMcDigits ) {
    SmartDataPtr<MCRichDigits> richMcDigits( eventSvc(), MCRichDigitLocation::Default );
    if ( !richMcDigits ) {
      Warning("Cannot locate MCRichDigits at "+MCRichDigitLocation::Default);
    } else {
      debug() << "Successfully located " << richMcDigits->size()
              << " MCRichDigits at " << MCRichDigitLocation::Default << endreq;
      for ( MCRichDigits::const_iterator imcDigit = richMcDigits->begin();
            imcDigit != richMcDigits->end(); ++imcDigit ) {
        debug() << "MCRichDigit key= " << (long)(*imcDigit)->key() << " : "
                << (*imcDigit)->key() << endreq;
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
    SmartDataPtr<RichDigits> richDigits( eventSvc(), RichDigitLocation::Default );
    if ( !richDigits ) {
      Warning("Cannot locate RichDigits at "+RichDigitLocation::Default);
    } else {
      debug() << "Successfully located " << richDigits->size()
              << " RichDigits at " << RichDigitLocation::Default << endreq;

      for ( RichDigits::const_iterator iDigit = richDigits->begin();
            iDigit != richDigits->end(); ++iDigit ) {
        debug() << "RichDigit key= " << (long)(*iDigit)->key() << " : " 
                << (*iDigit)->key() << endreq;
      }
    }
  } // end RichDigit print

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichDigiDataObjVerifier::finalize() {

  debug() << "Finalize" << endreq;

  // finalize base class
  return RichAlgBase::finalize();
}
