// $Id: DiLeptonInAcceptance.cpp,v 1.3 2009-08-10 13:15:56 tblake Exp $
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
  
  declareProperty( "LeptonIDOne" , m_leptonOnePDG = 13 );
  declareProperty( "LeptonIDTwo" , m_leptonTwoPDG = 13 );
  
  declareProperty( "LeptonOnePtMin", m_ptMinLOne = 0*Gaudi::Units::GeV );
  declareProperty( "LeptonOnePMin", m_pMinLOne = 0*Gaudi::Units::GeV );
  declareProperty( "LeptonTwoPtMin", m_ptMinLTwo = 0*Gaudi::Units::GeV );
  declareProperty( "LeptonTwoPMin", m_pMinLTwo = 0*Gaudi::Units::GeV );
  declareProperty( "LeptonOneMinTheta"  , m_thetaMinLOne = 10*Gaudi::Units::mrad );
  declareProperty( "LeptonOneMaxTheta"  , m_thetaMaxLOne = 400*Gaudi::Units::mrad );
  declareProperty( "LeptonTwoMinTheta"  , m_thetaMinLTwo = 10*Gaudi::Units::mrad );
  declareProperty( "LeptonTwoMaxTheta"  , m_thetaMaxLTwo = 400*Gaudi::Units::mrad );
  

  declareProperty( "MinMass"   , m_minMass = 0*Gaudi::Units::GeV );
  declareProperty( "MaxMass"   , m_maxMass = 100*Gaudi::Units::GeV );
 
  declareProperty( "RequireOppositeSign" , m_oppSign = true );
  declareProperty( "RequireSameSign" , m_sameSign = false );

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
  
  std::list< HepMC::GenParticle* > leptonOneList;
  std::list< HepMC::GenParticle* > leptonTwoList;
  
  if ( m_oppSign && m_sameSign ) {
    warning() << " Trying to force both opposite and same sign leptons! Choosing same sign " << endmsg;
  }
  
  
  for (LHCb::HepMCEvents::iterator theEventIter = theEvents->begin() ;
       theEventIter != theEvents->end() ; ++theEventIter) {
    
    
    for ( HepMC::GenEvent::particle_const_iterator
            iParticle = ( *theEventIter ) -> pGenEvt() -> particles_begin() ;
          iParticle != ( *theEventIter ) -> pGenEvt() -> particles_end() ; 
          ++iParticle ){
      
      HepMC::GenParticle* particle = (*iParticle);
      
      int absid = abs(particle->pdg_id());
      
      if ( m_leptonOnePDG == absid && //isInAcceptance( particle ) ){
           isInAcceptance( particle, m_pMinLOne, m_ptMinLOne, m_thetaMinLOne, m_thetaMaxLOne )  ){
        debug() << " Found lepton [" << m_leptonOnePDG << "] in accpetance" << endmsg;
        leptonOneList.push_back( particle );
      }
      
      if ( m_leptonTwoPDG == absid && //isInAcceptance( particle ) ) {
           isInAcceptance( particle, m_pMinLTwo, m_ptMinLTwo, m_thetaMinLTwo, m_thetaMaxLTwo )  ){
        debug() << " Found lepton [" << m_leptonTwoPDG << "] in accpetance" << endmsg;
        leptonTwoList.push_back( particle );
      }
    } 
  }
  
  
  if ( leptonOneList.empty() || leptonTwoList.empty() ){
    debug() << "No leptons found in this event" << endmsg ;
    return false;
  }
  
  // apply dimuon mass constraint
  double dimuonMass;
  
  std::list< HepMC::GenParticle * >::const_iterator iterOne;
  std::list< HepMC::GenParticle * >::const_iterator iterTwo;
  
  
  for ( iterOne = leptonOneList.begin() ; 
        iterOne != leptonOneList.end() ; ++iterOne ) {      
    
    for ( iterTwo = leptonTwoList.begin() ; 
          iterTwo != leptonTwoList.end() ; ++iterTwo ){
      
      // Check combination
      if ( !isCombination( (*iterOne), (*iterTwo) ) ) continue;
      
      // Apply Di Muon Mass Cut
      HepMC::FourVector sum ;
      sum.setPx( (*iterOne) -> momentum().px() + (*iterTwo)->momentum().px() ) ;
      sum.setPy( (*iterOne) -> momentum().py() + (*iterTwo)->momentum().py() ) ;
      sum.setPz( (*iterOne) -> momentum().px() + (*iterTwo)->momentum().pz() ) ;
      sum.setE ( (*iterOne) -> momentum().e()  + (*iterTwo)->momentum().e() ) ;
      dimuonMass = sum.m();
      
      if ( dimuonMass >= m_minMass && dimuonMass <= m_maxMass ){
        debug() << " Combination passes mass cut " << endmsg;
        return true;
      }  

      
    }
  }  
  
  return false;
}


bool DiLeptonInAcceptance::isCombination( const HepMC::GenParticle *particleOne, 
                                          const HepMC::GenParticle *particleTwo )
  const {
  
  if ( particleOne == particleTwo ) return false ;
  
  int combination =  (particleOne->pdg_id())*(particleTwo->pdg_id()) ;
  
  // Same sign takes precedence 
  if ( m_sameSign ) { 
    if ( combination < 0 ) return false ;
  } 
  else if ( m_oppSign ) {
    if ( combination > 0 ) return false;
  }
  
  return true ;
}



bool DiLeptonInAcceptance::isInAcceptance( const HepMC::GenParticle *p, 
                                           const double pmin, 
                                           const double ptmin, 
                                           const double thetamin, 
                                           const double thetamax ) const 
{
  
   double px, py, pz, pp, theta, pt;
   
   pz = p->momentum().pz();
   
   if (pz < 0) return false;
   
   px = p->momentum().px();
   py = p->momentum().py();
   
   pp = sqrt( px*px + py*py + pz*pz );
   if ( pp < pmin ) return false ;
   
   pt = sqrt( px*px + py*py);
   if ( pt < ptmin ) return false ;
   
   theta = acos( pz / pp );
   
   return ( theta <= thetamax && 
            theta >= thetamin );
}



//=============================================================================
