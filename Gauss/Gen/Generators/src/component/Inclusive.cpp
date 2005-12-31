// $Id: Inclusive.cpp,v 1.5 2005-12-31 17:32:39 robbep Exp $
// Include files 

// local
#include "Inclusive.h"

// standard
#include <cfloat>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// From LHCb
#include "Kernel/ParticleID.h"

// from Generators
#include "Generators/IProductionTool.h"
#include "Generators/IGenCutTool.h"
#include "Generators/GenCounters.h"

//=============================================================================
// Function to test if a HepMC::GenParticle is a B hadron at root of decay
//=============================================================================
struct isRootB : std::unary_function< const HepMC::GenParticle * , bool > {

  /// test operator, returns true if it is a root B
  bool operator() ( const HepMC::GenParticle * part ) const {

    // Do not consider documentation and special particles
    if ( part -> status() == 3 ) return false ;
    
    // Check if particle has a b quark
    LHCb::ParticleID thePid( part -> pdg_id() ) ;
    if ( ! thePid.hasBottom() ) return false ;

    // Check if particle has a mother
    if ( 0 == part -> production_vertex() ) return true ;

    // Check all parents of the B 
    HepMC::GenVertex::particles_in_const_iterator parent ;
    const HepMC::GenVertex * thePV = part -> production_vertex() ;
    for ( parent = thePV -> particles_in_const_begin() ;
          parent != thePV -> particles_in_const_end() ; ++parent ) {
      LHCb::ParticleID parentID( (*parent) -> pdg_id() ) ;
      if ( parentID.hasBottom() ) return false ;
    }

    // If no parent is a B, then it is a root B
    return true ;
  }
};

//=============================================================================
// Function to test if a HepMC::GenParticle is a D hadron at root of decay
//=============================================================================
struct isRootD : std::unary_function< const HepMC::GenParticle * , bool > {

  /// test operator, returns true if it is a root D
  bool operator() ( const HepMC::GenParticle * part ) const {

    // Do not consider documentation and special particles
    if ( part -> status() == 3 ) return false ;

    // Check if particle has a c quark
    LHCb::ParticleID thePid( part -> pdg_id() ) ;
    if ( ! thePid.hasCharm() ) return false ;

    // Check if particle has a mother
    if ( 0 == part -> production_vertex() ) return true ;

    // Check all parents of the D
    HepMC::GenVertex::particles_in_const_iterator parent ;
    const HepMC::GenVertex * thePV = part -> production_vertex() ;
    for ( parent = thePV -> particles_in_const_begin() ;
          parent != thePV -> particles_in_const_end() ; ++parent ) {
      LHCb::ParticleID parentID( (*parent) -> pdg_id() ) ;
      if ( parentID.hasCharm() ) return false ;
    }

    // If no parent is a D, then it is a root D
    return true ;
  }
};

//=============================================================================
// Function to test if a HepMC::GenParticle is a B hadron at end of decay tree
//=============================================================================
struct isEndB : std::unary_function< const HepMC::GenParticle * , bool > {

  /// Test operator. Returns true if particle is the last B
  bool operator() ( const HepMC::GenParticle * part ) const {

    // Do not look at special particles
    if ( part -> status() == 3 ) return false ;

    // Test if particle has a b quark
    LHCb::ParticleID thePid( part -> pdg_id() ) ;
    if ( ! thePid.hasBottom() ) return false ;

    // Test if the B has daughters
    if ( 0 == part -> end_vertex() ) return true ;

    // Loop over daughters to check if they are B hadrons
    HepMC::GenVertex::particles_out_const_iterator children ;
    const HepMC::GenVertex * theEV = part -> end_vertex() ;
    for ( children = theEV -> particles_out_const_begin() ;
          children != theEV -> particles_out_const_end() ; ++children ) {
      LHCb::ParticleID childID( (*children) -> pdg_id() ) ;
      if ( childID.hasBottom() ) return false ;
    }

    // If not, then it is a end B
    return true ;
  }
};

//=============================================================================
// Function to test if a HepMC::GenParticle is a D hadron at end of decay tree
//=============================================================================
struct isEndD : std::unary_function< const HepMC::GenParticle * , bool > {

