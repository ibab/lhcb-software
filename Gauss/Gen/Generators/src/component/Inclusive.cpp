// $Id: Inclusive.cpp,v 1.3 2005-11-29 15:56:19 robbep Exp $
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
#include "Kernel/ParticleID.h"

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
    m_nInvertedEvents ( 0 ) ,
    m_nB0             ( 0 ) ,
    m_nB0bar          ( 0 ) ,
    m_nBp             ( 0 ) ,
    m_nBm             ( 0 ) ,
    m_nBs0            ( 0 ) ,
    m_nBs0bar         ( 0 ) ,
    m_nbBaryon        ( 0 ) ,
    m_nantibBaryon    ( 0 ) ,
    m_nBcp            ( 0 ) ,
    m_nBcm            ( 0 ) ,
    m_nbb             ( 0 ) ,
    m_nD0             ( 0 ) ,
    m_nD0bar          ( 0 ) ,
    m_nDp             ( 0 ) ,
    m_nDm             ( 0 ) ,
    m_nDsp            ( 0 ) ,
    m_nDsm            ( 0 ) ,
    m_ncBaryon        ( 0 ) ,
    m_nanticBaryon    ( 0 ) ,
    m_ncc             ( 0 ) ,
    m_n0B             ( 0 ) ,
    m_n1B             ( 0 ) ,
    m_n2B             ( 0 ) ,
    m_n0C             ( 0 ) ,
    m_n1C             ( 0 ) ,
    m_n2C             ( 0 ) { 
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

  HardInfo * theHardInfo( 0 ) ;
  HepMC::GenEvent * theGenEvent( 0 ) ;
  
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    prepareInteraction( theEventVector, theHardVector, theGenEvent, 
                        theHardInfo ) ;

    sc = m_productionTool -> generateEvent( theGenEvent , theHardInfo ) ;
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
                                           theHardInfo ) ;
        
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
  info() << "************   Inclusive counters   **************" << std::endl ;
  
  info() << "Number of events before the cut                  : " 
         << m_nEventsBeforeCut << std::endl ;
  info() << "Number of events after the cut                   : "  
         << m_nEventsAfterCut << std::endl ;
  info() << "Number of z-inverted events                      : "
         << m_nInvertedEvents << std::endl ;
  unsigned int nAfter = m_nEventsAfterCut - m_nInvertedEvents ;
  info() << format( "Efficiency of the generator level cut            : " )
         << format( "%.5g +/- %.5g" , fraction( nAfter , m_nEventsBeforeCut ) ,
                    err_fraction( nAfter , m_nEventsBeforeCut ) ) 
         << std::endl << std::endl ;

  info() << "Number of B0                                     : " 
         << m_nB0 << std::endl ;
  unsigned int totalB = m_nB0 + m_nBp + m_nBs0 + m_nbBaryon + m_nBcp ;
  unsigned int totalBbar = m_nB0bar + m_nBm + m_nBs0bar + m_nantibBaryon + 
    m_nBcm ;
  info() << format( "Fraction of B0                                   : " )
         << format( "%.5g +/- %.5g" , fraction( m_nB0 , totalB ) , 
                    err_fraction( m_nB0 , totalB ) ) << std::endl ;  

  info() << "Number of B0bar                                  : " 
         << m_nB0bar << std::endl ;
  info() << format( "Fraction of B0bar                                : " )
         << format( "%.5g +/- %.5g" , fraction( m_nB0bar , totalBbar ) , 
                    err_fraction( m_nB0bar , totalBbar ) ) << std::endl ;  

  info() << "Number of B+                                     : "
         << m_nBp << std::endl ;
  info() << format( "Fraction of B+                                   : " )
         << format( "%.5g +/- %.5g" , fraction( m_nBp , totalB ) , 
                    err_fraction( m_nBp , totalB ) ) << std::endl ;  

  info() << "Number of B-                                     : "
         << m_nBm << std::endl ;
  info() << format( "Fraction of B-                                   : " )
         << format( "%.5g +/- %.5g" , fraction( m_nBm , totalBbar ) , 
                    err_fraction( m_nBm , totalBbar ) ) << std::endl ;  

  info() << "Number of Bs0                                    : " 
         << m_nBs0 << std::endl ;
  info() << format( "Fraction of Bs0                                  : " )
         << format( "%.5g +/- %.5g" , fraction( m_nBs0 , totalB ) , 
                    err_fraction( m_nBs0 , totalB ) ) << std::endl ;

  info() << "Number of Bs0bar                                 : "
         << m_nBs0bar << std::endl ;
  info() << format( "Fraction of Bs0bar                               : " )
         << format( "%.5g +/- %.5g" , fraction( m_nBs0bar , totalBbar ) , 
                    err_fraction( m_nBs0bar , totalBbar ) ) << std::endl ;  

  info() << "Number of b-baryons                              : "
         << m_nbBaryon << std::endl ;
  info() << format( "Fraction of b-baryons                            : " )
         << format( "%.5g +/- %.5g" , fraction( m_nbBaryon , totalB ) , 
                    err_fraction( m_nbBaryon , totalB ) ) << std::endl ;
  
  info() << "Number of anti-b-baryons                         : "
         << m_nantibBaryon << std::endl ;
  info() << format( "Fraction of anti-b-baryons                       : " )
         << format( "%.5g +/- %.5g" , fraction( m_nantibBaryon , totalBbar ) , 
                    err_fraction( m_nantibBaryon , totalBbar ) )
         << std::endl ;  

  info() << "Number of Bc+                                    : "
         << m_nBcp << std::endl ;
  info() << format( "Fraction of Bc+                                  : " )
         << format( "%.5g +/- %.5g" , fraction( m_nBcp , totalB ) , 
                    err_fraction( m_nBcp , totalB ) ) << std::endl ;

  info() << "Number of Bc-                                    : "
         << m_nBcm << std::endl ;
  info() << format( "Fraction of Bc-                                  : " )
         << format( "%.5g +/- %.5g" , fraction( m_nBcm , totalBbar ) , 
                    err_fraction( m_nBcm , totalBbar ) ) << std::endl ;

  info() << "Number of (bb)                                   : "
         << m_nbb << std::endl << std::endl ;

  info() << "Number of D0                                     : " 
         << m_nD0 << std::endl ;
  unsigned int totalC = m_nD0 + m_nDp + m_nDsp + m_ncBaryon ;
  unsigned int totalCbar = m_nD0bar + m_nDm + m_nDsm + m_nanticBaryon ;
  info() << format( "Fraction of D0                                   : " )
         << format( "%.5g +/- %.5g" , fraction( m_nD0 , totalC ) , 
                    err_fraction( m_nD0 , totalC ) ) << std::endl ;  

  info() << "Number of D0bar                                  : " 
         << m_nD0bar << std::endl ;
  info() << format( "Fraction of D0bar                                : " )
         << format( "%.5g +/- %.5g" , fraction( m_nD0bar , totalCbar ) , 
                    err_fraction( m_nD0bar , totalCbar ) ) << std::endl ;  

  info() << "Number of D+                                     : " 
         << m_nDp << std::endl ;
  info() << format( "Fraction of D+                                   : " )
         << format( "%.5g +/- %.5g" , fraction( m_nDp , totalC ) , 
                    err_fraction( m_nDp , totalC ) ) << std::endl ;  

  info() << "Number of D-                                     : " 
         << m_nDm << std::endl ;
  info() << format( "Fraction of D-                                   : " )
         << format( "%.5g +/- %.5g" , fraction( m_nDm , totalCbar ) , 
                    err_fraction( m_nDm , totalCbar ) ) << std::endl ;  

  info() << "Number of Ds+                                    : " 
         << m_nDsp << std::endl ;
  info() << format( "Fraction of Ds+                                  : " )
         << format( "%.5g +/- %.5g" , fraction( m_nDsp , totalC ) , 
                    err_fraction( m_nDsp , totalC ) ) << std::endl ;  

  info() << "Number of Ds-                                    : " 
         << m_nDsm << std::endl ;
  info() << format( "Fraction of Ds-                                  : " )
         << format( "%.5g +/- %.5g" , fraction( m_nDsm , totalCbar ) , 
                    err_fraction( m_nDsm , totalCbar ) ) << std::endl ;  

  info() << "Number of c-baryons                              : " 
         << m_ncBaryon << std::endl ;
  info() << format( "Fraction of c-baryons                            : " )
         << format( "%.5g +/- %.5g" , fraction( m_ncBaryon , totalC ) , 
                    err_fraction( m_ncBaryon , totalC ) ) << std::endl ;  

  info() << "Number of anti-c-baryons                         : " 
         << m_nanticBaryon << std::endl ;
  info() << format( "Fraction of anti-c-baryons                       : " )
         << format( "%.5g +/- %.5g" , fraction( m_nanticBaryon , totalCbar ) , 
                    err_fraction( m_nanticBaryon , totalCbar ) ) << std::endl ;
  
  info() << "Number of (cc)                                   : " 
         << m_ncc << std::endl << std::endl ;

  info() << "Number of B                                      : "
         << m_n0B << std::endl ;
  unsigned int total = m_n0B + m_n1B + m_n2B ;
  info() << format( "Fraction of B                                    : " )
         << format( "%.5g +/- %.5g" , fraction( m_n0B , total ) , 
                    err_fraction( m_n0B , total ) ) << std::endl ;

  info() << "Number of B*                                     : "
         << m_n1B << std::endl ;
  info() << format( "Fraction of B*                                   : " )
         << format( "%.5g +/- %.5g" , fraction( m_n1B , total ) , 
                    err_fraction( m_n1B , total ) ) << std::endl ;

  info() << "Number of B**                                    : "
         << m_n2B << std::endl ;
  info() << format( "Fraction of B**                                  : " )
         << format( "%.5g +/- %.5g" , fraction( m_n2B , total ) , 
                    err_fraction( m_n2B , total ) ) << std::endl << std::endl ;

  info() << "Number of D                                      : "
         << m_n0C << std::endl ;
  total = m_n0C + m_n1C + m_n2C ;
  info() << format( "Fraction of D                                    : " )
         << format( "%.5g +/- %.5g" , fraction( m_n0C , total ) , 
                    err_fraction( m_n0C , total ) ) << std::endl ;

  info() << "Number of D*                                     : "
         << m_n1C << std::endl ;
  info() << format( "Fraction of D*                                   : " )
         << format( "%.5g +/- %.5g" , fraction( m_n1C , total ) , 
                    err_fraction( m_n1C , total ) ) << std::endl ;

  info() << "Number of D**                                    : "
         << m_n2C << std::endl ;
  info() << format( "Fraction of D**                                  : " )
         << format( "%.5g +/- %.5g" , fraction( m_n2C , total )  , 
                    err_fraction( m_n2C , total ) ) << std::endl << std::endl ;

  info() << endmsg ;
}

