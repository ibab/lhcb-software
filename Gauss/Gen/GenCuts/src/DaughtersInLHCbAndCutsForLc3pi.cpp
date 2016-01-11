// Include files

// local
#include "DaughtersInLHCbAndCutsForLc3pi.h"

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
// Implementation file for class : DaughtersInLHCbAndCutsForLc3pi
//
// 2016-01-05 : Victor Renaudin
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( DaughtersInLHCbAndCutsForLc3pi )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaughtersInLHCbAndCutsForLc3pi::DaughtersInLHCbAndCutsForLc3pi( const std::string& type,
                                                                const std::string& name,
                                                                const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
  declareInterface< IGenCutTool >( this ) ;
  declareProperty( "ChargedThetaMin"   , m_chargedThetaMin   = 10 * Gaudi::Units::mrad ) ;
  declareProperty( "ChargedThetaMax"   , m_chargedThetaMax   = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMin"   , m_neutralThetaMin   = 5 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMax"   , m_neutralThetaMax   = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "LcPtCuts"          , m_lcptCut           = 1500 * Gaudi::Units::MeV ) ;
  declareProperty( "DaughtersPtMinCut" , m_daughtersptminCut = 150 * Gaudi::Units::MeV ) ;
  declareProperty( "DaughtersPtMaxCut" , m_daughtersptmaxCut = 150 * Gaudi::Units::MeV ) ;
  declareProperty( "DaughtersPMinCut"  , m_daughterspminCut  = 1000 * Gaudi::Units::MeV ) ;
  }

//=============================================================================
// Destructor
//=============================================================================
DaughtersInLHCbAndCutsForLc3pi::~DaughtersInLHCbAndCutsForLc3pi( ) { ; }

//=============================================================================
// AndWithMinP function
//=============================================================================
bool DaughtersInLHCbAndCutsForLc3pi::applyCut( ParticleVector & theParticleVector ,
                                               const HepMC::GenEvent * /* theEvent */ ,
                                               const LHCb::GenCollision * /* theHardInfo */ )
  const {
  ParticleVector::iterator it ;

  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ) {
    // Check that the signal is the Lc sig
    if ( abs( (*it) -> pdg_id() ) !=4122  )
      Exception( "The signal is not our dear Lc" ) ;

    if ( ! passCuts( *it ) ) {
      it = theParticleVector.erase( it ) ;
    } else ++it ;
  }

  return ( ! theParticleVector.empty() ) ;
}

