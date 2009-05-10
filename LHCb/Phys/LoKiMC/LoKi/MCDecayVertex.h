// $Id: MCDecayVertex.h,v 1.4 2009-05-10 10:11:42 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCDECAYVERTEX_H 
#define LOKI_MCDECAYVERTEX_H 1
// ============================================================================
// Include files
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace MCVertices
  {
    // ========================================================================
    /** @struct MCDecayVertex
     *  trivial structure needed for location of valid decay vertices 
     *  @see LHCb::MCVertex::isDecay
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-02
     */
    struct MCDecayVertex
    {
      inline bool operator() ( const LHCb::MCVertex* vertex ) const 
      { return 0 != vertex && vertex->isDecay () ; }
    } ;
    // ========================================================================
    /** @var IsDecay
     *  useful variable to indicate the decay vertex 
     *  @see LHCb::MCVertex::isDecay
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-02
     */
    const MCDecayVertex               IsDecay = MCDecayVertex()  ;
    // ========================================================================    
    /** simple function to get the decay vertex for the given 
     *  Monte Carlo particle 
     *
     *  @code 
     *  
     *  const LHCb::MCParticle* p = ... ;
     * 
     *  // get the decay vertex:
     *  const LHCb::MCVertex* decay = decayVertex ( p ) ;
     *
     *  @endcode 
     *  
     *  @see LHCb::MCParticle
     *  @see LHCb::MCVertex 
     *  @see LHCb::MCVertex::isDecay 
     *  @see LoKi::MCVertices::IsDecay
     *  @see LoKi::MCVertices::MCDecayVertex
     *
     *  @param p Monte Carlo particle 
     *  @return the decay vertex 
     *
     *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
     *  @date 2008-07-10
     */
    const LHCb::MCVertex* decayVertex ( const LHCb::MCParticle* p ) ;
    // ========================================================================
  } // end of namespace LoKi::MCVertices
  // ==========================================================================
} //end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCDECAYVERTEX_H
// ============================================================================

