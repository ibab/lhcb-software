// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0DU/src/DecisionUnit.cpp,v 1.2 2001-06-15 12:42:44 ocallot Exp $
//#define L0DU_DECISIONUNIT_CPP

#include <math.h>

// Gaudi 
#include <iostream>
#include "GaudiKernel/AlgFactory.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

// CLHEP

#include "CLHEP/Units/SystemOfUnits.h"

// Private

#include "DecisionUnit.h"

//--------------------------------------------------------------------
//
//  DecisionUnit: Choice of L0 trigger
//
//  Author  : Nicole Brun - Pascal Perret
//  Date    : 15 June 2001
//  Changes :   
//
//--------------------------------------------------------------------


static const AlgFactory<DecisionUnit>    Factory;
const IAlgFactory& DecisionUnitFactory = Factory;

// Useful local type definitions

typedef ObjectVector<L0CaloCandidate> InputSeqCalo;
typedef ObjectVector<L0MuonCandidate> InputSeqMuon; 


// Constructor
// Algorithm parameters which can be set at run time must be declared here.

DecisionUnit::DecisionUnit( const std::string& name,
                            ISvcLocator* pSvcLocator )
  : Algorithm      ( name, pSvcLocator )
  , m_nameOfInputL0CaloCandidate  ( "/Event/FE/L0/Calo" )
  , m_nameOfInputL0MuonCandidate  ( "/Event/MC/L0MuonCandidates" )
  , m_nameOfOutputDecisionUnit    ( "/Event/FE/L0/L0Decis" ) {  

  declareProperty( "L0CaloCandidateData" , m_nameOfInputL0CaloCandidate );
  declareProperty( "L0MuonCandidateData" , m_nameOfInputL0MuonCandidate );
  declareProperty( "AcceptedData"        , m_nameOfOutputDecisionUnit );

  declareProperty( "EElCut1"             , m_eElCut1    = 2.65 * GeV );
  declareProperty( "EElCut2"             , m_eElCut2    = 2.65 * GeV );
  declareProperty( "ScalEl"              , m_scalEl     = 0 );

  declareProperty( "EPhCut1"             , m_ePhCut1    = 3.82 * GeV);
  declareProperty( "EPhCut2"             , m_ePhCut2    = 3.82 * GeV);
  declareProperty( "ScalPh"              , m_scalPh     = 0 );

  declareProperty( "EHaCut1"             , m_eHaCut1    = 3.95 * GeV );
  declareProperty( "EHaCut2"             , m_eHaCut2    = 3.95 * GeV );
  declareProperty( "ScalHa"              , m_scalHa     = 0 );

  declareProperty( "ESumEtCut1"          , m_eSumEtCut1 = 5.00 * GeV );
  declareProperty( "ESumEtCut2"          , m_eSumEtCut2 = 5.00 * GeV );
  declareProperty( "ScalSumEt"           , m_scalSumEt  = 0 );

  declareProperty( "EPi0LCut1"           , m_ePi0LCut1  = 4.10 * GeV );
  declareProperty( "EPi0LCut2"           , m_ePi0LCut2  = 4.10 * GeV );
  declareProperty( "ScalPi0L"            , m_scalPi0L   = 0 );

  declareProperty( "EPi0GCut1"           , m_ePi0GCut1  = 4.60 * GeV );
  declareProperty( "EPi0GCut2"           , m_ePi0GCut2  = 4.60 * GeV);
  declareProperty( "ScalPi0G"            , m_scalPi0G   = 0 );

  declareProperty( "EMu1Cut1"            , m_eMu1Cut1   = 0.61 * GeV );
  declareProperty( "EMu1Cut2"            , m_eMu1Cut2   = 0.61 * GeV );
  declareProperty( "ScalMu1"             , m_scalMu1    = 0 );

  declareProperty( "ESumMuCut1"          , m_eSumMuCut1 = 1.83 * GeV );
  declareProperty( "ESumMuCut2"          , m_eSumMuCut2 = 1.83 * GeV );
  declareProperty( "ScalSumMu"           , m_scalSumMu  = 0 );
  declareProperty( "NMuSumMu"            , m_nMuSumMu   = 8 );
  
  m_typeL0Trig     = 0;
  
}

// Initialize
// Call by the application manager before reading any event data

