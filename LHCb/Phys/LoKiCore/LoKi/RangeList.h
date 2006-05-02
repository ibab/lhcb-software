// $Id: RangeList.h,v 1.4 2006-05-02 14:29:10 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_RANGELIST_H 
#define LOKI_RANGELIST_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/NamedRange.h"
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
  /** @class RangeList RangeList.h LoKi/RangeList.h
   *  
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-11-18
   */
  template<class RANGE>
  class RangeList_ 
  {
  public:
    typedef RANGE                                 Range    ;
    typedef std::vector<Range>                    Ranges   ;
    typedef typename Ranges::const_iterator       iterator ;
    typedef typename Ranges::const_iterator const_iterator ;
  protected:
    typedef RangeList_<RANGE>                      Self ;
  public:
    
    /** Standard constructor
     *  create empty list of ranges 
     */
    RangeList_() : m_ranges() {}
    
    /** constructor
     *  create list from 1 range
     *  @param range range 
     */
    RangeList_( const Range& range ) : m_ranges( 1 , range ) {}
    
    /** constructor
     *  create list from 2 ranges
     *  @param range1 range 
     *  @param range2 range 
     */
    RangeList_( const Range& range1 ,
                const Range& range2 ) : m_ranges() 
    {
      m_ranges.push_back( range1 ) ;
      m_ranges.push_back( range2 ) ;
    };
    
    /// destructor
    virtual ~RangeList_() ;
    
  public:
    
    /// empty list ? 
    bool          empty() const { return m_ranges.empty() ; }
    /// number of components 
    const size_t  size () const { return  m_ranges.size() ; }
    
    /// indexing operator 
    const Range& operator[] ( size_t index ) const 
    { return m_ranges[ index ] ; }    
    /// indexing operator 
    const Range& operator() ( size_t index ) const 
    { return  (*this)[ index ] ; }
    
    /// sequential access 
    iterator begin () const { return m_ranges.begin () ; }
    /// sequential access 
    iterator end   () const { return m_ranges.end   () ; }
    
    /// add 
    void add ( const Range&      range ) { m_ranges.push_back( range ) ; }
    /// add 
    void add ( const Ranges&     range ) 
    { for ( iterator it = range.begin() ; range.end() != it ; ++it ){ add( *it ) ; } }
    /// add 
    void add ( const Self&       range ) { add( range.ranges() ) ; }
    
    Self& operator+= ( const Range&  range ) { add ( range ) ; return *this ; }
    Self& operator+= ( const Ranges& range ) { add ( range ) ; return *this ; }
    Self& operator+= ( const Self&   range ) { add ( range ) ; return *this ; }
    
  public:
    
    // RangeList + <OTHER>
    template <class OTHER>
    Self operator+( const OTHER& range ) const 
    {
      Self tmp(*this);
      return tmp += range ;
    } ;
    
    // Range + RangeList
    friend Self operator+( const Range& range1 , 
                           const Self&  range2 )
    {
      return Self(range1)+=range2 ;
    } ;

  protected:
    
    /// access to the underlying storage 
    const Ranges& ranges() const { return m_ranges ; }
    
  private:
    
    // list of ranges itself 
    Ranges m_ranges ;
  };
  
  
}; // end of namespace LoKi 

// Range + Range
template <class RANGE> 
inline LoKi::RangeList_<RANGE> 
operator+( const typename LoKi::RangeList_<RANGE>::Range& range1 , 
           const typename LoKi::RangeList_<RANGE>::Range& range2 ) 
{
  return  LoKi::RangeList_<RANGE>( range1 , range2 ) ;
};

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_RANGELIST_H
// ============================================================================
