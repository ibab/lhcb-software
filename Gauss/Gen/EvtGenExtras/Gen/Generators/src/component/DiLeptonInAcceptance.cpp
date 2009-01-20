// $Id: DiLeptonInAcceptance.cpp,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "DiLeptonInAcceptance.h"

//#include "GaudiKernel/ToolFactory.h"
#include "Event/HepMCEvent.h"



//-----------------------------------------------------------------------------
// Implementation file for class : DiLeptonInAcceptance
//
// 2007-01-30 : Thomas Blake
//-----------------------------------------------------------------------------



DECLARE_TOOL_FACTORY( DiLeptonInAcceptance );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DiLeptonInAcceptance::DiLeptonInAcceptance( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IFullGenEventCutTool>( this );
  
  declareProperty( "PtMin"     , m_ptMin = 0*Gaudi::Units::GeV );
  declareProperty( "MinTheta"  , m_chargedThetaMin = 10*Gaudi::Units::mrad );
  declareProperty( "MaxTheta"  , m_chargedThetaMax = 400*Gaudi::Units::mrad );
  declareProperty( "MinMass"   , m_minMass = 0*Gaudi::Units::GeV );
  declareProperty( "MaxMass"   , m_maxMass = 100*Gaudi::Units::GeV );
  declareProperty( "PositiveLepton" , m_leptonPlusPDG = -13 );
  declareProperty( "NegativeLepton" , m_leptonMinusPDG = 13 );
  declareProperty( "CCFlag"    , m_ccFlag = true );
}

//=============================================================================
// Destructor
//=============================================================================

DiLeptonInAcceptance::~DiLeptonInAcceptance() {} 


//=============================================================================
// Selection function
//=============================================================================
bool DiLeptonInAcceptance::studyFullEvent( LHCb::HepMCEvents * theEvents ,
                                         LHCb::GenCollisions * /* col */ )
  const {

  std::list< HepMC::GenParticle* > leptonPlusList;
  std::list< HepMC::GenParticle* > leptonMinusList;

  for (LHCb::HepMCEvents::iterator theEventIter = theEvents->begin() ;
       theEventIter != theEvents->end() ; ++theEventIter) {
    
    
     for ( HepMC::GenEvent::particle_const_iterator
             iParticle = ( *theEventIter ) -> pGenEvt() -> particles_begin() ;
           iParticle != ( *theEventIter ) -> pGenEvt() -> particles_end() ; 
           ++iParticle ){
      
       HepMC::GenParticle* particle = (*iParticle);
       
       if ( isPositiveLepton( particle ) && isInAcceptance( particle ) ){
         debug() << " Found l+ in accpetance" << endmsg;
         leptonPlusList.push_back( particle );
       }
       
       if ( isNegativeLepton( particle ) && isInAcceptance( particle ) ){
         debug() << " Found l- in accpetance" << endmsg;
         leptonMinusList.push_back( particle );
       }       
     }

  }
  
  
  if ( leptonPlusList.empty() || leptonMinusList.empty() ){
    debug() << "No lepton pair found in this event" << endmsg ;
    return false;
  }
  
  // apply dimuon mass constraint
  double dimuonMass;
  
    
  for (std::list< HepMC::GenParticle * >::const_iterator 
         plusIter = leptonPlusList.begin() ; 
       plusIter != leptonPlusList.end() ; ++plusIter ) {      
      
    for (std::list <HepMC::GenParticle *>::const_iterator 
           minusIter = leptonMinusList.begin() ; 
         minusIter != leptonMinusList.end() ; ++minusIter ){


      // Check combination
      if ( !isCombination( (*plusIter), (*minusIter) ) ) continue;
      
      // Apply Di Muon Mass Cut
      HepMC::FourVector sum ;
      sum.setPx( (*minusIter) -> momentum().px() + (*plusIter)->momentum().px() ) ;
      sum.setPy( (*minusIter) -> momentum().py() + (*plusIter)->momentum().py() ) ;
      sum.setPz( (*minusIter) -> momentum().px() + (*plusIter)->momentum().pz() ) ;
      sum.setE ( (*minusIter) -> momentum().e()  + (*plusIter)->momentum().e() ) ;
      dimuonMass = sum.m();
      
      if ( dimuonMass >= m_minMass && dimuonMass <= m_maxMass ){
        debug() << " Combination passes mass cut " << endmsg;
        return true;
      }  

      
    }
  }  
  
  return false;
}

bool DiLeptonInAcceptance::isCombination( const HepMC::GenParticle *pPlus, 
                                          const HepMC::GenParticle *pMinus )
  const {
  
  if ( m_leptonPlusPDG  == pPlus->pdg_id() && 
       m_leptonMinusPDG == pMinus->pdg_id() ) return true;
  
  
  if ( m_ccFlag ){
    if ( -m_leptonPlusPDG  == pMinus->pdg_id() &&
         -m_leptonMinusPDG == pPlus->pdg_id() ) return true;
  }
  
  return false;
}

bool DiLeptonInAcceptance::isPositiveLepton( const HepMC::GenParticle *p )
  const {
  
  if ( -11 == p->pdg_id() || -13 == p->pdg_id() ) {
    debug() << "found positive letpon " << endmsg;
    return true;
  }
  
  return false;
}


bool DiLeptonInAcceptance::isNegativeLepton( const HepMC::GenParticle *p ) 
  const {
  
  if ( 11 == p->pdg_id() || 13 == p->pdg_id() ) {
    debug() << "found negative letpon" << endmsg;
    return true;
  }

  return false;  
}

bool DiLeptonInAcceptance::isInAcceptance( const HepMC::GenParticle *p )
  const {
  
   double px, py, pz, pp, theta, pt;
   
   pz = p->momentum().pz();
   
   if (pz < 0) return false;
   
   px = p->momentum().px();
   py = p->momentum().py();
   
   pp = sqrt( px*px + py*py + pz*pz );
   pt = sqrt( px*px + py*py);
   theta = acos( pz / pp );
   
   return ( pt    >= m_ptMin && 
            theta <= m_chargedThetaMax && 
            theta >= m_chargedThetaMin );  
}





//=============================================================================
