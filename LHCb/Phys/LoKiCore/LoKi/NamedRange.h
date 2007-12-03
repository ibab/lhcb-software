// $Id: NamedRange.h,v 1.8 2007-12-03 12:03:22 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_NAMEDRANGE_H 
#define LOKI_NAMEDRANGE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Range.h"
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
  /** @class NamedRange NamedRange.h LoKi/NamedRange.h
   *
   *  New concept of "named" range : range with name 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-11-19
   */
  template <class TYPE>
  class NamedRange_ : public LoKi::Range_<TYPE> 
  {
  public:
    typedef LoKi::Range_<TYPE>  Base ;
  protected:
    typedef NamedRange_<TYPE>   Self ;
  public:
    /// default constructor
    NamedRange_() : Base() , m_name() {};
    /** Constructor 
     *  @param ibegin  iterator to begin of the sequence
     *  @param iend    iterator to end   of the sequence
     *  @param name    name of the range 
     */
    NamedRange_( typename Base::iterator ibegin    , 
                 typename Base::iterator iend      , 
                 const std::string&      name = "" ) 
      : Base   ( ibegin , iend ) , m_name ( name ) {} ;
    /** constructor from the base class 
     *  @param base base objects 
     *  @param name name of the range 
     */
    NamedRange_( const Base&        base      , 
                 const std::string& name = "" ) 
      : Base   ( base ) , m_name ( name ) {};
    /** constructor from the base class 
     *  @param base base objects 
     *  @param name name of the range 
     */
    NamedRange_( const typename Base::_Base& base      , 
                 const std::string&          name = "" ) 
      : Base   ( base ) , m_name ( name ) {};
    /** constructor from the base class 
     *  @param base base objects 
     *  @param name name of the range 
     */
    NamedRange_( const typename Base::Container& base      , 
                 const std::string&              name = "" ) 
      : Base   ( base ) , m_name ( name ) {};
    /* constructor of empty range/sequence
     * @param ibegin  iterator to begin of empty sequence
     *  @param name name of the range 
     */
    NamedRange_( typename Base::iterator ibegin      , 
                 const std::string&      name   = "" ) 
      : Base   ( ibegin , ibegin ) , m_name ( name ) {};
    /// destructor
    ~NamedRange_(){};
  public:
    /// get a "slice" of a range, in Python style   
    inline NamedRange_ slice( long index1 , long index2 ) const 
    {  return NamedRange_( Base::slice ( index1 , index2 ) , m_name ) ; }
  public:
    /// get the name of the range 
    const std::string& name() const { return m_name ; }
    /// set the name of the range 
    void  setName( const std::string& value ) { m_name = value ; }
  private:    
    // the name of the range 
    std::string m_name ; ///< the name of the range
  };
  // ==========================================================================
  /** simple function to create the named range form arbitrary container 
   *  
   *  @code
   *
   *    const CONTAINER& cnt = ... ;
   *   
   *    NamedRange_<CONTAINER> r = range ( cnt , "some name") ;
   *  
   *  @endcode 
   *  
   *  @author Vanya BELYAEV ibelyaev@physics.syre.edu
   *  @date 2007-11-29
   */
  template <class CONTAINER>
  inline 
  NamedRange_<CONTAINER> 
  range ( const CONTAINER&   cnt  , 
          const std::string& name ) 
  { return NamedRange_<CONTAINER>( cnt.begin() , cnt.end() , name ) ; }
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_NAMEDRANGE_H
// ============================================================================
