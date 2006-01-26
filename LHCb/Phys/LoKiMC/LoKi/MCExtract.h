// $Id: MCExtract.h,v 1.1.1.1 2006-01-26 16:13:39 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_EXTRACT_H 
#define LOKI_EXTRACT_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <vector>
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// ============================================================================
// LoKi 
// ============================================================================
#include  "LoKi/MCTypes.h"
// ============================================================================

// ============================================================================
/** @file
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
 *  with the campain of Dr.O.Callot at al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

namespace LoKi 
{
  namespace Extract 
  {    
    /** @fn getMCParticles 
     *  Simple function to extract (recursively) all Monte Carlo 
     *  particles form the given 
     *  Monte Carlo particle
     *  Indeed it is just a flat representation of the whole 
     *  Monte Carlo decay tree.
     * 
     *  @code 
     *  
     *  std::vector<const LHCb::MCParticle*> MCPs ;
     *
     *  const LHCb::MCParticle* particle  = ... ;
     *
     *  // all daughter particles 
     *  MCPs              particles       ; 
     *  LoKi::Extract::getMCParticles
     *   ( particle , std::back_inserter( partciles ) ) ;
     *  
     *  // only pi0s 
     *  MCPs  pi0s ; 
     *  LoKi::Extract::getMCParticles
     *        ( particle                        , 
     *          std::back_inserter( pi0s )      , 
     *          111 == MCID                     ) ;
     *
     *  @endcode 
     *
     *  @param particle input pointer to MCParticle object
     *  @param output   output iterator to container of pointer to MCParticle
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     *  @date 2003-02-07
     */
    template <class OUTPUT, class PREDICATE> 
    inline OUTPUT  getMCParticles 
    ( const LHCb::MCParticle* particle , 
      OUTPUT                  output   , 
      const PREDICATE&        cut      )
    {
      if ( 0 == particle ) { return output ; } // RETURN 
      // fill and advance the output iterator 
      if ( cut ( particle ) ) 
      { *output  = const_cast<LHCb::MCParticle*>( particle ) ; ++output ; }
      // loop over all decay tree 
      typedef SmartRefVector<LHCb::MCVertex>    MCVs ;
      typedef SmartRefVector<LHCb::MCParticle>  MCPs ;
      const MCVs& endVertices = particle->endVertices();
      for ( MCVs::const_iterator ivx = endVertices.begin() ; 
            endVertices.end() != ivx ; ++ivx ) 
      {
        const LHCb::MCVertex* vx = *ivx ;
        if ( 0 == vx ) { continue ; }                         // CONTINUE 
        const MCPs& products = vx->products() ;
        for ( MCPs::const_iterator ip = products.begin() ; 
              products.end() != ip ; ++ip ) 
        {
          const LHCb::MCParticle* child = *ip ;
          if ( 0 == child ) { continue ; }                     // CONTINUE
          // start the recursion 
          output = getMCParticles ( child , output , cut ) ;   //RECURSION
        }
      }
      //
      return output ;
    };
    
    template <class MCPARTICLE , class OUTPUT, class PREDICATE>
    inline OUTPUT getMCParticles 
    ( MCPARTICLE        begin  , 
      MCPARTICLE        end    , 
      OUTPUT            output , 
      const PREDICATE&  cut    )
    {
      for ( ; begin != end ; ++begin ) 
      { output = getMCParticles ( *begin , output , cut ) ; }
      return output ;
    };
    
    template <class OUTPUT> 
    inline OUTPUT  getMCParticles 
    ( const LHCb::MCParticle* particle , 
      OUTPUT                  output   )
    { return getMCParticles 
        ( particle , 
          output   , 
          LoKi::BooleanConstant<const LHCb::MCParticle*> ( true )  ) ; }
    
    template <class MCPARTICLE , class OUTPUT>
    inline OUTPUT getMCParticles 
    ( MCPARTICLE        begin  , 
      MCPARTICLE        end    , 
      OUTPUT            output )
    { return getMCParticles 
        ( begin  , 
          end    , 
          output , 
          LoKi::BooleanConstant<const LHCb::MCParticle*> ( true )  ) ; }
    
    /** @fn mcParticles 
     *  Simple function to extract all Monte Carlo particles that 
     *  make contribution to the given particle. 
     *  Indeed it is just a flat representation of the Monet Carlo decay tree.
     * 
     *  @code 
     *  
     *  std::vector<const LHCb::MCParticle*> MCPs ;
     *
     *  const MCParticle* particle  = ... ;
     *
     *  // all daughter particles 
     *  MCPs              particles       ; 
     *  LoKi::Extract::mcParticles
     *   ( particle , std::back_inserter( particles ) ) ;
     *  
     *  // only pi0s 
     *  MCPs  pi0s ; 
     *  LoKi::Extract::mcParticles
     *   ( particle    , 
     *     std::back_inserter( pi0s )      , 
     *     111 == MCID ) ;
     *
     *  @endcode 
     *
     *  @param particle input pointer to MCParticle object
     *  @param output   output iterator to container of pointer to MCParticle
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     *  @date 2003-02-07
     */
    template <class OUTPUT, class PREDICATE> 
    inline OUTPUT mcParticles 
    ( const LHCb::MCParticle* particle , 
      OUTPUT            output   , 
      const PREDICATE&  cut      )
    { return getMCParticles ( particle , output , cut ) ; } ;
    
    template <class MCPARTICLE , class OUTPUT, class PREDICATE>
    inline OUTPUT mcParticles 
    ( MCPARTICLE        begin   , 
      MCPARTICLE        end     , 
      OUTPUT            output  , 
      const PREDICATE&  cut     )
    { return getMCParticles ( begin , end , output , cut ) ; } ;
    
    template <class OUTPUT> 
    inline OUTPUT mcParticles 
    ( const LHCb::MCParticle* particle , 
      OUTPUT            output   )
    { return getMCparticles ( particle , output ) ; } ;
    
    template <class MCPARTICLE , class OUTPUT>
    inline OUTPUT mcParticles 
    ( MCPARTICLE        begin   , 
      MCPARTICLE        end     , 
      OUTPUT            output  )
    { return getMCparticles ( begin , end , output ) ; } ;

    
    /** @fn getMCdaugters
     *  Simple function to extract the daughters from the given 
     *  Monte Carlo particle
     * 
     *  @code 
     *  
     *  std::vector<const LHCb::MCParticle*> MCPs ;
     *
     *  const LHCb::MCParticle* particle  = ... ;
     *
     *  // daughter particles 
     *  MCPs              particles       ; 
     *  LoKi::Extract::getMCdaughters
     *    ( particle , std::back_inserter( partciles ) ) ;
     *  
     *  // only pi0s 
     *  MCPs  pi0s ; 
     *  LoKi::Extract::getMCdaughters
     *    ( particle                        , 
     *      std::back_inserter( pi0s )      , 
     *      111 == MCID                     ) ;
     *
     *  @endcode 
     *
     *  @param particle input pointer to MCParticle object
     *  @param output   output iterator to container of pointer to MCParticle
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     *  @date 2003-02-07
     */
    template <class OUTPUT, class PREDICATE> 
    inline OUTPUT getMCdaughters
    ( const LHCb::MCParticle* particle , 
      OUTPUT                  output   , 
      const PREDICATE&        cut      )
    {
      if ( 0 == particle ) { return output ; } // RETURN 
      // loop over daughter particles  
      typedef SmartRefVector<LHCb::MCVertex>    MCVs ;
      typedef SmartRefVector<LHCb::MCParticle>  MCPs ;
      const MCVs& endVertices = particle->endVertices();
      for ( MCVs::const_iterator ivx = endVertices.begin() ; 
            endVertices.end() != ivx ; ++ivx ) 
      {
        const LHCb::MCVertex* vx = *ivx ;
        if ( 0 == vx ) { continue ; }           // CONTINUE 
        const MCPs& products = vx->products() ;
        for ( MCPs::const_iterator ip = products.begin() ; 
              products.end() != ip ; ++ip ) 
        {
          const LHCb::MCParticle* child = *ip ;
          if ( 0 == child || !cut ( child ) ) { continue ; }  // CONTINUE 
          // fill and advance output iterator 
          *output  = const_cast<LHCb::MCParticle*>( child ) ;
          ++output ; 
        }
      }
      //
      return output ;
    };
    
    template <class MCPARTICLE , class OUTPUT, class PREDICATE>
    inline OUTPUT getMCdaughters 
    ( MCPARTICLE        begin   , 
      MCPARTICLE        end     , 
      OUTPUT            output  , 
      const PREDICATE&  cut     )
    { 
      for ( ; begin != end ; ++begin ) 
      { output = getMCdaughters ( *begin , output , cut ) ; }
      return output ;
    } ;
    
    template <class OUTPUT> 
    inline OUTPUT getMCdaughters
    ( const LHCb::MCParticle* particle , 
      OUTPUT                  output   )
    { return getMCdaughters 
        ( particle , 
          output   , 
          LoKi::BooleanConstant<const LHCb::MCParticle*> ( true )  ) ; }
    
    template <class MCPARTICLE , class OUTPUT>
    inline OUTPUT getMCdaughters 
    ( MCPARTICLE        begin   , 
      MCPARTICLE        end     , 
      OUTPUT            output  )
    { return getMCdaughters 
        ( begin  , 
          end    ,  
          output , 
          LoKi::BooleanConstant<const LHCb::MCParticle*> ( true )  ) ; }
    
    /** @fn mcDaughters
     *  Simple function to extract the daughters from the given 
     *  Monte Carlo particle
     * 
     *  @code 
     *  
     *  std::vector<const LHCb::MCParticle*> MCPs ;
     *
     *  const LHCb::MCParticle* particle  = ... ;
     *
     *  // daughter particles 
     *  MCPs              particles       ; 
     *  LoKi::Extract::getMCdaughters
     *    ( particle , std::back_inserter( partciles ) ) ;
     *  
     *  // only pi0s 
     *  MCPs  pi0s ; 
     *  LoKi::Extract::getMCdaughters
     *    ( particle                        , 
     *      std::back_inserter( pi0s )      , 
     *      111 == MCID                     ) ;
     *
     *  @endcode 
     *
     *  @param particle input pointer to MCParticle object
     *  @param output   output iterator to container of pointer to MCParticle
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     *  @date 2003-02-07
     */
    template <class OUTPUT, class PREDICATE> 
    inline OUTPUT mcDaughters
    ( const LHCb::MCParticle* particle , 
      OUTPUT                  output   , 
      const PREDICATE&        cut      )
    { return getMCdaughters( particle , output , cut ) ; }
    
    template <class MCPARTICLE , class OUTPUT, class PREDICATE>
    inline OUTPUT mcDdaughters 
    ( MCPARTICLE        begin   , 
      MCPARTICLE        end     , 
      OUTPUT            output  , 
      const PREDICATE&  cut     )
    { return getMCDaughters( begin , end , output , cut ) ; }

    template <class OUTPUT> 
    inline OUTPUT mcDaughters
    ( const LHCb::MCParticle* particle , 
      OUTPUT                  output   )
    { return getMCdaughters ( particle , output ) ; }
    
    template <class MCPARTICLE , class OUTPUT>
    inline OUTPUT mdDaughters 
    ( MCPARTICLE        begin   , 
      MCPARTICLE        end     , 
      OUTPUT            output  )
    { return getMCdaughters ( begin , end , output ) ; }

    //
  }; // end of namespace Extract 
  //
}; // end of namespace LoKi 


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_EXTRACT_H
// ============================================================================