  /// Test operator. Returns true if it is the last D
  bool operator() ( const HepMC::GenParticle * part ) const {

    // Do not look at special particles
    if ( part -> status() == 3 ) return false ;

    // Check if it has a c quark
    LHCb::ParticleID thePid( part -> pdg_id() ) ;
    if ( ! thePid.hasCharm() ) return false ;

    // Check if it has daughters
    if ( 0 == part -> end_vertex() ) return true ;

    // Loop over the daughters to find a D hadron
    HepMC::GenVertex::particles_out_const_iterator children ;
    const HepMC::GenVertex * theEV = part -> end_vertex() ;
    for ( children = theEV -> particles_out_const_begin() ;
          children != theEV -> particles_out_const_end() ; ++children ) {
      LHCb::ParticleID childID( (*children) -> pdg_id() ) ;
      if ( childID.hasCharm() ) return false ;
    }

    // If not, then it is a End D
    return true ;
  }
};

//-----------------------------------------------------------------------------
// Implementation file for class : Inclusive
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<Inclusive>          s_factory ;
const        IToolFactory& InclusiveFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Inclusive::Inclusive( const std::string& type, const std::string& name,
                      const IInterface* parent )
  : ExternalGenerator ( type, name , parent ) ,
    m_lightestMass    ( DBL_MAX ) ,
    m_lightestPID     ( 0 ) ,
    m_nEventsBeforeCut( 0 ) , m_nEventsAfterCut ( 0 ) ,
    m_nInvertedEvents ( 0 ) ,
    m_nB0             ( 0 ) , m_nB0bar          ( 0 ) ,
    m_nBp             ( 0 ) , m_nBm             ( 0 ) ,
    m_nBs0            ( 0 ) , m_nBs0bar         ( 0 ) ,
    m_nbBaryon        ( 0 ) , m_nantibBaryon    ( 0 ) ,
    m_nBcp            ( 0 ) , m_nBcm            ( 0 ) ,
    m_nbb             ( 0 ) ,
    m_nD0             ( 0 ) , m_nD0bar          ( 0 ) ,
    m_nDp             ( 0 ) , m_nDm             ( 0 ) ,
    m_nDsp            ( 0 ) , m_nDsm            ( 0 ) ,
    m_ncBaryon        ( 0 ) , m_nanticBaryon    ( 0 ) ,
    m_ncc             ( 0 ) ,
    m_n0starB         ( 0 ) , m_n1starB( 0 ) , m_n2starB( 0 ) ,
    m_n0starC         ( 0 ) , m_n1starC( 0 ) , m_n2starC( 0 ) { 
    declareProperty( "InclusivePIDList" , m_pidVector ) ;
  }

