// $Id: Range.h,v 1.4 2006-05-02 14:29:10 ibelyaev Exp $
// ============================================================================
// $CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_RANGE_H
#define LOKI_RANGE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <utility>
#include <vector>
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

namespace LoKi
{  
  namespace Range_Local 
  {
    /** @fn rangeException
     *  Helpful function to throw an out-of-range exception for class Range_
     *  @param index invalid index
     *  @param size  range size 
     */
    void rangeException 
    ( const long   index , 
      const size_t size  ) ;
  };
  
  /** @class Range_ Range.h LoKi/Range.h
   *
   *  Useful class for representation of "sequence" of the objects
   *  through the range of valid iterators.
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-12
   */
  template <class CONTAINER>
  class Range_ : 
    public std::pair<typename CONTAINER::const_iterator,
                     typename CONTAINER::const_iterator> 
  {
  public:
    /// type for actual contained iterator
    typedef CONTAINER                                  Container              ;
    typedef typename Container::value_type             value_type             ;
    typedef typename Container::const_iterator         iterator               ;
    typedef typename Container::const_iterator         const_iterator         ;
    typedef typename Container::const_reverse_iterator reverse_iterator       ;
    typedef typename Container::const_reverse_iterator const_reverse_iterator ;
    typedef typename Container::value_type             value_type             ;
    typedef typename Container::const_reference        reference              ;
    typedef typename Container::const_reference        const_reference        ;
    /// internal types 
    typedef std::pair<iterator,iterator>               _Base                  ;
    typedef Range_<Container>                          _Self                  ;
  public:
    
    /// default constructor
    Range_() : _Base( iterator() , iterator() ) {};
    
    /** Constructor 
     *  @param ibegin  iterator to begin of the sequence
     *  @param iend    iterator to end   of the sequence
     */
    Range_( iterator ibegin , iterator iend ) : _Base ( ibegin , iend ) {} ;
    
    /** constructor from the pair of iterators
     *  @param base pair of the iterators
     */
    Range_( const _Base&      base ) : _Base( base ) {};
    
    /** constructor from the container
     *  @param cont  reference to the container
     */
    Range_( const Container& cont ) : _Base( cont.begin() , cont.end() ) {} ;
    
    /* constructor of empty range/sequence
     * @param ibegin  iterator to begin of empty sequence
     */
    Range_( iterator ibegin       ) : _Base( ibegin , ibegin ) {};
    
    /// destructor
    ~Range_(){};
    
    /// empty sequence ?
    inline bool   empty () const { return _Base::second == _Base::first  ; }
    /// size of the sequence (number of elements)
    inline size_t size  () const { return _Base::second  - _Base::first  ; }
    
    /// access to begin of the sequence (const version )
    inline iterator begin () const { return _Base::first  ; }
    /// access to end   of the sequence (const version)
    inline iterator end   () const { return _Base::second ; }
    
    /// access to begin of the reversed sequence (const)
    inline reverse_iterator rbegin   () const { return end   () ; }
    /// access to begin of the reversed sequence (const)
    inline reverse_iterator rend     () const { return begin () ; }
    
    /// access for the first element (only for non-empty ranges!)
    inline const_reference front () const { return *( begin ()     ) ; }
    /// access for the back  element (only for non-empty ranges!)
    inline const_reference back  () const { return *( end   () - 1 ) ; }
    
    /// get a "slice" of a range, in Python style   
    inline Range_ slice( long index1 , long index2 ) const 
    {
      // trivial cases 
      if ( empty() || index1 == index2 ) { return Range_() ; } // RETURN
      // adjust indices 
      if ( index1 < 0      ) { index1 += size () ; }
      if ( index2 < 0      ) { index2 += size () ; }
      if ( index1 < 0      ) { return  Range_ () ; }            // RETURN 
      if ( index2 < index1 ) { return  Range_ () ; }            // RETURN 
      
      if ( index1 > (long) size () ) { return  Range_() ; }     // RETURN
      if ( index2 > (long) size () ) { index2  = size() ; }
      
      return Range_( begin() + index1 , begin() + index2 ) ;    // RETURN 
    };
    
    /** non-checked access to the elements by index 
     *  (valid only for non-empty sequences)
     *  @param index the index of the lement to be accessed 
     */
    inline const_reference operator () ( const size_t index ) const
    { return *( begin() + index ) ; }
    
    /** Checked access to the elements by index 
     *  (valid only for all sequences)
     *  @exception LoKi::Exception for out-of-range access  
     *  @param index the index of the element to be accessed 
     */
    inline const_reference at          ( const long index ) const 
    {
      using namespace LoKi::Range_Local ;
      if( index < 0 || index >= (long) size () )
      { rangeException( index , size() ) ; }
      return (*this) ( index );
    };    
    
    /** non-checked access to the elements by index 
     *  (valid only for non-empty sequences)
     *  (for Python collection)
     *  @attention return method by 'value', not by 'reference'
     *  @param index the index of the lement to be accessed 
     */
    inline value_type operator [] ( const long index ) const
    { return *( begin() + index ) ; }
    
  }; // end of class Range_
  
}; // end of namespace LoKi

// ============================================================================
// The END
// ============================================================================
#endif // LOOK_RANGE_H
// ============================================================================
