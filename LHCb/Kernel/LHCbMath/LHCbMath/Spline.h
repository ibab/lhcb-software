// Id:$ 
// ============================================================================
#ifndef LHCBMATH_SPLINE_H 
#define LHCBMATH_SPLINE_H 1
// ============================================================================
// Include file 
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
#include <utility>
#include <memory>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// GaudiGSL 
// ============================================================================
#include "GaudiMath/Interpolation.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/ValueWithError.h"
// ============================================================================
// Forward declarations
// ============================================================================
// ROOT 
// ============================================================================
class TH1          ; 
class TH2          ; 
class TGraph       ; 
class TGraphErrors ;
// ============================================================================
// GaudiGSL 
// ============================================================================
namespace Genfun
{ 
  namespace GaudiMathImplementation 
  { 
    class SplineBase ; 
  } 
}
// ============================================================================
/** @file LHCbMath/Spline.h
 *
 *  useful objects for splines  
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2013-03-15
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $author$
 */
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Math 
  {
    // ========================================================================
    namespace Splines 
    {
      // ======================================================================
      /// the actual type of "simple" data 
      typedef std::vector<std::pair<double,double> >           DATA    ;
      typedef std::pair<double,Gaudi::Math::ValueWithError>    PAIR    ;
      /// the actual type of data with errors  
      typedef std::vector<PAIR>                                DATAERR ;
      // ======================================================================
      /// get the values from the histogram 
      GAUDI_API DATA    getValues ( const TH1&          histo ) ;
      /// get the errors from the histogram 
      GAUDI_API DATA    getErrors ( const TH1&          histo ) ;
      /// get the data   from the histogram 
      GAUDI_API DATAERR getData   ( const TH1&          histo ) ;
      /// get the values from the data vector 
      GAUDI_API DATA    getValues ( const DATAERR&      data  ) ;
      /// get the values from the data vector 
      GAUDI_API DATA    getErrors ( const DATAERR&      histo ) ;
      /// get the values from the graph 
      GAUDI_API DATA    getValues ( const TGraph&       graph ) ;
      /// get the values from the graph 
      GAUDI_API DATA    getErrors ( const TGraphErrors& graph ) ;
      /// get the values from the graph
      GAUDI_API DATAERR getData   ( const TGraphErrors& graph ) ;
      // ======================================================================
      /** simple lagrange interpolation 
       *  @attention it can be CPU-inefficient!
       *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
       *  @date 2013-03-15
       */ 
      GAUDI_API double  
      interpolate  ( const DATA&    data , const double x ) ;
      // ======================================================================
      /** simple lagrange interpolation 
       *  @attention it can be CPU-inefficient!
       *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
       *  @date 2013-03-15
       */ 
      GAUDI_API Gaudi::Math::ValueWithError  
      interpolate  ( const DATAERR& data , const double x ) ;
      // ======================================================================
    } //                                  end of namespace Gaudi::Math::Splines 
    // ========================================================================
    /** @class Spline LHCbMath/Spline.h
     *  
     *  Simple spline for 1D-distributions, e.g. historgam or graph
     *  it is a wrapper over class GaudiMath::Spline 
     *
     *  @see GaudiMath::Spline 
     *  @see GaudiMath::SplineBase
     *  @see Genfun::GaudiMathImplementation::SplineBase ; 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2013-03-14
     */
    class GAUDI_API Spline
      : public std::unary_function<double,double>
    {
      // ======================================================================
    public: 
      // ======================================================================   
      typedef Gaudi::Math::Splines::DATA     DATA    ;
      typedef Gaudi::Math::Splines::DATAERR  DATAERR ;
      // ======================================================================
    public: 
      // ======================================================================
      /** Standard constructor
       *  @param data  data to be interpolated 
       *  @param type  interpolation type 
       *  @param null  set to null outside the range?
       *  @param scale the scale for the input variable
       *  @param shift the bias  for the input variable
       */
      Spline
      ( const DATA&                           data , 
        const GaudiMath::Interpolation::Type& type   = GaudiMath::Interpolation::Akima , 
        const bool                            null   = false                           , 
        const double                          scale  = 1  , 
        const double                          shift  = 0  ) ;
      // ======================================================================
      /** Standard constructor
       *  @param data  data to be interpolated 
       *  @param type  interpolation type 
       *  @param null  set to null outside the range?
       *  @param scale the scale for the input variable
       *  @param shift the bias  for the input variable
       */
      Spline
      ( const DATAERR&                        data , 
        const GaudiMath::Interpolation::Type& type   = GaudiMath::Interpolation::Akima , 
        const bool                            null   = false                           , 
        const double                          scale  = 1  , 
        const double                          shift  = 0  ) ;
      // ======================================================================
      /** Standard constructor
       *  @param histo the histogram to be interpolated  
       *  @param type  interpolation type 
       *  @param null  set to null outside the range?
       *  @param scale the scale for the input variable
       *  @param shift the bias  for the input variable
       */
      Spline
      ( const TH1&                            histo                                    , 
        const GaudiMath::Interpolation::Type& type   = GaudiMath::Interpolation::Akima , 
        const bool                            null   = true                            , 
        const double                          scale  = 1  , 
        const double                          shift  = 0  ) ;
      // ======================================================================
      /** Standard constructor
       *  @param graph the graph to be interpolated  
       *  @param type  interpolation type 
       *  @param null  set to null outside the range?
       *  @param scale the scale for the input variable
       *  @param shift the bias  for the input variable
       */
      Spline
      ( const TGraph&                         graph                                    , 
        const GaudiMath::Interpolation::Type& type   = GaudiMath::Interpolation::Akima , 
        const bool                            null   = false                           , 
        const double                          scale  = 1  , 
        const double                          shift  = 0  ) ;
      /// destructor 
      ~Spline() ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      Spline () ;  // default constructor is disabled 
      /// copy    constructor is disabled 
      Spline ( const Spline& ) ;  // default constructor is disabled
      /// assignement is disabled 
      Spline& operator= ( const Spline& ) ;  // assignement is disabled
      // ======================================================================
    public:
      // ======================================================================
      bool    nullify () const { return   m_null  ; }
      double  scale   () const { return   m_scale ; }
      double  shift   () const { return   m_shift ; }
      double  xmin    () const { return   m_xmin  ; }
      double  xmax    () const { return   m_xmax  ; }  
      double  vmin    () const { return ( m_xmin - m_shift ) / m_scale ; }
      double  vmax    () const { return ( m_xmax - m_shift ) / m_scale ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the value 
      double operator()  ( const double x    ) const ;
      /// get the derivative 
      double derivative  ( const double x    ) const ;
      /// get the second derivative 
      double derivative2 ( const double x    ) const ;
      /// get the integral (when applicable) 
      double integral    () const ;
      /// get the integral 
      double integral    ( const double xmin , 
                           const double xmax ) const ;
      // ======================================================================      
    private:
      // ======================================================================
      /// nullify the content outside limits ?
      bool    m_null  ; // nullify the content outside limits ? 
      /// scale factor for the variable 
      double  m_scale ; // scale factor for the variable 
      /// shift for the variable 
      double  m_shift ; // shift for the variable 
      /// low  value (for nullification)
      double  m_xmin  ; // low value (for nullification)
      /// high value (for nullification)
      double  m_xmax  ; // high value (for nullification)
      // ======================================================================
      /// the actual type of spline object 
      typedef Genfun::GaudiMathImplementation::SplineBase  SPLINE ;      
      /// the spline itself
      std::unique_ptr<SPLINE> m_spline ; // the spline itself
      // ======================================================================
    };
    // ========================================================================
    /** @class SplineErrors LHCbMath/Spline.h
     *  
     *  Spline that take into account the uncertainties 
     *  it is just a part of ordinary splines 
     *
     *  @see Gaudi::Math::Spline 
     *  @see GaudiMath::Spline 
     *  @see GaudiMath::SplineBase
     *  @see Genfun::GaudiMathImplementation::SplineBase ; 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2013-03-14
     */
    class GAUDI_API SplineErrors
      : public std::unary_function<double,Gaudi::Math::ValueWithError>
    {
      // ======================================================================
    public: 
      // ======================================================================   
      typedef Gaudi::Math::Splines::DATAERR  DATAERR ;
      // ======================================================================
    public: 
      // ======================================================================
      /** Standard constructor
       *  @param data  data to be interpolated 
       *  @param type  interpolation type 
       *  @param null  set to null outside the range?
       *  @param scale the scale for the input variable
       *  @param shift the bias  for the input variable
       */
      SplineErrors
      ( const DATAERR&                        data , 
        const GaudiMath::Interpolation::Type& type   = GaudiMath::Interpolation::Akima , 
        const bool                            null   = false                           , 
        const double                          scale  = 1  , 
        const double                          shift  = 0  ) ;
      // ======================================================================
      /** Standard constructor
       *  @param histo the histogram to be interpolated  
       *  @param type  interpolation type 
       *  @param null  set to null outside the range?
       *  @param scale the scale for the input variable
       *  @param shift the bias  for the input variable
       */
      SplineErrors
      ( const TH1&                            histo                                    , 
        const GaudiMath::Interpolation::Type& type   = GaudiMath::Interpolation::Akima , 
        const bool                            null   = true                            , 
        const double                          scale  = 1  , 
        const double                          shift  = 0  ) ;
      // ======================================================================
      /** Standard constructor
       *  @param graph the graph to be interpolated  
       *  @param type  interpolation type 
       *  @param null  set to null outside the range?
       *  @param scale the scale for the input variable
       *  @param shift the bias  for the input variable
       */
      SplineErrors
      ( const TGraphErrors&                   graph                                    , 
        const GaudiMath::Interpolation::Type& type   = GaudiMath::Interpolation::Akima , 
        const bool                            null   = false                           , 
        const double                          scale  = 1  , 
        const double                          shift  = 0  ) ;
      /// destructor 
      ~SplineErrors() ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      SplineErrors () ;  // default constructor is disabled 
      /// copy    constructor is disabled 
      SplineErrors ( const SplineErrors& ) ;  // default constructor is disabled
      /// assignement is disabled 
      SplineErrors& operator= ( const SplineErrors& ) ;  // assignement is disabled
      // ======================================================================
    public:
      // ======================================================================
      /// get the value 
      Gaudi::Math::ValueWithError operator()  ( const double x ) const ;
      // ======================================================================      
    public:
      // ======================================================================      
      /// access to the spline itself 
      const Spline& spline ()  const { return m_spline ; } 
      const Spline& errors ()  const { return m_errors ; } 
      // ======================================================================      
    private:
      // ======================================================================
      /// the spline for the values 
      Spline m_spline ; // the spline for the values 
      /// the spline for the errors 
      Spline m_errors ; // the spline for the errors 
      // ======================================================================
    };
    // ========================================================================
    /** @class Interp2D LHCbMath/Spline.h
     *  
     *  Simple interpolation for 2D-histograms 
     *
     *  @warning it can be rather slow :-( 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2013-03-14
     */
    class GAUDI_API Interp2D 
      : public std::binary_function<double,double,Gaudi::Math::ValueWithError>
    {
      // ========================================================================
    public:
      // ========================================================================
      /// interpolation type  
      enum Type{
        Nearest    = 0 ,
        Linear         , 
        Parabolic      , 
        Cubic          , 
        Quartic        , 
        Quintic        , 
        Last           
      };
      // ========================================================================
    public:
      // ======================================================================== 
      /* Standard constructor
       *  @param data  data to be interpolated 
       *  @param type  interpolation type 
       *  @param null  set to null outside the range?
       *  @param scale the scale for the input variable
       *  @param shift the bias  for the input variable
       */
      Interp2D 
      ( const TH2&    data                , 
        const Type&   typex  = Cubic      , 
        const Type&   typey  = Cubic      , 
        const bool    null   = true       , 
        const double  scalex = 1          , 
        const double  scaley = 1          , 
        const double  shiftx = 0          ,
        const double  shifty = 0          ) ;
      /// destructor 
      ~Interp2D () ; // destructor
      // ======================================================================
    public:
      // ======================================================================
      /// get the value 
      Gaudi::Math::ValueWithError 
      operator()  ( const double x , const double y ) const ;
      // ======================================================================      
    private:
      // ======================================================================
      /// the historgam itself 
      const TH2* m_histo  ;  // the histogram itself 
      /// nullify outside the histogram range ? 
      bool       m_null   ;  // nullify outside the histogram range ? 
      /// interpolation type for X 
      Type       m_typex  ;  //  interpolation type for X 
      /// interpolation type for Y 
      Type       m_typey  ;  //  interpolation type for Y
      /// the scale parameter for X 
      double     m_scalex ;  // the scale parameter for X 
      /// the scale parameter for Y 
      double     m_scaley ;  // the scale parameter for Y
      /// the shift parameter for X 
      double     m_shiftx ;  // the shift parameter for X 
      /// the shift parameter for Y 
      double     m_shifty ;  // the shift parameter for Y
      // ======================================================================
    } ;
    // ========================================================================  
    /** @class Linear
     *  simple linear function that pass through two points
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2013-03-14
     */
    class GAUDI_API Linear : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /** constructor from two points
       *  @param x1 the first  abscissa 
       *  @param y1 function value at x1  
       *  @param x2 the second abscissa 
       *  @param y2 function value at x2  
       */
      Linear ( const double x1 = 0 , 
               const double y1 = 0 , 
               const double x2 = 1 , 
               const double y2 = 0 ) ;
      /// destructor 
      ~Linear() ; // destructor
      // ======================================================================      
    public:
      // ======================================================================
      /// get the value: y = a*x + b 
      double operator() ( const double x ) const  { return m_a * x + m_b ; }
      // ======================================================================      
    public:
      // ======================================================================      
      ///  y = a*x + b 
      double a () const { return m_a ; }
      ///  y = a*x + b 
      double b () const { return m_b ; }    
      // ======================================================================      
    private :
      // ======================================================================
      double  m_a ;
      double  m_b ;
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class LinearErr
     *  simple linear function that pass through two points
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2013-03-14
     */
    class GAUDI_API LinearErr
      : public std::unary_function<double,Gaudi::Math::ValueWithError>
    {
    public:
      // ======================================================================
      /** constructor from two points
       *  @param x1 the first  abscissa 
       *  @param y1 function value at x1  
       *  @param x2 the second abscissa 
       *  @param y2 function value at x2  
       */
      LinearErr
      ( const double                       x1 = 0                             , 
        const Gaudi::Math::ValueWithError& y1 = Gaudi::Math::ValueWithError() ,
        const double                       x2 = 1                             ,  
        const Gaudi::Math::ValueWithError& y2 = Gaudi::Math::ValueWithError() ) ;
      /// destructor 
      ~LinearErr () ; // destructor
      // ======================================================================      
    public:
      // ======================================================================
      /// get the value:
      Gaudi::Math::ValueWithError operator() ( const double x ) const  ;
      // ======================================================================      
    public:
      // ======================================================================      
      ///  y = a * x  + b 
      double a () const ;
      ///  y = a * x + b 
      double b () const ;
      // ======================================================================      
    private :
      // ======================================================================
      double                      m_x1 ;
      double                      m_x2 ;
      Gaudi::Math::ValueWithError m_y1 ;
      Gaudi::Math::ValueWithError m_y2 ;
      // ======================================================================      
    } ;
    // ========================================================================  
    /** @class Parabola
     *  simple parabola function that pass through three points
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2013-03-14
     */
    class GAUDI_API Parabola : public std::unary_function<double,double>
    {
    public:
      // ======================================================================
      /** constructor from three points
       *  @param x1 the first  abscissa 
       *  @param y1 function value at x1  
       *  @param x2 the second abscissa 
       *  @param y2 function value at x2  
       *  @param x3 the third abscissa 
       *  @param y3 function value at x3  
       */
      Parabola ( const double x1 = -1 , 
                 const double y1 =  0 , 
                 const double x2 =  0 , 
                 const double y2 =  0 ,
                 const double x3 =  1 , 
                 const double y3 =  0 ) ;
      /// destructor 
      ~Parabola () ; // destructor
      // ======================================================================      
    public:
      // ======================================================================
      /// get the value: y = a*x^2 + b*x + c  
      double operator() ( const double x ) const  
      { return ( m_a * x + m_b ) * x + m_c ; }
      // ======================================================================      
    public:
      // ======================================================================      
      /// y = a*x^2 + b*x + c  
      double a () const { return m_a ; }
      /// y = a*x^2 + b*x + c  
      double b () const { return m_b ; }    
      /// y = a*x^2 + b*x + c  
      double c () const { return m_c ; }    
      // ======================================================================      
    private :
      // ======================================================================
      double  m_a ;
      double  m_b ;
      double  m_c ;
      // ======================================================================      
    } ;
    // ========================================================================  
    /** @class ParabolaErr
     *  simple parabola function that pass through three points
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2013-03-14
     */
    class GAUDI_API ParabolaErr 
      : public std::unary_function<double,Gaudi::Math::ValueWithError>
    {
    public:
      // ======================================================================
      /** constructor from three points
       *  @param x1 the first  abscissa 
       *  @param y1 function value at x1  
       *  @param x2 the second abscissa 
       *  @param y2 function value at x2  
       *  @param x3 the third abscissa 
       *  @param y3 function value at x3  
       */
      ParabolaErr
      ( const double                       x1 = -1                            , 
        const Gaudi::Math::ValueWithError& y1 = Gaudi::Math::ValueWithError() ,
        const double                       x2 = 0                             , 
        const Gaudi::Math::ValueWithError& y2 = Gaudi::Math::ValueWithError() ,
        const double                       x3 = 1                             , 
        const Gaudi::Math::ValueWithError& y3 = Gaudi::Math::ValueWithError() ) ;
      /// destructor 
      ~ParabolaErr () ; // destructor
      // ======================================================================      
    public:
      // ======================================================================
      /// get the value: y = a*x^2 + b*x + c  
      Gaudi::Math::ValueWithError operator() ( const double x ) const ;
      // ======================================================================      
    public:
      // ======================================================================      
      /// y = a*x^2 + b*x + c  
      double a () const ;
      /// y = a*x^2 + b*x + c  
      double b () const ;
      /// y = a*x^2 + b*x + c  
      double c () const ;
      // ======================================================================      
    private : // input data 
      // ======================================================================
      double                      m_x1 ;
      double                      m_x2 ;
      double                      m_x3 ;
      //
      Gaudi::Math::ValueWithError m_y1 ;
      Gaudi::Math::ValueWithError m_y2 ;
      Gaudi::Math::ValueWithError m_y3 ;
      // ======================================================================      
    private : // helper data 
      // ======================================================================
      double                      m_d1 ;
      double                      m_d2 ;
      double                      m_d3 ;
      // ======================================================================      
    } ;
  } //                                             end of namespace Gaudi::Math
  // ==========================================================================
} //                                                    end of namesspace Gaudi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LHCBMATH_SPLINE_
// ============================================================================
