// $Id: FinderDict.h,v 1.1 2009-06-28 18:22:42 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_FINDERDICT_H 
#define LOKI_FINDERDICT_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SmartRefVector.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/DecayFinder.h"
#include "LoKi/GetType.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Dicts
  {
    // ========================================================================
    /** @class FinderDict LoKi/FinderDict.h
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-06-28
     */
    template <class TYPE>
    struct FinderDicts_
    {
      // ======================================================================
      /// get the actual "stripped" type 
      typedef typename LoKi::GetType<TYPE>::Type Type ;
      // ======================================================================
    public:
      // ======================================================================
      static bool __hasDecay__
      ( const Decays::Finder_<TYPE>&      finder , 
        const typename Type::Container&   input  ) 
      { return finder.hasDecay  ( input.begin() , input.end() ) ; }
      // ======================================================================
      static bool __hasDecay__
      ( const Decays::Finder_<TYPE>&      finder , 
        const typename Type::ConstVector& input  ) 
      { return finder.hasDecay  ( input.begin() , input.end() ) ; }
      // ======================================================================
      static bool __hasDecay__
      ( const Decays::Finder_<TYPE>&      finder , 
        const typename Type::Vector&      input  ) 
      { return finder.hasDecay  ( input.begin() , input.end() ) ; }
      // ======================================================================
      static bool __hasDecay__
      ( const Decays::Finder_<TYPE>&      finder , 
        const SmartRefVector<Type>&       input  ) 
      { return finder.hasDecay  ( input.begin() , input.end() ) ; }
      // ======================================================================
    public:
      // ======================================================================
      static size_t __findDecay__
      ( const Decays::Finder_<TYPE>&      finder , 
        const typename Type::Container&   input  , 
        typename Type::ConstVector&       output ) 
      { return finder.findDecay  ( input.begin() , input.end() , output ) ; }
      // ======================================================================
      static size_t __findDecay__
      ( const Decays::Finder_<TYPE>&      finder , 
        const typename Type::ConstVector& input  , 
        typename Type::ConstVector&       output ) 
      { return finder.findDecay  ( input.begin() , input.end() , output ) ; }
      // ======================================================================
      static size_t __findDecay__
      ( const Decays::Finder_<TYPE>&      finder , 
        const typename Type::Vector&      input  , 
        typename Type::ConstVector&       output ) 
      { return finder.findDecay  ( input.begin() , input.end() , output ) ; }
      // ======================================================================
      static size_t __findDecay__
      ( const Decays::Finder_<TYPE>&      finder , 
        const SmartRefVector<Type>&       input  , 
        typename Type::ConstVector&       output ) 
      { return finder.findDecay  ( input.begin() , input.end() , output ) ; }
      // ======================================================================
    } ;
    // ========================================================================
  } //                                             end of namespace LoKi::Dicts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_FINDERDICT_H
// ============================================================================
