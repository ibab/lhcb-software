// $Id: MCAlgs.h,v 1.4 2006-11-27 11:58:37 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/10/27 13:35:46  ibelyaev
//  fix for SLC4 platform
//
// Revision 1.2  2006/02/18 18:10:57  ibelyaev
//  fix a typo
//
// Revision 1.1  2006/02/09 15:42:06  ibelyaev
//  add LoKi::MCAlgs namespace with algorithms
//
// ============================================================================
#ifndef LOKI_MCALGS_H 
#define LOKI_MCALGS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
#include <functional>
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MCTypes.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for the functions from LoKi::Extract namespace 
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
 *  @date 2006-02-08 
 */
// ============================================================================

namespace LoKi 
{
  /** @namespace  LoKi::MCAlgs MCAlgs.h LoKi/MCAlgs.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-09
   */
  namespace MCAlgs 
  {
    
    /** @fn count_if
     *  the trivial algorithm to count the number of MC particles
     *  in the tree, which satisfy the certain criteria 
     *
     *  @param   first  'begin'-iterator for the sequence 
     *  @param   last   'end'-iterator for the sequence 
     *  @param   predicate criteria 
     *  @return  number of aprticles which satisfy the criteria
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-09
     */
    template <class OBJECT,class PREDICATE>
    inline size_t  count_if
    ( OBJECT           first     ,
      OBJECT           last      , 
      const PREDICATE& predicate ) ;
    
    /** @fn found
     *  the trivial algorithm to find the MC particles
     *  in the tree, which satisfy the certain criteria 
     *
     *  @param   first  'begin'-iterator for the sequence 
     *  @param   last   'end'-iterator for the sequence 
     *  @param   predicate criteria 
     *  @return  true of tehre exist at least one particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-09
     */
    template <class OBJECT,class PREDICATE>
    inline bool found
    ( OBJECT           first     ,
      OBJECT           last      , 
      const PREDICATE& predicate ) ;
    
    /** @fn count_if 
     *  the trivial algorithm to count number of MC particles
     *  in the tree, which satisfy the certain criteria 
     *
     *  Count number of muons in the decay tree of the B:
     *
     *  @code 
     *
     *  const LHCb::MCVertex* vx = .. ;
     *
     *  const size_n muons = 
     *     LoKi::MCAlgs::count_if ( vx , "mu+" == MCABSID ) ;
     * 
     *  @endcode 
     *
     *  @see LHCb::MCVertex
     *  @see LoKi::Cuts::MCABSID 
     *
     *  @param   vertex pointer to MC vertex 
     *  @param   predicate criteria 
     *  @return  number of aprticles which satisfy the criteria
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-09
     */
    template <class PREDICATE>
    inline size_t  count_if 
    ( const LHCb::MCVertex* vertex    , 
      const PREDICATE&      predicate )
    {
      if ( 0 == vertex ) { return 0 ; }                       // RETURN 
      //
      typedef SmartRefVector<LHCb::MCParticle> Products ;
      const Products& products = vertex->products() ;
      //
      return LoKi::MCAlgs::count_if 
        ( products.begin() , products.end() , predicate ) ;
    } ;
    
    /** @fn count_if 
     *  the trivial algorithm to count number of MC particles
     *  in the tree, which satisfy the certain criteria 
     *
     *  @code 
     *
     *  const LHCb::MCParticle* B = .. ;
     *
     *  const size_n muons = 
     *     LoKi::MCAlgs::count_if( B , "mu+" == MCABSID ) ;
     * 
     *  @endcode 
     *
     *
     *  @see LHCb::MCParticle
     *  @see LoKi::Cuts::MCABSID 
     *
     *  @param   particle pointer to MC particle  
     *  @param   predicate criteria 
     *  @return  number of aprticles which satisfy the criteria
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-09
     */
    template <class PREDICATE>
    inline size_t  count_if 
    ( const LHCb::MCParticle* particle   , 
      const PREDICATE&        predicate )
    {
      if ( 0 == particle ) { return 0 ; }
      // evaluate the predicate! 
      size_t result = predicate( particle ) ? 1 : 0 ;
      //
      typedef SmartRefVector<LHCb::MCVertex> EndVertices;
      const EndVertices& vertices = particle->endVertices() ;
      //
      return result + LoKi::MCAlgs::count_if 
        ( vertices.begin() , vertices.end() , predicate ) ;
    } ;
    
