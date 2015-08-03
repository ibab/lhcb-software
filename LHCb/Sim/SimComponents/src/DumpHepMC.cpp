// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/MsgStream.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// HepMC
// ============================================================================
#include "HepMC/GenEvent.h"
#include "GenEvent/HepMCUtils.h"

#include "DumpHepMC.h"

/** @file 
 *  implementation file for the class DumpHepMC.
 *  Just a renaming of the DumpMC class that was in Gen/Generators package
 */

// ============================================================================

DECLARE_ALGORITHM_FACTORY( DumpHepMC )

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
DumpHepMC::DumpHepMC ( const std::string& name , 
                       ISvcLocator*       isvc ) 
  : GaudiAlgorithm( name , isvc ) 
  , m_addresses()
{
  m_addresses.push_back( LHCb::HepMCEventLocation::Default ) ;  // default!
  // define the property 
  declareProperty ("Addresses" , m_addresses ) ;
}
// ============================================================================

// ============================================================================
// Destructor 
// ============================================================================
DumpHepMC::~DumpHepMC() {}
// ============================================================================

// ============================================================================
/** execution of the algoritm
 *  @see IAlgorithm 
 *  @return status code 
 */
// ============================================================================
StatusCode DumpHepMC::execute() 
{
  // no printout at all 
  if( !msgLevel( MSG::INFO ) ) { return StatusCode::SUCCESS ; }
  
  // get the stream
  MsgStream& log = info() ;
  
  for( Addresses::const_iterator ia = m_addresses.begin() ; 
       m_addresses.end() != ia ; ++ia ) 
  {
    //
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug () << " Inspect the container '" << *ia << "'" << endmsg ;
    LHCb::HepMCEvents* events = get<LHCb::HepMCEvents>( *ia ) ;
    if( 0 == events ) { continue ; }
    //
    info  () << " HepMC container '" << *ia << "' \t has " 
             << events->size() << " event(s) " << endmsg ;
    for ( LHCb::HepMCEvents::const_iterator ie = events->begin() ; 
          events->end() != ie ; ++ie ) 
    {
      const LHCb::HepMCEvent* event = *ie ;
      if ( 0 == event ) { continue ; }
      log << "  Generator '" << event->generatorName() << "'" << std::endl ;
      if ( log.isActive() ) 
        { orderedPrint( event->pGenEvt() , log.stream() ) ; }
      log << endmsg ;
    }
  };
  
  return StatusCode::SUCCESS;
}
// ============================================================================
// Print ordered HepMC
//=============================================================================
void DumpHepMC::orderedPrint( const HepMC::GenEvent * theEvent , 
                           std::ostream & ostr ) 
  const {
  // dumps the content of this event to ostr
  //   to dump to cout use: event.print();
  //   if you want to write this event to file outfile.txt you could use:
  //      std::ofstream outfile("outfile.txt"); event.print( outfile );
  ostr << "________________________________________"
       << "________________________________________\n";
  ostr << "GenEvent: #" << theEvent -> event_number() 
       << " ID=" << theEvent -> signal_process_id() 
       << " SignalProcessGenVertex Barcode: " 
       << ( theEvent -> signal_process_vertex() ? 
            theEvent -> signal_process_vertex()->barcode()
            : 0 )
       << "\n";
  ostr << " Entries this event: " 
       << theEvent -> vertices_size() << " vertices, "
       << theEvent -> particles_size() << " particles.\n"; 
  // print a legend to describe the particle info
  char particle_legend[80];
  sprintf( particle_legend,"      %9s %8s %9s,%9s,%9s,%8s %4s %9s",
           "Barcode","PDG ID","( Px","Py","Pz","E )","Stat","DecayVtx");
  ostr << "                                    GenParticle Legend\n"
       << particle_legend << "\n";
  ostr << "________________________________________"
       << "________________________________________\n";
  // Print all Vertices
  for ( HepMC::GenEvent::vertex_const_iterator 
          vtx = theEvent -> vertices_begin();
        vtx != theEvent -> vertices_end(); ++vtx ) {
    orderedVertexPrint(*vtx , ostr); 
  }
  ostr << "________________________________________"
       << "________________________________________" << std::endl;
}

