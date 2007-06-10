// $Id: Holder.h,v 1.1 2007-06-10 19:54:05 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_HOLDER_H 
#define LOKI_HOLDER_H 1
// ============================================================================
// Include files
// ============================================================================
#include "LoKi/Reference.h"
#include "LoKi/Field.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class Holder Holder.h LoKi/Holder.h
   *  Helper class to treat 2-argument functions 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2007-06-08
   */
  template <class TYPE1,class TYPE2>
  struct Holder : public std::pair <typename LoKi::Field<const TYPE1>::Type,
                                    typename LoKi::Field<const TYPE2>::Type>
  {
  protected:
    typedef typename  LoKi::Field<const TYPE1>::Type         First  ;
    typedef typename  LoKi::Field<const TYPE2>::Type         Second ;
    typedef std::pair<First,Second>                    Pair   ;
  public:
    /// constructor from data fields (including the default) 
    Holder ( First f = First() , Second s = Second() ) 
      : Pair ( f , s ) 
    {}
    /// constructor form the base 
    Holder ( const Pair & base )
      : Pair ( base ) 
    {}
    /// copy constuctor 
    Holder ( const Holder& copy  )
      : Pair ( copy ) 
    {}
  };
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// ============================================================================
#endif // LOKI_HOLDER_H
// ============================================================================