//=============================================================================
// Destructor
//=============================================================================
Inclusive::~Inclusive( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode Inclusive::initialize( ) {
  StatusCode sc = ExternalGenerator::initialize( ) ;
  if ( ! sc.isSuccess() ) return Error( "Cannot initialize base class !" ) ;

  if ( m_pidVector.empty() ) 
    return Error( "InclusivePIDList property is not set" ) ;

  // Transform vector into set
  for ( std::vector<int>::iterator it = m_pidVector.begin() ; 
        it != m_pidVector.end() ; ++it ) m_pids.insert( *it ) ;
  
  IParticlePropertySvc * ppSvc =
    svc< IParticlePropertySvc >( "ParticlePropertySvc" ) ;
  
  info() << "Generating Inclusive events of " ;
  PIDs::const_iterator it ;
  for ( it = m_pids.begin() ; it != m_pids.end() ; ++it ) {
    ParticleProperty * prop = ppSvc -> findByStdHepID( *it ) ;
    info() << prop -> particle() << " " ;
    if ( prop -> mass() < m_lightestMass ) {
      m_lightestMass = prop -> mass() ;
      m_lightestPID = prop -> pdgID() ;
    }
  }
  info() << endmsg ;  
  release( ppSvc ) ;

  return sc ;
}

//=============================================================================
// Generate Set of Event for Minimum Bias event type
//=============================================================================
bool Inclusive::generate( const unsigned int nPileUp , 
                          LHCb::HepMCEvents * theEvents , 
                          LHCb::GenCollisions * theCollisions ) {
  StatusCode sc ;
  bool result = false ;

  LHCb::GenCollision * theGenCollision( 0 ) ;
  HepMC::GenEvent * theGenEvent( 0 ) ;
  
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    prepareInteraction( theEvents , theCollisions , theGenEvent, 
                        theGenCollision ) ;

    sc = m_productionTool -> generateEvent( theGenEvent , theGenCollision ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;

    if ( ! result ) {
      // Decay particles heavier than the particles to look at
      decayHeavyParticles( theGenEvent , m_lightestMass , m_lightestPID ) ;
      
      // Check if one particle of the requested list is present in event
      ParticleVector theParticleList ;
      if ( checkPresence( m_pids , theGenEvent , theParticleList ) ) {
        
        m_nEventsBeforeCut++ ;
        bool passCut = true ;
        if ( 0 != m_cutTool ) 
          passCut = m_cutTool -> applyCut( theParticleList , theGenEvent , 
                                           theGenCollision ) ;
        
        if ( passCut && ( ! theParticleList.empty() ) ) {
          m_nEventsAfterCut++ ;
          result = true ;

          if ( 0 == nPositivePz( theParticleList ) ) {
            revertEvent( theGenEvent ) ;
            m_nInvertedEvents++ ;
          }

          // Update counters
          updateHadronCounters( theGenEvent ) ;
          updateExcitedStatesCounters( theGenEvent ) ;
        }
      }
    }
  }  
  
  return result ;
}

//=============================================================================
// Print the counters
//=============================================================================
void Inclusive::printCounters( ) const {
  using namespace GenCounters ;
  
  info() << "************   Inclusive counters   **************" << std::endl ;
  
  printEfficiency( info() , "generator level cut" , 
                   m_nEventsAfterCut - m_nInvertedEvents , 
                   m_nEventsBeforeCut ) ;
  printCounter( info() , "z-inverted events" , m_nInvertedEvents ) ;
  info() << std::endl ;

  unsigned int totalB = m_nB0 + m_nBp + m_nBs0 + m_nbBaryon + m_nBcp ;
  unsigned int totalBbar = m_nB0bar + m_nBm + m_nBs0bar + m_nantibBaryon + 
    m_nBcm ;

  printFraction( info() , "B0" , m_nB0 , totalB ) ;
  printFraction( info() , "B0bar" , m_nB0bar , totalBbar ) ;
  
  printFraction( info() , "B+" , m_nBp , totalB ) ;
  printFraction( info() , "B-" , m_nBm , totalB ) ;
 
  printFraction( info() , "Bs0" , m_nBs0 , totalB ) ;
  printFraction( info() , "Bs0bar" , m_nBs0bar , totalBbar ) ;

  printFraction( info() , "b-baryons" , m_nbBaryon , totalB ) ;
  printFraction( info() , "anti-b-baryons" , m_nantibBaryon , totalBbar ) ;

  printFraction( info() , "Bc+" , m_nBcp , totalB ) ;
  printFraction( info() , "Bc-" , m_nBcm , totalBbar ) ;

  printCounter( info() , "(bb)" , m_nbb ) ;
  info() << std::endl ;

  unsigned int totalC = m_nD0 + m_nDp + m_nDsp + m_ncBaryon ;
  unsigned int totalCbar = m_nD0bar + m_nDm + m_nDsm + m_nanticBaryon ;
  printFraction( info() , "D0" , m_nD0 , totalC ) ;
  printFraction( info() , "D0bar" , m_nD0bar , totalCbar ) ;

  printFraction( info() , "D+" , m_nDp , totalC ) ;
  printFraction( info() , "D-" , m_nDm , totalCbar ) ;

  printFraction( info() , "Ds+" , m_nDsp , totalC ) ;
  printFraction( info() , "Ds-" , m_nDsm , totalCbar ) ;

  printFraction( info() , "c-baryons" , m_ncBaryon , totalC ) ;
  printFraction( info() , "anti-c-baryons" , m_nanticBaryon , totalCbar ) ;
  printCounter( info() , "(cc)" , m_ncc ) ;
  info() << std::endl ;

  unsigned int total = m_n0starB + m_n1starB + m_n2starB ;
  printFraction( info() , "B" , m_n0starB , total ) ;
  printFraction( info() , "B*" , m_n1starB , total ) ;
  printFraction( info() , "B**" , m_n2starB , total ) ;
  info() << std::endl ;

  total = m_n0starC + m_n1starC + m_n2starC ;
  printFraction( info() , "D" , m_n0starC , total ) ;
  printFraction( info() , "D*" , m_n1starC , total ) ;
  printFraction( info() , "D**" , m_n2starC , total ) ;

  info() << endmsg ;
}

//=============================================================================
// Update the counters of number of different hadrons in selected events
//=============================================================================
void Inclusive::updateHadronCounters( const HepMC::GenEvent * theEvent ) {
  // Count B:
  std::vector< HepMC::GenParticle * > endB ;
  HepMC::copy_if( theEvent -> particles_begin() , theEvent -> particles_end() ,
                  std::back_inserter( endB ) , isEndB() ) ;
  std::vector< HepMC::GenParticle * >::const_iterator iter ;
  
  for ( iter = endB.begin() ; iter != endB.end() ; ++iter ) {
    LHCb::ParticleID thePid( (*iter) -> pdg_id() ) ;
    
    if ( thePid.isMeson() ) {
      if ( thePid.pid() > 0 ) {
        if ( thePid.hasUp() ) m_nBp++ ;
        else if ( thePid.hasDown() ) m_nB0++ ;
        else if ( thePid.hasStrange() ) m_nBs0++ ;
        else if ( thePid.hasCharm() ) m_nBcp++ ;
        else m_nbb++ ;
      } else {
        if ( thePid.hasUp() ) m_nBm++ ;
        else if ( thePid.hasDown() ) m_nB0bar++ ;
        else if ( thePid.hasStrange() ) m_nBs0bar++ ;
        else if ( thePid.hasCharm() ) m_nBcm++ ;
        else m_nbb++ ;
      }
    } else if ( thePid.isBaryon() ) {
      if ( thePid.pid() > 0 ) m_nbBaryon++ ;
      else m_nantibBaryon++ ;
    }
  }
  
  std::vector< HepMC::GenParticle * > endD ;
  HepMC::copy_if( theEvent -> particles_begin() , theEvent -> particles_end() ,
                  std::back_inserter( endD ) , isEndD() ) ;
  
  for ( iter = endD.begin() ; iter != endD.end() ; ++iter ) {
    LHCb::ParticleID thePid( (*iter) -> pdg_id() ) ;
    
    if ( thePid.isMeson() ) {
      if ( thePid.pid() > 0 ) {
        if ( thePid.hasUp() ) m_nD0++ ;
        else if ( thePid.hasDown() ) m_nDp++ ;
        else if ( thePid.hasStrange() ) m_nDsp++ ;
        else m_ncc++ ;
      } else {
        if ( thePid.hasUp() ) m_nD0bar++ ;
        else if ( thePid.hasDown() ) m_nDm++ ;
        else if ( thePid.hasStrange() ) m_nDsm++ ;
        else m_ncc++ ;
      }
    } else if ( thePid.isBaryon() ) {
      if ( thePid.pid() > 0 ) m_ncBaryon++ ;
      else m_nanticBaryon++ ;
    }
  } 
}

//=============================================================================
// Count excited states counters
//=============================================================================
void Inclusive::updateExcitedStatesCounters( const HepMC::GenEvent * 
                                             theEvent ) {
  // Count B :
  std::vector< HepMC::GenParticle * > rootB ;
  HepMC::copy_if( theEvent -> particles_begin() , theEvent -> particles_end() ,
                  std::back_inserter( rootB ) , isRootB() ) ;

  std::vector< HepMC::GenParticle * >::const_iterator iter ;

  for ( iter = rootB.begin() ; iter != rootB.end() ; ++iter ) {
    LHCb::ParticleID thePid( (*iter) -> pdg_id() ) ;

    if ( thePid.isMeson() ) {
      if ( 0 == thePid.lSpin() ) {
        if ( 1 == thePid.jSpin() ) ++m_n0starB ;
        else ++m_n1starB ;
      } else ++m_n2starB ;
    }
  }

  // Count D :
  std::vector< HepMC::GenParticle * > rootD ;
  HepMC::copy_if( theEvent -> particles_begin() , theEvent -> particles_end() ,
                  std::back_inserter( rootD ) , isRootD() ) ;

  for ( iter = rootD.begin() ; iter != rootD.end() ; ++iter ) {
    LHCb::ParticleID thePid( (*iter) -> pdg_id() ) ;
    if ( thePid.isMeson() ) {
      if ( 0 == thePid.lSpin() ) {
        if ( 1 == thePid.jSpin() ) ++m_n0starC ;
        else ++m_n1starC ;
      } else ++m_n2starC ;
    }
  }       
}

