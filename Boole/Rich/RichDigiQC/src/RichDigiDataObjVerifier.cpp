// $Id: RichDigiDataObjVerifier.cpp,v 1.2 2003-11-02 21:55:00 jonrob Exp $

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

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Initialize base class
  if ( !RichAlgBase::initialize() ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode RichDigiDataObjVerifier::execute() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Execute" << endreq;

  // MCRichHits
  if ( m_bdMCHits ) {
    SmartDataPtr<MCRichHits> mcHits( eventSvc(), m_mchitTES );
    if ( !mcHits ) {
      msg << MSG::WARNING << "Cannot locate MCRichHits at " << m_mchitTES << endreq;
    } else {
      msg << MSG::DEBUG << "Successfully located " << mcHits->size()
          << " MCRichHits at " << m_mchitTES << endreq;
      for ( MCRichHits::const_iterator iHit = mcHits->begin();
            iHit != mcHits->end(); ++iHit ) {
        msg << MSG::DEBUG << "MCRichHit key= " << (*iHit)->key() << endreq;
        MCParticle * mcPart = (*iHit)->mcParticle();
        int mcPartKey = ( mcPart ? mcPart->key() : -1 ); 
        msg << MSG::DEBUG << "  Parent MCParticle key= "
            << mcPartKey << endreq;
        std::cout << "   Data members " << **iHit << std::endl;
      }
    }
  }

  // MCRichDeposits
  if ( m_bdMCDeposits ) {
    SmartDataPtr<MCRichDeposits> mcDeps( eventSvc(), m_mcdepTES );
    if ( !mcDeps ) {
      msg << MSG::WARNING << "Cannot locate MCRichDeposits at " << m_mcdepTES << endreq;
    } else {
      msg << MSG::DEBUG << "Successfully located " << mcDeps->size()
          << " MCRichDeposits at " << m_mcdepTES << endreq;
      for ( MCRichDeposits::const_iterator iDep = mcDeps->begin();
            iDep != mcDeps->end(); ++iDep ) {
        msg << MSG::DEBUG << "MCRichDeposit key= " << (int)(*iDep)->key() << " : "
            << (*iDep)->key() << endreq;
        msg << MSG::DEBUG << "  Parent MCRichHit key= "
            << (int)(*iDep)->parentHit()->key() << endreq;
        std::cout << "   Data members " << **iDep << std::endl;
      }
    }
  } // end MCRichDeposits print

  // MCRichSummedDeposits
  if ( m_bdMCSumDeposits ) {
    SmartDataPtr<MCRichSummedDeposits> mcSumDeps( eventSvc(), m_mcsumdepTES );
    if ( !mcSumDeps ) {
      msg << MSG::WARNING << "Cannot locate MCRichSummedDeposits at "
          << m_mcsumdepTES << endreq;
    } else {
      msg << MSG::DEBUG << "Successfully located " << mcSumDeps->size()
          << " MCRichSummedDeposits at " << m_mcsumdepTES << endreq;
      for ( MCRichSummedDeposits::const_iterator iSumDep = mcSumDeps->begin();
            iSumDep != mcSumDeps->end(); ++iSumDep ) {
        msg << MSG::DEBUG << "MCRichSummedDeposit key= " << (int)(*iSumDep)->key() << " : "
            << (*iSumDep)->key() << endreq;
        msg << "  MCRichDeposits(" << (*iSumDep)->deposits().size() << ") keys= ";
        for ( SmartRefVector<MCRichDeposit>::const_iterator iDep = (*iSumDep)->deposits().begin();
              iDep != (*iSumDep)->deposits().end(); ++iDep ) {
          msg << (int)(*iDep)->key() << " ";
        }
        msg << endreq;
        std::cout << "   Data members " << **iSumDep << std::endl;
      }
    }
  } // end MCRichSummedDeposits print

  // MCRichDigits
  if ( m_bdMcDigits ) {
    SmartDataPtr<MCRichDigits> richMcDigits( eventSvc(), m_mcdigitTES );
    if ( !richMcDigits ) {
      msg << MSG::WARNING << "Cannot locate MCRichDigits at " << m_mcdigitTES << endreq;
    } else {
      msg << MSG::DEBUG << "Successfully located " << richMcDigits->size()
          << " MCRichDigits at " << m_mcdigitTES << endreq;
      for ( MCRichDigits::const_iterator imcDigit = richMcDigits->begin();
            imcDigit != richMcDigits->end(); ++imcDigit ) {
        msg << MSG::DEBUG << "MCRichDigit key= " << (int)(*imcDigit)->key() << " : "
            << (*imcDigit)->key() << endreq;
        msg << "  MCRichHits(" << (*imcDigit)->hits().size() << ") keys= ";
        for ( SmartRefVector<MCRichHit>::const_iterator iHit = (*imcDigit)->hits().begin();
              iHit != (*imcDigit)->hits().end(); ++iHit ) {
          msg << (int)(*iHit)->key() << " ";
        }
        msg << endreq;
      }
    }
  } // end MCRichDigit print

  // RichDigits
  if ( m_bdDigits ) {
    SmartDataPtr<RichDigits> richDigits( eventSvc(), m_digitTES );
    if ( !richDigits ) {
      msg << MSG::WARNING << "Cannot locate RichDigits at " << m_digitTES << endreq;
    } else {
      msg << MSG::DEBUG << "Successfully located " << richDigits->size()
          << " RichDigits at " << m_digitTES << endreq;

      for ( RichDigits::const_iterator iDigit = richDigits->begin();
            iDigit != richDigits->end(); ++iDigit ) {
        msg << MSG::DEBUG << "RichDigit key= " << (int)(*iDigit)->key() << " : "
            << (*iDigit)->key() << endreq;
      }
    }
  } // end RichDigit print

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichDigiDataObjVerifier::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  // finalize base class
  return RichAlgBase::finalize();
}
