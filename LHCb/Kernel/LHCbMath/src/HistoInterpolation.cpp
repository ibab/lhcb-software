// $Id$ 
// ============================================================================
// Include files 
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
#include "LHCbMath/LHCbMath.h"
#include "LHCbMath/ValueWithError.h"
// ============================================================================
// Local
// ============================================================================
#include "LHCbMath/HistoInterpolation.h"
// ============================================================================
// ROOT 
// ============================================================================
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TAxis.h"
// ============================================================================
/** @file 
 *  Implementation file for class : Gaudi::Math::HistoInterpolation
 *  @see  Gaudi::Math::HistoInterpolation
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
namespace 
{
  // ==========================================================================
  /// equality criteria for doubles
  const LHCb::Math::Equal_To<double> s_equal ; // equality criteria for doubles
  /// zero for doubles  
  const LHCb::Math::Zero<double>     s_zero  ; // zero for doubles
  // ==========================================================================
  // get bin content for 1D-histogram 
  inline Gaudi::Math::ValueWithError _bin_ 
  ( const TH1&         h1 , 
    const unsigned int i  ) 
  {    
    const double v = h1.GetBinContent   ( i ) ;
    const double e = h1.GetBinError     ( i ) ;
    return  Gaudi::Math::ValueWithError ( v , e * e ) ;
  }
  // get bin content for 2D-histogram 
  inline Gaudi::Math::ValueWithError _bin_ 
  ( const TH2&         h2 , 
    const unsigned int ix , 
    const unsigned int iy )
  {    
    const double v = h2.GetBinContent   ( ix , iy    ) ;
    const double e = h2.GetBinError     ( ix , iy    ) ;
    return  Gaudi::Math::ValueWithError ( v  , e * e ) ;
  }
  // get bin content for 3D-histogram 
  inline Gaudi::Math::ValueWithError _bin_ 
  ( const TH3&         h3 , 
    const unsigned int ix , 
    const unsigned int iy ,
    const unsigned int iz )
  {    
    const double v = h3.GetBinContent   ( ix , iy , iz ) ;
    const double e = h3.GetBinError     ( ix , iy , iz ) ;
    return  Gaudi::Math::ValueWithError ( v  , e * e ) ;
  }
  // ==========================================================================
  // linear interpolation 
  inline Gaudi::Math::ValueWithError _linear_ 
  ( const double                       x  , 
    const double                       x1 , 
    const double                       x2 , 
    const Gaudi::Math::ValueWithError& v1 , 
    const Gaudi::Math::ValueWithError& v2 )
  {
    const double dx = 1 / ( x1 - x2 )      ;
    const double c1 =     ( x1 - x  ) * dx ;
    const double c2 =     ( x  - x2 ) * dx ;
    //
    const double vv = v2.value() * c1      + v1.value() * c2      ;
    const double e2 = v2.cov2 () * c1 * c1 + v1.cov2 () * c2 * c2 ;
    //
    return Gaudi::Math::ValueWithError ( vv , e2 ) ;
  }
  // ==========================================================================
  // quadratic interpolation 
  inline 
  Gaudi::Math::ValueWithError _quadratic_ 
  ( const double                       x  , 
    const double                       x0 , 
    const double                       x1 , 
    const double                       x2 , 
    const Gaudi::Math::ValueWithError& v0 , 
    const Gaudi::Math::ValueWithError& v1 , 
    const Gaudi::Math::ValueWithError& v2 )
  {
    const double dx0  = x - x0 ;
    const double dx1  = x - x1 ;
    const double dx2  = x - x2 ;
    //
    const double dx01 = x0 - x1 ;
    const double dx02 = x0 - x2 ;
    const double dx12 = x1 - x2 ;
    //
    const double c0   = dx1 * dx2 / ( dx01 * dx02 ) ;
    const double c1   = dx0 * dx2 / ( dx01 * dx12 ) ; 
    const double c2   = dx0 * dx1 / ( dx02 * dx12 ) ; 
    //
    const double vv = v0.value() * c0 + v2.value() * c2 - v1.value() * c1 ;
    //
    const double e2 = 
      v0.cov2 () * c0 * c0 +
      v1.cov2 () * c1 * c1 +
      v2.cov2 () * c2 * c2 ;
    //
    return Gaudi::Math::ValueWithError ( vv , e2 ) ; 
  }
  // ==========================================================================
  // bilinear interpolation 
  inline Gaudi::Math::ValueWithError _bilinear_ 
  ( const double                       x   , 
    const double                       y   , 
    const double                       x0  , 
    const double                       x1  , 
    const double                       y0  , 
    const double                       y1  , 
    const Gaudi::Math::ValueWithError& v00 , 
    const Gaudi::Math::ValueWithError& v10 , 
    const Gaudi::Math::ValueWithError& v01 ,
    const Gaudi::Math::ValueWithError& v11 )
  {
    return _linear_ 
      ( x  , 
        x0 , 
        x1 ,
        _linear_ ( y , y0 , y1 , v00 , v01 ) , 
        _linear_ ( y , y0 , y1 , v10 , v11 ) ) ;
  }
  // ==========================================================================
  // biquadratic interpolation 
  inline 
  Gaudi::Math::ValueWithError _biquadratic_ 
  ( const double                       x   , 
    const double                       y   , 
    const double                       x0  , 
    const double                       x1  , 
    const double                       x2  , 
    const double                       y0  , 
    const double                       y1  , 
    const double                       y2  , 
    //
    const Gaudi::Math::ValueWithError& v00 , 
    const Gaudi::Math::ValueWithError& v10 , 
    const Gaudi::Math::ValueWithError& v20 , 
    //
    const Gaudi::Math::ValueWithError& v01 , 
    const Gaudi::Math::ValueWithError& v11 , 
    const Gaudi::Math::ValueWithError& v21 , 
    //
    const Gaudi::Math::ValueWithError& v02 , 
    const Gaudi::Math::ValueWithError& v12 , 
    const Gaudi::Math::ValueWithError& v22 )
  {
    return _quadratic_ 
      ( x , x0 , x1 , x2 , 
        _quadratic_ ( y , y0 , y1 , y2 , v00 , v01 , v02 ) , 
        _quadratic_ ( y , y0 , y1 , y2 , v10 , v11 , v12 ) , 
        _quadratic_ ( y , y0 , y1 , y2 , v20 , v21 , v22 ) ) ;
  }
  // ==========================================================================
  // trilinear interpolation 
  inline Gaudi::Math::ValueWithError _trilinear_ 
  ( const double                       x    , 
    const double                       y    , 
    const double                       z    , 
    const double                       x0   , 
    const double                       x1   , 
    const double                       y0   , 
    const double                       y1   , 
    const double                       z0   , 
    const double                       z1   , 
    //
    const Gaudi::Math::ValueWithError& v000 , 
    const Gaudi::Math::ValueWithError& v100 , 
    //
    const Gaudi::Math::ValueWithError& v010 ,
    const Gaudi::Math::ValueWithError& v110 ,
    //
    const Gaudi::Math::ValueWithError& v001 , 
    const Gaudi::Math::ValueWithError& v101 , 
    //
    const Gaudi::Math::ValueWithError& v011 ,
    const Gaudi::Math::ValueWithError& v111 )
  {
    return _bilinear_ ( x  , y  , 
                        x0 , x1 , 
                        y0 , y1 ,
                        _linear_ ( z , z0 , z1 , v000 , v001 ) , 
                        _linear_ ( z , z0 , z1 , v100 , v101 ) , 
                        _linear_ ( z , z0 , z1 , v010 , v011 ) , 
                        _linear_ ( z , z0 , z1 , v110 , v111 ) ) ;
  }
  // ==========================================================================
  // triquadratic interpolation 
  inline Gaudi::Math::ValueWithError _triquadratic_ 
  ( const double                       x    , 
    const double                       y    , 
    const double                       z    , 
    const double                       x0   , 
    const double                       x1   , 
    const double                       x2   , 
    const double                       y0   , 
    const double                       y1   , 
    const double                       y2   , 
    const double                       z0   , 
    const double                       z1   , 
    const double                       z2   , 
    const Gaudi::Math::ValueWithError& v000 , 
    const Gaudi::Math::ValueWithError& v100 , 
    const Gaudi::Math::ValueWithError& v200 , 
    const Gaudi::Math::ValueWithError& v010 , 
    const Gaudi::Math::ValueWithError& v110 , 
    const Gaudi::Math::ValueWithError& v210 , 
    const Gaudi::Math::ValueWithError& v020 , 
    const Gaudi::Math::ValueWithError& v120 , 
    const Gaudi::Math::ValueWithError& v220 , 
    //
    const Gaudi::Math::ValueWithError& v001 , 
    const Gaudi::Math::ValueWithError& v101 , 
    const Gaudi::Math::ValueWithError& v201 , 
    const Gaudi::Math::ValueWithError& v011 , 
    const Gaudi::Math::ValueWithError& v111 , 
    const Gaudi::Math::ValueWithError& v211 , 
    const Gaudi::Math::ValueWithError& v021 , 
    const Gaudi::Math::ValueWithError& v121 , 
    const Gaudi::Math::ValueWithError& v221 ,
    //
    const Gaudi::Math::ValueWithError& v002 , 
    const Gaudi::Math::ValueWithError& v102 , 
    const Gaudi::Math::ValueWithError& v202 , 
    const Gaudi::Math::ValueWithError& v012 , 
    const Gaudi::Math::ValueWithError& v112 , 
    const Gaudi::Math::ValueWithError& v212 , 
    const Gaudi::Math::ValueWithError& v022 , 
    const Gaudi::Math::ValueWithError& v122 , 
    const Gaudi::Math::ValueWithError& v222 ) 
  {
    return _quadratic_ 
      ( z , z0 , z1 , z2 , 
        _biquadratic_ ( x    , y           ,  
                        x0   , x1   , x2   , 
                        y0   , y1   , y2   , 
                        v000 , v100 , v200 , 
                        v010 , v110 , v210 , 
                        v020 , v120 , v220 ) ,
        _biquadratic_ ( x    , y           ,  
                        x0   , x1   , x2   , 
                        y0   , y1   , y2   , 
                        v001 , v101 , v201 , 
                        v011 , v111 , v211 , 
                        v021 , v121 , v221 ) ,
        _biquadratic_ ( x    , y           ,  
                        x0   , x1   , x2   , 
                        y0   , y1   , y2   , 
                        v002 , v102 , v202 , 
                        v012 , v112 , v212 , 
                        v022 , v122 , v222 ) ) ;  
  }
  // ==========================================================================
}
// ============================================================================
// 1D-interpolation 
// ============================================================================
/** linear interpolation between two points 
 *  @param x  (INPUT) the x-value
 *  @param x0 (INPUT) x-coordinate of the first  point 
 *  @param x1 (INPUT) x-coordinate of the second point 
 *  @param y0 (INPUT) y-coordinate of the first  point 
 *  @param y1 (INPUT) y-coordinate of the second point 
 *  @return result of linear interpolation
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::HistoInterpolation::interpolate
( const double                       x  , 
  const double                       x0 , 
  const double                       x1 ,
  const Gaudi::Math::ValueWithError& y0 , 
  const Gaudi::Math::ValueWithError& y1 ) 
{ return _linear_ ( x , x0, x1 , y0 , y1 ) ; }
// ============================================================================
/* quadratic (parabolic)  interpolation between three points 
 *  @param x  (INPUT) the x-value
 *  @param x0 (INPUT) x-coordinate of the first  point 
 *  @param x1 (INPUT) x-coordinate of the second point 
 *  @param x2 (INPUT) x-coordinate of the third  point 
 *  @param y0 (INPUT) y-coordinate of the first  point 
 *  @param y1 (INPUT) y-coordinate of the second point 
 *  @param x2 (INPUT) x-coordinate of the third  point 
 *  @return result of  quadratic (parabolic) interpolation
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::HistoInterpolation::interpolate
( const double                       x  , 
  const double                       x0 , 
  const double                       x1 ,
  const double                       x2 ,
  const Gaudi::Math::ValueWithError& y0 , 
  const Gaudi::Math::ValueWithError& y1 , 
  const Gaudi::Math::ValueWithError& y2 ) 
{ return _quadratic_( x , x0, x1 , x2 , y0 , y1 , y2 ) ; }
// ============================================================================
// 2D-interpolation 
// ============================================================================
/* bi-linear interpolation on grid 
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
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::HistoInterpolation::interpolate
( const double                       x   , 
  const double                       y   , 
  const double                       x0  , 
  const double                       x1  ,
  const double                       y0  , 
  const double                       y1  ,
  const Gaudi::Math::ValueWithError& f00 , 
  const Gaudi::Math::ValueWithError& f10 ,
  const Gaudi::Math::ValueWithError& f01 , 
  const Gaudi::Math::ValueWithError& f11 ) 
{
  return _bilinear_ ( x   , y   ,  
                      x0  , x1  , 
                      y0  , y1  , 
                      f00 , f10 , 
                      f01 , f11 ) ;
}
// ============================================================================
/* bi-quadratic interpolation on grid 
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
 *  @return result of bi-quadratic interpolation
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::HistoInterpolation::interpolate
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
  const Gaudi::Math::ValueWithError& f22 ) 
{
  return _biquadratic_ 
    ( x   , y   , 
      x0  , x1  , x2  , 
      y0  , y1  , y2  , 
      f00 , f10 , f20 , 
      f01 , f11 , f21 , 
      f02 , f12 , f22 ) ;      
}
// ============================================================================
/* interpolate 1D historgam 
 *  @param h1   (INPUT) input histogram 
 *  @param x    (INPUT) the value 
 *  @para  type (INPUT) interpolation type 
 *  @return valeu of interpolated function
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::HistoInterpolation::interpolate_1D
( const TH1&                                  h1 , 
  const double                                x  ,
  const Gaudi::Math::HistoInterpolation::Type t  ) 
{
  const TAxis* ax   = h1.GetXaxis()  ;
  if ( 0 == ax )                   { return ValueWithError() ; } // RETURN 
  //
  // get main parameters and treat the special "easy" cases:
  //
  const double       xmin  = ax->GetXmin  () ;
  if ( xmin > x )                  { return ValueWithError() ; } // RETURN 
  //
  const double       xmax  = ax->GetXmax  () ;
  if ( xmax < x )                  { return ValueWithError() ; } // RETURN 
  //
  if ( s_equal ( x , xmin ) ) 
  {
    const double v  = h1.GetBinContent ( 1    ) ;
    const double ev = h1.GetBinError   ( 1    ) ;
    return ValueWithError ( v , ev * ev ) ;                      // RETURN 
  }
  //
  const unsigned int nbins = ax->GetNbins () ;
  //
  if ( s_equal ( x , xmax ) ) 
  {
    const double v  = h1.GetBinContent ( nbins ) ;
    const double ev = h1.GetBinError   ( nbins ) ;
    return ValueWithError ( v , ev * ev ) ;                       // RETURN 
  }
  // 
  // adjust the interpolation type 
  //
  const Type itype = 
    ( t <= Nearest                  ) ? Nearest    : 
    ( 1 >= nbins                    ) ? Nearest    : 
    ( 2 == nbins  && t >= Linear    ) ? Linear     :
    ( 3 == nbins  && t >= Quadratic ) ? Quadratic  :
    (                t >= Quadratic ) ? Quadratic  : t ;
  //
  // the regular case 
  //
  const unsigned int ib = ax->FindFixBin ( x ) ;
  if ( 0 == ib || nbins + 1 == ib ) { return ValueWithError() ;  } // RETURN
  //
  if ( Nearest == itype ) 
  {
    const double v  = h1.GetBinContent ( ib ) ;
    const double ev = h1.GetBinError   ( ib ) ;
    return ValueWithError ( v , ev * ev ) ;                       // RETURN 
  }
  //
  const double xc = ax->GetBinCenter ( ib ) ;
  //
  // treat left of the first and right half of the last bin separately:
  //
  if ( ( 1 == ib && x <= xc ) || ( nbins == ib && xc <= x ) ) 
  {
    const double v  = h1.GetBinContent ( ib ) ;
    const double ev = h1.GetBinError   ( ib ) ;
    return ValueWithError ( v , ev * ev ) ;                       // RETURN 
  }
  //
  // linear interpolation 
  //
  if ( Linear == itype )
  {
    const unsigned int i1 = x < xc ? ib - 1 : ib ;
    const unsigned int i2 = i1 + 1 ;
    //
    return _linear_ 
      ( x                            , 
        ax->GetBinCenter ( i1 )      , 
        ax->GetBinCenter ( i2 )      , 
        _bin_            ( h1 , i1 ) , 
        _bin_            ( h1 , i2 ) ) ;
  }
  //
  // quadratic interpolation 
  //
  const unsigned int i0 = 
    1     >= ib ? 1         :
    nbins <= ib ? nbins - 2 : ib - 1 ;
  const unsigned int i1 = i0 + 1 ;
  const unsigned int i2 = i0 + 2 ;
  //
  return _quadratic_ 
    ( x                            , 
      ax->GetBinCenter ( i0 )      , 
      ax->GetBinCenter ( i1 )      , 
      ax->GetBinCenter ( i2 )      , 
      _bin_            ( h1 , i0 ) , 
      _bin_            ( h1 , i1 ) , 
      _bin_            ( h1 , i2 ) ) ;
}
// ============================================================================
/* interpolate 2D historgam 
 *  @param  h2    (INPUT) input histogram 
 *  @param  x     (INPUT) the value 
 *  @param  y     (INPUT) the value 
 *  @para  tx   (INPUT) interpolation type in x-direction
 *  @para  ty   (INPUT) interpolation type in y-direction
 *  @return valeu of interpolated function
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::HistoInterpolation::interpolate_2D
( const TH2&                                  h2 , 
  const double                                x  ,
  const double                                y  ,
  const Gaudi::Math::HistoInterpolation::Type tx , 
  const Gaudi::Math::HistoInterpolation::Type ty ) 
{
  const TAxis* ax   = h2.GetXaxis()  ;
  if ( 0 == ax )                   { return ValueWithError() ; } // RETURN 
  const TAxis* ay   = h2.GetYaxis()  ;
  if ( 0 == ay )                   { return ValueWithError() ; } // RETURN 
  //
  // get main parameters and treat the special "easy" cases:
  //
  const double       xmin  = ax->GetXmin  () ;
  if ( xmin > x )                  { return ValueWithError() ; } // RETURN 
  //
  const double       xmax  = ax->GetXmax  () ;
  if ( xmax < x )                  { return ValueWithError() ; } // RETURN 
  //
  const double       ymin  = ay->GetXmin  () ;
  if ( ymin > y )                  { return ValueWithError() ; } // RETURN 
  //
  const double       ymax  = ay->GetXmax  () ;
  if ( ymax < y )                  { return ValueWithError() ; } // RETURN 
  //  
  const unsigned int nbx = ax->GetNbins () ;
  const unsigned int nby = ay->GetNbins () ;
  //
  // adjust the interpolation type 
  //
  Type itypex = 
    ( tx <= Nearest                ) ? Nearest    : 
    ( 1  >= nbx                    ) ? Nearest    : 
    ( 2  == nbx && tx >= Linear    ) ? Linear     :
    ( 3  == nbx && tx >= Quadratic ) ? Quadratic  :
    (              tx >= Quadratic ) ? Quadratic  : tx ;  
  //
  Type itypey = 
    ( ty <= Nearest                ) ? Nearest    : 
    ( 1  >= nby                    ) ? Nearest    : 
    ( 2  == nby && ty >= Linear    ) ? Linear     :
    ( 3  == nby && ty >= Quadratic ) ? Quadratic  :
    (              ty >= Quadratic ) ? Quadratic  : ty ;
  //
  // find the bin 
  //
  unsigned int ibx = ax -> FindFixBin ( x ) ;
  unsigned int iby = ay -> FindFixBin ( y ) ;
  //
  if      ( 0       == ibx && s_equal ( x , xmin ) ) { ibx += 1 ; }
  else if ( nbx + 1 == ibx && s_equal ( x , xmax ) ) { ibx -= 1 ; }
  if      ( 0       == iby && s_equal ( y , ymin ) ) { iby += 1 ; }
  else if ( nby + 1 == iby && s_equal ( y , ymax ) ) { iby -= 1 ; }
  //
  if ( 0 == ibx || nbx < ibx ) { return ValueWithError () ; }      // RETURN 
  if ( 0 == iby || nby < iby ) { return ValueWithError () ; }      // RETURN 
  //
  if ( Nearest == itypex && Nearest == itypey ) 
  { return _bin_ ( h2 , ibx , iby ) ; }                            // RETURN 
  //
  // get bin centres 
  //
  const double xc = ax->GetBinCenter ( ibx ) ;
  const double yc = ay->GetBinCenter ( iby ) ;
  //
  // special "x-edge" case: 
  //
  const bool x_edge =  ( 1 == ibx && x <= xc ) || ( nbx == ibx && x >= xc ) ;
  const bool y_edge =  ( 1 == iby && y <= yc ) || ( nby == iby && y >= yc ) ;
  //
  // corners, no interpolation, get the value  
  //
  if ( x_edge && y_edge ) { return _bin_( h2 , ibx , iby ) ; } // RETURN 
  //
  // adjust again interpolation rules 
  //
  if ( x_edge ) { itypex = Nearest ; }
  if ( y_edge ) { itypey = Nearest ; }
  //
  if      ( Nearest == itypex && Linear == itypey )     // (2) 
  {
    const unsigned int i1 = y < yc ? iby - 1 : iby ;
    const unsigned int i2 = i1 + 1 ;
    return _linear_ 
      ( y                                  , 
        ay->GetBinCenter ( i1 )            , 
        ay->GetBinCenter ( i2 )            , 
        _bin_            ( h2 , ibx , i1 ) ,
        _bin_            ( h2 , ibx , i2 ) ) ;
  }
  else if ( Nearest == itypey && Linear == itypex )    // (3)
  {
    const unsigned int i1 = x < xc ? ibx - 1 : ibx ;
    const unsigned int i2 = i1 + 1 ;
    return _linear_ 
      ( x                                  , 
        ax->GetBinCenter ( i1 )            , 
        ax->GetBinCenter ( i2 )            , 
        _bin_            ( h2 , i1 , iby ) ,
        _bin_            ( h2 , i2 , iby ) ) ;
  }
  else if ( Nearest == itypex && Quadratic == itypey )  // (4) 
  {
    // quadratic interpolation in y 
    const unsigned int i0 = 
      1   >= iby ? 1       :
      nby <= iby ? nby - 2 : iby - 1 ;
    const unsigned int i1 = i0 + 1 ;
    const unsigned int i2 = i0 + 2 ;
    return _quadratic_ 
      ( y                                , 
        ay->GetBinCenter ( i0 )          , 
        ay->GetBinCenter ( i1 )          , 
        ay->GetBinCenter ( i2 )          , 
        _bin_            ( h2 , ibx , i0 ) , 
        _bin_            ( h2 , ibx , i1 ) , 
        _bin_            ( h2 , ibx , i2 ) ) ;
  }
  else if ( Nearest == itypey && Quadratic == itypex )  // (5) 
  {
    // quadratic interpolation in X
    const unsigned int i0 = 
      1   >= ibx ? 1       :
      nbx <= ibx ? nbx - 2 : ibx - 1 ;
    const unsigned int i1 = i0 + 1 ;
    const unsigned int i2 = i0 + 2 ;
    return _quadratic_ 
      ( x                                , 
        ax->GetBinCenter ( i0 )          , 
        ax->GetBinCenter ( i1 )          , 
        ax->GetBinCenter ( i2 )          , 
        _bin_            ( h2 , i0 , iby ) , 
        _bin_            ( h2 , i1 , iby ) , 
        _bin_            ( h2 , i2 , iby ) ) ;
  }
  else if ( Linear == itypex && Linear == itypey )  // (6) 
  {
    const unsigned int ix1 = x < xc ? ibx - 1 : ibx ;
    const unsigned int ix2 = ix1 + 1 ;
    const unsigned int iy1 = y < yc ? iby - 1 : iby ;
    const unsigned int iy2 = iy1 + 1 ;
    //
    return _bilinear_ 
      ( x    ,   y   , 
        ax->GetBinCenter ( ix1 ) , 
        ax->GetBinCenter ( ix2 ) , 
        ay->GetBinCenter ( iy1 ) , 
        ay->GetBinCenter ( iy2 ) , 
        _bin_ ( h2 , ix1 , iy1 ) , 
        _bin_ ( h2 , ix2 , iy1 ) , 
        _bin_ ( h2 , ix1 , iy2 ) , 
        _bin_ ( h2 , ix2 , iy2 ) ) ;
  }
  else if ( Linear == itypex && Quadratic == itypey )   // (7)  
  {
    // linear interpolation in X 
    const unsigned int ix1 = x < xc ? ibx - 1 : ibx ;
    const unsigned int ix2 = ix1 + 1 ;
    const double        x1 = ax->GetBinCenter ( ix1 ) ;
    const double        x2 = ax->GetBinCenter ( ix2 ) ;
    //
    // quadratic interpolation in Y
    const unsigned int iy0 = 
      1   >= iby ? 1       :
      nby <= iby ? nby - 2 : iby - 1 ;
    const unsigned int iy1 = iy0 + 1 ;
    const unsigned int iy2 = iy0 + 2 ;
    //
    return _quadratic_ 
      ( y                                , 
        ay->GetBinCenter ( iy0 )         , 
        ay->GetBinCenter ( iy1 )         , 
        ay->GetBinCenter ( iy2 )         , 
        _linear_ ( x , x1 , x2 , _bin_ ( h2 , ix1 , iy0 ) , _bin_ ( h2 , ix2 , iy0 ) ) , 
        _linear_ ( x , x1 , x2 , _bin_ ( h2 , ix1 , iy1 ) , _bin_ ( h2 , ix2 , iy1 ) ) , 
        _linear_ ( x , x1 , x2 , _bin_ ( h2 , ix1 , iy2 ) , _bin_ ( h2 , ix2 , iy2 ) ) ) ;
  }
  else if ( Linear == itypey && Quadratic == itypex ) 
  {
    // linear interpolation in Y 
    const unsigned int iy1 = y < yc ? iby - 1 : iby ;
    const unsigned int iy2 = iy1 + 1 ;
    const double        y1 = ay->GetBinCenter ( iy1 ) ;
    const double        y2 = ay->GetBinCenter ( iy2 ) ;
    //
    // quadratic interpolation in X
    const unsigned int ix0 = 
      1   >= ibx ? 1       :
      nbx <= ibx ? nbx - 2 : ibx - 1 ;
    const unsigned int ix1 = ix0 + 1 ;
    const unsigned int ix2 = ix0 + 2 ;
    //
    return _quadratic_ 
      ( x                                , 
        ax->GetBinCenter ( ix0 )         , 
        ax->GetBinCenter ( ix1 )         , 
        ax->GetBinCenter ( ix2 )         , 
        _linear_ ( y , y1 , y2 , _bin_ ( h2 , ix0 , iy1 ) , _bin_ ( h2 , ix0 , iy2 ) ) , 
        _linear_ ( y , y1 , y2 , _bin_ ( h2 , ix1 , iy1 ) , _bin_ ( h2 , ix1 , iy2 ) ) , 
        _linear_ ( y , y1 , y2 , _bin_ ( h2 , ix2 , iy1 ) , _bin_ ( h2 , ix2 , iy2 ) ) ) ;
  }
  else if ( Quadratic == itypey && Quadratic == itypex ) 
  {
    // Final case: bi-quadratic 
    //
    const unsigned int ix0 =
      1   == ibx     ? 1 :
      nbx == ibx     ? nbx - 2 : ibx - 1 ;
    const unsigned int iy0 =
      1   == iby     ? 1 :
      nby == iby     ? nby - 2 : iby - 1 ;
    //
    const unsigned int ix1 = ix0 + 1 ;
    const unsigned int ix2 = ix1 + 1 ;
    const unsigned int iy1 = iy0 + 1 ;
    const unsigned int iy2 = iy1 + 1 ;
    //
    return _biquadratic_ 
      ( x    ,   y   , 
        ax->GetBinCenter ( ix0 ) , 
        ax->GetBinCenter ( ix1 ) , 
        ax->GetBinCenter ( ix2 ) , 
        ay->GetBinCenter ( iy0 ) ,
        ay->GetBinCenter ( iy1 ) , 
        ay->GetBinCenter ( iy2 ) , 
        _bin_ ( h2 , ix0 , iy0 ) , 
        _bin_ ( h2 , ix1 , iy0 ) , 
        _bin_ ( h2 , ix2 , iy0 ) , 
        _bin_ ( h2 , ix0 , iy1 ) , 
        _bin_ ( h2 , ix1 , iy1 ) , 
        _bin_ ( h2 , ix2 , iy1 ) , 
        _bin_ ( h2 , ix0 , iy2 ) , 
        _bin_ ( h2 , ix1 , iy2 ) , 
        _bin_ ( h2 , ix2 , iy2 ) ) ;
  }
  //
  return _bin_ ( h2 , ibx , iby ) ;
}
// ============================================================================
/*  interpolate 3D historgam 
 *  @param  h3    (INPUT) input histogram 
 *  @param  x     (INPUT) the x-value 
 *  @param  y     (INPUT) the y-value 
 *  @param  y     (INPUT) the z-value 
 *  @para   type  (INPUT) interpolation type 
 *  @return valeu of interpolated function
 */
