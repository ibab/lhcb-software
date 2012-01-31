// $Id: DaughtersInLHCbAndMassCut.cpp,v 1.7 2008-07-09 14:33:47 robbep Exp $
// Include files 

// local
#include "DaughtersInLHCbAndMassCut.h"

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
// Implementation file for class : DaughtersInLHCbAndMassCut
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( DaughtersInLHCbAndMassCut );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaughtersInLHCbAndMassCut::DaughtersInLHCbAndMassCut( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IGenCutTool >( this ) ;
    declareProperty( "ChargedThetaMin" , m_chargedThetaMin = 10 * Gaudi::Units::mrad ) ;
    declareProperty( "ChargedThetaMax" , m_chargedThetaMax = 400 * Gaudi::Units::mrad ) ;
    declareProperty( "NeutralThetaMin" , m_neutralThetaMin = 5 * Gaudi::Units::mrad ) ;
    declareProperty( "NeutralThetaMax" , m_neutralThetaMax = 400 * Gaudi::Units::mrad ) ;
    declareProperty( "LeptonOneID"     , m_LeptonOneID = -11 );
    declareProperty( "LeptonTwoID"     , m_LeptonTwoID =  11 );
    declareProperty( "HadronOneABSID"  , m_HadronOneABSID = 321 );
    declareProperty( "HadronTwoABSID"  , m_HadronTwoABSID = 211 );
    declareProperty( "DausPTMin"       , m_DausPTMin = 0.2 * Gaudi::Units::GeV );  
    declareProperty( "eeMassMax"       , m_eeMassMax = 1.5 * Gaudi::Units::GeV );
    declareProperty( "eeKstarMassMin"  , m_eeKstarMassMin = 4.0 * Gaudi::Units::GeV );
    

}

//=============================================================================
// Destructor 
//=============================================================================
DaughtersInLHCbAndMassCut::~DaughtersInLHCbAndMassCut( ) { ; }

//=============================================================================
// Acceptance function
//=============================================================================
bool DaughtersInLHCbAndMassCut::applyCut( ParticleVector & theParticleVector ,
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
// Functions to test if all daughters are in acceptance
//=============================================================================
bool DaughtersInLHCbAndMassCut::passCuts( const HepMC::GenParticle * theSignal ) const {
  HepMC::GenVertex * EV = theSignal -> end_vertex() ;
  if ( 0 == EV ) return true ;

  typedef std::vector< HepMC::GenParticle * > Particles ;
  Particles stables ;
  
  Particles ePlusList ;
  Particles eMinusList ;
  Particles KaonList ;
  Particles PionList ;  

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
    
    if ( m_LeptonOneID == (*it) -> pdg_id() ) ePlusList.push_back( *it ) ;
    if ( m_LeptonTwoID == (*it) -> pdg_id() ) eMinusList.push_back( *it ) ;

    if ( m_HadronOneABSID == abs( (*it) -> pdg_id() ) ) KaonList.push_back( *it );
    if ( m_HadronTwoABSID == abs( (*it) -> pdg_id() ) ) PionList.push_back( *it );

   
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
    }
  }
  
  debug() << "Event passed geometry cut!" << endmsg ;

  if ( ePlusList.empty() || eMinusList.empty() || KaonList.empty() || PionList.empty() ){
    debug() << "No (e+,e-,K,pi) found in this event" << endmsg ;
    return false;
  } else {
    debug() << "e+: " << ePlusList.size() << endmsg;
    debug() << "e-: " << eMinusList.size() << endmsg;
    debug() << "K:  " << KaonList.size() << endmsg;
    debug() << "Pi: " << PionList.size() << endmsg;
  }
  
  
  //====================================================================
  // Check mass 
  //====================================================================
  HepMC::FourVector eeVect;
  HepMC::FourVector eeKstarVect;
  
  for ( Particles::const_iterator itePlus = ePlusList.begin() ; itePlus != ePlusList.end() ;
        ++itePlus ) {

    // PT Cut 
    if( (*itePlus)->momentum().perp() < m_DausPTMin ) continue;
    
    for ( Particles::const_iterator iteMinus = eMinusList.begin() ; iteMinus != eMinusList.end() ;
          ++iteMinus ) {
      
      // PT Cut 
      if( (*iteMinus)->momentum().perp() < m_DausPTMin ) continue;
      
      //eeVect = (*itePlus)->momentum() + (*iteMinus)->momentum() ;
      eeVect.setPx( (*itePlus)->momentum().px() + (*iteMinus)->momentum().px() );
      eeVect.setPy( (*itePlus)->momentum().py() + (*iteMinus)->momentum().py() );
      eeVect.setPz( (*itePlus)->momentum().pz() + (*iteMinus)->momentum().pz() );
      eeVect.setE(  (*itePlus)->momentum().e()  + (*iteMinus)->momentum().e() );
      
      // eeMass > eeMassMin 
      if( eeVect.m() > m_eeMassMax ) continue;
      
      for ( Particles::const_iterator itKaon = KaonList.begin() ; itKaon != KaonList.end() ;
            ++itKaon ) {
        
        // PT Cut
        if( (*itKaon)->momentum().perp() < m_DausPTMin ) continue;
        
        for ( Particles::const_iterator itPion = PionList.begin() ; itPion != PionList.end() ;
              ++itPion ) {
          
          // PT Cut
          if( (*itPion)->momentum().perp() < m_DausPTMin ) continue;
          
          if( (((*itKaon)->pdg_id()) * ((*itPion)->pdg_id()))>0 ) continue;
          
          //eeKstarVect = eeVect + (*itKaon)->momentum() + (*itPion)->momentum() ;
          eeKstarVect.setPx( eeVect.px() + (*itKaon)->momentum().px() + (*itPion)->momentum().px() );
          eeKstarVect.setPy( eeVect.py() + (*itKaon)->momentum().py() + (*itPion)->momentum().py() );
          eeKstarVect.setPz( eeVect.pz() + (*itKaon)->momentum().pz() + (*itPion)->momentum().pz() );
          eeKstarVect.setE(  eeVect.e()  + (*itKaon)->momentum().e()  + (*itPion)->momentum().e() );
          
          if( eeKstarVect.m() >  m_eeKstarMassMin ) return true ;
          else {
            debug() << "Event not pass Mass cut, with mass:" << eeKstarVect.m() << endmsg ;
          }
          
        }
      }
      
    }
  }
  
  return false ;
}
