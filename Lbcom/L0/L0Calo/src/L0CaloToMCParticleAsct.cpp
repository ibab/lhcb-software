// $Id: L0CaloToMCParticleAsct.cpp,v 1.4 2005-01-12 09:19:38 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from Event
#include "Event/MCParticle.h"
#include "Event/MCCaloDigit.h"
#include "Event/L0CaloCandidate.h"

#include "Linker/LinkerWithKey.h"
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
  : GaudiAlgorithm ( name , pSvcLocator ) 
  , m_inputContainer       ( L0CaloCandidateLocation::Default )
  , m_minimalFraction      ( 0.10 )
{
  declareProperty( "InputContainer"   , m_inputContainer  );
  declareProperty( "MinimalFraction"  , m_minimalFraction );  
}

//=============================================================================
// Destructor
//=============================================================================
L0CaloToMCParticleAsct::~L0CaloToMCParticleAsct() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloToMCParticleAsct::execute() {

  debug() << "==> Execute" << endreq;
  
  L0CaloCandidates* candidates = get<L0CaloCandidates>( m_inputContainer );

  LinkerWithKey< MCParticle > myLink( eventSvc(), msgSvc(), m_inputContainer );

  MCCaloDigits* mcEcalDigs = get<MCCaloDigits>( MCCaloDigitLocation::Ecal );
  MCCaloDigits* mcHcalDigs = get<MCCaloDigits>( MCCaloDigitLocation::Hcal );

  MCCaloDigits* mcDigs = 0;

  L0CaloCandidates::const_iterator cand;
  for ( cand = candidates->begin() ; candidates->end() != cand ; ++cand ) {
    mcDigs = 0;
    if ( ( L0Calo::Electron  == (*cand)->type() ) ||
         ( L0Calo::Photon    == (*cand)->type() ) ||
         ( L0Calo::Pi0Local  == (*cand)->type() ) ||
         ( L0Calo::Pi0Global == (*cand)->type() )   ) {
      mcDigs = mcEcalDigs;
    } else if ( ( L0Calo::Hadron  == (*cand)->type() )  ) {
      mcDigs = mcHcalDigs;
    }
    if ( 0 != mcDigs ) {
      debug() << "Candidate " << *(*cand) << endreq;
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
              verbose() << "  cell " << tmpCell << " Part " << part->key() 
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
            myLink.link( *cand, parts[ll], energies[ll] / eTot );
            debug() << " .. Relate MC " << parts[ll]->key()
                    << " with fraction " << energies[ll] / eTot << endreq;
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
