// $Id: EqualTo.h,v 1.1 2007-11-28 13:56:32 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_EQUALTO_H 
#define LOKI_EQUALTO_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
// ============================================================================
// Boost 
// ============================================================================
#include "boost/call_traits.hpp"
// ============================================================================
namespace LoKi 
{
  namespace StdMath
  {
    // ========================================================================
    /** compare two double numbers with the relative
     *  precision   'epsilon'
     *
     *  Essentially it is a wrapper to gsl_fcmp function 
     *  from GSL library
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
     *  helper strcutire for comparison of floating values 
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
    /// the explicit speciazation for doubles 
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
    /// the explicit speciazation for long doubles 
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
    /// the explicit speciazation for floats 
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
    
  } // end of namespace LoKi::StdMath 
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_EQUALTO_H
// ============================================================================
