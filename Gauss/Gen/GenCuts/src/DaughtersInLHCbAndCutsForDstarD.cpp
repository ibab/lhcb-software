// Include files 

// local
#include "DaughtersInLHCbAndCutsForDstarD.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Transform4DTypes.h"

// from Kernel
#include "Kernel/ParticleID.h"
#include "GaudiKernel/Vector4DTypes.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// from Generators
#include "GenEvent/HepMCUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaughtersInLHCbAndCutsForDstarD
//
// 2015-04-23 : Guy Wormser
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( DaughtersInLHCbAndCutsForDstarD )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaughtersInLHCbAndCutsForDstarD::DaughtersInLHCbAndCutsForDstarD( const std::string& type,
                                                                const std::string& name,
                                                                const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
  declareInterface< IGenCutTool >( this ) ;
  declareProperty( "ChargedThetaMin"   , m_chargedThetaMin   = 10 * Gaudi::Units::mrad ) ;
  declareProperty( "ChargedThetaMax"   , m_chargedThetaMax   = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMin"   , m_neutralThetaMin   = 5 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMax"   , m_neutralThetaMax   = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "D0PtCuts"          , m_d0ptCut           = 1500 * Gaudi::Units::MeV ) ;
  declareProperty( "DaughtersPtMinCut" , m_daughtersptminCut = 150 * Gaudi::Units::MeV ) ;
  declareProperty( "DaughtersPtMaxCut" , m_daughtersptmaxCut = 150 * Gaudi::Units::MeV ) ;
  declareProperty( "DaughtersPMinCut"  , m_daughterspminCut  = 1000 * Gaudi::Units::MeV ) ;
  declareProperty( "SoftPiPtCut"       , m_softpiptCut       = 100 * Gaudi::Units::MeV ) ;
  declareProperty( "D0CTauCut"         , m_d0ctauCut         = -1. ) ;  
}

//=============================================================================
// Destructor 
//=============================================================================
DaughtersInLHCbAndCutsForDstarD::~DaughtersInLHCbAndCutsForDstarD( ) { ; }

//=============================================================================
// AndWithMinP function
//=============================================================================
bool DaughtersInLHCbAndCutsForDstarD::applyCut( ParticleVector & theParticleVector ,
                                               const HepMC::GenEvent * /* theEvent */ ,
                                               const LHCb::GenCollision * /* theHardInfo */ )
  const {
  ParticleVector::iterator it ;
  
  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ) {  
 if( msgLevel (MSG::DEBUG))debug()<<" particle type"<<(*it)->pdg_id()<<"vtx "<<(*it)->end_vertex()<<endmsg;  
    // Check that the signal is the D* sig
    if ( abs( (*it) -> pdg_id() ) !=413  ) 
         Exception( "The signal is not our dear D*" ) ;
    
    if ( ! passCuts( *it ) ) {
      it = theParticleVector.erase( it ) ;
    } else ++it ;
  }

  return ( ! theParticleVector.empty() ) ;
}