StatusCode DecisionUnit::initialize() {
  
  MsgStream log(messageService(),name());

  log << MSG::INFO 
      << "Initialization" 
      << endreq;

// ** check for the valid names of the input/output data containers

  if ( m_nameOfInputL0CaloCandidate.empty() ) { 
    log <<  MSG::ERROR
        << "The name of the LOCaloCandidate data container is empty!" 
        << endreq; 
    return StatusCode::FAILURE; 
  }
  if ( m_nameOfInputL0MuonCandidate.empty() ) { 
    log <<  MSG::ERROR
        << "The name of the LOMuonCandidate data container is empty!" 
	<< endreq; 
    return StatusCode::FAILURE; 
  }
  if ( m_nameOfOutputDecisionUnit.empty() ) { 
    log <<  MSG::ERROR
        << "The name of the L0TrigDecis data container is empty!" 
	<< endreq; 
    return StatusCode::FAILURE; 
  }

  // Raz for scaling

  int indexCand = 0;
  while ( indexCand != L0Trig::numberTrig ) {
    m_nEtSupCut2.push_back( 0 );
    indexCand++;
  }


  log << MSG::INFO
      << "Initalization completed successfully"
      << endreq;
  return StatusCode::SUCCESS;

}

// Execute
// Call by the application manager once per event

