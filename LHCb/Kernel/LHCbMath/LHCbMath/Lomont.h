// $Id: Lomont.h,v 1.1 2008-11-08 15:03:38 ibelyaev Exp $
// ============================================================================
#ifndef LHCBMATH_LOMONT_H 
#define LHCBMATH_LOMONT_H 1
// ============================================================================
namespace LHCb
{
  namespace Math 
  {
    // ========================================================================
    /** equality comparison of float numbers using as the metric the maximal 
     *  number of Units in the Last Place (ULP).
     *  It is a slightly modified version of very efficient implementation 
     *  of the initial Bruce Dawson's algorithm by Chris Lomont.
     *
     *  @see www.lomont.org 
     *  @see http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
     *
     *  C.Lomont claims the algorithm is factor 2-10 more efficient 
     *  with respect to  Knuth's algorithm fomr comparions of floating number 
     *  using the relative precision.
     *
     *  The effective relative difference depends on the choice of 
     *   <c>maxULPS</c>:
     *  - For the case of maxULPs=1, (of cource it is totally unphysical case!!!)
     *  the effective relative precision r = |a-b|/(|a|+|b|)is 
     *  between 3.5e-8 and 5.5e-8 for |a|,|b|>1.e-37, and 
     *  then it quickly goes to ~1 
     *  - For the case of maxULPS=10 
     *  the effective relative precision is 
     *  between 3e-8 and 6e-7 for |a|,|b|>1.e-37, and 
     *  then it quickly goes to ~1 
     *  - For the case of maxULPS=100 
     *  the effective relative precision is 
     *  around ~6e-6 for |a|,|b|>1.e-37, and 
     *  then it quickly goes to ~1 
     *  - For the case of maxULPS=1000 
     *  the effective relative precision is 
     *  around ~6e-5 for |a|,|b|>1.e-37, and 
     *  then it quickly goes to ~1 
     *  
     *  @param  af the first number 
     *  @param  bf the second number 
     *  @param  maxULPS the maximal metric deciation in the terms of 
     *                 maximal number of units in the last place
     *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
     *  @date 2008-11-08
     */
    bool lomont_compare_float
    ( const float          af            , 
      const float          bf            , 
      const unsigned short maxULPs = 100 ) ;
    // ========================================================================
    /** equality comparison of float numbers using as the metric the maximal 
     *  number of Units in the Last Place (ULP).
     *  It is a slightly modified version of very efficient implementation 
     *  of the initial Bruce Dawson's algorithm by Chris Lomont.
     *
     *  @see www.lomont.org 
     *  @see http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
     *
     *  C.Lomont claims the algorithm is factor 2-10 more efficient 
     *  with respect to  Knuth's algorithm fomr comparions of floating number 
     *  using the relative precision.
     *
     *  The effective relative difference depends on the choice of 
     *   <c>maxULPS</c>:
     *  - For the case of maxULPs=1, (of cource it is totally unphysical case!!!)
     *  the effective relative precision r = |a-b|/(|a|+|b|)is 
     *  ~6e-16 for |a|,|b|>1.e-304, and 
     *  then it quickly goes to ~1 
     *  
     *  @param  af the first number 
     *  @param  bf the second number 
     *  @param  maxULPS the maximal metric deciation in the terms of 
     *                 maximal number of units in the last place
     *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
     *  @date 2008-11-08
     */
    bool lomont_compare_double
    ( const double         af      , 
      const double         bf      , 
      const unsigned int   maxULPs ) ;
    // ========================================================================
    /** Get the floating number that representation 
     *  is different with respect  to the argument for 
     *  the certain number of "Units in the Last Position".
     *  For ulps=1, it is just next float number, for ulps=-1 is is the 
     *  previous one.
     *
     *  This routine is very convinient to test the parameter maxULPS for
     *  the routine LHCb::Math::lomont_compare_float 
     *
     *  @see LHCb:Math::lomont_compare
     *  @param af the reference number 
     *  @param ulps the bias 
     *  @return the biased float number (on distance "ulps")
     *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
     *  @date 2008-11-08
     */  
    float next_float ( const float af , const short ulps ) ;
    // ========================================================================
    /** Get the floating number that representation 
     *  is different with respect  to the argument for 
     *  the certain number of "Units in the Last Position".
     *  For ulps=1, it is just next float number, for ulps=-1 is is the 
     *  previous one.
     *
     *  This routine is very convinient to test the parameter maxULPS for
     *  the routine LHCb::Math::lomont_compare_float 
     *
     *  @see LHCb:Math::lomont_compare
     *  @param af the reference number 
     *  @param ulps the bias 
     *  @return the biased float number (on distance "ulps")
     *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
     *  @date 2008-11-08
     */  
    double next_double ( const double  af , const short ulps ) ;
    // ========================================================================
  } // end of namespace LHCb::Math
  // ==========================================================================
} // end of namespace LHCb
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_LOMONT_H
// ============================================================================
