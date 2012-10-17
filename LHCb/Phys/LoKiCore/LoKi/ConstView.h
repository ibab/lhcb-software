// ============================================================================
#ifndef LOKI_CONSTVIEW_H 
#define LOKI_CONSTVIEW_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <utility>
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/ConstIterator.h"
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
  /** @class ConstView ConstView.h LoKi/ConstView.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu 
   *  @date   2006-03-29
   */
  template <class CONTAINER>
  struct ConstView : public std::pair
  <
    typename LoKi::ConstIterator<typename CONTAINER::const_iterator> ,
    typename LoKi::ConstIterator<typename CONTAINER::const_iterator>
  >
  {
  public:
    typedef LoKi::ConstIterator<typename CONTAINER::const_iterator> const_iterator ;
    typedef const_iterator                                          iterator ;
    typedef typename iterator::value_type                         value_type ;
    typedef typename iterator::reference                     const_reference ;
    typedef const_reference                                        reference ;
    typedef typename iterator::pointer                               pointer ;
    typedef typename CONTAINER::size_type                          size_type ;
  protected:
    typedef std::pair<iterator,iterator> _Base ;
  public: 
    /// default constructor constructor
    ConstView () : _Base(){} ; 
    /// constructor form iterators 
    template <class IT>
    ConstView ( IT first , IT last ) : _Base( first , last ) {}
    /// constructor from the "base"
    template <class IT>
    ConstView ( const std::pair<IT,IT>& base ) : _Base  ( base ) {}
    /// constructor form the container 
    ConstView ( const CONTAINER& c ) : _Base( c.begin() , c.end() ) {}
    /// compiler generated copy , assigmenet and destructor are OK
  public:
    /// begin-iterator for the sequence 
    const iterator& begin () const { return _Base::first  ; }
    /// end-iterator for the sequence 
    const iterator& end   () const { return _Base::second ; }
    /// the first eqlement : undefined for empty ranges 
    const_reference front () const { return  *begin() ; } 
    /// the last element : undefined for empty ranges 
    const_reference back  () const { return  *(begin()+(size()-1)) ; }
    /// empty sequence ? 
    bool empty () const { return _Base::first == _Base::second  ; }
    /// number of elements in the sequence 
    size_type size  () const { return _Base::second - _Base::first   ; }
    /// get the element at the given index 
    const_reference operator() ( const size_type index ) const 
    { return *(begin() + index); }
    /// get the element at the given index 
    const_reference operator[] ( const size_type index ) const
    { return (*this)( index ) ; }
    /// get the element at the given index 
    const_reference at         ( const size_type index ) const
    { return (*this)( index ) ; }
    /// get the slice : undefined for invalid indices 
    ConstView slice ( size_type i1 , size_type i2 ) const 
    { return ConstView( begin() + i1 , begin() + i2 ) ; }
  };
  // ==========================================================================  
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CONSTVIEW_H
// ============================================================================