StatusCode DecisionUnit::execute() {

  MsgStream log(messageService(),name());

  log << MSG::DEBUG
      << "Execution"
      << endreq;

  m_typeL0Trig = 0;
  m_Electron = 0;
  m_Photon   = 0;
  m_Hadron   = 0;
  m_SumEt    = 0;
  m_Pi0Local = 0;
  m_Pi0Global = 0;
  m_Muon1 = 0;

  // Create the L0DUReport container object  
  
  ObjectVector<L0DUReport>* output = 
                            new ObjectVector<L0DUReport>();
  
  // Search if the output directory exists

  std::string nameOfOutputDirectory = m_nameOfOutputDecisionUnit;
  std::string::size_type pos = nameOfOutputDirectory.find_last_of('/');
  if ( std::string::npos != pos ) {
    nameOfOutputDirectory.erase(pos);
  }
  SmartDataPtr<DataObject> outDir( eventDataService(), nameOfOutputDirectory );
  if ( 0 == outDir ) {
    log << MSG::ERROR 
        << "Unknow OutputDirectory = "
        << nameOfOutputDirectory 
        << endreq;
  }

  // Registering of DecisUnit container object into the event data store
 
  StatusCode sc = 
    eventDataService()->registerObject( m_nameOfOutputDecisionUnit, output );
  if ( sc.isFailure() ) {
    delete output;
    log << MSG::ERROR
        << "Unable to register output L0TrigDecis container "
        << "into the transient event data store" 
        << endreq;
    return StatusCode::FAILURE;
  }

  // Find candidates calo

  SmartDataPtr<InputSeqCalo> smartCandCalo ( eventDataService() , 
  				             m_nameOfInputL0CaloCandidate ); 
  if ( 0 != smartCandCalo ) { 
    InputSeqCalo* candCalo = (InputSeqCalo*) smartCandCalo; 
    InputSeqCalo::const_iterator itCandCalo = candCalo->begin();
    if ( candCalo->end() == candCalo->begin() ) {
      log << MSG::DEBUG
          << "No Calo candidates"
          << endreq;
    }
    while ( itCandCalo != candCalo->end() ) {
      log << MSG::DEBUG 
          << "Cand "
          << (*itCandCalo)->ID() 
          << " "
          << (*itCandCalo)->typeName() 
          << " Et(GeV) = "
          << (*itCandCalo)->et() / GeV 
          << endreq;
      log << MSG::DEBUG
          <<"  eCut1(GeV) = "; 
      if ( L0::Electron == (*itCandCalo)->type() ) {
        m_Electron = (*itCandCalo);
        log << m_eElCut1 / GeV
            <<" eCut2(GeV) = "
            << m_eElCut2 / GeV
            << " scal = "
            << m_scalEl; 
      }
      if ( L0::Photon == (*itCandCalo)->type() ) {
        m_Photon = (*itCandCalo);
        log << m_ePhCut1 / GeV
            <<" eCut2(GeV) = "
            << m_ePhCut2 / GeV
            << " scal = "
            << m_scalPh; 
      } 
      if ( L0::Hadron == (*itCandCalo)->type() ) {
        m_Hadron = (*itCandCalo);
        log << m_eHaCut1 / GeV
            <<" eCut2(GeV) = "
            << m_eHaCut2 / GeV
            << " scal = "
            << m_scalHa; 
      }
      if ( L0::SumEt == (*itCandCalo)->type() ) {
        m_SumEt = (*itCandCalo);
        log << m_eSumEtCut1 / GeV
            <<" eCut2(GeV) = "
            << m_eSumEtCut2 / GeV
            << " scal not used"; 
      } 
      if ( L0::Pi0Local == (*itCandCalo)->type() ) {
        m_Pi0Local = (*itCandCalo);
        log << m_ePi0LCut1 / GeV
            <<" eCut2(GeV) = "
            << m_ePi0LCut2 / GeV
            << " scal = "
            << m_scalPi0L; 
      }
      if ( L0::Pi0Global == (*itCandCalo)->type() ) {
        m_Pi0Global = (*itCandCalo);
        log << m_ePi0GCut1 / GeV
            <<" eCut2(GeV) = "
            << m_ePi0GCut2 / GeV
            << " scal = "
            << m_scalPi0G; 
      } 
      log << endreq;
      itCandCalo++;
    }
  }
  else { 
    log << MSG::DEBUG
        << "Unable to retrieve L0CaloCandidate data container = "
 	<< m_nameOfInputL0CaloCandidate
        << endreq; 
  }

  // Find candidates muon

  double eSumMuons = 0.0;
  SmartDataPtr<InputSeqMuon> smartCandMuon ( eventDataService() ,
                                             m_nameOfInputL0MuonCandidate ); 
  if ( 0 != smartCandMuon ) {
    InputSeqMuon* candMuon = (InputSeqMuon*) smartCandMuon;
    InputSeqMuon::const_iterator itCandMuon = candMuon->begin();
    if ( candMuon->end() == candMuon->begin() ) {
      log << MSG::DEBUG
          << "No Muon candidates"
          << endreq;
    }
    else {
      std::vector<double> eMuons; 
      while ( itCandMuon != candMuon->end() ) {
        log << MSG::DEBUG 
            << "Cand Muon " 
            << " Pt(GeV) = "
            << (*itCandMuon)->pt() 
            << "\n( x y teta phi status = "
            << (*itCandMuon)->x()
            << " "
            << (*itCandMuon)->y()
            << " "
            << (*itCandMuon)->theta()
            << " "
            << (*itCandMuon)->phi()
            << " "
            << (*itCandMuon)->status()
            << " )"
            << endreq;
        if ( L0Muon::OK == (*itCandMuon)->status() ) {
          eMuons.push_back( fabs( (*itCandMuon)->pt() ) * GeV );
        }
        itCandMuon++;
      }
      if ( 0 != eMuons.size() ) {
        std::greater<double> greater_than;
        std::sort( eMuons.begin(), eMuons.end(), greater_than );
        std::vector<double>::const_iterator itEMu = eMuons.begin();
        log << MSG::DEBUG 
            << "Energies abs. (GeV) Muons ordonned"
            << endreq;
        log << MSG::DEBUG
            << "  ( ordonned to have first the greaters )"
            << endreq;
        log << MSG::DEBUG
            << "Only Muons with status OK and not the same energy"
            << endreq;
        std::vector<double>::const_iterator itEMuRetain = 
          eMuons.erase( std::unique(eMuons.begin(), eMuons.end() ),
                        eMuons.end() );
        log << MSG::DEBUG
	    << "Data eCut1, eCut2, scal for the greater muon"
            << endreq;
      }
      else {
        log << MSG::DEBUG
            << "No Muons with status OK"
            << endreq;
      }
      if ( 1 <= eMuons.size() ) {
        itCandMuon = candMuon->begin();
        while ( itCandMuon != candMuon->end() ) {
          if ( eMuons[ 0 ] == fabs( (*itCandMuon)->pt() ) * GeV ) {
            break;
          }
          itCandMuon++;   
        }
        m_Muon1 = (*itCandMuon);
        log << MSG::DEBUG
            << "  eCut1(Gev) = "
            << m_eMu1Cut1 / GeV
            <<" eCut2(GeV) = "
            << m_eMu1Cut2 / GeV
            << " scal = "
            << m_scalMu1 
            << endreq;
      }
      int indMuon = 0;
      while ( ( m_nMuSumMu > indMuon ) &&
              ( (int) eMuons.size() > indMuon ) ) {
        eSumMuons += eMuons[ indMuon ];
        indMuon++;
      }
      log << MSG::DEBUG
          << "Sum muons on "
          << m_nMuSumMu
          << " asked Only "
          << indMuon
          << " finded"
          << endreq;
      log << MSG::DEBUG
          << "Energy abs (GeV) sum muons = "
          << eSumMuons / GeV
          << endreq;
    }
  }
  else { 
    log << MSG::DEBUG
        << "Unable to retrieve L0MuonCandidates "
        << m_nameOfInputL0MuonCandidate
        << endreq; 
  }

  // L0 trigger algorithm
  // 
  // For Electron, Photon, PioLocal, PioGlobal, Hadron
  //   and for Mu1
  // Associated bit trig set ( with energy cut1 )
  //   if eSumEt >= eSumEtCut1 and NOT PileUpVeto and eCandidat > eCandidatCut1
  // Associated bit trig_down set ( with energy cut2 )
  //   if eSumEt >= eSumEtCut1 and NOT PileUpVeto and downscaling
  //
  // For SumMu
  // Associated bit trig set ( with energy cut1 )
  //   if eSumEt >= eSumEtCut1 and eSumMu > eSumMuCut1
  // Associated bit trig-down set ( with energy cut2 )
  //   if eSumEt >= eSumEtCut1 and downscaling
   
  bool L0SumEt = false;
  bool L0NotVeto = true;

  if ( 0 != m_SumEt ) {
    if ( m_eSumEtCut1 <= m_SumEt->et() ) {
      L0SumEt = true;
    }
  }
 
  if ( L0SumEt && L0NotVeto ) {

    if ( 0 !=  m_Electron) {
      setTrig( L0Trig::Electron, m_Electron->et(), m_eElCut1 );
      setTrig_down( L0Trig::Electron_down, m_Electron->et(),
                                           m_eElCut2, m_scalEl );    
    }

    if ( 0 != m_Photon ) {
      setTrig( L0Trig::Photon, m_Photon->et(), m_ePhCut1 );
      setTrig_down( L0Trig::Photon_down, m_Photon->et(),
                                         m_ePhCut2, m_scalPh );
    }
    
    if ( 0 != m_Pi0Local ) {
      setTrig( L0Trig::Pi0Local, m_Pi0Local->et(), m_ePi0LCut1 );
      setTrig_down( L0Trig::Pi0Local_down, m_Pi0Local->et(), m_ePi0LCut2,
                                                             m_scalPi0L );
    }
    
    if ( 0 != m_Pi0Global ) {
      setTrig( L0Trig::Pi0Global, m_Pi0Global->et(), m_ePi0GCut1);
      setTrig_down( L0Trig::Pi0Global_down, m_Pi0Global->et(),
                                            m_ePi0GCut2, m_scalPi0G );
    }
    
    if ( 0 != m_Hadron ) {
      setTrig( L0Trig::Hadron, m_Hadron->et(), m_eHaCut1 );
      setTrig_down( L0Trig::Hadron_down, m_Hadron->et(),
                                         m_eHaCut2, m_scalHa );
    }
    
    if ( 0 != m_Muon1 ) {
      setTrig( L0Trig::Mu1, fabs( m_Muon1->pt() ) * GeV, m_eMu1Cut1 ); 
      setTrig_down( L0Trig::Mu1_down, fabs( m_Muon1->pt() ) * GeV,
                                      m_eMu1Cut2, m_scalMu1 );
    }

  }

  if ( L0SumEt ) {
    setTrig( L0Trig::SumMu, eSumMuons, m_eSumMuCut1 );
    setTrig_down( L0Trig::SumMu_down, eSumMuons,
                                      m_eSumMuCut2, m_scalSumMu );
  }

    
  // Save result for others uses 

  L0DUReport* decisUnit = new L0DUReport( m_typeL0Trig );
  log << MSG::DEBUG
      << decisUnit->typeL0TrigName()
      << endreq;
  
  log << MSG::DEBUG
      << "Execution completed successfully" 
      << endreq;
  
  return StatusCode::SUCCESS; 
}

// Finalize
// Call by the application manager at the end of the application

StatusCode DecisionUnit::finalize() {
  
  MsgStream log(messageService(),name());
    
  log << MSG::DEBUG << "finalize" << endreq;
  
  return StatusCode::SUCCESS;
}

// For one candidat,
//  set one bit if ecut1 < e

void DecisionUnit::setTrig( L0Trig::Trig cand,
                            double eCand,
                            double eCut1 ) {
  if ( eCut1 < eCand ) {
    m_typeL0Trig |= 1 << L0Trig::Decis;
    m_typeL0Trig |= 1 << cand;
  }
  return;
}


// For one  candidat,
//  set one bit if ecut2 < e

void DecisionUnit::setTrig_down( L0Trig::Trig cand,
                                 double eCand,
                                 double eCut2,
                                 int scal ) {
  if ( eCut2 < eCand ) {
    m_nEtSupCut2[ (int) cand ] += 1 ;
    if ( scal == m_nEtSupCut2[ cand ] ) {
      m_typeL0Trig |= 1 << L0Trig::Decis;
      m_typeL0Trig |= 1 << cand;
      m_nEtSupCut2[ (int) cand ] = 0;
    }
  }
  return;
}




