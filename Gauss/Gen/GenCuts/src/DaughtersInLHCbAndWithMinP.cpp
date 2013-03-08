// $Id: DaughtersInLHCbAndWithMinP.cpp,v 1.7 2008-07-09 14:33:47 robbep Exp $
// Include files 

// local
#include "DaughtersInLHCbAndWithMinP.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Kernel
#include "Kernel/ParticleID.h"
#include "GaudiKernel/Vector4DTypes.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// from Generators
#include "GenEvent/HepMCUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaughtersInLHCbAndWithMinP
//
// 2005-03-02 : Alex Shires
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( DaughtersInLHCbAndWithMinP );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaughtersInLHCbAndWithMinP::DaughtersInLHCbAndWithMinP( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IGenCutTool >( this ) ;
    declareProperty( "ChargedThetaMin" , m_chargedThetaMin = 10 * Gaudi::Units::mrad ) ;
    declareProperty( "ChargedThetaMax" , m_chargedThetaMax = 400 * Gaudi::Units::mrad ) ;
    declareProperty( "NeutralThetaMin" , m_neutralThetaMin = 5 * Gaudi::Units::mrad ) ;
    declareProperty( "NeutralThetaMax" , m_neutralThetaMax = 400 * Gaudi::Units::mrad ) ;
    declareProperty( "MinMuonP" ,        m_minMuonP = 3000 * Gaudi::Units::MeV ) ;
    declareProperty( "MinTrackP" ,       m_minTrackP = 1000 * Gaudi::Units::MeV ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
DaughtersInLHCbAndWithMinP::~DaughtersInLHCbAndWithMinP( ) { ; }

//=============================================================================
// AndWithMinP function
//=============================================================================
bool DaughtersInLHCbAndWithMinP::applyCut( ParticleVector & theParticleVector ,
                                const HepMC::GenEvent * /* theEvent */ ,
                                const LHCb::GenCollision * /* theHardInfo */ )
  const {
  ParticleVector::iterator it ;
  
  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ) {    
    if ( ! passCuts( *it ) ) {
      it = theParticleVector.erase( it ) ;
    } else ++it ;
  }

  
  return ( ! theParticleVector.empty() ) ;
}

//=============================================================================
// Functions to test if all daughters are in AndWithMinP
//=============================================================================
bool DaughtersInLHCbAndWithMinP::passCuts( const HepMC::GenParticle * theSignal ) const {
  HepMC::GenVertex * EV = theSignal -> end_vertex() ;
  if ( 0 == EV ) return true ;

  typedef std::vector< HepMC::GenParticle * > Particles ;
  Particles stables ;
  HepMC::GenVertex::particle_iterator iter ;

  for ( iter = EV -> particles_begin( HepMC::descendants ) ; 
        iter != EV -> particles_end( HepMC::descendants ) ; ++iter ) {
    if ( 0 == (*iter) -> end_vertex() ) stables.push_back( *iter ) ;
  }  

  if ( stables.empty() )
    Exception( "Signal has no stable daughters !" ) ;

  double angle( 0. ) ;
  double firstpz = stables.front() -> momentum().pz() ;

  debug() << "New event" << endmsg ;

  for ( Particles::const_iterator it = stables.begin() ; it != stables.end() ;
        ++it ) {

    debug() << "Check particle " << (*it) -> pdg_id() << " with angle " 
            << (*it) -> momentum().theta() / Gaudi::Units::mrad 
            << " mrad." << endmsg ;
   
    // Remove neutrinos
    if ( ( 12 == abs( (*it) -> pdg_id() ) ) || 
         ( 14 == abs( (*it) -> pdg_id() ) ) || 
         ( 16 == abs( (*it) -> pdg_id() ) ) ) continue ;
 
    // Don't use daughters of Lambda and KS:
    HepMC::GenParticle * theParent ;
    theParent = 
      *( (*it) -> production_vertex() -> particles_in_const_begin() ) ;
    if ( 3122 == abs( theParent -> pdg_id() ) ) continue ;
    if ( 310 == theParent -> pdg_id() ) continue ;

    // Consider only gammas from pi0 and eta
    if ( 22 == (*it) -> pdg_id() ) {
      if ( ( 111 != theParent -> pdg_id() ) &&
           ( 221 != theParent -> pdg_id() ) ) continue ;
    }

    // All particles in same direction
    if ( 0 > ( firstpz * ( (*it) -> momentum().pz() ) ) ) return false ;

    angle = (*it) -> momentum().theta() ;
    
    LHCb::ParticleID pid( (*it) -> pdg_id() ) ;
    if ( 0 == pid.threeCharge() ) {
      if ( fabs( sin( angle ) ) > fabs( sin( m_neutralThetaMax ) ) ) 
        return false ;
      if ( fabs( sin( angle ) ) < fabs( sin( m_neutralThetaMin ) ) ) 
        return false ;
    } else {
      if ( fabs( sin( angle ) ) > fabs( sin( m_chargedThetaMax ) ) ) 
        return false ;
      if ( fabs( sin( angle ) ) < fabs( sin( m_chargedThetaMin ) ) ) 
        return false ;
      //check momentum
      bool pass = true;
      if ( 13 == abs( (*it) -> pdg_id() ) )  pass = momentumCut((*it), m_minMuonP );
      else   pass = momentumCut((*it), m_minTrackP );
      if (!pass) return false;
    }
  }
  

  debug() << "Event passed !" << endmsg ;
  
  return true ;
}


bool DaughtersInLHCbAndWithMinP::momentumCut( const HepMC::GenParticle *p, 
                                           const double pmin ) const 
{
   bool pass(true);  
   double px, py, pz, pp;
   
   pz = p->momentum().pz();
   
   if (pz < 0) pass = false;
   else {
       px = p->momentum().px();
       py = p->momentum().py();
   
       pp = sqrt( px*px + py*py + pz*pz );

       if ( pp < pmin ) pass = false ;
   }
      
   return pass;
}

