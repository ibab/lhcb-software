// $Id: LHCbMathDict.h,v 1.10 2010-05-18 09:08:21 cattanem Exp $
#ifndef DICT_LHCBMATHDICT_H 
#define DICT_LHCBMATHDICT_H 1

// Include files

/** @file LHCbMathDict.h dict/LHCbMathDict.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-06-20
 */
#ifdef _WIN32
#pragma warning ( disable : 4305 ) // truncation double to Scalar in GenVector
#pragma warning ( disable : 4244 ) // conversion double to float in GenVector
#endif
#include "LHCbMath/GeomFun.h"
#include "LHCbMath/LineTypes.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "LHCbMath/MD5.h"
#include "LHCbMath/ValueWithError.h"
#include "LHCbMath/Point3DWithError.h"
#include "LHCbMath/Vector3DWithError.h"
#include "LHCbMath/LorentzVectorWithError.h"
#include <vector>
#include <list>
namespace Gaudi
{
  // ==========================================================================
  namespace Math 
  {
    // ========================================================================
    template <typename aPoint, typename aLine, typename aPlane>
    struct GF
    {
      static bool intersection( const aLine&  line      ,
                                const aPlane& plane     ,
                                aPoint&       intersect ,
                                double&       mu        )
      {
        return Gaudi::Math::intersection<aLine, aPlane, aPoint>(line, 
                                                                plane, 
                                                                intersect, 
                                                                mu);
      }

      static bool intersection( const aPlane& plane0    ,
                                const aPlane& plane1    ,
                                aLine&        intersect ) 
      {
        return Gaudi::Math::intersection<aLine, aPlane>(plane0,
                                                        plane1,
                                                        intersect);
      }
      
      static  bool intersection( const aPlane& plane0    ,
                                 const aPlane& plane1    ,
                                 const aPlane& plane2    ,
                                 aPoint&       intersect )
      {
        return Gaudi::Math::intersection<aPoint, aPlane>(plane0,
                                                         plane1,
                                                         plane2,
                                                         intersect);
      }

      static double impactParameter(const aPoint&  point ,
                                    const aLine&   line  ) 
      {
        return Gaudi::Math::impactParameter<aPoint,aLine>(point, line);
      }

      static double distance( const aLine& line0 ,
                              const aLine& line1 )
      {
        return Gaudi::Math::distance<aLine, aLine>(line0, line1);
      }
      
      static  bool closestPoints( const aLine& line0 ,
                                  const aLine& line1 ,
                                  aPoint&      p0    ,
                                  aPoint&      p1    )
      {
        return Gaudi::Math::closestPoints<aLine, aLine, aPoint>(line0, 
                                                                line1, 
                                                                p0, 
                                                                p1);
      }
      
      static double closestPointParam( const aPoint&  point ,
                                       const aLine&   line  )
      {
        return Gaudi::Math::closestPointParam<aLine, aPoint>(point, line);
      }
      
      static aPoint closestPoint(const aPoint&  point ,
                          const aLine& line)
      {
        return Gaudi::Math::closestPoint<aLine, aPoint>(point, line);
      }
      
      static bool closestPointParams( const aLine& line0 ,
                                      const aLine& line1 ,
                                      double&      mu0   , 
                                      double&      mu1   ) 
      {
        return Gaudi::Math::closestPointParams<aLine, aLine>(line0,
                                                             line1,
                                                             mu0,
                                                             mu1);
        
      }
      
      static bool parallel( const aLine& line0 ,
                            const aLine& line1 )
      {
        return Gaudi::Math::parallel<aLine, aLine>(line0, line1);
      }

    };
    typedef GF<XYZPoint, XYZLine, Plane3D> XYZGeomFun;
    
