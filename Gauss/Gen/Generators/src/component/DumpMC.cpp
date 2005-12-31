// $Id: DumpMC.cpp,v 1.2 2005-12-31 17:32:01 robbep Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// 
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================

#include "DumpMC.h"

/** @file 
 *  implementation file for the class DumpMC 
 */

// ============================================================================
static const AlgFactory<DumpMC>         s_Factory ;
// ============================================================================
const       IAlgFactory&DumpMCFactory = s_Factory ;
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
DumpMC::DumpMC ( const std::string& name , 
                 ISvcLocator*       isvc ) 
  : GaudiAlgorithm( name , isvc ) 
  , m_addresses()
{
  m_addresses.push_back( LHCb::HepMCEventLocation::Default ) ;  // default!
  // define the property 
  declareProperty ("Addresses" , m_addresses ) ;
};
// ============================================================================

// ============================================================================
// Destructor 
// ============================================================================
DumpMC::~DumpMC() {}
// ============================================================================

// ============================================================================
/** execution of the algoritm
 *  @see IAlgorithm 
 *  @return status code 
 */
// ============================================================================
StatusCode DumpMC::execute() 
{
  // no printout at all 
  if( !msgLevel( MSG::INFO ) ) { return StatusCode::SUCCESS ; }
  
  // get the stream
  MsgStream& log = info() ;
  
  for( Addresses::const_iterator ia = m_addresses.begin() ; 
       m_addresses.end() != ia ; ++ia ) 
  {
    //
    debug () << " Inspect the container '" << *ia << "'" << endreq ;
    LHCb::HepMCEvents* events = get<LHCb::HepMCEvents>( *ia ) ;
    if( 0 == events ) { continue ; }
    //
    info  () << " HepMC container '" << *ia << "' \t has " 
             << events->size() << " event(s) " << endreq ;
    for ( LHCb::HepMCEvents::const_iterator ie = events->begin() ; 
          events->end() != ie ; ++ie ) 
    {
      const LHCb::HepMCEvent* event = *ie ;
      if ( 0 == event ) { continue ; }
      log << "  Generator '" << event->generatorName() << "'" << std::endl ;
      if ( log.isActive() ) 
      { event->pGenEvt()->print( log.stream() ) ; }
      log << endreq ;
    }
  };
  
  return StatusCode::SUCCESS;
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
