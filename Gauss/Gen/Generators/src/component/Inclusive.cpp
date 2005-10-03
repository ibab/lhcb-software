// $Id: Inclusive.cpp,v 1.1 2005-10-03 10:29:42 robbep Exp $
// Include files 

// local
#include "Inclusive.h"

// standard
#include <cfloat>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// Event 
#include "Event/HepMCEvent.h"
#include "Event/HardInfo.h"

// from Generators
#include "Generators/IProductionTool.h"
#include "Generators/IGenCutTool.h"
#include "Generators/HepMCUtils.h"

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
Inclusive::Inclusive( const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
  : ExternalGenerator ( type, name , parent ) ,
    m_lightestMass    ( DBL_MAX ) ,
    m_lightestPID     ( 0 ) ,
    m_nEventsBeforeCut( 0 ) ,
    m_nEventsAfterCut ( 0 ) ,
    m_nInvertedEvents ( 0 ) { 
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
                          EventVector & theEventVector , 
                          HardVector  & theHardVector ) {
  StatusCode sc ;
  bool result = false ;
  
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    HepMCEvent * theHepMCEvent = new HepMCEvent( m_productionTool -> name() ,
                                                 1 , 1 ) ;
    HardInfo * theHardInfo = new HardInfo( ) ;

    HepMC::GenEvent * theGenEvent = theHepMCEvent -> pGenEvt() ;
    theHardInfo -> setEvent( theHepMCEvent ) ;

    sc = m_productionTool -> generateEvent( theGenEvent , theHardInfo ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;

    theEventVector.push_back( theHepMCEvent ) ;
    theHardVector .push_back( theHardInfo   ) ;

    if ( ! result ) {
      // Decay particles heavier than the particles to look at
      decayHeavyParticles( theGenEvent , m_lightestMass , m_lightestPID ) ;
      
      // Check if one particle of the requested list is present in event
      ParticleVector theParticleList ;
      if ( checkPresence( m_pids , theGenEvent , theParticleList ) ) {
        
        m_nEventsBeforeCut++ ;
        if ( 0 != m_cutTool ) m_cutTool -> applyCut( theParticleList ) ;
        
        if ( ! theParticleList.empty() ) {
          m_nEventsAfterCut++ ;
          result = true ;

          if ( 0 == nPositivePz( theParticleList ) ) {
            revertEvent( theGenEvent ) ;
            m_nInvertedEvents++ ;
          }
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
  info() << "Number of events before the cut = " << m_nEventsBeforeCut 
         << endmsg;
  info() << "Number of events after the cut = "  << m_nEventsAfterCut  
         << endmsg;
  info() << "Number of z-inverted events  = " << m_nInvertedEvents << endmsg ;
}
