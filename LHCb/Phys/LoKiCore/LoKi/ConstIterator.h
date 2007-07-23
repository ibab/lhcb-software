// $Id: ConstIterator.h,v 1.5 2007-07-23 17:07:37 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_CONSTITERATOR_H 
#define LOKI_CONSTITERATOR_H 1
// ============================================================================
// Include files
// ============================================================================
// STD&STL 
// ============================================================================
#include <iterator>
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Const.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-29
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @struct ConstIterator
   *  iterator which mimics the behaviou of the 
   *  general iterator over non-mutable sequence of non-mutable elements
   *  e.g. the sequence of "TYPE" elements looks like sequence of "const TYPE"
   *  elements, the sequence of "TYPE*" elements behaves like sequence of 
   *  "TYPE const * const" elements, etc.. The "vizible" type is detemined by 
   *  LoKi::Const<SEQUENCE::value_type>::Value 
   *  @see _Const
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-03-15
   */
  template <class ITERATOR>
  struct ConstIterator : public std::iterator 
  <
    typename ITERATOR::iterator_category                        , // from the base  
    typename LoKi::Const<typename ITERATOR::value_type>::Value  , // Const!
    typename ITERATOR::difference_type                          , // from the base 
    typename LoKi::Const<typename ITERATOR::pointer>::Value     , // Const!
    typename LoKi::Const<typename ITERATOR::reference>::Value     // Const!
  >
  {
  protected:
    /// One of the @link iterator_tags tag types@endlink.
    typedef typename ITERATOR::iterator_category                iterator_category;
    /// The type "pointed to" by the iterator.
    typedef typename LoKi::Const<typename ITERATOR::value_type>::Value value_type;
    /// Distance between iterators is represented as this type.
    typedef typename ITERATOR::difference_type                    difference_type;
    /// This type represents a pointer-to-value_type.
    typedef typename LoKi::Const<typename ITERATOR::pointer>::Value       pointer;
    /// This type represents a reference-to-value_type.
    typedef typename LoKi::Const<typename ITERATOR::reference>::Value   reference;
  public:
    /// the default constructor : the base must be  default contrubctible!
    ConstIterator() : m_it() {} ;
    /// the constructor ftom the base 
    ConstIterator( const ITERATOR& base  ) : m_it ( base ) {} ;
    /// compiler generated copy and assignement are OK
    
    /// the magic starts here
    pointer   operator->() const { return  m_it.operator->() ; } ;
    /// ... and it continues here
    reference operator* () const { return *m_it              ; } ;
    
    // all other lines are just boring repetitions...
    
    /// pre-incrment
    ConstIterator& operator++()      
    { ++m_it  ; return *this ; }
    /// post-incrment 
    ConstIterator  operator++( int ) 
    { ConstIterator tmp(*this) ; m_it++ ; return tmp ; }
    /// pre-decrement 
    ConstIterator& operator--()
    { --m_it  ; return *this ; }
    /// post-decrement 
    ConstIterator  operator--( int ) 
    { ConstIterator tmp(*this) ; m_it-- ; return tmp ; }
    //
    ConstIterator& operator+=( const difference_type offset ) 
    { std::advance ( m_it ,      offset ) ; return *this ; } ;
    //
    ConstIterator& operator-=( const difference_type offset ) 
    { std::advance ( m_it , -1 * offset ) ; return *this ; } ;
    //
    difference_type operator-( const      ITERATOR& right ) const
    { return std::distance( m_it , right ) ; }
    //
    difference_type operator-( const ConstIterator& right ) const
    { return (*this) - right.m_it          ; }
    //
    bool operator== ( const      ITERATOR& right ) const 
    { return m_it    == right      ; }
    //
    bool operator== ( const ConstIterator& right ) const
    { return (*this) == right.m_it ; }
    //
    bool operator!= ( const      ITERATOR& right ) const 
    { return  !( *this == right )  ; }
    bool operator!= ( const ConstIterator& right ) const
    { return  !( *this == right )  ; }
    //
    ConstIterator  operator+( const difference_type offset ) 
    {
      ConstIterator tmp(*this) ;
      return tmp += offset ;
    };
    //
    ConstIterator  operator-( const difference_type offset ) 
    {
      ConstIterator tmp(*this) ;
      return tmp -= offset ;
    };
    //
    friend ConstIterator 
    operator+ 
    ( const difference_type offset , 
      const ConstIterator&  right  ) { return right + offset ; } ;
    //
    friend difference_type
    operator-
    ( const ITERATOR&      right , 
      const ConstIterator& left  ) 
    { return std::distance ( right ,  left.m_it ) ; }
    //
    friend bool operator== 
    ( const ITERATOR&      right , 
      const ConstIterator& left  ) { return right == left ; }
    //
    friend bool operator!= 
    ( const ITERATOR&      right , 
      const ConstIterator& left  ) { return right != left ; }
    //
  private:
    // the underlying iterator  
    ITERATOR m_it ; ///< the underlying iterator  
  };
  // ==========================================================================  
}  // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CONSTITERATOR_H
// ============================================================================
