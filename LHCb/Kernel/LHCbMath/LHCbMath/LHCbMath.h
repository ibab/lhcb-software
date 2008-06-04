
//---------------------------------------------------------------------------------
/** @file
 *
 *  Collection of math related functions for general use in LHCb
 *
 *  CVS Log :-
 *  $Id: LHCbMath.h,v 1.4 2008-06-04 09:51:35 mneedham Exp $
 *
 *  @author Juan PALACIOS
 *  @date   2005-11-21
 */
//---------------------------------------------------------------------------------

#ifndef LHCBMATH_LHCBMATH_H 
#define LHCBMATH_LHCBMATH_H 1

// Include files

#include <cmath>
#include <algorithm>
#include <functional>
#include "boost/call_traits.hpp"

// ============================================================================

/** @namespace LHCb::Math
 * Collection of math related functions for general use in LHCb
 */
namespace LHCb {
  namespace Math 
  {
    /// Parameters for numerical calculations (M.Needham)
    static const double hiTolerance    = 1e-40;
    static const double lowTolerance   = 1e-20;
    static const double looseTolerance = 1e-5;
    static const double     sqrt_12 = 3.4641016151377546; // sqrt(12.)
    static const double inv_sqrt_12 = 0.2886751345948129; // 1./sqrt(12.)
    //--------------------------------------------------------------------------
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
      double m_eps ;
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
      long double m_eps ;
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
      float m_eps ;
    } ;

   // ========================================================================
    /** Round to nearest integer. Rounds half integers to nearest even integer.
     *  @author Matt Needham 
     */
    inline int round(const double x)  {
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

    inline int round(const float x)  {
      return LHCb::Math::round( double(x) );
    }
  }
};

#endif // LHCBMATH_LHCBMATH_H
