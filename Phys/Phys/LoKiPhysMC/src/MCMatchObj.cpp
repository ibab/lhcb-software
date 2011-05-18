// $Id: MCMatchObj.cpp,v 1.7 2010-03-14 13:35:56 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2009/03/12 12:54:13  ibelyaev
// MCMatchObj: speed-up drastically Monte Carlo truth matching
//
// 
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
#include <algorithm>
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysExtract.h"
// ============================================================================
// LoKiMC 
// ============================================================================
#include "LoKi/MCExtract.h"
// ============================================================================
#include "LoKi/MCMatchObj.h"
#include "LoKi/MCChild.h"
// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::MCMatchObj
 *
 *  This file is a part of LoKi project - 
 *  ``C++ ToolKit  for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly disagrees with 
 *  the smear campaign of Dr.O.Callot et al.:
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software''
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-10
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  struct BasicP : public std::unary_function<const LHCb::Particle*,bool>
  {
    inline bool operator() ( const LHCb::Particle* p ) const 
    {
      return
	0 != p                   && 
	p -> isBasicParticle()   &&
	p -> daughters().empty() &&
	0 == p->endVertex()     ;
    }
  };
  // ==========================================================================
}
// ============================================================================
/* Standard constructor
 *  @param name object name 
 *  @param reporter error reporter 
 */
// ============================================================================
LoKi::MCMatchObj::MCMatchObj
( const std::string&     name     , 
  const LoKi::IReporter* reporter )
  : LoKi::Base ( name , reporter ) 
{}
// ============================================================================
// destructor 
// ============================================================================
LoKi::MCMatchObj::~MCMatchObj() {}
// ============================================================================
/*  check the match of MC truth information 
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
/*  check the match of MC truth information  
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
  
  // check ProtoParticle? 
  if ( !m_tablePP2MC .empty () )
  { 
    // check for the protoparticle 
    const LHCb::ProtoParticle* proto = particle->proto() ;
    if ( 0 != proto )  
    {
      // 3) ProtoParticle -> MCParticle with weight 
      if ( matchInTables 
           ( m_tablePP2MC    , proto , mcparticle ) ) { return true ; } // RETURN
    }
  }
  //
  if ( !m_tableT2MC .empty () || !m_tableT2MCW  .empty () ) 
  {
    const LHCb::ProtoParticle* proto = particle->proto() ;
    if ( 0 != proto ) 
    {
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
  }
  
  /// get the list of daughters to initiate the recursion 
  LoKi::MCTypes::MCContainer children ;
  children.reserve ( 20 ) ;
  LoKi::MCChild::daughters ( mcparticle , children ) ;
  
  // the further action differ for composed and basic particles:
  BasicP basic ;
  
  if ( basic ( particle ) )  
    {
      //   A) For *BASIC* particles: 
      // match a particle with at least one MCParticle from the expanded MC-tree 
      return children.end() != match ( particle          , 
				       children.begin () , 
				       children.end   () ) ;
    }
  //     B) For *COMPOSED* Particles: 
  // require that *ALL* daughters are matched 
  if ( match ( particle->daughters() .begin () , 
	       particle->daughters() .end   () , 
	       children              .begin () , 
	       children              .end   () ) ) { return true ; }
  //
  //     B') the last check for nontrivial structure 
  // the case has been pointed by Steve Blusk 
  //
  if  ( particle->daughters().size() != 
	std::count_if ( particle->daughters().begin () , 
			particle->daughters().end   () , BasicP() ) ) 
    {
      LHCb::Particle::ConstVector vct ;
      vct.reserve ( 16 ) ;
      LoKi::Extract::particles ( particle , std::back_inserter( vct ) , BasicP() ) ;
      //
      return  match ( particle->daughters() .begin () , 
		      particle->daughters() .end   () , 
		      children              .begin () , 
		      children              .end   () ) ;     // RETURN 
      
    }
  //
  return false ;
} 
// ============================================================================
// clear the internal storage
// ============================================================================
void LoKi::MCMatchObj::clear() 
{
  m_tableP2MC   . clear() ;
  m_tableP2MCW  . clear() ;
  m_tablePP2MC  . clear() ;
  m_tableT2MC   . clear() ;
  m_tableT2MC   . clear() ;
}
// ============================================================================
// The END 
// ============================================================================

