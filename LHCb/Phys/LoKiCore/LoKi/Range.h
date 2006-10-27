// $Id: Range.h,v 1.7 2006-10-27 13:34:18 ibelyaev Exp $
// ============================================================================
// $CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2006/10/10 09:03:21  ibelyaev
//  many tiny fixed needed for good&valid dictionaries
//
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

  /** @struct RangeBase_ Range.h LoKi/Range.h
   *  helper class to simplify the dealing with LoKi ranges in Python
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-09-01
   */
  struct RangeBase_ {} ;
  
  /** @class Range_ Range.h LoKi/Range.h
   *
   *  Useful class for representation of "sequence" of the objects
   *  through the range of valid iterators.
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-12
   */
  template <class CONTAINER>
  class Range_ : public RangeBase_ 
  {
  public:
    typedef std::pair<typename CONTAINER::const_iterator,
                      typename CONTAINER::const_iterator> Base ;
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
    Range_() : m_base( iterator() , iterator() ) {};
    /** Constructor 
     *  @param ibegin  iterator to begin of the sequence
     *  @param iend    iterator to end   of the sequence
     */
    Range_( iterator ibegin , iterator iend ) : m_base ( ibegin , iend ) {} ;
    /** constructor from the pair of iterators
     *  @param base pair of the iterators
     */
    Range_( const Base&      base ) : m_base( base ) {};
    /** constructor from the container
     *  @param cont  reference to the container
     */
    Range_( const Container& cont ) : m_base( cont.begin() , cont.end() ) {} ;
    /* constructor of empty range/sequence
     * @param ibegin  iterator to begin of empty sequence
     */
    Range_( iterator ibegin       ) : m_base( ibegin , ibegin ) {};
    /// destructor
    ~Range_(){};
    /// empty sequence ?
    inline bool   empty () const { return m_base.second == m_base.first  ; }
    /// size of the sequence (number of elements)
    inline size_t size  () const { return m_base.second  - m_base.first  ; }
    /// access to begin of the sequence (const version )
    inline iterator begin () const { return m_base.first  ; }
    /// access to end   of the sequence (const version)
    inline iterator end   () const { return m_base.second ; }
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
      // check 
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
      if ( index < 0 || index >= (long) size () )
      { rangeException( index , size() ) ; }
      return (*this) ( index );
    };
    /** non-checked access to the elements by index 
     *  (valid only for non-empty sequences)
     *  (for Python collection)
     *  @attention return method by 'value', not by 'reference'
     *  @param index the index of the lement to be accessed 
     */
    inline const_reference operator [] ( const long index ) const
    { return *( begin() + index ) ; }
  public:
    /// conversion operator to the std::pair 
    operator const Base&      () const { return base () ; }
    /// conversion operator to the std::pair 
    inline   const Base& base () const { return m_base ; }    
  private:
    // the base itself 
    Base m_base ;    ///<the base itself 
  }; // end of class Range_
  
}; // end of namespace LoKi

// ============================================================================
// The END
// ============================================================================
#endif // LOOK_RANGE_H
// ============================================================================
