// ----------------------------------------------------------------------------
/** @file
 *
 *  Collection of math related functions for general use in LHCb
 *
 *  CVS Log :-
 *  $Id: LHCbMath.h,v 1.5 2008-09-17 13:45:53 ibelyaev Exp $
 *
 *  @author Juan PALACIOS
 *  @date   2005-11-21
 */
// ----------------------------------------------------------------------------
#ifndef LHCBMATH_LHCBMATH_H 
#define LHCBMATH_LHCBMATH_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <cmath>
#include <algorithm>
#include <functional>
// ============================================================================
// Boost 
// ============================================================================
#include "boost/call_traits.hpp"
#include "boost/integer_traits.hpp"
#include "boost/static_assert.hpp"
// ============================================================================
namespace LHCb 
{
  // ==========================================================================
  /** @namespace LHCb::Math
   *  Collection of math related functions for general use in LHCb
   */
  namespace Math 
  {
    /// Parameters for numerical calculations (M.Needham)
    static const double hiTolerance    = 1e-40;
    static const double lowTolerance   = 1e-20;
    static const double looseTolerance = 1e-5;
    static const double     sqrt_12 = 3.4641016151377546; // sqrt(12.)
    static const double inv_sqrt_12 = 0.2886751345948129; // 1./sqrt(12.)
    // ========================================================================
    /** @struct abs_less 
     *  comparison by absolute value 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */
    template <class TYPE>
    struct abs_less : std::binary_function<TYPE,TYPE,TYPE>
    {
      inline TYPE operator() 
        ( typename boost::call_traits<const TYPE>::param_type v1 ,
          typename boost::call_traits<const TYPE>::param_type v2 ) const 
      {
        return m_eval ( std::fabs( v1 ) , std::fabs( v2 ) ) ; 
      }
      /// evaluator: 
      std::less<TYPE> m_eval ;
    } ;
    // ========================================================================
    /** @struct abs_greater
     *  comparison by absolute value 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */
    template <class TYPE>
    struct abs_greater : std::binary_function<TYPE,TYPE,TYPE>
    {
      inline TYPE operator() 
      ( typename boost::call_traits<const TYPE>::param_type v1 ,
        typename boost::call_traits<const TYPE>::param_type v2 ) const 
      {
        return m_eval ( std::fabs( v1 ) , std::fabs( v2 ) ) ; 
      }
      /// evaluator: 
      std::greater<TYPE> m_eval ;
    } ;
    // ========================================================================
    /** return "min_by_abs"
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */        
    template <class TYPE> 
    inline TYPE absMin ( TYPE v1 , TYPE v2 ) 
    { return std::min ( std::fabs ( v1 ) , std::fabs ( v2 ) ) ; }
    // ========================================================================
    /** return  "max_by_abs"
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-17
     */
    template <class TYPE> 
    inline TYPE absMax ( TYPE v1 , TYPE v2 ) 
    { return std::max ( std::fabs ( v1 ) , std::fabs ( v2 ) ) ; }

