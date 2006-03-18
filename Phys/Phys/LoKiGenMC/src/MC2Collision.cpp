// $Id: MC2Collision.cpp,v 1.1 2006-03-18 18:26:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
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

// ============================================================================
bool MC2Collision::FromCollision<LHCb::MCVertex>::operator() 
  ( const LHCb::MCVertex* vertex ) const 
{
  if ( 0 == vertex   ) { return false ; }                      // RETURN  
  if ( 0 == m_table  )
  {
    LoKi::Report::Error
      ( "MC2Collision::FromCollision: Table* points to NULL!");
    return false ;                                                 // RETURN 
  }
  if ( 0 == m_collision ) 
  {
    LoKi::Report::Warning
      ( "MC2Collision::FromCollision: LHCb::GenCollision* points to NULL!");
  }

  typedef LHCb::MCVertex2Collision::Range Range ;
  Range r = m_table->relations ( vertex ) ;
  for ( Range::iterator ir = r.begin() ; r.end() != ir ; ++ir ) 
  {
    if ( m_collision == ir->to() ) { return true ; }                // RETURN 
  }
  //
  return false ;
};
// ============================================================================
bool MC2Collision::FromCollision<LHCb::MCParticle>::operator() 
  ( const LHCb::MCParticle* particle ) const 
{
  if ( 0 == particle ) { return false ; }
  return m_evaluator ( particle->primaryVertex() ) ;
} ;
// ============================================================================
bool MC2Collision::FromCollision<LHCb::MCHit>::operator() 
  ( const LHCb::MCHit* hit ) const 
{
  if ( 0 == hit ) { return false ; }
  return m_evaluator ( hit->mcParticle() ) ;
} ;
// ============================================================================
bool MC2Collision::FromCollision<LHCb::MCRichHit>::operator() 
  ( const LHCb::MCRichHit* hit ) const 
{
  if ( 0 == hit ) { return false ; }
  return m_evaluator ( hit->mcParticle() ) ;
} ;
// ============================================================================
bool MC2Collision::FromCollision<LHCb::MCCaloHit>::operator() 
  ( const LHCb::MCCaloHit* hit ) const 
{
  if ( 0 == hit ) { return false ; }
  return m_evaluator ( hit->particle() ) ;
} ;
// ============================================================================
bool MC2Collision::FromCollision<LHCb::MCOTDeposit>::operator() 
  ( const LHCb::MCOTDeposit* hit ) const 
{
  if ( 0 == hit ) { return false ; }
  return m_evaluator ( hit->mcHit() ) ;
} ;
// ============================================================================
bool MC2Collision::FromCollision<LHCb::MCSTDeposit>::operator() 
  ( const LHCb::MCSTDeposit* hit ) const 
{
  if ( 0 == hit ) { return false ; }
  return m_evaluator ( hit->mcHit() ) ;
} ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
