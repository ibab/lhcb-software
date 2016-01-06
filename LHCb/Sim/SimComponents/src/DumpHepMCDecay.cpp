// Include files 
#include "boost/lexical_cast.hpp"

// HepMC 
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// PartProp
#include "Kernel/IParticlePropertySvc.h" 
#include "Kernel/ParticleID.h"
#include "Kernel/ParticleProperty.h" 

// GenEvent
#include "Event/HepMCEvent.h"

// local
#include "DumpHepMCDecay.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DumpHepMCDecay
//
// 2004-02-18 : Ivan Belyaev
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DumpHepMCDecay )


//=============================================================================
// Standard constructor 
//=============================================================================
DumpHepMCDecay::DumpHepMCDecay( const std::string& name , 
                                ISvcLocator*       isvc ) 
  : GaudiAlgorithm ( name , isvc  )
  , m_addresses ()
  , m_particles ()
  , m_quarks    ()
  , m_levels    ( 4 )
  , m_ppSvc     ( 0 )
{

  m_addresses .push_back( LHCb::HepMCEventLocation::Default ) ;  // default
  m_quarks    .push_back( LHCb::ParticleID::bottom          ) ;  // default 

  // define the property 
  declareProperty ( "Addresses" , m_addresses ) ;
  declareProperty ( "Particles" , m_particles ) ;
  declareProperty ( "Quarks"    , m_quarks    ) ;
  declareProperty ( "MaxLevels" , m_levels    ) ;
}

//=============================================================================
// Initialization of the algoritm
//=============================================================================
StatusCode DumpHepMCDecay::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if( sc.isFailure() ) { 
    return Error ( "Unable to initialize 'GaudiAlgorithm' base ", sc ); 
  }

  for( PIDs::const_iterator iq = m_quarks.begin() ; 
       m_quarks.end() != iq ; ++iq ) {
    if( LHCb::ParticleID::down > *iq || LHCb::ParticleID::top  < *iq  ) { 
      return Error ( " Invalid Quark ID="  + 
                     boost::lexical_cast<std::string>( *iq ) );
    }
  }
  return StatusCode::SUCCESS;    
}

