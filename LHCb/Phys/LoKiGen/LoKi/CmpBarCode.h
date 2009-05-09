// $Id: CmpBarCode.h,v 1.3 2009-05-09 19:21:38 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_CMPBARCODE_H 
#define LOKI_CMPBARCODE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
// ============================================================================
// HepMC 
// ============================================================================
#include "HepMC/GenParticle.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace GenParticles 
  {
    // ========================================================================
    /** @struct CmpBarCode CmpBarCode.h LoKi/CmpBarCode.h
     *  "srickt-less" comparisons of HepMC::GenParticle pointers 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-06-24
     */
    struct CmpBarCode 
    {
      // ======================================================================
      /// the only one essential method: compare two particles 
      inline bool operator() 
        ( const HepMC::GenParticle* a , 
          const HepMC::GenParticle* b ) 
      {
        return
          a == b ? false :                   // they are equal, not "less" 
          0 == a ? true  :                   // NULL is always less 
          0 == b ? false :                   // NULL is always less  
          a->barcode () < b->barcode () ;    // compare by barcode 
      }
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::GenParticles
  // ==========================================================================
  namespace GenTypes 
  {
    // ========================================================================
    /// the type of ordered set generator particles 
    typedef std::set<const HepMC::GenParticle*,
                     LoKi::GenParticles::CmpBarCode>                   GenSet ;
    // ========================================================================    
  }
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CMPBARCODE_H
// ============================================================================