//=============================================================================
// Functions to test if the Lc and 3 pi from the daughters are in acceptance
//=============================================================================
bool DaughtersInLHCbAndCutsForLc3pi::passCuts( const HepMC::GenParticle * theSignal ) const {
  HepMC::GenVertex * EV = theSignal -> end_vertex() ;
  if ( 0 == EV ) return true ;

  typedef std::vector< HepMC::GenParticle * > Particles ;
  Particles stables ;
  Particles Lc_parents ;
  Particles lcdaughters ;

  HepMC::GenVertex::particle_iterator iter ;
  // first check on the Lc origin : it should come from a B decay
  int bOK=0;
  debug() << "Check the Lc ancestor " <<endmsg;
  double zB, goodzB;
  zB=-999.;
  goodzB=-999.;
  bool firstB = true;

 for ( iter = EV -> particles_begin( HepMC::ancestors ) ;
        iter != EV -> particles_end( HepMC::ancestors ) ; ++iter ) {

   Lc_parents.push_back( *iter );
    // is there a B
   if ( int(abs( (*iter) -> pdg_id() ))%10000> 500&& int(abs( (*iter) -> pdg_id() ))%10000<600 )bOK++;
   if ( int(abs( (*iter) -> pdg_id() ))%10000> 5000&& int(abs( (*iter) -> pdg_id() ))%10000<6000 )bOK++;

   // will be used to be sure that the B of a relative is the same B as the Lc one
   if ( ( int(abs( (*iter) -> pdg_id() ))%10000> 500&& int(abs( (*iter) -> pdg_id() ))%10000<600 && firstB == true) ||
   ( int(abs( (*iter) -> pdg_id() ))%10000> 5000&& int(abs( (*iter) -> pdg_id() ))%10000<6000 && firstB == true) ){
       goodzB = (*iter)->end_vertex()->position().z();
       firstB = false;
   }
 }
 debug() <<"start of Lc parent loop "<<bOK<<endmsg;
 if(bOK==0) return false;

for ( Particles::const_iterator it = Lc_parents.begin() ; it != Lc_parents.end() ;
        ++it ) {
  debug()<<" particle type"<<(*it)->pdg_id()<<"vtx "<<(*it)->end_vertex()<<endmsg;
  if((*it)->end_vertex()==0)continue;
  debug()<<"vtx "<<(*it)->end_vertex()->position().z()<<endmsg;
  if(abs((*it)->pdg_id())!=511&&abs((*it)->pdg_id())!=521&&abs((*it)->pdg_id())!=531&&abs((*it)->pdg_id())!=5122)continue;
  zB= (*it)->end_vertex()->position().z();
break;
  }

 debug() << "N beauty in Lc ancestors "<<bOK<<" vertex position "<<zB<<endmsg;

        //ask all Lc daughters in acceptance
  for ( iter = EV -> particles_begin( HepMC::descendants ) ;
        iter != EV -> particles_end( HepMC::descendants ) ; ++iter ) {
    // Fill all daughters
    if ( 0 == (*iter) -> end_vertex() ) stables.push_back( *iter ) ;

  }

  if ( stables.empty() )
    Exception( "Signal has no stable daughters !" ) ;

  int nKLc=0;
  int npiLc=0;
  int npLc=0;

  for ( iter = EV -> particles_begin( HepMC::descendants ) ;
        iter != EV -> particles_end( HepMC::descendants ) ; ++iter ) {
    // Fill all daughters but exclude photons (can be radiative photons)
    if ( 0 == (*iter) -> end_vertex() )
      if ( 22 != (*iter) -> pdg_id() )lcdaughters.push_back( *iter ) ;
    if ( 211 == abs(int((*iter) -> pdg_id() )))npiLc++;
    if ( 321 == abs(int((*iter) -> pdg_id())) )nKLc++;
    if ( 2212 == abs(int((*iter) -> pdg_id())) )npLc++;

  }

  if ( lcdaughters.empty() )
    Exception( "No Lc daughters in signal chain !" );
        // check that the Lc goes into p K pi
        debug() << "Check Lc daughters nK "<<nKLc<<" Npi "<<npiLc<<" Np "<<npLc<<endmsg;
        if(nKLc*npiLc*npLc!=1) return false;
  double angle( 0. ) ;
  double firstpz = stables.front() -> momentum().pz() ;

  debug() << "New event" << endmsg ;

  for ( Particles::const_iterator it = stables.begin() ; it != stables.end() ;
        ++it ) {

    debug() << "Check particle " << (*it) -> pdg_id() << " with angle "
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
  debug() << "Check other cuts" << endmsg ;

  debug() << "Lc pT = " << theSignal -> momentum().perp() << endmsg ;
  if ( theSignal -> momentum().perp() < m_lcptCut ) return false ;

  double minpt = 14.*Gaudi::Units::TeV ;
  double maxpt = 0. ;
  double minp  = 14.*Gaudi::Units::TeV ;


  for ( Particles::const_iterator it = lcdaughters.begin() ; it != lcdaughters.end() ;
        ++it ) {
    debug() << "Daughter pT = " << (*it) -> momentum().perp()
            << " p = " << (*it) -> momentum().rho() << endmsg ;

  if ( (*it) -> momentum().perp() > maxpt )
      maxpt = (*it) -> momentum().perp() ;
    if ( (*it) -> momentum().perp() < minpt )
      minpt = (*it) -> momentum().perp() ;
    if ( (*it) -> momentum().rho() < minp )
      minp = (*it) -> momentum().rho() ;
  }

  debug() << "Min Pt = " << minpt
          << " Max Pt = " << maxpt
          << " Min P = " << minp << endmsg ;

  if ( minpt < m_daughtersptminCut ) return false ;
  if ( maxpt < m_daughtersptmaxCut ) return false ;
  if ( minp  < m_daughterspminCut  ) return false ;
        // now look for 3 pions in the Lc family
  debug() << "Check for 3 pions in addition to Lc " <<zB<<endmsg;
        int Npions_inacc=0;

  for ( iter = EV -> particles_begin( HepMC::relatives) ;
        iter != EV -> particles_end( HepMC::relatives ) ; ++iter ) {

 if ( 0 == (*iter) -> end_vertex() ) stables.push_back( *iter ) ;
  }

    //now loop on the stable particles
    //we  look at all the children of the B ancestor by requiring z>ZB
    //this will include the 3 Lc daughters so we ask 4 pions in the acceptance
for ( Particles::const_iterator it = stables.begin() ; it != stables.end() ;
        ++it ) {

    double zB_B_parent_stables;
    zB_B_parent_stables=-999.;
    // bool firstB_stables = true;
    HepMC::GenVertex * EV_parent_stables = (*it) -> production_vertex() ;

    HepMC::GenVertex::particle_iterator parent = EV_parent_stables -> particles_begin( HepMC::ancestors );

    //We want to be sure that the stables come from the same B as the Lc
    while( ( (int(abs( (*parent) -> pdg_id() ))%10000> 500 && int(abs( (*parent) -> pdg_id() ))%10000<600) ||
     (int(abs( (*parent) -> pdg_id() ))%10000> 5000 && int(abs( (*parent) -> pdg_id() ))%10000<6000) ) && ( (*parent) -> status() != 4 || (*parent) -> status() != 1 ) ){
         parent = (*parent) -> production_vertex() -> particles_begin( HepMC::ancestors );
    }

    if ( (int(abs( (*parent) -> pdg_id() ))%10000> 500 && int(abs( (*parent) -> pdg_id() ))%10000<600) ||
     (int(abs( (*parent) -> pdg_id() ))%10000> 5000 && int(abs( (*parent) -> pdg_id() ))%10000<6000) ){
         zB_B_parent_stables = (*parent) -> end_vertex() -> position().z();
     }

    if ((*it)->production_vertex()==0) continue;

    debug()<<"particle relative"<<(*it) -> pdg_id()<<" z " << (*it)->production_vertex()->position().z()<<endmsg;

    if( zB_B_parent_stables == goodzB) continue;

    if( (*it)->production_vertex()->position().z()<zB) continue;

    if( (*it)->production_vertex()->position().z()>200.) continue;

    if ( 211 != abs((*it) -> pdg_id() )) continue;

    angle = (*it) -> momentum().theta() ;

    if ( fabs( sin( angle ) ) < fabs(  sin( m_chargedThetaMin ) ) ) continue;
    if ( fabs( sin( angle ) ) > fabs( sin( m_chargedThetaMax ) ) ) continue;
    Npions_inacc++;
  }
    debug() << "Number of  pions in the Lc family that passed " <<Npions_inacc<<endmsg;

         if (Npions_inacc<4)return false ;
  debug() << "Event passed !" << endmsg ;

  return true ;
}
