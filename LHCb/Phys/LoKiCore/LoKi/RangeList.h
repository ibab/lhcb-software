// $Id: RangeList.h,v 1.10 2010-04-06 20:06:39 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_RANGELIST_H 
#define LOKI_RANGELIST_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Range.h"
#include "GaudiKernel/NamedRange.h"
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
  // ==========================================================================
  /** @class RangeList LoKi/RangeList.h
   *  The list of ranges 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-11-18
   */
  template<class RANGE>
  class RangeList_ 
  {
  public:
    // ========================================================================
    typedef RANGE                                 Range    ;
    typedef std::vector<Range>                    Ranges   ;
    typedef typename Ranges::const_iterator       iterator ;
    typedef typename Ranges::const_iterator const_iterator ;
    // ========================================================================
  protected:
    // ========================================================================
    typedef RangeList_<RANGE>                      Self ;
    // ========================================================================
  public:
    // ========================================================================
    /** Standard constructor
     *  create empty list of ranges 
     */
    RangeList_() : m_ranges() {}
    /** constructor
     *  create list from 1 range
     *  @param range range 
     */
    explicit 
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
    virtual ~RangeList_(){} ;
    // ========================================================================
  public:
    // ========================================================================
    /// empty list ? 
    bool          empty() const { return m_ranges.empty() ; }
    /// number of components 
    size_t        size () const { return  m_ranges.size() ; }
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
    /// add 
    Self& operator+= ( const Range&  range ) { add ( range ) ; return *this ; }
    /// add 
    Self& operator+= ( const Ranges& range ) { add ( range ) ; return *this ; }
    /// add 
    Self& operator+= ( const Self&   range ) { add ( range ) ; return *this ; }
    // ========================================================================
  public:
  protected:
    // ========================================================================
    /// access to the underlying storage 
    const Ranges& ranges() const { return m_ranges ; }
    // ========================================================================
  private:
    // ========================================================================
    /// list of ranges itself 
    Ranges m_ranges ;     // list of ranges itself
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// RangeList + <OTHER>
// ============================================================================
template <class RANGE,class OTHER>
inline 
LoKi::RangeList_<RANGE> 
operator+( const LoKi::RangeList_<RANGE>& range1 ,
           const OTHER&                   range2 )
{
  return LoKi::RangeList_<RANGE>(range1) += range2 ;
} 
// ============================================================================
// Range + RangeList
// ============================================================================
template <class RANGE> 
inline 
LoKi::RangeList_<RANGE> 
operator+( const typename LoKi::RangeList_<RANGE>::Range& range1 , 
           const typename LoKi::RangeList_<RANGE>&        range2 ) 
{
  return LoKi::RangeList_<RANGE>( range1 )+= range2 ;
} 
// ============================================================================
// Range + Range
// ============================================================================
template <class RANGE> 
inline 
LoKi::RangeList_<RANGE> 
operator+( const typename LoKi::RangeList_<RANGE>::Range& range1 , 
           const typename LoKi::RangeList_<RANGE>::Range& range2 ) 
{
  return  LoKi::RangeList_<RANGE>( range1 , range2 ) ;
} 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_RANGELIST_H
// ============================================================================
