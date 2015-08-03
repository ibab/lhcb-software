// Include files 

// from Event
#include "Event/MCParticle.h"
#include "Event/MCCaloDigit.h"
#include "Event/L0CaloCandidate.h"
#include "Event/L0DUBase.h"

#include "Linker/LinkerWithKey.h"
// local
#include "L0CaloToMCParticleAsct.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloToMCParticleAsct
// Associate the truth to the L0Calo Candidates
// 2002-07-01 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( L0CaloToMCParticleAsct )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0CaloToMCParticleAsct::L0CaloToMCParticleAsct( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
  , m_inputContainer       ( LHCb::L0CaloCandidateLocation::Default )
  , m_minimalFraction      ( 0.10 )
{
  declareProperty( "InputContainer"   , m_inputContainer  );
  declareProperty( "MinimalFraction"  , m_minimalFraction );  
}

//=============================================================================
// Destructor
//=============================================================================
L0CaloToMCParticleAsct::~L0CaloToMCParticleAsct() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloToMCParticleAsct::execute() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  
  LHCb::L0CaloCandidates* candidates = get<LHCb::L0CaloCandidates>( m_inputContainer );

  LinkerWithKey< LHCb::MCParticle, LHCb::L0CaloCandidate > myLink( eventSvc(), msgSvc(), m_inputContainer );

  LHCb::MCCaloDigits* mcEcalDigs = get<LHCb::MCCaloDigits>( LHCb::MCCaloDigitLocation::Ecal );
  LHCb::MCCaloDigits* mcHcalDigs = get<LHCb::MCCaloDigits>( LHCb::MCCaloDigitLocation::Hcal );

  LHCb::MCCaloDigits* mcDigs = 0;

  LHCb::L0CaloCandidates::const_iterator cand;
  for ( cand = candidates->begin() ; candidates->end() != cand ; ++cand ) {
    mcDigs = 0;
    if ( ( L0DUBase::CaloType::Electron  == (*cand)->type() ) ||
         ( L0DUBase::CaloType::Photon    == (*cand)->type() ) ||
         ( L0DUBase::CaloType::Pi0Local  == (*cand)->type() ) ||
         ( L0DUBase::CaloType::Pi0Global == (*cand)->type() )   ) {
      mcDigs = mcEcalDigs;
    } else if ( ( L0DUBase::CaloType::Hadron  == (*cand)->type() )  ) {
      mcDigs = mcHcalDigs;
    }
    if ( 0 != mcDigs ) {
      if( msgLevel(MSG::DEBUG) ) debug() << "Candidate " << *(*cand) << endmsg;
      std::vector<const LHCb::MCParticle*> parts;
      std::vector<double> energies;
      double eTot = 0.;
      
      LHCb::CaloCellID cell = (*cand)->id();
      for ( int jj = 0 ; 2 > jj ; jj++ ) {
        for ( int kk = 0 ; 2 > kk ; kk++ ) {
          LHCb::CaloCellID tmpCell = LHCb::CaloCellID( cell.calo(), cell.area(),
                                                       cell.row()+jj, cell.col()+kk );
          LHCb::MCCaloDigit* dig = mcDigs->object( tmpCell );
          if ( 0 != dig ) {
            SmartRefVector<LHCb::MCCaloHit> hits = dig->hits();
            for ( SmartRefVector<LHCb::MCCaloHit>::const_iterator ith = hits.begin();
                  hits.end() != ith; ith++ ) {
              const LHCb::MCParticle* part = (*ith)->particle();
              double energy = (*ith)->activeE();
              eTot += energy;
              if( msgLevel(MSG::VERBOSE) ) 
                verbose() << "  cell " << tmpCell << " Part " << part->key() 
                          << " e " << energy << endmsg;
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
            myLink.link( *cand, parts[ll], energies[ll] / eTot );
            if( msgLevel(MSG::DEBUG) ) 
              debug() << " .. Relate MC " << parts[ll]->key()
                      << " with fraction " << energies[ll] / eTot << endmsg;
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
