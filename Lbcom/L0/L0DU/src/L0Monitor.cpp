// $Id: L0Monitor.cpp,v 1.2 2002-09-12 11:53:29 ocallot Exp $
// Include files
#include "stdio.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"
// L0 info
#include "Event/L0CaloCandidate.h"
#include "Event/L0MuonCandidate.h"
#include "Event/L0PuVeto.h"
#include "Event/L0Muon.h"
#include "Event/L0DUReport.h"
#include "Event/MCParticle.h"

// local
#include "L0Monitor.h"
//-----------------------------------------------------------------------------
// Implementation file for class : L0Monitor
//
// 29/06/2001 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<L0Monitor>          Factory ;
const        IAlgFactory& L0MonitorFactory = Factory ;

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
L0Monitor::L0Monitor( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  , m_tupleName  ( "" )
{
  declareProperty( "nTupleName", m_tupleName );
}

//=============================================================================
// Destructor
//=============================================================================
L0Monitor::~L0Monitor() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0Monitor::initialize() {

  MsgStream log(messageService(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  m_fillNtuple =  ("" != m_tupleName);
  if ( m_fillNtuple ) {

    NTuplePtr nt1( ntupleSvc(), m_tupleName );
    if ( !nt1 ) {
      nt1 = ntupleSvc()->book( m_tupleName, CLID_RowWiseTuple, "L0Monitor" );
      if ( nt1 ) {
        nt1->addItem( "Decision", m_decis );
        nt1->addItem( "Electron", m_elec );
        nt1->addItem( "Photon  ", m_phot );
        nt1->addItem( "Pi0Local", m_pi0l );
        nt1->addItem( "Pi0Glob ", m_pi0g );
        nt1->addItem( "Hadron  ", m_hadr );
        nt1->addItem( "Hadron2 ", m_had2 );
        nt1->addItem( "SumEt   ", m_sume );
        nt1->addItem( "SpdMult ", m_spdm );
        nt1->addItem( "Pileup  ", m_pile );
        nt1->addItem( "Muon1   ", m_emu1 );
        nt1->addItem( "Muon2   ", m_emu2 );
        nt1->addItem( "Muon3   ", m_emu3 );
        nt1->addItem( "SumMuon ", m_sumu );
        nt1->addItem( "TruElec ", m_tElec );
        nt1->addItem( "TruPhot ", m_tPhot );
        nt1->addItem( "TruPi0  ", m_tPi0  );
        nt1->addItem( "TruHadr ", m_tHadr );
        nt1->addItem( "TruMuon ", m_tMuon );
        log << MSG::INFO << "Booking OK " << endreq;
      } else {
        log << MSG::ERROR << "Failure to book" << endreq;
        return StatusCode::FAILURE;
      }
    } else {
      log << MSG::ERROR << "Already existing NTuple " << m_tupleName << endreq;
      return StatusCode::FAILURE;
    }
  }
  m_totEvt    = 0.;
  m_goodL0    = 0.;
  m_puVeto    = 0.;
  m_trigElec  = 0.;
  m_trigPhot  = 0.;
  m_trigHadr  = 0.;
  m_trigPi0L  = 0.;
  m_trigPi0G  = 0.;
  m_trigPi0   = 0.;
  m_trigMuon  = 0.;
  m_trigSumMu = 0.;

  m_exclElec  = 0.;
  m_exclPhot  = 0.;
  m_exclHadr  = 0.;
  m_exclPi0L  = 0.;
  m_exclPi0G  = 0.;
  m_exclPi0   = 0.;
  m_exclMuon  = 0.;
  m_exclSumMu = 0.;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0Monitor::execute() {

  MsgStream  log( messageService(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  StatusCode status;

  double elec = 0.;
  double phot = 0.;
  double pi0l = 0.;
  double pi0g = 0.;
  double hadr = 0.;
  double had2 = 0.;
  double sume = 0.;
  double spdm = 0.;

  double pile = 0.;


  std::string containerName = L0DUReportLocation::Default;

  SmartDataPtr< L0DUReport >  decision ( eventSvc() , containerName );
  if( 0 == decision ) {
    log << MSG::ERROR << "Unable to retrieve " << containerName << endreq;
    return StatusCode::SUCCESS;
  }
  log << MSG::DEBUG << "L0 decision : " << decision->typeL0Trig()
      << " is " << decision->typeL0TrigName() << endreq;

  int decis = decision->typeL0Trig();
  m_totEvt  += 1.;
  
  if ( 0 != decis ) {
    int pattern = 0;
    m_goodL0 += 1.;
    if ( decision->isTrig( L0Trig::Electron  ) ) pattern += 1;
    if ( decision->isTrig( L0Trig::Photon    ) ) pattern += 2;
    if ( decision->isTrig( L0Trig::Hadron    ) ) pattern += 4;
    if ( decision->isTrig( L0Trig::Pi0Local  ) ) pattern += 8;
    if ( decision->isTrig( L0Trig::Pi0Global ) ) pattern += 16;
    if ( decision->isTrig( L0Trig::Mu1       ) ) pattern += 32;
    if ( decision->isTrig( L0Trig::SumMu     ) ) pattern += 64;

    if ( pattern & 1  ) m_trigElec  += 1.;
    if ( pattern & 2  ) m_trigPhot  += 1.;
    if ( pattern & 4  ) m_trigHadr  += 1.;
    if ( pattern & 8  ) m_trigPi0L  += 1.;
    if ( pattern & 16 ) m_trigPi0G  += 1.;
    if ( pattern & 24 ) m_trigPi0   += 1.;
    if ( pattern & 32 ) m_trigMuon  += 1.;
    if ( pattern & 64 ) m_trigSumMu += 1.;

    if ( pattern == 1  ) m_exclElec  += 1.;
    if ( pattern == 2  ) m_exclPhot  += 1.;
    if ( pattern == 4  ) m_exclHadr  += 1.;
    if ( pattern == 8  ) m_exclPi0L  += 1.;
    if ( pattern == 16 ) m_exclPi0G  += 1.;
    if ( (pattern == 8 ) || 
         (pattern == 16) || 
         (pattern == 24)   ) m_exclPi0   += 1.;
    if ( pattern == 32 ) m_exclMuon  += 1.;
    if ( pattern == 64 ) m_exclSumMu += 1.;
    
  }

  containerName = L0CaloCandidateLocation::Default;

  SmartDataPtr< L0CaloCandidates > candidates ( eventSvc() , containerName );
  if( 0 == candidates ) {
    log << MSG::ERROR << "Unable to retrieve " << containerName << endreq;
    return StatusCode::SUCCESS;
  }

  L0CaloCandidates::const_iterator cand;
  for ( cand = candidates->begin() ; candidates->end() != cand ; ++cand ) {
    if ( L0Calo::Electron == (*cand)->type()  ) {
      elec = (*cand)->et()/GeV ;
    } else if ( L0Calo::Photon    == (*cand)->type() ){
      phot = (*cand)->et()/GeV ;
    } else if ( L0Calo::Pi0Local  == (*cand)->type() ){
      pi0l = (*cand)->et()/GeV ;
    } else if ( L0Calo::Pi0Global == (*cand)->type() ){
      pi0g = (*cand)->et()/GeV ;
    } else if ( L0Calo::Hadron    == (*cand)->type() ){
      hadr = (*cand)->et()/GeV ;
    } else if ( L0Calo::Hadron2   == (*cand)->type() ){
      had2 = (*cand)->et()/GeV ;
    } else if ( L0Calo::SumEt     == (*cand)->type() ){
      sume = (*cand)->et()/GeV ;
    } else if ( L0Calo::SpdMult   == (*cand)->type() ){
      spdm = (*cand)->etCode();
    }
  }
  log << MSG::DEBUG
      << "Elec "  << elec
      << " Phot " << phot
      << " Pi0L " << pi0l
      << " Pi0G " << pi0g
      << " Hadr " << hadr
      << " Had2 " << had2
      << " SumE " << sume 
      << " SpdM " << spdm << endreq;
  /**
   * Muon
   */

  SmartDataPtr<L0MuonCandidates> candMuon ( eventSvc() ,
                                            L0MuonCandidateLocation::Default);

  double emu1 = 0;
  double emu2 = 0;
  double emu3 = 0;
  double sumu = 0;

  if ( 0 != candMuon ) {
    L0MuonCandidates::const_iterator itMuon ;
    for ( itMuon = candMuon->begin() ; candMuon->end() != itMuon ; ++itMuon ) {
      if ( L0Muon::OK == (*itMuon)->status() ) {
        double eMuon = fabs( (*itMuon)->pt() / GeV );
        if ( 0 < eMuon ) {
          if ( eMuon > emu1 ) {
            emu3 = emu2;
            emu2 = emu1;
            emu1 = eMuon;
            log << MSG::VERBOSE << "Added in 1 " << eMuon << endreq;
          } else if ( eMuon > emu2 ) {
            emu3 = emu2;
            emu2 = eMuon;
            log << MSG::VERBOSE << "Added in 2 " << eMuon << endreq;
          } else if ( eMuon > emu3 ) {
            emu3 = eMuon;
            log << MSG::VERBOSE << "Added in 3 " << eMuon << endreq;
          }
          sumu += eMuon;
          
          log << MSG::VERBOSE << "eMuon " << eMuon << " emu1 " << emu1
              << " emu2 " << emu2 << " emu3 " << emu3
              << " sumu " << sumu << endreq;
        }
      }
    }
    log << MSG::DEBUG << "Muon 1/2/3 Sum E = " << emu1 << " "
        << emu2 << " " << emu3 << " " << sumu << endreq;
  } else {
    log << MSG::DEBUG << "Muon container not found " << endreq;
  }

  /**
   * Pile-up veto
   */
  SmartDataPtr<L0PuVeto>  L0PileUp ( eventDataService(),
                                     L0PuVetoLocation::Default );
  if ( 0 != L0PileUp ) {
    if ( 0 != L0PileUp->decision() ) {
      pile = 1.;
      m_puVeto += 1.;
    }
  }
  log << MSG::DEBUG << "pileup = " << pile << endreq;

  if ( m_fillNtuple ) {
    /**
     * Scans the truth, to get the highest object of each kind.
     */
    SmartDataPtr< MCParticles> partCtnr( eventDataService(),
                                       MCParticleLocation::Default );
    if ( 0 == partCtnr ) {
      log << MSG::INFO << "No MCParticle container !" << endreq;
      return StatusCode::SUCCESS;
    }
    float angle;
    float Et;
    int   pID;

    double tElec = 0.;
    double tPhot = 0.;
    double tPi0  = 0.;
    double tHadr = 0.;
    double tMuon = 0.;
    
    MCParticles::const_iterator pItr ;
    for ( pItr = partCtnr->begin(); partCtnr->end() != pItr; pItr++ ){
      angle = (*pItr)->momentum().theta();
      if ( ( 0.030 < angle ) && ( 0.250 > angle )  ) {
        Et = (*pItr)->momentum().e() * sin( angle ) / GeV;
        pID = abs( (*pItr)->particleID().pid() );
        
        if ( 22 == pID  ) {
          if ( tPhot < Et ) tPhot = Et;
        } else if ( 11 == pID) {
          if ( tElec < Et ) tElec = Et;
        } else if ( 13 == pID ) {
          if ( tMuon < Et ) tMuon = Et;
        } else if ( 111 == pID ){
          if ( tPi0 < Et ) tPi0 = Et;
        } else if ( (211 == pID) || ( 321 == pID) || (2212 == pID ) ) {
          if ( tHadr < Et ) tHadr = Et;
        }
      }
    }
    
    /**
     * Fill the ntuple
     */
    m_decis = decis;
    m_elec  = elec;
    m_phot  = phot;
    m_pi0l  = pi0l;
    m_pi0g  = pi0g;
    m_hadr  = hadr;
    m_sume  = sume;
    m_spdm  = spdm;

    m_pile  = pile;
    m_emu1  = emu1;
    m_emu2  = emu2;
    m_emu3  = emu3;
    m_sumu  = sumu;
    m_tElec = tElec;
    m_tPhot = tPhot;
    m_tPi0  = tPi0;
    m_tHadr = tHadr;
    m_tMuon = tMuon;

    NTuplePtr nt1( ntupleSvc(), m_tupleName );
    if ( nt1 ) {
      status = ntupleSvc()->writeRecord( nt1.ptr() );
      if ( !status.isSuccess() ) {
        log << MSG::ERROR << "Failure to write event in NTuple "
            << m_tupleName << endreq;
        return StatusCode::FAILURE;
      }
    } else {
      log << MSG::INFO << "Can't find the NTuple " << m_tupleName << endreq;
    }
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0Monitor::finalize() {

  MsgStream log(messageService(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;
  
  log << MSG::INFO << "== L0 Performance on " << m_totEvt << " events." 
      << endreq;
  double fact;
  if ( 1. > m_totEvt ) {
    fact = 0.;
  } else {
    fact = 100. / m_totEvt;
  }
  
  log << "PileUp Veto  : " 
      << format( " %8.0f  %6.2f %%", m_puVeto, fact * m_puVeto ) << endreq;
  log << "Accepted L0  : " 
      << format( " %8.0f  %6.2f %%", m_goodL0, fact * m_goodL0 )<< endreq;

  log << "  Electron   : " 
      << format( " %8.0f  %6.2f %%  excl %8.0f  %6.2f %%", 
                 m_trigElec, fact*m_trigElec, m_exclElec, fact*m_exclElec )
      << endreq;

  log << "    Photon   : " 
      << format( " %8.0f  %6.2f %%  excl %8.0f  %6.2f %%", 
                 m_trigPhot, fact*m_trigPhot, m_exclPhot, fact*m_exclPhot )
      << endreq;
  log << "    Hadron   : " 
      << format( " %8.0f  %6.2f %%  excl %8.0f  %6.2f %%", 
                 m_trigHadr, fact*m_trigHadr, m_exclHadr, fact*m_exclHadr )
      << endreq;
  log << " Pi0 Local   : " 
      << format( " %8.0f  %6.2f %%  excl %8.0f  %6.2f %%", 
                 m_trigPi0L, fact*m_trigPi0L, m_exclPi0L, fact*m_exclPi0L )
      << endreq;
  log << "Pi0 Global   : " 
      << format( " %8.0f  %6.2f %%  excl %8.0f  %6.2f %%", 
                 m_trigPi0G, fact*m_trigPi0G, m_exclPi0G, fact*m_exclPi0G )
      << endreq;
  log << "   any Pi0   : " 
      << format( " %8.0f  %6.2f %%  excl %8.0f  %6.2f %%", 
                 m_trigPi0, fact*m_trigPi0, m_exclPi0, fact*m_exclPi0 )
      << endreq;
  log << "      Muon   : " 
      << format( " %8.0f  %6.2f %%  excl %8.0f  %6.2f %%", 
                 m_trigMuon, fact*m_trigMuon, m_exclMuon, fact*m_exclMuon )
      << endreq;
  log << "  Sum Muon   : " 
      << format( " %8.0f  %6.2f %%  excl %8.0f  %6.2f %%", 
                 m_trigSumMu, fact*m_trigSumMu, m_exclSumMu, fact*m_exclSumMu )
      << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
