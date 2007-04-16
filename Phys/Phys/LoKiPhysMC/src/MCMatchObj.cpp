// $Id: MCMatchObj.cpp,v 1.5 2007-04-16 16:16:48 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2006/10/10 09:16:53  ibelyaev
//  tiny fixes needed for good&valid dictionaries
//
// Revision 1.3  2006/08/29 11:40:47  ibelyaev
//  many fixed to simplify MC-match
//
// Revision 1.2  2006/04/09 20:14:25  ibelyaev
//  fix for Win32
//
// Revision 1.1.1.1  2006/03/14 19:12:21  ibelyaev
// New package : RC <---> MC links for LoKi 
// 
// ============================================================================
// Include files
// ============================================================================
// LoKiMC 
// ============================================================================
#include "LoKi/MCExtract.h"
// ============================================================================
#include "LoKi/MCMatchObj.h"
// ============================================================================

// ============================================================================
/** @file
 *
 * Implementation file for class LoKi::MCMatchObj
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-10
 */
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param name object name 
 *  @param reporter error reporter 
 */
// ============================================================================
LoKi::MCMatchObj::MCMatchObj
( const std::string&     name     , 
  const LoKi::IReporter* reporter )
  : LoKi::Base ( name , reporter ) 
{};
// ============================================================================
/// destructor 
// ============================================================================
LoKi::MCMatchObj::~MCMatchObj()
{};
// ============================================================================
/** check the match of MC truth information 
 *  @param  particle    pointer to Particle object 
 *  @param  range       range of MC particles 
 *  @return true if particle matches at least 1 MC particle from range 
 */
// ============================================================================
bool LoKi::MCMatchObj::match     
( const LHCb::Particle*       particle ,
  const LoKi::Types::MCRange& range    ) const 
{ return range.end() != match ( particle , range.begin() , range.end() ) ; }
// ============================================================================
/** check the match of MC truth information  
 *  @param  particle    pointer to Particle object  
 *  @param  mcparticle  pointer to MCParticle object 
 *  @return true        if the particle and mcparticle has "match"
 */
// ============================================================================
bool LoKi::MCMatchObj::match   
( const LHCb::Particle*   particle   , 
  const LHCb::MCParticle* mcparticle )  const 
{ 
  if     ( 0 == particle   ) 
  { Warning ( "match():   Particle* points to NULL" ) ; return false ; } 
  else if( 0 == mcparticle ) 
  { Warning ( "match(): MCParticle* points to NULL" ) ; return false ; }
  
  // look throught the existing tables:

  // 1) Particle->MCParticle 
  if ( matchInTables
       ( m_tableP2MC   , particle , mcparticle  ) ) { return true ; } // RETURN  
  // 2) Particle->MCParticle with weight 
  if ( matchInTables
       ( m_tableP2MCW  , particle , mcparticle  ) ) { return true ; } // RETURN
  
  // check for the protoparticle 
  const LHCb::ProtoParticle* proto = particle->proto() ;
  if ( 0 != proto )  
  {
    // 3) ProtoParticle -> MCParticle with weight 
    if ( matchInTables 
         ( m_tablePP2MC    , proto , mcparticle ) ) { return true ; } // RETURN
    // check for the track 
    const LHCb::Track* track = proto->track() ;
    if ( 0 != track ) 
    {
      // 4) Track->MCParticle 
      if ( matchInTables
           ( m_tableT2MC   , track , mcparticle ) ) { return true ; } // RETURN
      // 5) Track->MCParticle with weight 
      if ( matchInTables
           ( m_tableT2MCW  , track , mcparticle ) ) { return true ; } // RETURN
    }
  }
  
  /// expand the whole MC-tree into the the flat list 
  LoKi::MCTypes::MCContainer children ;
  children.reserve ( 100 ) ;
  // get all particles except the tree head  itself 
  LoKi::Extract::mcParticles
    ( mcparticle , 
      std::back_inserter ( children ) , 
      std::not1 ( std::bind2nd ( std::equal_to<const LHCb::MCParticle*>(),
                                 mcparticle ) ) ) ;
  
  // the further action differ for composed and basic particles:
  
  if ( particle->isBasicParticle() ) 
  {
    //   A) For *BASIC* particles: 
    // match a particle with at least one MCParticle from the expanded MC-tree 
    return children.end() != match ( particle          , 
                                     children.begin () , 
                                     children.end   () ) ;
  }
  //     B) For *COMPOSED* Particles: 
  // require that *ALL* daughters are matched 
  return match ( particle->daughters() .begin () , 
                 particle->daughters() .end   () , 
                 children              .begin () , 
                 children              .end   () ) ;
} ;
// ============================================================================
/// clear th einetrnal storage
// ============================================================================
void LoKi::MCMatchObj::clear() 
{
  m_tableP2MC   .clear() ;
  m_tableP2MCW  .clear() ;
  m_tablePP2MC  .clear() ;
  m_tableT2MC   .clear() ;
  m_tableT2MC   .clear() ;
};
// ============================================================================
// The END 
// ============================================================================

