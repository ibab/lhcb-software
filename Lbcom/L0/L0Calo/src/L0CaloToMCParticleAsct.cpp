// $Id: L0CaloToMCParticleAsct.cpp,v 1.1 2002-07-01 08:17:12 ocallot Exp $
// Include files 

// LHCb Kernel
#include "Relations/RelationWeighted1D.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/IDataProviderSvc.h"

// from Event
#include "Event/MCParticle.h"
#include "Event/MCCaloDigit.h"
#include "Event/L0CaloCandidate.h"

// local
#include "L0CaloToMCParticleAsct.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloToMCParticleAsct
// Associate the truth to the L0Calo Candidates
// 2002-07-01 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<L0CaloToMCParticleAsct>          s_factory ;
const        IAlgFactory& L0CaloToMCParticleAsctFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0CaloToMCParticleAsct::L0CaloToMCParticleAsct( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ) 
  , m_inputContainer       ( L0CaloCandidateLocation::Default )
  , m_outputTable          ( "Rec/Relations/L0CaloCandidates" )
  , m_minimalFraction      ( 0.10 )
{
  declareProperty( "InputContainer"   , m_inputContainer  );
  declareProperty( "OutputTable"      , m_outputTable     );
  declareProperty( "MinimalFraction"  , m_minimalFraction );  
}

//=============================================================================
// Destructor
//=============================================================================
L0CaloToMCParticleAsct::~L0CaloToMCParticleAsct() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0CaloToMCParticleAsct::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloToMCParticleAsct::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  SmartDataPtr< L0CaloCandidates > candidates ( eventSvc(), m_inputContainer );
  if( 0 == candidates ) { 
    log << MSG::ERROR << "Unable to retrieve " << m_inputContainer << endreq; 
    return StatusCode::SUCCESS;
  }   

  SmartDataPtr<MCCaloDigits > mcEcalDigs ( eventSvc(), 
                                           MCCaloDigitLocation::Ecal );
  SmartDataPtr<MCCaloDigits > mcHcalDigs ( eventSvc(), 
                                           MCCaloDigitLocation::Hcal );

  typedef RelationWeighted1D<L0CaloCandidate,MCParticle,double>  Table;
  // create relation table and register it in the event transient store
  Table* table = new Table();
  StatusCode sc = eventSvc()->registerObject( m_outputTable, table );
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Unable to register the relation container="
        << m_outputTable << endreq;
    log << MSG::ERROR << "Status is " << sc << endreq;
    return sc ;
  }

  MCCaloDigits* mcDigs = 0;

  L0CaloCandidates::const_iterator cand;
  for ( cand = candidates->begin() ; candidates->end() != cand ; ++cand ) {
    mcDigs = 0;
    if ( ( L0Calo::Electron == (*cand)->type() ) ||
         ( L0Calo::Photon   == (*cand)->type() ) ||
         ( L0Calo::Pi0Local == (*cand)->type() )   ) {
      mcDigs = mcEcalDigs;
    } else if ( ( L0Calo::Hadron  == (*cand)->type() ) ||
                ( L0Calo::Hadron2 == (*cand)->type() )    ) {
      mcDigs = mcHcalDigs;
    }
    if ( 0 != mcDigs ) {
      log << MSG::DEBUG << "Candidate " << *cand << endreq;
      std::vector<const MCParticle*> parts;
      std::vector<double> energies;
      double eTot = 0.;
      
      CaloCellID cell = (*cand)->id();
      for ( int jj = 0 ; 2 > jj ; jj++ ) {
        for ( int kk = 0 ; 2 > kk ; kk++ ) {
          CaloCellID tmpCell = CaloCellID( cell.calo(), cell.area(),
                                           cell.row()+jj, cell.col()+kk );
          MCCaloDigit* dig = mcDigs->object( tmpCell );
          if ( 0 != dig ) {
            SmartRefVector<MCCaloHit> hits = dig->hits();
            for ( SmartRefVector<MCCaloHit>::const_iterator ith = hits.begin();
                  hits.end() != ith; ith++ ) {
              const MCParticle* part = (*ith)->particle();
              double energy = (*ith)->activeE();
              eTot += energy;
              log << MSG::VERBOSE 
                  << "  cell " << tmpCell << " Part " << part->key() 
                  << " e " << energy << endreq;
              bool add = true;
              for ( unsigned int ll = 0; parts.size() > ll; ll++ ) {
                if ( part == parts[ll] ) {
                  energies[ll] += energy;
                  add = false;
                }
              }
              if ( add ) {
                parts.push_back( part );
                energies.push_back( energy );
              }
            }
          }
        }
      }
      //== Now, select those particles with enough energy
      if ( 0 < eTot ) {
        for ( unsigned int ll = 0; parts.size() > ll; ll++ ) {
          if ( energies[ll] > m_minimalFraction * eTot ) {
            table->relate( *cand, parts[ll], energies[ll] / eTot );
            log << MSG::DEBUG << " .. Relate MC " << parts[ll]->key()
                << " with fraction " << energies[ll] / eTot << endreq;
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0CaloToMCParticleAsct::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