    /** @fn found 
     *  trivial algorithm which returns true if 
     *  the exist at least one particle in the tree which satisfy 
     *  the certain criteria
     *
     *  Check the presence of high-PT muon in the 
     *   decay tree of the particle:
     *
     *  @code 
     *  
     *  const LHCb::MCParticle* B = ... ;
     *  
     *  MCCut lepton = "mu+" == MCABSID && PT > 2 * GeV ;
     * 
     *  const bool good = found ( B , lepton ) ;
     * 
     *  @endcode 
     *
     *  @see LoKi::Types::MCCut
     *  @see LoKi::Cuts::MCABSID 
     *  @see LoKi::Cuts::MCPT 
     *
     *  @param   particle pointer to MC particle  
     *  @param   predicate criteria 
     *  @return  number of aprticles which satisfy the criteria
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-09
     */
    template <class PREDICATE>
    inline bool found  
    ( const LHCb::MCParticle* particle  , 
      const PREDICATE&        predicate )
    {
      if ( 0 == particle         ) { return false ; }   // RETURN 
      // evaluate the predicate! 
      if ( predicate( particle ) ) { return true  ; }   // RETURN 
      //
      typedef SmartRefVector<LHCb::MCVertex> EndVertices;
      const EndVertices& vertices = particle->endVertices() ;
      //
      return LoKi::MCAlgs::found 
        ( vertices.begin() , vertices.end() , predicate ) ;
    } ;
    
    /** @fn found 
     *  trivial algorithm which returns true if 
     *  the exist at least one particle in the tree which satisfy 
     *  the certain criteria
     *
     *  @code 
     *  
     *  const LHCb::MCVertex* vx = ... ;
     *  
     *  MCCut lepton = "mu+" == MCABSID && PT > 2 * GeV ;
     * 
     *  const bool good = found ( vx , lepton ) ;
     * 
     *  @endcode 
     *
     *  @see LoKi::Types::MCCut
     *  @see LoKi::Cuts::MCABSID 
     *  @see LoKi::Cuts::MCPT 
     *  
     *  @param   vertex  pointer to MC vertex 
     *  @param   predicate criteria 
     *  @return  number of aprticles which satisfy the criteria
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-09
     */
    template <class PREDICATE>
    inline bool found  
    ( const LHCb::MCVertex*   vertex    , 
      const PREDICATE&        predicate )
    {
      if ( 0 == vertex           ) { return false ; }   // RETURN 
      //
      typedef SmartRefVector<LHCb::MCParticle> Products ;
      const Products& products = vertex->products() ;
      //
      return LoKi::MCAlgs::found 
        ( products.begin() , products.end() , predicate ) ;
    } ;

    /** @fn count_if
     *  the trivial algorithm to count the number of MC particles
     *  in the tree, which satisfy the certain criteria 
     *
     *  @param   first  'begin'-iterator for the sequence 
     *  @param   last   'end'-iterator for the sequence 
     *  @param   predicate criteria 
     *  @return  number of aprticles which satisfy the criteria
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-09
     */
    template <class OBJECT,class PREDICATE>
    inline size_t  count_if
    ( OBJECT           first     ,
      OBJECT           last      , 
      const PREDICATE& predicate )
    {
      size_t result = 0 ;
      for ( ; first != last ; ++first ) 
      { result += LoKi::MCAlgs::count_if ( *first , predicate ) ; }
      //
      return result ;
    } ;

    /** @fn found
     *  the trivial algorithm to find the MC particles
     *  in the tree, which satisfy the certain criteria 
     *
     *  @param   first  'begin'-iterator for the sequence 
     *  @param   last   'end'-iterator for the sequence 
     *  @param   predicate criteria 
     *  @return  true of tehre exist at least one particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-02-09
     */
    template <class OBJECT,class PREDICATE>
    inline bool found
    ( OBJECT           first     ,
      OBJECT           last      , 
      const PREDICATE& predicate )
    {
      for ( ; first != last ; ++first ) 
      { 
        if ( LoKi::MCAlgs::found ( *first , predicate ) ) 
        { return true ; }                                      // RETURN 
      }
      return false ;
    } ;
    
  }  // end of namespace LoKi::MCTrees
} // end of namespace LoKi 
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCALGS_H
// ============================================================================
