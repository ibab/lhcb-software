// $Id: MC2Collision.cpp,v 1.2 2007-03-02 13:26:58 cattanem Exp $
// ============================================================================
// Include files 
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelation.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/GenCollision.h"
#include "Event/MCVertex.h"
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/MCCaloHit.h"
#include "Event/MCRichHit.h"
#include "Event/MCOTDeposit.h"
#include "Event/MCSTDeposit.h"
// ============================================================================
// Kernel ?
// ============================================================================
#include "Kernel/MC2Collision.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Report.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace MC2Collision
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-18 
 */
// ============================================================================

// ============================================================================
const LHCb::GenCollision* 
MC2Collision::collision 
( const LHCb::MCVertex*           vertex , 
  const LHCb::MCVertex2Collision* table  ) 
{
  // trivial check 
  if ( 0 == vertex ) { return 0 ; }                           // RETURN 
  if ( 0 == table  )
  {
    LoKi::Report::Error( "MC2Collision::collision: Table points to NULL!");
    return 0 ;                                                 // RETURN 
  }
  // direct check in the table 
  LHCb::MCVertex2Collision::Range r = table->relations ( vertex ) ;
  if ( !r.empty() ) { return r.front().to() ; }                // RETURN 
  // check the primary vertex 
  const LHCb::MCVertex* prim = vertex->primaryVertex() ;
  if ( prim != vertex ) { return collision ( prim , table ) ; } // RETURN 
  // 
  return 0 ;
} ;
// ============================================================================
const LHCb::GenCollision* 
MC2Collision::collision 
( const LHCb::MCParticle*         particle , 
  const LHCb::MCVertex2Collision* table    ) 
{
  // trivial check 
  if ( 0 == particle ) { return 0 ; }
  return collision ( particle->primaryVertex() , table ) ;
} ;
// ============================================================================
const LHCb::GenCollision* 
MC2Collision::collision 
( const LHCb::MCHit*              hit    , 
  const LHCb::MCVertex2Collision* table  )
{
  // trivial check 
  if ( 0 == hit ) { return 0 ; }
  return collision ( hit->mcParticle() , table ) ;
} ;
// ============================================================================
const LHCb::GenCollision* 
MC2Collision::collision 
( const LHCb::MCCaloHit*          hit    , 
  const LHCb::MCVertex2Collision* table  ) 
{
  // trivial check 
  if ( 0 == hit ) { return 0 ; }
  return collision ( hit->particle() , table ) ;
} ;
// ============================================================================
const LHCb::GenCollision* 
MC2Collision::collision 
( const LHCb::MCOTDeposit*        hit    , 
  const LHCb::MCVertex2Collision* table  ) 
{
  // trivial check 
  if ( 0 == hit ) { return 0 ; }
  return collision ( hit->mcHit() , table ) ;
};
// ============================================================================
const LHCb::GenCollision* 
MC2Collision::collision 
( const LHCb::MCSTDeposit*        hit    , 
  const LHCb::MCVertex2Collision* table  ) 
{
  // trivial check 
  if ( 0 == hit ) { return 0 ; }
  return collision ( hit->mcHit() , table ) ;
};
// ============================================================================
const LHCb::GenCollision* 
MC2Collision::collision 
( const LHCb::MCRichHit*          hit    , 
  const LHCb::MCVertex2Collision* table  )
{
  // trivial check 
  if ( 0 == hit ) { return 0 ; }
  return collision ( hit->mcParticle() , table ) ;
} ;
// ============================================================================
const LHCb::MCVertex* 
MC2Collision::primaryVertex 
( const LHCb::MCVertex* vertex ) 
{
  if ( 0 == vertex ) { return 0 ; }
  return vertex->primaryVertex() ;
} ;
// ============================================================================
const LHCb::MCVertex* 
MC2Collision::primaryVertex 
( const LHCb::MCParticle* particle  ) 
{
  if ( 0 == particle ) { return 0 ; }
  return particle->primaryVertex() ;
} ;
// ============================================================================
const LHCb::MCVertex* 
MC2Collision::primaryVertex 
( const LHCb::MCHit* hit ) 
{
  if ( 0 == hit ) { return 0 ; }
  return primaryVertex ( hit->mcParticle() ) ;
} ;
// ============================================================================
const LHCb::MCVertex* 
MC2Collision::primaryVertex 
( const LHCb::MCRichHit* hit ) 
{
  if ( 0 == hit ) { return 0 ; }
  return primaryVertex ( hit->mcParticle() ) ;
} ;
// ============================================================================
const LHCb::MCVertex* 
MC2Collision::primaryVertex 
( const LHCb::MCCaloHit* hit ) 
{
  if ( 0 == hit ) { return 0 ; }
  return primaryVertex ( hit->particle() ) ;
} ;
// ============================================================================
const LHCb::MCVertex* 
MC2Collision::primaryVertex 
( const LHCb::MCOTDeposit* hit ) 
{
  if ( 0 == hit ) { return 0 ; }
  return primaryVertex ( hit->mcHit() ) ;
} ;
// ============================================================================
const LHCb::MCVertex* 
MC2Collision::primaryVertex 
( const LHCb::MCSTDeposit* hit ) 
{
  if ( 0 == hit ) { return 0 ; }
  return primaryVertex ( hit->mcHit() ) ;
} ;
// ============================================================================
/** Helper function which checs, is the given (primary) vertex 
 *  has as an origin the given collision
 *  @param vertex the vertex 
 *  @param collision the collision
 *  @param table the information source
 *  @return true if the vertex comes from the collision
 */
// ============================================================================
bool MC2Collision::fromCollision 
( const LHCb::MCVertex*            vertex    , 
  const LHCb::GenCollision*        collision , 
  const LHCb::MCVertex2Collision*  table     ) 
{
  if ( 0 == vertex ) { return false ; }                               // RETURN 
  if ( 0 == table  ) 
  {
    LoKi::Report::Error
      ("MC2Collision::fromCollision: Table* points to NULL"); 
    return false ;                                                    // RETURN   
  }
  if ( 0 == collision )
  {
    LoKi::Report::Warning
      ("MC2Collision::fromCollision: LHCb::GenCollision* points to NULL"); 
  }
  typedef LHCb::MCVertex2Collision::Range Range ;
  Range r = table->relations ( vertex ) ;
  for ( Range::iterator ir = r.begin() ; r.end() != ir ; ++ir ) 
  {
    if ( collision == ir->to() ) { return true ; }                    // RETURN
  }
  return false ;                                                      // RETURN 
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