//=============================================================================
// Update the counters of number of different hadrons in selected events
//=============================================================================
void Inclusive::updateHadronCounters( const HepMC::GenEvent * theEvent ) {
  HepMC::GenEvent::particle_const_iterator iter ;
  for ( iter = theEvent -> particles_begin() ; 
        iter != theEvent -> particles_end() ; ++iter ) {
    if ( 3 == (*iter) -> status() ) continue ;
    ParticleID thePid( (*iter) -> pdg_id() ) ;
    if ( thePid.hasBottom() ) {
      if ( 0 != (*iter) -> end_vertex() ) {
        HepMC::GenVertex::particles_out_const_iterator child ;
        const HepMC::GenVertex * theEV = (*iter) -> end_vertex() ;
        for ( child = theEV -> particles_out_const_begin( ) ;
              child != theEV -> particles_out_const_end( ) ; ++child ) {
          ParticleID childID( (*child) -> pdg_id() ) ;
          if ( childID.hasBottom() ) continue ;
        }
      }
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
    } else if ( thePid.hasCharm() ) {
      if ( 0 != (*iter) -> end_vertex() ) {
        HepMC::GenVertex::particles_out_const_iterator child ;
        const HepMC::GenVertex * theEV = (*iter) -> end_vertex() ;
        for ( child = theEV -> particles_out_const_begin( ) ;
              child != theEV -> particles_out_const_end( ) ; ++child ) {
          ParticleID childID( (*child) -> pdg_id() ) ;
          if ( childID.hasCharm() ) continue ;
        }
      }
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
}
//=============================================================================
// Count excited states counters
//=============================================================================
void Inclusive::updateExcitedStatesCounters( const HepMC::GenEvent * 
                                             theEvent ) {
  HepMC::GenEvent::particle_const_iterator iter ;
  for ( iter = theEvent -> particles_begin() ; 
        iter != theEvent -> particles_end() ; ++iter ) {
    if ( 3 == (*iter) -> status() ) continue ;
    ParticleID thePid( (*iter) -> pdg_id() ) ;
    if ( thePid.hasBottom() ) {
      if ( 0 != (*iter) -> production_vertex() ) {
        HepMC::GenVertex::particles_in_const_iterator parent ;
        const HepMC::GenVertex * thePV = (*iter) -> production_vertex() ;
        for ( parent = thePV -> particles_in_const_begin() ;
              parent != thePV -> particles_in_const_end() ; ++parent ) {
          ParticleID parentID( (*parent) -> pdg_id() ) ;
          if ( parentID.hasBottom() ) continue ;
        }
      }
      if ( thePid.isMeson() ) {
        if ( 1 == thePid.jSpin() ) m_n0B++ ;
        else if ( ( 3 == thePid.jSpin() ) && ( 1 == thePid.lSpin()) ) m_n1B++ ;
        else m_n2B++ ;
      }
    }
    if ( thePid.hasCharm() ) {
      if ( 0 != (*iter) -> production_vertex() ) {
        HepMC::GenVertex::particles_in_const_iterator parent ;
        const HepMC::GenVertex * thePV = (*iter) -> production_vertex() ;
        for ( parent = thePV -> particles_in_const_begin() ;
              parent != thePV -> particles_in_const_end() ; ++parent ) {
          ParticleID parentID( (*parent) -> pdg_id() ) ;
          if ( parentID.hasCharm() ) continue ;
        }
      }
      if ( thePid.isMeson() ) {
        if ( 1 == thePid.jSpin() ) m_n0C++ ;
        else if ( ( 3 == thePid.jSpin() ) && ( 1 == thePid.lSpin()) ) m_n1C++ ;
        else m_n2C++ ;
      }
    }
  }       
}