    // ========================================================================
    /** compare two double numbers with relative precision 'epsilon'
     *
     *  Essentially it is a wrapper to gsl_fcmp function from GSL library
     *  See D.E.Knuth, "Seminumerical Algorithms", section 4.2.2
     *
     *  @todo one needs to use the "fast" IEEE-based algorithm
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-27
     */
    bool equal_to_double
    ( const double value1           ,
      const double value2           ,
      const double epsilon = 1.0e-6 ) ;
    // ========================================================================
    /** @struct Equal_To
     *  helper structure for comparison of floating values
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-11-27
     */
    template <class TYPE>
    struct Equal_To : public std::binary_function<TYPE,TYPE,bool>
    {
      typedef typename boost::call_traits<const TYPE>::param_type T ;
      /// constructor
      Equal_To ( T /* v */ ) {}
      /// comparison
      inline bool operator() ( T v1 , T v2 ) const
      {
        std::equal_to<TYPE> cmp ;
        return cmp ( v1 , v2 ) ;
      }
    } ;
    // ========================================================================
    /// partial specialization for const-types
    template <class TYPE>
    struct Equal_To<const TYPE>: public Equal_To<TYPE>{} ;
    // ========================================================================
    /// partial specialization for references
    template <class TYPE>
    struct Equal_To<TYPE&>: public Equal_To<TYPE>{} ;
    // ========================================================================
    /// explicit specialization for doubles
    template <>
    struct Equal_To<double>
    {
      /// constructor
      Equal_To ( const double eps = 1.0e-6 ) : m_eps ( eps ) {}
      /// comparison:
      inline bool operator() ( const double v1 , const double v2 ) const
      {
        return equal_to_double ( v1 , v2 , m_eps ) ;
      }
    private :
      /// the precision 
      double m_eps ;  // the precision 
    };
    // ========================================================================
    /// explicit specialization for long doubles
    template <>
    struct Equal_To<long double>
    {
      /// constructor
      Equal_To ( const long double eps = 1.0e-6 ) : m_eps ( eps ) {}
      /// comparison:
      inline bool operator() ( const long double v1 ,
                               const long double v2 ) const
      {
        return equal_to_double
          ( static_cast<double> ( v1    ) ,
            static_cast<double> ( v2    ) ,
            static_cast<double> ( m_eps ) ) ;
      }
    private :
      /// the precision 
      long double m_eps ; // the precision 
    };
    // ========================================================================
    /// explicit specialization for floats
    template <>
    struct Equal_To<float>
    {
      /// constructor
      Equal_To ( const float eps = 1.0e-6 ) : m_eps ( eps ) {}
      /// comparison:
      inline bool operator() ( const float v1 , const float v2 ) const
      {
        return equal_to_double ( v1 , v2 , m_eps ) ;
      }
    private :
      /// the precision 
      float m_eps ; // the precision 
    } ;
    // ========================================================================
    /** Round to nearest integer. Rounds half integers to nearest even integer.
     *  @author Matt Needham 
     */
    inline int round ( const double x )  {
      int i;
      LHCb::Math::Equal_To<double> equal_to(lowTolerance);
      if (x >= 0.0) {
        i = int(x + 0.5);
        if (equal_to(x + 0.5, double(i)) && i & 1) --i;
      }
      else {
        i = int(x - 0.5);
        if (equal_to(x - 0.5 , double(i)) && i & 1) ++i;
        
      }
      return i;
    }
    // ======================================================================== 
    /** Round to nearest integer. Rounds half integers to nearest even integer.
     *  @author Matt Needham 
     */
    inline int round ( const float x )  
    {
      return LHCb::Math::round ( double ( x ) ) ;
    }
    // ========================================================================
    /** check if the double value is actually equal to the integer value  
     *  @param val value to be compared with the integer 
     *  @param ref the reference integer number 
     *  @param eps1 the absolute precision
     *  @param eps2 the relative precision
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-17
     */        
    inline bool equal_to_int 
    ( const double val          , 
      const int    ref          , 
      const double eps1 = 1.e-6 , 
      const double eps2 = 1.e-3 /
      ( boost::integer_traits<int>::const_max - 
        boost::integer_traits<int>::const_min ) )
    {
      BOOST_STATIC_ASSERT(boost::integer_traits<int>::is_specialized ) ;
      // 1) try the straightforward comparison 
      if ( val == ref                 ) { return true ; } // RETURN 
      // 2) check the limits 
      if ( val > boost::integer_traits<int>::const_max || 
           val < boost::integer_traits<int>::const_min ) { return false ; }
      // 3) compare the doubles 
      if ( std::fabs ( val - ref ) <= eps1 ) { return true  ; }
      // 3') compare the doubles  
      LHCb::Math::Equal_To<double> cmp ( eps2 ) ;
      return cmp ( val , ref ) ;
    }
    // ========================================================================
    /** check if the double value is actually equal to the integer value  
     *  @param ref the reference integer  number 
     *  @param val value to be compared with the integer 
     *  @param eps1 the absolute precision
     *  @param eps2 the relative precision
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-17
     */        
    inline bool equal_to_int 
    ( const int    ref          , 
      const double val          , 
      const double eps1 = 1.e-6 , 
      const double eps2 = 1.e-3 /
      ( boost::integer_traits<int>::const_max - 
        boost::integer_traits<int>::const_min ) )
    { 
      BOOST_STATIC_ASSERT(boost::integer_traits<int>::is_specialized ) ;
      return equal_to_int ( val , ref , eps1 , eps2 ) ; 
    }
    // ========================================================================
    /** check if the double value is actually equal to the unsigned integer value  
     *  @param val value to be compared with the unsigned integer 
     *  @param ref the reference unsigned integer number 
     *  @param eps1 the absolute precision
     *  @param eps2 the relative precision
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-17
     */        
    inline bool equal_to_uint 
    ( const double       val         , 
      const unsigned int ref         , 
      const double eps1 = 1.e-6      , 
      const double eps2 = 1.e-3 /
      ( boost::integer_traits<unsigned int>::const_max ) )
    {
      BOOST_STATIC_ASSERT(boost::integer_traits<unsigned int>::is_specialized ) ;
      // 1) straightforward comparison 
      if ( val == ref ) { return true ; }                             // RETURN 
      // check the limits 
      if ( val > boost::integer_traits<unsigned int>::const_max || 
           val < boost::integer_traits<unsigned int>::const_min ) { return false ; }
      // 3) compare the doubles 
      if ( std::fabs ( val - ref ) <= eps1 ) { return true  ; }
      // 3') compare the doubles  
      LHCb::Math::Equal_To<double> cmp ( eps2 ) ;
      return cmp ( val , ref ) ;
    }
    // ========================================================================
    /** check if the double value is actually equal to the integer value  
     *  @param val value to be compared with the unsigned integer 
     *  @param ref the reference unsigned integer number 
     *  @param eps the absolute precision 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-09-17
     */        
    inline bool equal_to_uint 
    ( const unsigned int ref          , 
      const double       val          ,
      const double       eps1 = 1.e-6 , 
      const double       eps2 = 1.e-3 /
      ( boost::integer_traits<unsigned int>::const_max ) )
    { return equal_to_uint ( val , ref , eps1 , eps2 ) ; }
    // ========================================================================
  } // end of namespace LHcb::Math 
  // ==========================================================================
} // end of namespace LHCb 
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_LHCBMATH_H
// ============================================================================
