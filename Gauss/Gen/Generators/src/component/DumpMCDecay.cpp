// $Id: DumpMCDecay.cpp,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// 
// ============================================================================
// Include files 
// ============================================================================
// Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IParticlePropertySvc.h" 
#include "GaudiKernel/ParticleProperty.h" 
// ============================================================================
// HepMC 
// ============================================================================
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/ParticleID.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// local
// ============================================================================
#include "DumpMCDecay.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/lexical_cast.hpp"
// ============================================================================

/** @file 
 *  Implementation file for the class DumpMCDecay
 *  @date 2004-02-18 
 *  @author Vanya BELYAEV Ivan.Belyav@itep.ru
 */

// ============================================================================
/** @var s_Factory 
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<DumpMCDecay>         s_Factory ;
// ============================================================================
/** @var DumMCDecayFactory 
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
const        IAlgFactory&DumpMCDecayFactory = s_Factory ; 
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @see GaudiAlgorithm
 *  @see      Algorithm
 *  @see      AlgFactory
 *  @see     IAlgFactory 
 *  @param name algorithm instance's name 
 *  @param iscv pointer to Service Locator 
 */
// ============================================================================
DumpMCDecay::DumpMCDecay 
( const std::string& name ,
  ISvcLocator*       isvc )
  : GaudiAlgorithm ( name , isvc  )
  , m_addresses ()
  , m_particles ()
  , m_quarks    ()
  //
  , m_ppSvc     ( 0 )
{
  //
  m_addresses .push_back( HepMCEventLocation::Default ) ;  // default
  m_quarks    .push_back( ParticleID::bottom          ) ;  // default 
  // define the property 
  declareProperty ( "Addresses" , m_addresses ) ;
  declareProperty ( "Particles" , m_particles ) ;
  declareProperty ( "Quarks"    , m_quarks    ) ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
DumpMCDecay::~DumpMCDecay() {}; 
// ============================================================================

// ============================================================================
/** initialization of the algoritm
 *  @see GaudiAlgorithm
 *  @see      Algorithm
 *  @see     IAlgorithm 
 *  @return status code 
 */
// ============================================================================
StatusCode DumpMCDecay::initialize ()
{
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if( sc.isFailure() ) 
  { return Error ( "Unable to initialize 'GaudiAlgorithm' base ", sc ) ; }
  
  for( PIDs::const_iterator iq = m_quarks.begin() ; 
       m_quarks.end() != iq ; ++iq ) 
  {
    if( ParticleID::down > *iq || ParticleID::top  < *iq  ) 
    { return Error ( " Invalid Quark ID="  + 
                     boost::lexical_cast<std::string>( *iq ) ) ; }
  };
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** execution of the algoritm
 *  @see IAlgorithm 
 *  @return status code 
 */
// ============================================================================
StatusCode DumpMCDecay::execute    ()
{  
  
  bool found = false ;
  
  if ( !m_particles.empty() || !m_quarks.empty() ) 
  {
    // get the stream
    MsgStream& log = info() ;
    
    for( Addresses::const_iterator ia = m_addresses.begin() ; 
         m_addresses.end() != ia ; ++ia ) 
    {
      //
      HepMCEvents* events = get<HepMCEvents>( *ia ) ;
      if( 0 == events ) { continue ; }
      //
      log << " Container '" << *ia << "' " << endreq ;
      for ( HepMCEvents::const_iterator ie = events->begin() ; 
            events->end() != ie ; ++ie ) 
      {
        const HepMCEvent* event = *ie ;
        if ( 0 == event ) { continue ; }                     // CONTINUE 
        const HepMC::GenEvent* evt = event->pGenEvt() ;
        if ( 0 == evt   ) { continue ; }                     // CONTINUE 
        
        for( HepMC::GenEvent::particle_const_iterator ip = 
               evt->particles_begin () ; evt->particles_end() != ip ; ++ip )
        {
          const HepMC::GenParticle* particle = *ip ;
          if( 0 == particle ) { continue ; }                // CONTINUE
          
          bool print = false ;
          if ( m_particles.end () != 
               std::find ( m_particles .  begin  ()       , 
                           m_particles .  end    ()       , 
                           particle    -> pdg_id () ) ) 
          { print = true ; }
          
          for ( PIDs::const_iterator iq = m_quarks.begin() ;
                m_quarks.end() != iq && !print ; ++iq ) 
          {
            // use LHCb flavour of ParticleID class !                  // NB
            ParticleID        p =  ParticleID ( particle -> pdg_id () ) ;
            ParticleID::Quark q =  ParticleID::Quark ( *iq  ) ;
            if( p.hasQuark( q ) ) { print = true ; }
          }
          
          if ( print )
          { 
            found = true ;
            if( log.isActive() ) 
            { 
              log << std::endl ; 
              printDecay ( particle ,  log.stream() , 0  ) ; 
            }
          }
        }
      }
    }
    log << endreq ;
  }
  
  if ( !found ) 
  { Warning ( " No specified Particles/Quarks are found! " ) ; }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** print the decay tree of the particle 
 *  @param particle pointer to teh particle to be printed 
 *  @param stream   output stream 
 *  @param level    decay level
 *  @return status code 
 */
// ============================================================================
StatusCode DumpMCDecay::printDecay 
( const HepMC::GenParticle* particle , 
  std::ostream&             stream   , 
  unsigned int              level    ) const 
{
  if( 0 == particle ) 
  { return Error ( " printDecay(): HepMC::GenParticle* points to NULL" ) ; }
  
  static char s_buf[24] ;
  stream << std::string( s_buf , s_buf + sprintf ( s_buf ,"%3d" , level ) ) ;
  
  {
    const unsigned int s_maxLevel = 10 ;
    const std::string  pName = particleName( particle ) ;
    if( level < s_maxLevel  ) 
    { stream << " " 
             << std::string (                 level   * 2 , ' ' ) 
             << "|-> " 
             << pName 
             << std::string (  ( s_maxLevel - level ) * 2 , ' ' ) ; }
    else 
    { stream << " " << std::string( 2 * s_maxLevel  , ' ' ) 
             << "|-> " << pName ; }
  }
  
  // print the particle itself
  particle->print( stream ) ;
  
  const HepMC::GenVertex* vertex = particle->end_vertex() ;
  if ( 0 == vertex  ) { return StatusCode::SUCCESS ; }                // RETURN
  
  typedef HepMC::GenVertex::particles_out_const_iterator IT ;
  for( IT ip = vertex -> particles_out_const_begin() ; 
       vertex -> particles_out_const_end() != ip ; ++ip )  
  {
    const HepMC::GenParticle* daughter = *ip ;
    if( 0 == daughter ) { continue ; }                              // CONTINUE 
    printDecay( daughter , stream , level + 1 ) ;                  // RECURSION 
  }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
namespace 
{
  // ==========================================================================
  /** @fn adjust  
   *  adjust the particle name to the desired format
   *  @param name name to be adjusted 
   *  @return adjusted name 
   */
  // ==========================================================================
  inline std::string adjust( const std::string& name ) 
  {
    static const size_t s_maxSize = 12 ;
    std::string  tmp ( name ) ;
    const size_t size = tmp.size() ;
    if      ( s_maxSize > size ) 
    { tmp += std::string ( s_maxSize - size , ' ' ) ; }
    else if ( s_maxSize < size )
    { 
      tmp  = std::string ( tmp.begin() , tmp.begin() + s_maxSize ) ; 
      tmp[s_maxSize-1]='#' ; 
    }
    //
    return tmp ;  
  };
};
// ============================================================================

// ============================================================================
/** get the particle name in the string fixed form
 *  @param particle pointer to the particle
 *  @param particle name 
 */
// ============================================================================
std::string  DumpMCDecay::particleName 
( const HepMC::GenParticle* particle ) const 
{
  
  if( 0 == particle ) 
  {
    Error ( "particlename(): HepMC::GenParticle* points to NULL!" ) ;
    return adjust ( "#INVALID****" ) ;
  }
  
  if( 0 == m_ppSvc ) 
  { m_ppSvc = svc<IParticlePropertySvc> ( "ParticlePropertySvc" , true ) ; } ;
  
  const int pdg_id    = particle->pdg_id() ;
  const ParticleProperty* pp = 0 ;
  
  pp = m_ppSvc -> findByStdHepID ( pdg_id ) ;
  if( 0 != pp ) { return adjust( pp->particle() ) ; }
  
  Warning  ( "particleName(): ParticleProperty* points to NULL for PDG=" +
             boost::lexical_cast<std::string> ( pdg_id ) , 
             StatusCode::SUCCESS , 0 ) ;
  
  return adjust ( "#UNKNOWN****" ) ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

