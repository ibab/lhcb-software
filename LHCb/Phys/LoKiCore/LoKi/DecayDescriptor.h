// $Id: DecayDescriptor.h,v 1.1 2009-05-22 18:12:36 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_DECAYDESCRIPTOR_H 
#define LOKI_DECAYDESCRIPTOR_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
// ============================================================================
namespace Decays 
{
  // ==========================================================================
  namespace Trees 
  {
    // ========================================================================
    /** the type of maching algorithm:
     *
     *   - <c>Daughters</c> match within (all) direct daughetrs 
     *   - <c>Sections</c>  match within all sections of the decay graph 
     *
     *  For C++ classes the nmatchiung algorithm is defined through 
     *  the length of the arrow:
     *   
     *   - <c>Daughters</c> are defined through "short" arrow:
     *   - <c>Sections</c> are defined through "long" arrow:
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-30
     */       
    enum Alg {
      Daughters         = 0 , // match only for the direct daughters 
      Sections              , // match in all graph sections 
    } ;
    // ======================================================================
    /** @enum Oscillation 
     *  simple e-num to distinguish oscillated and non-oscillated particles 
     *
     *   - <c>Undefined</c> : no indication are requred 
     *   - <c>Oscillated</c>: 
     *        <c>[ A ]os -> X Y Z </c>, for (MC- and HepMC-decays only)  
     *   - <c>NotOscillated</c>: 
     *        <c>[ A ]nos -> X Y Z </c>, for (MC- and HepMC-decays only)  
     *
     *  @author Vanya BELYAEV Ivan.BELYAEV@nikhef.nl
     *  @see LHCb:MCParticle
     *  @see LHCb:MCParticle::hasOscillated 
     *  @date   2008-05-28
     */
    enum Oscillation {
      /// no need to check the oscillation flag
      Undefined = 0 , //   non-defined 
      /// require the positive oscilaltion flag 
      Oscillated    , //    
      /// require the negative oscilaltion flag 
      NotOscillated   // 
    } ;     
    // ========================================================================
    /** @enum Arrow 
     *  simple enum to mark varios type of arrows for decay descriptors 
     *  - "->"  simple decay match 
     *  - "-->" match decay in sections 
     *  - "->"  simple decay match ignoring photons  
     *  - "-->" match decay in sections innoring photons 
     *  - "-x>"  simple decay match, ignoring secondary interactions  
     *  - "-->" match decay in sections, ignoring secondary interactions  
     *  - "->"  simple decay match ignoring photons and secondary interactions   
     *  - "-->" match decay in sections innoring photons and secondary interactions
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-05-06 
     */     
    enum Arrow {
      /// 
      Single       ,   // single arrow        "->" 
      LongSingle   ,   // long single arrow  "-->" 
      Double       ,   // dobule arrow        "=>" 
      LongDouble   ,   // long double arrow  "==>"
      SingleX      ,   // single arrow       "-x>" 
      LongSingleX  ,   // long single arrow "--x>" 
      DoubleX      ,   // dobule arrow       "=x>" 
      LongDoubleX  ,   // long double arrow "==x>"
      // =====================================================================
    } ;
    // ======================================================================
    /** simple function to extract "decay-only" flag from the arrow 
     *  @param   arrow the arrow
     *  @return  "decay-only" flag 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-05-06 
     */     
    bool decayOnly ( const Arrow& arrow ) ;
    // ========================================================================
    /** simple function to extract "algorithm" flag from the arrow 
     *  @param   arrow the arrow
     *  @return  "algorithm" flag 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-05-06 
     */     
    Alg  algorithm ( const Arrow& arrow ) ;
    // ========================================================================
    /** simple function to extract "photos" flag from the arrow 
     *  @param   arrow the arrow
     *  @return  "photos" flag  
     *  @see Decays::Trees:Arrow 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-05-06 
     */     
    bool photos    ( const Arrow& arrow ) ; 
    /** get the string representation of the arrow 
     *  @param    arr the arrow
     *  @return  strnig representation of the arrow
     *  @see Decays::Trees:Arrow 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-05-06 
     */     
    std::string arrow ( const Arrow& arr ) ;
    // ========================================================================
    /** valid arrow? 
     *  @param   arrow the arrow
     *  @return  true for valid arrow 
     *  @see Decays::Trees:Arrow 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-05-06 
     */     
    bool valid ( const Arrow& arrow ) ;
    // ========================================================================
    /** valid arrow? 
     *  @param   arrow the arrow
     *  @return  true for valid arrow 
     *  @see Decays::Trees:Arrow 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-05-06 
     */     
    bool valid ( const std::string& arrow ) ;
    // ========================================================================
  } // end of namespace Decays::Trees 
  // ==========================================================================
} // end of namespace Decays
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_DECAY_H
// ============================================================================