//=============================================================================
// Functions to test if the D* and 3 pi from the daughters are in acceptance
//=============================================================================
bool DaughtersInLHCbAndCutsForDstarD::passCuts( const HepMC::GenParticle * theSignal ) const {
  HepMC::GenVertex * EV = theSignal -> end_vertex() ;
  if ( 0 == EV ) return true ;

  typedef std::vector< HepMC::GenParticle * > Particles ;
  Particles unstables, stables ;
 Particles dstar_parents ;
  HepMC::GenParticle * theSoftPion( 0 ) ;
  HepMC::GenParticle * theD0( 0 ) ;
  HepMC::GenParticle * theParent( 0 );
  Particles d0daughters ;
  
  HepMC::GenVertex::particle_iterator iter ;
  // first check on the D* origin : it should come from a B decay
  int bOK=0;
  if( msgLevel (MSG::DEBUG))debug() << "Check the D* ancestor " <<endmsg;
  double zB;
  zB=-999.;
  
 for ( iter = EV -> particles_begin( HepMC::ancestors ) ; 
        iter != EV -> particles_end( HepMC::ancestors ) ; ++iter ) {
   
   dstar_parents.push_back( *iter );
   
    // is there a B
   if ( int(abs( (*iter) -> pdg_id() ))%10000> 500&& int(abs( (*iter) -> pdg_id() ))%10000<600 )bOK++;
 if ( int(abs( (*iter) -> pdg_id() ))%10000> 5000&& int(abs( (*iter) -> pdg_id() ))%10000<6000 )bOK++;  
 } 
 if( msgLevel (MSG::DEBUG))debug() <<"start of D* parent lmoop"<<bOK<<endmsg;
 if(bOK==0) return false;
 
for ( Particles::const_iterator it = dstar_parents.begin() ; it != dstar_parents.end() ;
        ++it ) {
  if( msgLevel (MSG::DEBUG))debug()<<" particle type"<<(*it)->pdg_id()<<"vtx "<<(*it)->end_vertex()<<endmsg;
  if((*it)->end_vertex()==0)continue;
  if( msgLevel (MSG::DEBUG))debug()<<"vtx "<<(*it)->end_vertex()->position().z()<<endmsg;
  if(abs((*it)->pdg_id())!=511&&abs((*it)->pdg_id())!=521&&abs((*it)->pdg_id())!=531&&abs((*it)->pdg_id())!=5122)continue;
  zB= (*it)->end_vertex()->position().z();
break;
  }
 
 if( msgLevel (MSG::DEBUG))debug() << "N beautty in D* ancestors"<<bOK<<" vertex position "<<zB<<endmsg;
       
        //ask all D* daughters in acceptance
  for ( iter = EV -> particles_begin( HepMC::descendants ) ; 
        iter != EV -> particles_end( HepMC::descendants ) ; ++iter ) {
    // Fill all daughters
    if ( 0 == (*iter) -> end_vertex() ) stables.push_back( *iter ) ;

    // The D0
    if ( abs( (*iter) -> pdg_id() ) == 421 ) theD0 = (*iter) ;

    // The soft pion
    theParent = *( (*iter) -> production_vertex() -> particles_in_const_begin() ) ;
    if ( ( 413 == abs( theParent -> pdg_id() ) ) && ( 211 == abs( (*iter) -> pdg_id() ) ) )
      theSoftPion = (*iter) ;
  }

  if ( 0 == theD0 )
    Exception( "No D0 in the signal decay chain !" ) ;

  if ( stables.empty() )
    Exception( "Signal has no stable daughters !" ) ;

  if ( 0 == theSoftPion ) 
    Exception( "No soft pion in the decay chain !" ) ;  

  // daughters of D0
   HepMC::GenVertex *EVD0 = theD0 -> end_vertex() ;
  if ( 0 == EVD0 ) 
    Exception( "The D0 has no daughters" ) ;
  int nKD0=0;
  int npiD0=0;
  
  for ( iter = EVD0 -> particles_begin( HepMC::descendants ) ; 
        iter != EVD0 -> particles_end( HepMC::descendants ) ; ++iter ) {
    // Fill all daughters but exclude photons (can be radiative photons)
    if ( 0 == (*iter) -> end_vertex() )
      if ( 22 != (*iter) -> pdg_id() )d0daughters.push_back( *iter ) ;
    if ( 211 == abs(int((*iter) -> pdg_id() )))npiD0++;
    if ( 321 == abs(int((*iter) -> pdg_id())) )nKD0++;
    
  }

  if ( d0daughters.empty() )
    Exception( "No D0 daughters in signal chain !" );
        // check that the d0 goes into K pi
        if( msgLevel (MSG::DEBUG))debug() << "Check D0 duaghters nK " << nKD0<<"N pi"<<npiD0<<endmsg;
        if(nKD0*npiD0!=1) return false;
  double angle( 0. ) ;
  double firstpz = stables.front() -> momentum().pz() ;

  if( msgLevel (MSG::DEBUG))debug() << "New event" << endmsg ;

  for ( Particles::const_iterator it = stables.begin() ; it != stables.end() ;
        ++it ) {

    if( msgLevel (MSG::DEBUG))debug() << "Check particle " << (*it) -> pdg_id() << " with angle " 
            << (*it) -> momentum().theta() / Gaudi::Units::mrad 
            << " mrad." << endmsg ;
    // All particles in same direction
    if ( 0 > ( firstpz * ( (*it) -> momentum().pz() ) ) ) return false ;

    angle = (*it) -> momentum().theta() ;
    
    LHCb::ParticleID pid( (*it) -> pdg_id() ) ;
    //only charged tracks to be considered
    if ( 0 != pid.threeCharge() ) {
        if ( fabs( sin( angle ) ) > fabs( sin( m_chargedThetaMax ) ) ) 
        return false ;
      if ( fabs( sin( angle ) ) < fabs( sin( m_chargedThetaMin ) ) ) 
        return false ;
    }
  }

  // Now check other cuts
  if( msgLevel (MSG::DEBUG))debug() << "Check other cuts" << endmsg ;

  if( msgLevel (MSG::DEBUG))debug() << "D0 pT = " << theD0 -> momentum().perp() << endmsg ;
  if ( theD0 -> momentum().perp() < m_d0ptCut ) return false ;
  
 

  if( msgLevel (MSG::DEBUG))debug() << "Soft pion pT = " << theSoftPion -> momentum().perp() << endmsg ;
  if ( theSoftPion -> momentum().perp() < m_softpiptCut ) return false ;
  
  double minpt = 14.*Gaudi::Units::TeV ;
  double maxpt = 0. ;
  double minp  = 14.*Gaudi::Units::TeV ;
 
  
  for ( Particles::const_iterator it = d0daughters.begin() ; it != d0daughters.end() ;
        ++it ) {  
    if( msgLevel (MSG::DEBUG))debug() << "Daughter pT = " << (*it) -> momentum().perp() 
            << " p = " << (*it) -> momentum().rho() << endmsg ;
   
  if ( (*it) -> momentum().perp() > maxpt ) 
      maxpt = (*it) -> momentum().perp() ;
    if ( (*it) -> momentum().perp() < minpt ) 
      minpt = (*it) -> momentum().perp() ;
    if ( (*it) -> momentum().rho() < minp ) 
      minp = (*it) -> momentum().rho() ;
  }
  
  if( msgLevel (MSG::DEBUG))debug() << "Min Pt = " << minpt
          << " Max Pt = " << maxpt 
          << " Min P = " << minp << endmsg ;
  
  if ( minpt < m_daughtersptminCut ) return false ;
  if ( maxpt < m_daughtersptmaxCut ) return false ;
  if ( minp  < m_daughterspminCut  ) return false ;
        // now look for 3 pions in the D* family
  if( msgLevel (MSG::DEBUG))debug() << "Check for 3 pions in addition to D* " <<zB<<endmsg;
        int Npions_inacc=0;
        
  for ( iter = EVD0 -> particles_begin( HepMC::relatives) ; 
        iter != EVD0 -> particles_end( HepMC::relatives ) ; ++iter ) {
   
 if ( 0 == (*iter) -> end_vertex() ) stables.push_back( *iter ) ;
 if ( 0 != (*iter) -> end_vertex() ) unstables.push_back( *iter ) ;
  }
  //now loop on the unstable particles
 //we  look at all the children of the B ancestor by requiring z>ZB
    //we ask a double charm event but no Ds
  int nD;
  nD=0;
  
  //the family contains both B.. we have to request that the second D comes from teh same B  
for ( Particles::const_iterator it = unstables.begin() ; it != unstables.end() ;
        ++it ) {
   
    if ((*it)->production_vertex()==0) continue;
    if( msgLevel (MSG::DEBUG))debug()<<"particle relative"<<(*it) -> pdg_id()<<" z " << (*it)->production_vertex()->position().z()<<endmsg;
   
   
    if( (*it)->production_vertex()->position().z()!=zB) continue;
 if ( 421 == abs((*it) -> pdg_id() ))nD++;
      if ( 411 == abs((*it) -> pdg_id() ))nD++;
  
  }
 if (nD<2) return false;
 
  //now loop on the stable particles
 //we  look at all the children of the B ancestor by requiring z>ZB
    //this will include the 3 D* daughters so we ask 5 pions in the accpetance
for ( Particles::const_iterator it = stables.begin() ; it != stables.end() ;
        ++it ) {
   
    if ((*it)->production_vertex()==0) continue;
    if( msgLevel (MSG::DEBUG))debug()<<"particle relative"<<(*it) -> pdg_id()<<" z " << (*it)->production_vertex()->position().z()<<endmsg;
    if( (*it)->production_vertex()->position().z()<zB) continue;
 if( (*it)->production_vertex()->position().z()>200.) continue;
    if ( 211 != abs((*it) -> pdg_id() )) continue;
angle = (*it) -> momentum().theta() ;
      if ( fabs( sin( angle ) ) < fabs(  sin( m_chargedThetaMin ) ) ) continue;
      if ( fabs( sin( angle ) ) > fabs( sin( m_chargedThetaMax ) ) ) continue;
      Npions_inacc++;
  }
    if( msgLevel (MSG::DEBUG))debug() << "Number of  pions in the D* family that passed " <<Npions_inacc<<endmsg;
    
         if (Npions_inacc<5)return false ;
  if( msgLevel (MSG::DEBUG))debug() << "Event passed !" << endmsg ;
  
  return true ;
}