//=============================================================================
// Execution of the algoritm
//=============================================================================
StatusCode DumpHepMCDecay::execute() {
  
  bool found = false ;
  if ( !m_particles.empty() || !m_quarks.empty() ) {

    info() << " Decay dump [cut-off at "
           << m_levels << " levels] " << endmsg ;

    for( Addresses::const_iterator ia = m_addresses.begin() ; 
         m_addresses.end() != ia ; ++ia ) {

      LHCb::HepMCEvents* events = get<LHCb::HepMCEvents>( *ia ) ;
      if( 0 == events ) { continue ; }

      info() << " Container '"  << *ia << "' " << endmsg ;
      for ( LHCb::HepMCEvents::const_iterator ie = events->begin() ; 
            events->end() != ie ; ++ie ) {

        const LHCb::HepMCEvent* event = *ie ;
        if ( 0 == event ) { continue ; }                     // CONTINUE 
        const HepMC::GenEvent* evt = event->pGenEvt() ;
        if ( 0 == evt   ) { continue ; }                     // CONTINUE 

        for ( HepMC::GenEvent::particle_const_iterator ip = 
                evt->particles_begin () ; evt->particles_end() != ip ; ++ip ) {

          const HepMC::GenParticle* particle = *ip ;
          if( 0 == particle ) { continue ; }                // CONTINUE

          bool print = false ;
          if ( m_particles.end () != 
               std::find ( m_particles.begin(), m_particles.end(), 
                           particle->pdg_id() ) ) {
            print = true ; 
          }
          for ( PIDs::const_iterator iq = m_quarks.begin() ;
                m_quarks.end() != iq && !print ; ++iq ) {

            // use LHCb flavour of ParticleID class !                  // NB
            LHCb::ParticleID p = LHCb::ParticleID ( particle->pdg_id() ) ;
            LHCb::ParticleID::Quark q =  LHCb::ParticleID::Quark ( *iq  ) ;
            if( p.hasQuark( q ) ) { print = true ; }
          }

          if ( print ) { 
            found = true ;
            if( info().isActive() ) { 
              info() << std::endl ; 
              printDecay ( particle ,  info().stream() , 0  ) ; 
            }
          }
        }
      }
    }
    info() << endmsg ;
  }

  if ( !found ) { 
    Warning ( " No specified Particles/Quarks are found! " ).ignore() ;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// print the decay tree of the particle 
//=============================================================================
StatusCode DumpHepMCDecay::printDecay( const HepMC::GenParticle* particle, 
                                    std::ostream&  stream, 
                                    unsigned int level ) const {
  if( 0 == particle ) { 
    return Error ( " printDecay(): HepMC::GenParticle* points to NULL" ) ; 
  }

  static char s_buf[24] ;
  stream << std::string( s_buf , s_buf + sprintf ( s_buf ,"%3d" , level ) ) ;

  {
    const unsigned int s_maxLevel = 10 ;
    const std::string  pName = particleName( particle ) ;
    if ( level < s_maxLevel  ) { 
      stream << " " 
             << std::string (                 level   * 2 , ' ' ) 
             << "|-> " 
             << pName 
             << std::string (  ( s_maxLevel - level ) * 2 , ' ' ) ; 
    } else { 
      stream << " " << std::string( 2 * s_maxLevel  , ' ' ) 
             << "|-> " << pName ; 
    }
  }

  // print the particle itself
  particle->print( stream ) ;

  const HepMC::GenVertex* vertex = particle->end_vertex() ;
  if ( 0 == vertex  ) { return StatusCode::SUCCESS ; }                // RETURN

  if ( m_levels <= int(level) ) { return StatusCode::SUCCESS  ; }

  // loop over all daughters 
  typedef HepMC::GenVertex::particles_out_const_iterator IT ;
  for( IT ip = vertex -> particles_out_const_begin() ; 
       vertex -> particles_out_const_end() != ip ; ++ip ) {
    const HepMC::GenParticle* daughter = *ip ;
    if ( 0 == daughter ) { continue ; }              // CONTINUE 
    printDecay ( daughter , stream , level + 1 ) ;  // RECURSION 
  }

  return StatusCode::SUCCESS ;
}

//=============================================================================
namespace {

  /*
   * @fn adjust
   * adjust the particle name to the desired format
   *  @param name name to be adjusted
   *  @return adjusted name
   *
   */

  inline std::string adjust( const std::string& name ) {
    static const size_t s_maxSize = 12 ;
    std::string  tmp ( name ) ;
    const size_t size = tmp.size() ;
    if      ( s_maxSize > size ) { 
      tmp += std::string ( s_maxSize - size , ' ' ) ; 
    } else if ( s_maxSize < size ) { 
      tmp  = std::string ( tmp.begin() , tmp.begin() + s_maxSize ) ; 
      tmp[s_maxSize-1]='#' ; 
    }

    return tmp ;  
  }
  
}


//=============================================================================
// get the particle name in the string fixed form
//=============================================================================
std::string DumpHepMCDecay::particleName
                         (const HepMC::GenParticle* particle ) const { 

  if ( 0 == particle ) {
    Error ( "particlename(): HepMC::GenParticle* points to NULL!" ) ;
    return adjust ( "#INVALID****" ) ;
  }

  if( 0 == m_ppSvc ) { 
    m_ppSvc = svc<LHCb::IParticlePropertySvc> ( "LHCb::ParticlePropertySvc" , true ) ; 
  }

  const int pdg_id    = particle->pdg_id() ;
  const LHCb::ParticleProperty* pp = 0 ;

  pp = m_ppSvc -> find( LHCb::ParticleID(pdg_id) ) ;
  if( 0 != pp ) { return adjust( pp->particle() ) ; }

  Warning  ( "particleName(): ParticleProperty* points to NULL for PDG=" +
             boost::lexical_cast<std::string> ( pdg_id ) , 
             StatusCode::SUCCESS , 0 ) ;

  return adjust ( "#UNKNOWN****" ) ;
}

//=============================================================================