//=============================================================================
// Print vertices ordered 
//=============================================================================
void DumpHepMC::orderedVertexPrint( HepMC::GenVertex * theVertex , 
                                    std::ostream & ostr ) const {
  char outline[80];
  if ( theVertex -> barcode()!=0 ) {
    if ( theVertex -> position() != HepMC::FourVector(0,0,0,0) ) {
      sprintf( outline,
               "Vertex:%9d ID:%5d (X,cT)=%+9.2e,%+9.2e,%+9.2e,%+9.2e"
               ,theVertex -> barcode(), theVertex -> id(),
               theVertex -> position().x(),
               theVertex -> position().y(),
               theVertex -> position().z(),
               theVertex -> position().t() );
    } else {
      sprintf( outline, "GenVertex:%9d ID:%5d (X,cT):0", 
               theVertex -> barcode(), theVertex -> id() );
    }
  } else {
    // If the vertex doesn't have a unique barcode assigned, then
    //  we print its memory address instead... so that the
    //  print out gives us a unique tag for the particle.
    if ( theVertex -> position() != HepMC::FourVector(0,0,0,0) ) {
      sprintf( outline,
               "Vertex:%9p ID:%5d (X,cT)=%+9.2e,%+9.2e,%+9.2e,%+9.2e"
               ,(void*)theVertex, theVertex -> id(),
               theVertex -> position().x(),
               theVertex -> position().y(),
               theVertex -> position().z(),
               theVertex -> position().t() );
    } else {
      sprintf( outline, "GenVertex:%9p ID:%5d (X,cT):0", 
               (void*)theVertex, theVertex -> id() );
    }
  }
  
  ostr << outline << std::endl;
  // print the weights if there are any
  if ( ! theVertex -> weights().empty() ) {
    ostr << " Wgts(" << theVertex -> weights().size() << ")=";
    for ( HepMC::WeightContainer::const_iterator wgt = 
            theVertex -> weights().begin();
          wgt != theVertex -> weights().end(); wgt++ ) { ostr << *wgt << " "; }
    ostr << std::endl;
  }
  // print out all the incoming, then outgoing particles
  std::list< const HepMC::GenParticle * > lP ;
  for ( HepMC::GenVertex::particles_in_const_iterator part1 = 
          theVertex -> particles_in_const_begin();
        part1 != theVertex -> particles_in_const_end(); part1++ ) 
    lP.push_back( *part1 ) ;
  lP.sort( HepMCUtils::compareHepMCParticles ) ;
  std::list< const HepMC::GenParticle * >::iterator it ;
  for ( it = lP.begin() ; it != lP.end() ; ++it ) {
    if ( it == lP.begin() ) {
      char label[5];
      sprintf( label," I:%2d", theVertex -> particles_in_size() );
      ostr << label; 
    } else { ostr << "     "; }
    //(*part1)->print( ostr );  //uncomment for long debugging printout
    ostr << **it << std::endl;
  }
  
  std::list< const HepMC::GenParticle * > lP2 ;
  for ( HepMC::GenVertex::particles_out_const_iterator 
          part2 = theVertex -> particles_out_const_begin();
        part2 != theVertex -> particles_out_const_end(); part2++ ) 
    lP2.push_back( *part2 ) ;
  lP2.sort( HepMCUtils::compareHepMCParticles ) ;
  std::list< const HepMC::GenParticle * >::iterator it2 ;
  
  for ( it2 = lP2.begin() ; it2 != lP2.end() ; ++it2 ) {
    if ( it2 == lP2.begin() ) { 
      char label[5];
      sprintf( label," O:%2d", theVertex -> particles_out_size() );
      ostr << label; 
    } else { ostr << "     "; }
    //(*part2)->print( ostr ); // uncomment for long debugging printout
    ostr << **it2 << std::endl;
  }
}
// ============================================================================
// The END 
// ============================================================================
