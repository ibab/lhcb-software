// ============================================================================
// Include files
// ============================================================================
// HepMC 
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// GenEvent
#include "Event/HepMCEvent.h"

// Local
#include "DumpHepMCDecay.h"
// ============================================================================
/** @class DumpHepMCTree 
 *  simple class to dump HepMC::GenEvent obejcts 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-10-25
 */
class DumpHepMCTree : public DumpHepMCDecay 
{
  // friend factory for instantiation  
  friend class AlgFactory<DumpHepMCTree> ;
public:
  virtual StatusCode execute() ;
public:
  /** print the decay tree of the particle 
   *  @param vertex  pointer to the vertex to be printed 
   *  @param stream   output stream 
   *  @param level    decay level
   *  @return statsu code 
   */
  StatusCode printDecay 
  ( HepMC::GenVertex* vertex                , 
    std::ostream&     stream    = std::cout , 
    unsigned int      level     = 0         ) const ;
protected:
  DumpHepMCTree
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : DumpHepMCDecay ( name , pSvc )
  {}
  virtual ~DumpHepMCTree(){}  
} ;
// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( DumpHepMCTree )
// ============================================================================
StatusCode DumpHepMCTree::execute() 
{
  // get the stream
  MsgStream& log = info() ;
  log << " Tree dump [cut-off at " << m_levels << " levels] " << endmsg ;
  //
  for( Addresses::const_iterator ia = m_addresses.begin() ; 
       m_addresses.end() != ia ; ++ia ) 
  {
    //
    LHCb::HepMCEvents* events = get<LHCb::HepMCEvents>( *ia ) ;
    if( 0 == events ) { continue ; }
    //
    log << " Container '"  << *ia << "' " << endmsg ;
    for ( LHCb::HepMCEvents::const_iterator ie = events->begin() ; 
          events->end() != ie ; ++ie ) 
    {
      const LHCb::HepMCEvent* event = *ie ;
      if ( 0 == event ) { continue ; }                     // CONTINUE 
      const HepMC::GenEvent* evt = event->pGenEvt() ;
      if ( 0 == evt   ) { continue ; }                     // CONTINUE 
      log << " #particles/vertices : "
          << evt->particles_size() << "/" 
          << evt->vertices_size() << endmsg ;
      
      HepMC::GenVertex* signal = evt->signal_process_vertex() ;
      if ( 0 == signal ) 
      {
        signal = evt->barcode_to_vertex( -1 ) ;
        if ( 0 != signal ) 
        {  Warning ( "Signal_process_vertex is NULL, use -1 " ) ;            }
        else 
        {  Warning ( "Signal_process_vertex is NULL, skip!  " ) ; continue ; }
      }
      if ( log.isActive() ) 
      { printDecay ( signal , log.stream() , 0 ) ; }
    }
  }
  log << endmsg ;
  return StatusCode::SUCCESS ;
}
// ============================================================================
/** print the decay tree of the particle 
 *  @param vertex  pointer to the vertex to be printed 
 *  @param stream   output stream 
 *  @param level    decay level
 *  @return statsu code 
 */
// ============================================================================
StatusCode DumpHepMCTree::printDecay 
( HepMC::GenVertex* vertex  , 
  std::ostream&     stream  , 
  unsigned int      level   ) const 
{
  if ( 0 == vertex ) { return StatusCode::FAILURE ; }
  
  typedef HepMC::GenVertex::particle_iterator IP ;
  IP begin = vertex -> particles_begin ( HepMC::children ) ;
  IP end   = vertex -> particles_end   ( HepMC::children ) ;
  for ( ; begin != end ; ++begin ) 
  { DumpHepMCDecay::printDecay ( *begin , stream , level ) ; }
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// The END 
// ============================================================================
