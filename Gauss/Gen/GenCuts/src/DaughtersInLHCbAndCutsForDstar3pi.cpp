// Include files 

// local
#include "DaughtersInLHCbAndCutsForDstar3pi.h"

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
// Implementation file for class : DaughtersInLHCbAndCutsForDstar3pi
//
// 2015-04-23 : Guy Wormser
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( DaughtersInLHCbAndCutsForDstar3pi )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaughtersInLHCbAndCutsForDstar3pi::DaughtersInLHCbAndCutsForDstar3pi( const std::string& type,
                                                                      const std::string& name,
                                                                      const IInterface* parent )
: GaudiTool ( type, name , parent ) 
{
  
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
DaughtersInLHCbAndCutsForDstar3pi::~DaughtersInLHCbAndCutsForDstar3pi( ) 
{
  ;
}


//=============================================================================
// AndWithMinP function
//=============================================================================
bool DaughtersInLHCbAndCutsForDstar3pi::applyCut( ParticleVector & theParticleVector ,
                                                  const HepMC::GenEvent * /* theEvent */ ,
                                                  const LHCb::GenCollision * /* theHardInfo */ )
  const 
{
  
  ParticleVector::iterator it ;
  
  
  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ) 
  {
        
    // Check that the signal is the D* sig
    if ( abs( (*it) -> pdg_id() ) !=413  ) 
      Exception( "The signal is not our dear D*" ) ;
    
    
    if ( ! passCuts( *it ) ) 
    {
      
      it = theParticleVector.erase( it ) ;
      
    }
    else ++it ;
    
  }
  

  return ( ! theParticleVector.empty() ) ;
  
}


//=============================================================================
// Functions to test if the D* and 3 pi from the daughters are in acceptance
//=============================================================================
bool DaughtersInLHCbAndCutsForDstar3pi::passCuts( const HepMC::GenParticle * theSignal ) const 
{
  
  HepMC::GenVertex * EV = theSignal -> end_vertex() ;
  
  if ( 0 == EV ) return true ;
  

  typedef std::vector< HepMC::GenParticle * > Particles ;
  
  Particles stables ;
  
  Particles dstar_parents ;
  
  HepMC::GenParticle * theSoftPion( 0 ) ;
  
  HepMC::GenParticle * theD0( 0 ) ;
  
  HepMC::GenParticle * theParent( 0 );
  
  Particles d0daughters ;
  
  
  HepMC::GenVertex::particle_iterator iter ;
  
  // first check on the D* origin on -> momentum().perp() < m_softpiptCut ) return false ;
  

    double minpt = 14.*Gaudi::Units::TeV ;
  
  double maxpt = 0. ;
  
  double minp  = 14.*Gaudi::Units::TeV ;
  
 
  
  for ( Particles::const_iterator it = d0daughters.begin() ; it != d0daughters.end() ;
        ++it ) 
  {
      
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
  
  // now look for 3 pions in the D* family
  debug() << "Check for 3 pions in addition to D* " <<zB<<endmsg;
  
  int Npions_inacc=0;
  
        
  for ( iter = EVD0 -> particles_begin( HepMC::relatives) ; 
        iter != EVD0 -> particles_end( HepMC::relatives ) ; ++iter ) 
  {
    
   
    if ( 0 == (*iter) -> end_vertex() ) stables.push_back( *iter ) ;
    
  }
  
  //now loop on the stable particles
  //we  look at all the children of the B ancestor by requiring z>ZB
  //this will include the 3 D* daughters so we ask 5 pions in the accpetance
  for ( Particles::const_iterator it = stables.begin() ; it != stables.end() ;
        ++it ) 
  {
    
   
    if ((*it)->production_vertex()==0) continue;
    
    debug()<<"particle relative"<<(*it) -> pdg_id()<<" z " << (*it)->production_vertex()->position().z()<<endmsg;
    
    if( (*it)->production_vertex()->position().z()<zB) continue;
    
    if( (*it)->production_vertex()->position().z()>200.) continue;
    
    if ( 211 != abs((*it) -> pdg_id() )) continue;
    
    angle = (*it) -> momentum().theta() ;
    
    if ( fabs( sin( angle ) ) < fabs(  sin( m_chargedThetaMin ) ) ) continue;
    
    if ( fabs( sin( angle ) ) > fabs( sin( m_chargedThetaMax ) ) ) continue;
    
    Npions_inacc++;
    
  }
  
  debug() << "Number of  pions in the D* family that passed " <<Npions_inacc<<endmsg;
  
    
  if (Npions_inacc<5)return false ;
  
  debug() << "Event passed !" << endmsg ;
  
  
  return true ;
  
}

