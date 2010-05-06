// $Id: LoKi_Track2MC.cpp,v 1.1 2010-05-06 11:14:48 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/Algo.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelationWeighted2D.h"
// ============================================================================
// DaVinciMCKernel
// ============================================================================
#include "Kernel/Track2MC.h"
// ============================================================================
/** @file 
 *  Simple algorithm to illustrate the usage of  Track<-->MC relation tables 
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2010-05-06
 */
// ============================================================================
LOKI_ALGORITHM(Track2MCEx)
{
  
  // get Tracks from Transient Event Store  
  LHCb::Track::Range tracks = 
    get<LHCb::Track::Range> ( LHCb::TrackLocation::Default    ) ;
  
  // get the relation table form Transient Event Store  
  const LHCb::Track2MC2D* table2d  = 
    get <LHCb::Track2MC2D>  ( LHCb::Track2MCLocation::Default ) ;
  
  // get "inverse" table : MCParticle -> Track 
  const LHCb::MC2Track* itable = table2d->inverse() ;
  
  // Track -> MCParticle links 
  
  // loop over the tracks:
  for ( LHCb::Track::Range::iterator itrack = tracks.begin() ; 
        tracks.end() != itrack ; ++itrack ) 
  {
    const LHCb::Track* track = *itrack ;
    if ( 0 == track    ) { continue ; }                          // CONTINUE  
    
    // get all relations for the given track:
    LHCb::Track2MC2D::Range links = table2d->relations ( track ) ;
    if ( links.empty() ) { continue ; }                          // CONTINUE 
    
    // loop over related MC-particles :
    for ( LHCb::Track2MC2D::Range::iterator link = links.begin() ; 
          links.end() != link ; ++link ) 
    {
      // get the related MC-particle 
      const LHCb::MCParticle* mcparticle = link->to() ;
      // get the "weight" associated with the given link:
      const double weight = link->weight() ;
      // print: 
      // ...
      info() << "The key/weight for related MC-particle " 
             << mcparticle->key() << "/"  
             << track->key()      << "/" << weight << endmsg ;
      
    } //                                            end of the loop over links

  } //                                              end of the loop over tracks
  
  
  // MCParticle -> Track links
  
  //  get all MC-particles from Transient store 
  LHCb::MCParticle::Container* mcparticles = 
    get<LHCb::MCParticle::Container> ( LHCb::MCParticleLocation::Default ) ;
  
  // loop over all MC-particles:
  for ( LHCb::MCParticle::Container::iterator imc = mcparticles->begin() ; 
        mcparticles->end() != imc ; ++imc ) 
  {
    const LHCb::MCParticle* mc = *imc ;
    if ( 0 == mc ) { continue ; }                                // CONTINUE 
    
    // get all related tracks:
    LHCb::MC2Track::Range links = itable->relations ( mc ) ;
    if ( links.empty() ) { continue ; }                          // CONTINUE 
    
    // links over all related tracks:
    for (  LHCb::MC2Track::Range::iterator link = links.begin() ; 
           links.end() != link ; ++link ) 
    {
      // get the related track
      const LHCb::Track* track  = link->to() ;
      // get the "weight" associated with the given link:
      const double       weight = link->weight() ;
      // print
      // ...
      info() << "The key/weight for related Track " 
             << mc    -> key () << "/"  
             << track -> key () << "/" << weight << endmsg ;
      
    } //                                 end of the loop over links

  } //                                   end of the loop over all MC-particles 
  
  
  return StatusCode::SUCCESS ;
  
}
// ============================================================================
// The END 
// ============================================================================
