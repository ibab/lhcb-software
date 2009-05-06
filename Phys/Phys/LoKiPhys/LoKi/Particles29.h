// $Id: Particles29.h,v 1.2 2009-05-06 19:31:24 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES29_H 
#define LOKI_PARTICLES29_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/AuxDesktopBase.h"
// ============================================================================
/** @file LoKi/Particles29.h
 *  The file with functors for Jaap Panman
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design. 
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-04-30
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /** @class DHCoplanarity
     *
     *  The functor for Jaap Panman:
     *   - """ I am looking (again) at B-> h D0 decays, where the D0-> pi pi Ks.  
     *         What we are trying to do is filter as much as possible the 
     *         trigger before we are going to invoke the seeding to find the Ks 
     *         in downstream tracks.
     *         The criterion which we cannot apply yet is the co-planarity 
     *         of the straight line connecting the primary vertex and 
     *         the D0 decay vertex, and the straight line defined by the "h" 
     *         (can be K, K*, mu, etc).  The co-planarity can be expressed 
     *         as a distance of closest approach of these two straight lines.
     *         As implementation I would suggest a "CombinationCut" with 
     *         the name ABPVDVDOCA (array-best primary vertex-decay vertex).  
     *         So if the decay descriptor would be [ "B+ -> D0 K+", 
     *         "B- -> D0 K-" ] this would make the straight line between BPV 
     *         and the D0, and the K+- vectors to make a DOCA.   
     *        (or if there is a Kstar, it would use the combined K+pi vector.) """
     *
     *  @attention the functor is sensitive to the decay structure 
     *             the first particle <b>MUST</b> have the valid end-vertex
     *
     *  @see LoKi::Cuts::BPVDVDOCA 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-04-30
     */
    class DHCoplanarity 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
      , public virtual LoKi::AuxDesktopBase 
    {
    public:
      // ======================================================================
      /// MANDATORY : virtual destrcutor 
      virtual ~DHCoplanarity() ;
      /// MANDATORY: clone method ("virtual destructor") 
      virtual  DHCoplanarity* clone() const ;
      /** MANDATORY: the only one essential method 
       *
       *  """
       *  The criterion which we cannot apply yet is the co-planarity 
       *  of the straight line connecting the primary vertex and 
       *  the D0 decay vertex, and the straight line defined by the "h" 
       *  (can be K, K*, mu, etc). 
       *  The co-planarity can be expressed as a distance of closest 
       *  approach of these two straight lines.
       *  As implementation I would suggest a "CombinationCut" with 
       *  the name ABPVDVDOCA (array-best primary vertex-decay vertex).  
       *  So if the decay descriptor would be [ "B+ -> D0 K+", 
       *  "B- -> D0 K-" ] this would make the straight line between BPV 
       *  and the D0, and the K+- vectors to make a DOCA.   
       *  (or if there is a Kstar, it would use the combined K+pi vector.) 
       *  """
       *  @attention the functor is sensitive to the decay structure 
       *             the first particle <b>MUST</b> have the valid end-vertex
       */
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printot
      virtual std::ostream& fillStream  ( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
  } // end of namespace LoKi::Particles 
  // ==========================================================================
  namespace Cuts 
  {
    /** @var  BPVDVDOCA
     *  
     * From Jaap Panman:
     *  """
     *  The criterion which we cannot apply yet is the co-planarity 
     *  of the straight line connecting the primary vertex and 
     *  the D0 decay vertex, and the straight line defined by the "h" 
     *  (can be K, K*, mu, etc). 
     *  The co-planarity can be expressed as a distance of closest 
     *  approach of these two straight lines.
     *  As implementation I would suggest a "CombinationCut" with 
     *  the name ABPVDVDOCA (array-best primary vertex-decay vertex).  
     *  So if the decay descriptor would be [ "B+ -> D0 K+", 
     *  "B- -> D0 K-" ] this would make the straight line between BPV 
     *  and the D0, and the K+- vectors to make a DOCA.   
     *  (or if there is a Kstar, it would use the combined K+pi vector.) 
     *  """
     *  @attention the functor is sensitive to the decay structure 
     *             the first particle <b>MUST</b> have the valid end-vertex
     */
    const LoKi::Particles::DHCoplanarity                            BPVDVDOCA ;
    // ========================================================================
  }  
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END  
// ============================================================================
#endif // LOKI_PARTICLES29_H
// ============================================================================