    // ========================================================================
    class Functions 
    {
    public:
      // ====================================================================== 
      /// evaluate the mean of a and b 
      static ValueWithError mean
      ( const ValueWithError& a , 
        const ValueWithError& b ) { return a.mean ( b ) ; }
      // =====================================================================-
      /// evaluate chi2 
      static double chi2
      ( const ValueWithError& a , 
        const ValueWithError& b ) { return a.chi2 ( b ) ; }
      /// evaluate chi2 
      static  double chi2
      ( const ValueWithError& a , 
        const double          b ) { return a.chi2 ( b ) ; }
      /// evaluate chi2 
      static double chi2
      ( const double          b ,
        const ValueWithError& a ) { return a.chi2 ( b ) ; }
      // ======================================================================
      /// evaluate the "fraction"  a/(a+b)
      static ValueWithError frac 
      ( const ValueWithError& a , 
        const ValueWithError& b ) { return a.frac ( b ) ; }
      /// evaluate the "fraction"  a/(a+b)
      static ValueWithError frac 
      ( const ValueWithError& a , 
        const double          b ) { return a.frac ( b ) ; }
      /// evaluate the "fraction"  a/(a+b)
      static ValueWithError frac 
      ( const double          a , 
        const ValueWithError& b ) { return frac ( ValueWithError ( a ) , b ) ; }
      /// evaluate the "fraction"  a/(a+b)
      static ValueWithError frac 
      ( const double          a , 
        const double          b ) { return frac ( ValueWithError ( a ) , b ) ; }
      // ======================================================================
      /// evaluate the "asymmetry"  (a-b)/(a+b)
      static ValueWithError asym 
      ( const ValueWithError& a , 
        const ValueWithError& b ) { return a.asym ( b ) ; }
      /// evaluate the "asymmetry"  (a-b)/(a+b)
      static ValueWithError asym 
      ( const ValueWithError& a , 
        const double          b ) { return a.asym ( b ) ; }
      /// evaluate the "asymmetry"  (a-b)/(a+b)
      static ValueWithError asym 
      ( const double          a , 
        const ValueWithError& b ) { return asym ( ValueWithError ( a ) , b ) ; }
      /// evaluate the "asymmetry"  (a-b)/(a+b)
      static ValueWithError asym 
      ( const double          a , 
        const double          b ) { return asym ( ValueWithError ( a ) , b ) ; }
      // ======================================================================
      /** evaluate the binomial efficiency for Bernulli scheme 
       *  @param n (INPUT) number of 'success' 
       *  @param N (INPUT) total number 
       *  @return the binomial efficiency 
       */
      static ValueWithError binomEff   
      ( const size_t n , 
        const size_t N ) 
      { return Gaudi::Math::binomEff ( n , N ) ; }
      // ========================================================================
    } ;
    // ========================================================================
  } //                                             end of namespace Gaudi::Math 
  // ==========================================================================
} //                                                     end of namespace Gaudi 
// ============================================================================
namespace
{
  // ==========================================================================
  struct __Instantiations
  {
    __Instantiations();
    
#ifndef WIN32 // FIXME: Does not compile on Windows
    Gaudi::Math::XYZLine       __lineXYZ;
    Gaudi::Math::Polar3DLine   __linePolar3D;
    Gaudi::Math::RhoEtaPhiLine __lineRhoEtaPhi;
    Gaudi::Math::RhoZPhiLine   __lineRhoZPhi;

    Gaudi::Math::XYZLineF       __lineXYZF;
    Gaudi::Math::Polar3DLineF   __linePolar3DF;
    Gaudi::Math::RhoEtaPhiLineF __lineRhoEtaPhiF;
    Gaudi::Math::RhoZPhiLineF   __lineRhoZPhiF;
#endif // WIN32
    Gaudi::Math::XYZGeomFun __geomFunXYZ;

    Gaudi::Math::MD5       __mathMD5;
    std::vector<Gaudi::Math::MD5>       __stdvector_mathMD5;
    std::list<Gaudi::Math::MD5>       __stdlist_mathMD5;
    //
    Gaudi::Math::SVectorWithError<2,double> __sv2 ;
    Gaudi::Math::SVectorWithError<3,double> __sv3 ;
    Gaudi::Math::SVectorWithError<4,double> __sv4 ;
    Gaudi::Math::SVectorWithError<5,double> __sv5 ;
    //
    std::vector<Gaudi::Math::ValueWithError>  _dver ;
  };
}

#endif // DICT_LHCBMATHDICT_H
