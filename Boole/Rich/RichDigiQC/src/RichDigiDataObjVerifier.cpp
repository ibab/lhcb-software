// $Id: RichDigiDataObjVerifier.cpp,v 1.3 2004-03-16 13:52:46 jonesc Exp $

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
  : RichAlgBase ( name, pSvcLocator ) {

  // Declare job options
  declareProperty( "RichDigits", m_digitTES = RichDigitLocation::Default );
  declareProperty( "MCRichDigits", m_mcdigitTES = MCRichDigitLocation::Default );
  declareProperty( "MCRichDeposits", m_mcdepTES = MCRichDepositLocation::Default );
  declareProperty( "MCRichSummedDeposits", m_mcsumdepTES = MCRichSummedDepositLocation::Default );
  declareProperty( "MCRichHits", m_mchitTES = MCRichHitLocation::Default );

  declareProperty( "PrintRichDigits", m_bdDigits = true );
  declareProperty( "PrintMCRichDigits", m_bdMcDigits = true );
  declareProperty( "PrintMCRichDeposits", m_bdMCDeposits = true );
  declareProperty( "PrintMCRichSummedDeposits", m_bdMCSumDeposits = true );
  declareProperty( "PrintMCRichHits", m_bdMCHits = true );

}

// Destructor
RichDigiDataObjVerifier::~RichDigiDataObjVerifier() {};

// Initialisation
StatusCode RichDigiDataObjVerifier::initialize() {

  debug() << "Initialize" << endreq;

  // Initialize base class
  return RichAlgBase::initialize();
};

// Main execution
StatusCode RichDigiDataObjVerifier::execute() {

  debug() << "Execute" << endreq;

  // MCRichHits
  if ( m_bdMCHits ) {
    SmartDataPtr<MCRichHits> mcHits( eventSvc(), m_mchitTES );
    if ( !mcHits ) {
      warning() << "Cannot locate MCRichHits at " << m_mchitTES << endreq;
    } else {
      debug() << "Successfully located " << mcHits->size()
              << " MCRichHits at " << m_mchitTES << endreq;
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

  // MCRichDeposits
  if ( m_bdMCDeposits ) {
    SmartDataPtr<MCRichDeposits> mcDeps( eventSvc(), m_mcdepTES );
    if ( !mcDeps ) {
      warning() << "Cannot locate MCRichDeposits at " << m_mcdepTES << endreq;
    } else {
      debug() << "Successfully located " << mcDeps->size()
              << " MCRichDeposits at " << m_mcdepTES << endreq;
      for ( MCRichDeposits::const_iterator iDep = mcDeps->begin();
            iDep != mcDeps->end(); ++iDep ) {
        debug() << "MCRichDeposit key= " << (int)(*iDep)->key() << " : "
                << (*iDep)->key() << endreq;
        debug() << "  Parent MCRichHit key= "
                << (int)(*iDep)->parentHit()->key() << endreq;
        std::cout << "   Data members " << **iDep << std::endl;
      }
    }
  } // end MCRichDeposits print

  // MCRichSummedDeposits
  if ( m_bdMCSumDeposits ) {
    SmartDataPtr<MCRichSummedDeposits> mcSumDeps( eventSvc(), m_mcsumdepTES );
    if ( !mcSumDeps ) {
      warning() << "Cannot locate MCRichSummedDeposits at "
                << m_mcsumdepTES << endreq;
    } else {
      debug() << "Successfully located " << mcSumDeps->size()
              << " MCRichSummedDeposits at " << m_mcsumdepTES << endreq;
      for ( MCRichSummedDeposits::const_iterator iSumDep = mcSumDeps->begin();
            iSumDep != mcSumDeps->end(); ++iSumDep ) {
        debug() << "MCRichSummedDeposit key= " << (int)(*iSumDep)->key() << " : "
                << (*iSumDep)->key() << endreq;
        debug() << "  MCRichDeposits(" << (*iSumDep)->deposits().size() << ") keys= ";
        for ( SmartRefVector<MCRichDeposit>::const_iterator iDep = (*iSumDep)->deposits().begin();
              iDep != (*iSumDep)->deposits().end(); ++iDep ) {
          debug() << (int)(*iDep)->key() << " ";
        }
        debug() << endreq;
        std::cout << "   Data members " << **iSumDep << std::endl;
      }
    }
  } // end MCRichSummedDeposits print

  // MCRichDigits
  if ( m_bdMcDigits ) {
    SmartDataPtr<MCRichDigits> richMcDigits( eventSvc(), m_mcdigitTES );
    if ( !richMcDigits ) {
      warning() << "Cannot locate MCRichDigits at " << m_mcdigitTES << endreq;
    } else {
      debug() << "Successfully located " << richMcDigits->size()
              << " MCRichDigits at " << m_mcdigitTES << endreq;
      for ( MCRichDigits::const_iterator imcDigit = richMcDigits->begin();
            imcDigit != richMcDigits->end(); ++imcDigit ) {
        debug() << "MCRichDigit key= " << (int)(*imcDigit)->key() << " : "
                << (*imcDigit)->key() << endreq;
        debug() << "  MCRichHits(" << (*imcDigit)->hits().size() << ") keys= ";
        for ( SmartRefVector<MCRichHit>::const_iterator iHit = (*imcDigit)->hits().begin();
              iHit != (*imcDigit)->hits().end(); ++iHit ) {
          debug() << (int)(*iHit)->key() << " ";
        }
        debug() << endreq;
      }
    }
  } // end MCRichDigit print

  // RichDigits
  if ( m_bdDigits ) {
    SmartDataPtr<RichDigits> richDigits( eventSvc(), m_digitTES );
    if ( !richDigits ) {
      warning() << "Cannot locate RichDigits at " << m_digitTES << endreq;
    } else {
      debug() << "Successfully located " << richDigits->size()
              << " RichDigits at " << m_digitTES << endreq;

      for ( RichDigits::const_iterator iDigit = richDigits->begin();
            iDigit != richDigits->end(); ++iDigit ) {
        debug() << "RichDigit key= " << (int)(*iDigit)->key() << " : "
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