// ============================================================================
Gaudi::Math::ValueWithError 
Gaudi::Math::HistoInterpolation::interpolate_3D
( const TH3&                                  h3 , 
  const double                                x  ,
  const double                                y  ,
  const double                                z  ,
  const Gaudi::Math::HistoInterpolation::Type tx , 
  const Gaudi::Math::HistoInterpolation::Type ty , 
  const Gaudi::Math::HistoInterpolation::Type tz ) 
{
  const TAxis* ax   = h3.GetXaxis()  ;
  if ( 0 == ax )                   { return ValueWithError() ; } // RETURN 
  const TAxis* ay   = h3.GetYaxis()  ;
  if ( 0 == ay )                   { return ValueWithError() ; } // RETURN 
  const TAxis* az   = h3.GetZaxis()  ;
  if ( 0 == az )                   { return ValueWithError() ; } // RETURN 
  //
  // get main parameters and treat the special "easy" cases:
  //
  const double       xmin  = ax->GetXmin  () ;
  if ( xmin > x )                  { return ValueWithError() ; } // RETURN 
  //
  const double       xmax  = ax->GetXmax  () ;
  if ( xmax < x )                  { return ValueWithError() ; } // RETURN 
  //
  const double       ymin  = ay->GetXmin  () ;
  if ( ymin > y )                  { return ValueWithError() ; } // RETURN 
  //
  const double       ymax  = ay->GetXmax  () ;
  if ( ymax < y )                  { return ValueWithError() ; } // RETURN 
  //  
  const double       zmin  = az->GetXmin  () ;
  if ( zmin > z )                  { return ValueWithError() ; } // RETURN 
  //
  const double       zmax  = az->GetXmax  () ;
  if ( zmax < z )                  { return ValueWithError() ; } // RETURN 
  //  
  const unsigned int nbx = ax->GetNbins () ;
  const unsigned int nby = ay->GetNbins () ;
  const unsigned int nbz = az->GetNbins () ;
  //
  // adjust the interpolation type 
  //
  Type itypex = 
    ( tx <= Nearest                ) ? Nearest    : 
    ( 1  >= nbx                    ) ? Nearest    : 
    ( 2  == nbx && tx >= Linear    ) ? Linear     :
    ( 3  == nbx && tx >= Quadratic ) ? Quadratic  :
    (              tx >= Quadratic ) ? Quadratic  : tx ;  
  //
  Type itypey = 
    ( ty <= Nearest                ) ? Nearest    : 
    ( 1  >= nby                    ) ? Nearest    : 
    ( 2  == nby && ty >= Linear    ) ? Linear     :
    ( 3  == nby && ty >= Quadratic ) ? Quadratic  :
    (              ty >= Quadratic ) ? Quadratic  : ty ;
  //
  Type itypez = 
    ( tz <= Nearest                ) ? Nearest    : 
    ( 1  >= nbz                    ) ? Nearest    : 
    ( 2  == nbz && tz >= Linear    ) ? Linear     :
    ( 3  == nbz && tz >= Quadratic ) ? Quadratic  :
    (              tz >= Quadratic ) ? Quadratic  : tz ;
  //
  // find the bin 
  //
  unsigned int ibx = ax -> FindFixBin ( x ) ;
  unsigned int iby = ay -> FindFixBin ( y ) ;
  unsigned int ibz = az -> FindFixBin ( z ) ;
  //
  if      ( 0       == ibx && s_equal ( x , xmin ) ) { ibx += 1 ; }
  else if ( nbx + 1 == ibx && s_equal ( x , xmax ) ) { ibx -= 1 ; }
  if      ( 0       == iby && s_equal ( y , ymin ) ) { iby += 1 ; }
  else if ( nby + 1 == iby && s_equal ( y , ymax ) ) { iby -= 1 ; }
  if      ( 0       == ibz && s_equal ( z , zmin ) ) { ibz += 1 ; }
  else if ( nbz + 1 == ibz && s_equal ( z , zmax ) ) { ibz -= 1 ; }
  //
  if ( 0 == ibx || nbx < ibx ) { return ValueWithError () ; }      // RETURN 
  if ( 0 == iby || nby < iby ) { return ValueWithError () ; }      // RETURN 
  if ( 0 == ibz || nbz < ibz ) { return ValueWithError () ; }      // RETURN 
  //
  if ( Nearest == itypex && Nearest == itypey && Nearest == itypez ) 
  { return _bin_ ( h3 , ibx , iby , ibz ) ; }                      // RETURN 
  //
  // get bin centres 
  const double xc = ax->GetBinCenter ( ibx ) ;
  const double yc = ay->GetBinCenter ( iby ) ;
  const double zc = az->GetBinCenter ( ibz ) ;
  //
  const bool x_edge =  1 == nbx || ( 1 == ibx && x <= xc ) || ( nbx == ibx && x >= xc ) ;
  const bool y_edge =  1 == nby || ( 1 == iby && y <= yc ) || ( nby == iby && y >= yc ) ;
  const bool z_edge =  1 == nbz || ( 1 == ibz && z <= zc ) || ( nbz == ibz && z >= zc ) ;
  //
  // corners, no interpolation, get the value  
  //
  if ( x_edge && y_edge && z_edge ) { return _bin_( h3 , ibx , iby , ibz ) ; } // RETURN 
  //
  if ( x_edge ) { itypex = Nearest ; }
  if ( y_edge ) { itypey = Nearest ; }
  if ( z_edge ) { itypez = Nearest ; }  
  //
  // the regular (tri)linear case 
  //
  if (  Linear == itypex && Linear == itypey && Linear == itypez ) 
  {
    const unsigned int ix1 = x < xc ? ibx - 1 : ibx ;
    const unsigned int ix2 = ix1 + 1 ;
    const unsigned int iy1 = y < yc ? iby - 1 : iby ;
    const unsigned int iy2 = iy1 + 1 ;
    const unsigned int iz1 = z < zc ? ibz - 1 : ibz ;
    const unsigned int iz2 = iz1 + 1 ;
    return _trilinear_ 
      (  x , y , z , 
         ax -> GetBinCenter ( ix1 ) , 
         ax -> GetBinCenter ( ix2 ) , 
         ay -> GetBinCenter ( iy1 ) , 
         ay -> GetBinCenter ( iy2 ) , 
         az -> GetBinCenter ( iz1 ) , 
         az -> GetBinCenter ( iz2 ) ,
         _bin_ ( h3 , ix1 , iy1 , iz1 ) , 
         _bin_ ( h3 , ix2 , iy1 , iz1 ) , 
         _bin_ ( h3 , ix1 , iy2 , iz1 ) , 
         _bin_ ( h3 , ix2 , iy2 , iz1 ) , 
         _bin_ ( h3 , ix1 , iy1 , iz2 ) , 
         _bin_ ( h3 , ix2 , iy1 , iz2 ) , 
         _bin_ ( h3 , ix1 , iy2 , iz2 ) , 
         _bin_ ( h3 , ix2 , iy2 , iz2 ) ) ;
  }
  //
  // the regular quadratic case 
  //
  if ( Quadratic == itypex && Quadratic == itypey && Quadratic == itypez )
  {
    //
    const unsigned int ix0 = 
      1   >= ibx ? 1       :
      nbx <= ibx ? nbx - 2 : ibx - 1 ;
    const unsigned int ix1 = ix0 + 1 ;
    const unsigned int ix2 = ix0 + 2 ;
    //
    const unsigned int iy0 = 
      1   >= iby ? 1       :
      nby <= iby ? nby - 2 : iby - 1 ;
    const unsigned int iy1 = iy0 + 1 ;
    const unsigned int iy2 = iy0 + 2 ;
    //
    const unsigned int iz0 = 
      1   >= ibz ? 1       :
      nbz <= ibz ? nbz - 2 : ibz - 1 ;
    const unsigned int iz1 = iz0 + 1 ;
    const unsigned int iz2 = iz0 + 2 ;
    //
    return _triquadratic_ 
      (  x , y , z , 
         ax -> GetBinCenter ( ix0 ) , 
         ax -> GetBinCenter ( ix1 ) , 
         ax -> GetBinCenter ( ix2 ) , 
         ay -> GetBinCenter ( iy0 ) , 
         ay -> GetBinCenter ( iy1 ) , 
         ay -> GetBinCenter ( iy2 ) , 
         az -> GetBinCenter ( iz0 ) , 
         az -> GetBinCenter ( iz1 ) , 
         az -> GetBinCenter ( iz2 ) , 
         //
         _bin_ ( h3 , ix0 , iy0 , iz0 ) , 
         _bin_ ( h3 , ix1 , iy0 , iz0 ) , 
         _bin_ ( h3 , ix2 , iy0 , iz0 ) , 
         _bin_ ( h3 , ix0 , iy1 , iz0 ) , 
         _bin_ ( h3 , ix1 , iy1 , iz0 ) , 
         _bin_ ( h3 , ix2 , iy1 , iz0 ) , 
         _bin_ ( h3 , ix0 , iy2 , iz0 ) , 
         _bin_ ( h3 , ix1 , iy2 , iz0 ) , 
         _bin_ ( h3 , ix2 , iy2 , iz0 ) , 
         //
         _bin_ ( h3 , ix0 , iy0 , iz1 ) , 
         _bin_ ( h3 , ix1 , iy0 , iz1 ) , 
         _bin_ ( h3 , ix2 , iy0 , iz1 ) , 
         _bin_ ( h3 , ix0 , iy1 , iz1 ) , 
         _bin_ ( h3 , ix1 , iy1 , iz1 ) , 
         _bin_ ( h3 , ix2 , iy1 , iz1 ) , 
         _bin_ ( h3 , ix0 , iy2 , iz1 ) , 
         _bin_ ( h3 , ix1 , iy2 , iz1 ) , 
         _bin_ ( h3 , ix2 , iy2 , iz1 ) , 
         //
         _bin_ ( h3 , ix0 , iy0 , iz2 ) , 
         _bin_ ( h3 , ix1 , iy0 , iz2 ) , 
         _bin_ ( h3 , ix2 , iy0 , iz2 ) , 
         _bin_ ( h3 , ix0 , iy1 , iz2 ) , 
         _bin_ ( h3 , ix1 , iy1 , iz2 ) , 
         _bin_ ( h3 , ix2 , iy1 , iz2 ) , 
         _bin_ ( h3 , ix0 , iy2 , iz2 ) , 
         _bin_ ( h3 , ix1 , iy2 , iz2 ) ,
         _bin_ ( h3 , ix2 , iy2 , iz2 ) ) ;
  }
  //
  // No special treatment of edges  :-(
  //
  return _bin_( h3 , ibx , iby , ibz ) ;  // RETURN 
}


  
  

// ============================================================================
// The END 
// ============================================================================
