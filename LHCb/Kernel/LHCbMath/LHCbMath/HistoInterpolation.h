// $Id$
// ============================================================================
#ifndef LHCBMATH_HISTOINTERPOLATE_H 
#define LHCBMATH_HISTOINTERPOLATE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// LHCbMAth
// ============================================================================
#include "LHCbMath/ValueWithError.h"
// ============================================================================
// forward declarations 
// ============================================================================
class TH1 ; // from ROOT 
class TH2 ; // from ROOT 
class TH3 ; // from ROOT 
// ============================================================================
/** @file HistoInterpolation.h LHCbMath/HistoInterpolation.h
 *  Collection of primitive utilities for hiostorgam interpoaltion 
 *
 *  Originally developed for Ostap in python
 *  Translated to C++ to get some gain in CPU 
 *
 *  @author Vanya Belyaev
 *  @date   2015-10-12
 *
 *  Version           $Revision$
 *  Last mofidication $Date$
 *                 by $Author$
 */
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Math 
  {
    // ========================================================================
    /** @class HistoInterpolation
     *  Collection of primitive utilities for hiostorgam interpoaltion 
     *
     *  Originally developed for Ostap in python
     *  Translated to C++ to get some gain in CPU 
     *
     *  @author Vanya Belyaev
     *  @date   2015-10-12
     */
    class GAUDI_API HistoInterpolation 
    {
    public:
      // ======================================================================
      enum Type { Nearest   , 
                  Linear    , 
                  Quadratic } ;
      // ======================================================================
    public: // 1D interpolation 
      // ======================================================================
      /** linear interpolation between two points 
       *  @param x  (INPUT) the x-value
       *  @param x0 (INPUT) x-coordinate of the first  point 
       *  @param x1 (INPUT) x-coordinate of the second point 
       *  @param y0 (INPUT) y-coordinate of the first  point  \f$ y(x_0) \f$ 
       *  @param y1 (INPUT) y-coordinate of the second point  \f$ y(x_1) \f$  
       *  @return result of linear interpolation \f$ y(x) \f$
       */
      static Gaudi::Math::ValueWithError interpolate
        ( const double x  , 
          const double x0 , 
          const double x1 ,
          const Gaudi::Math::ValueWithError& y0 , 
          const Gaudi::Math::ValueWithError& y1 ) ;
      // ======================================================================
      /** quadratic (parabolic)  interpolation between three points 
       *  @param x  (INPUT) the x-value
       *  @param x0 (INPUT) x-coordinate of the first  point 
       *  @param x1 (INPUT) x-coordinate of the second point 
       *  @param x2 (INPUT) x-coordinate of the third  point 
       *  @param y0 (INPUT) y-coordinate of the first  point \f$ y(x_0) \f$  
       *  @param y1 (INPUT) y-coordinate of the second point \f$ y(x_1) \f$ 
       *  @param y2 (INPUT) x-coordinate of the third  point \f$ y(x_2) \f$ 
       *  @return result of  quadratic (parabolic) interpolation  \f$ y(x) \f$
       */
      static Gaudi::Math::ValueWithError interpolate
        ( const double                       x  , 
          const double                       x0 , 
          const double                       x1 ,
          const double                       x2 ,
          const Gaudi::Math::ValueWithError& y0 , 
          const Gaudi::Math::ValueWithError& y1 , 
          const Gaudi::Math::ValueWithError& y2 ) ;
      // ======================================================================
    public: // 2D interpolation 
      // ======================================================================
      /** bi-linear interpolation on grid 
       *  @param x   (INPUT) the x-value
       *  @param y   (INPUT) the x-value
       *  @param x0  (INPUT) the first  x-coordinate on the grid 
       *  @param x1  (INPUT) the second x-coordinate on the grid 
       *  @param y0  (INPUT) the first  y-coordinate on the grid 
       *  @param y1  (INPUT) the second y-coordinate on the grid 
       *  @param f00 (INPUT) function value for (x0,y0)
       *  @param f10 (INPUT) function value for (x1,y0)
       *  @param f01 (INPUT) function value for (x0,y1)
       *  @param f11 (INPUT) function value for (x1,y1)
       *  @return result of bi-linear interpolation
       */
      static Gaudi::Math::ValueWithError interpolate
        ( const double                       x   , 
          const double                       y   , 
          const double                       x0  , 
          const double                       x1  ,
          const double                       y0  , 
          const double                       y1  ,
          const Gaudi::Math::ValueWithError& f00 , 
          const Gaudi::Math::ValueWithError& f10 ,
          const Gaudi::Math::ValueWithError& f01 , 
          const Gaudi::Math::ValueWithError& f11 ) ;
      // ======================================================================
      /** bi-quadratic interpolation on grid 
       *  @param x    (INPUT) the x-value
       *  @param y    (INPUT) the x-value
       *  @param x0   (INPUT) the first  x-coordinate on the grid 
       *  @param x1   (INPUT) the second x-coordinate on the grid 
       *  @param x2   (INPUT) the third  x-coordinate on the grid 
       *  @param y0   (INPUT) the first  y-coordinate on the grid 
       *  @param y1   (INPUT) the second y-coordinate on the grid 
       *  @param y2   (INPUT) the third  y-coordinate on the grid 
       *  @param f00  (INPUT) function value for (x0,y0)
       *  @param f10  (INPUT) function value for (x1,y0)
       *  @param f20  (INPUT) function value for (x2,y0)
       *  @param f01  (INPUT) function value for (x0,y1)
       *  @param f11  (INPUT) function value for (x1,y1)
       *  @param f21  (INPUT) function value for (x2,y1)
       *  @param f02  (INPUT) function value for (x0,y2)
       *  @param f12  (INPUT) function value for (x1,y2)
       *  @param f22  (INPUT) function value for (x2,y2)
       *  @return result of bi-quadrate interpolation
       */
      static Gaudi::Math::ValueWithError interpolate
        ( const double                       x   , 
          const double                       y   , 
          const double                       x0  , 
          const double                       x1  ,
          const double                       x2  ,
          const double                       y0  , 
          const double                       y1  ,
          const double                       y2  ,
          const Gaudi::Math::ValueWithError& f00 , 
          const Gaudi::Math::ValueWithError& f10 ,
          const Gaudi::Math::ValueWithError& f20 ,
          const Gaudi::Math::ValueWithError& f01 , 
          const Gaudi::Math::ValueWithError& f11 ,
          const Gaudi::Math::ValueWithError& f21 ,
          const Gaudi::Math::ValueWithError& f02 , 
          const Gaudi::Math::ValueWithError& f12 ,
          const Gaudi::Math::ValueWithError& f22 ) ;
      // ======================================================================
    public:
      // ======================================================================
      /** interpolate 1D historgam 
       *  @param h1   (INPUT) input histogram 
       *  @param x    (INPUT) the x-value 
       *  @para  type (INPUT) interpolation type 
       *  @return value of interpolated function
       */
      static Gaudi::Math::ValueWithError interpolate_1D
        ( const TH1&   h1         , 
          const double x          ,
          const Type   t = Linear ) ;
      // ======================================================================
      /** interpolate 2D historgam 
       *  @param h2   (INPUT) input histogram 
       *  @param x    (INPUT) the x-value 
       *  @param y    (INPUT) the y-value 
       *  @para  tx   (INPUT) interpolation type in x-direction
       *  @para  ty   (INPUT) interpolation type in y-direction
       *  @return valeu of interpolated function
       */
      static Gaudi::Math::ValueWithError interpolate_2D 
        ( const TH2&   h1          , 
          const double x           ,
          const double y           ,
          const Type   tx = Linear , 
          const Type   ty = Linear ) ;
      // ======================================================================
      /** interpolate 3D historgam 
       *  @param h3   (INPUT) input histogram 
       *  @param x    (INPUT) the x-value 
       *  @param y    (INPUT) the y-value 
       *  @param z    (INPUT) the z-value 
       *  @para  tx   (INPUT) interpolation type in x-direction
       *  @para  ty   (INPUT) interpolation type in y-direction
       *  @para  tz   (INPUT) interpolation type in z-direction
       *  @return valeu of interpolated function
       */
      static Gaudi::Math::ValueWithError interpolate_3D 
        ( const TH3&   h3          , 
          const double x           ,
          const double y           ,
          const double z           ,
          Type         tx = Linear , 
          Type         ty = Linear ,
          Type         tz = Linear ) ;
      // ======================================================================
    } ;  
    // ========================================================================
  } //                                         The end of namespace Gaudi::Math
  // ==========================================================================
} //                                                 The end of namespace Gaudi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LHCBMATH_HISTOINTERPOLATE_H
// ============================================================================
