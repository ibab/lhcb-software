// $Id$
// ============================================================================
#ifndef DICT_LHCBMATHDICT_H
#define DICT_LHCBMATHDICT_H 1
// ============================================================================
// Include files
// ============================================================================
#include <vector>
#include <list>
// ============================================================================
#ifdef _WIN32
#pragma warning ( disable : 4305 ) // truncation double to Scalar in GenVector
#pragma warning ( disable : 4244 ) // conversion double to float in GenVector
#endif
// ============================================================================
#include "LHCbMath/GeomFun.h"
#include "LHCbMath/LineTypes.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "LHCbMath/MD5.h"
#include "LHCbMath/ValueWithError.h"
#include "LHCbMath/Point3DWithError.h"
#include "LHCbMath/Vector3DWithError.h"
#include "LHCbMath/LorentzVectorWithError.h"
#include "LHCbMath/ParticleParams.h"
#include "LHCbMath/Blind.h"
#include "LHCbMath/EigenSystem.h"
#include "LHCbMath/Functions.h"
#include "LHCbMath/LHCbMath.h"
#include "LHCbMath/Chi2Solution.h"
#include "LHCbMath/Chi2Fit.h"
#include "LHCbMath/Spline.h"
#include "LHCbMath/NSphere.h"
#include "LHCbMath/WStatEntity.h"
#include "LHCbMath/NStatEntity.h"
#include "LHCbMath/BSpline.h"
#include "LHCbMath/Bernstein.h"
#include "LHCbMath/Polynomials.h"
#include "LHCbMath/Choose.h"
#include "LHCbMath/MoreFunctions.h"
#include "LHCbMath/Binomial.h"
#include "LHCbMath/Combine.h"
// ============================================================================
/** @file LHCbMathDict.h
 *
 *  @author Juan PALACIOS
 *  @date   2008-06-20
 */
// ============================================================================
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
      // ======================================================================
    } ;
    // ========================================================================
    class EigenSystems
    {
      // ======================================================================
    public : // eigen values
      // ======================================================================
      // 2x2
      static Gaudi::Vector2 eigenValues
      ( const Gaudi::SymMatrix2x2& mtrx          ,
        const bool                 sorted = true )
      {
        Gaudi::Math::GSL::EigenSystem system ;
        return system.eigenValues ( mtrx , sorted ) ;
      }
      // 3x3
      static Gaudi::Vector3 eigenValues
      ( const Gaudi::SymMatrix3x3& mtrx          ,
        const bool                 sorted = true )
      {
        Gaudi::Math::GSL::EigenSystem system ;
        return system.eigenValues ( mtrx , sorted ) ;
      }
      // 4x4
      static Gaudi::Vector4 eigenValues
      ( const Gaudi::SymMatrix4x4& mtrx          ,
        const bool                 sorted = true )
      {
        Gaudi::Math::GSL::EigenSystem system ;
        return system.eigenValues ( mtrx , sorted ) ;
      }
      // ======================================================================
    public: // eigen vectors
      // ======================================================================
      // 2x2
      static StatusCode eigenVectors
      ( const Gaudi::SymMatrix2x2&   mtrx          ,
        Gaudi::Vector2&              vals          ,
        std::vector<Gaudi::Vector2>& vecs          ,
        const bool                   sorted = true )
      {
        Gaudi::Math::GSL::EigenSystem system ;
        return system.eigenVectors ( mtrx , vals , vecs , sorted ) ;
      }
      // 3x3
      static StatusCode eigenVectors
      ( const Gaudi::SymMatrix3x3&   mtrx          ,
        Gaudi::Vector3&              vals          ,
        std::vector<Gaudi::Vector3>& vecs          ,
        const bool                   sorted = true )
      {
        Gaudi::Math::GSL::EigenSystem system ;
        return system.eigenVectors ( mtrx , vals , vecs , sorted ) ;
      }
      // 4x4
      static StatusCode eigenVectors
      ( const Gaudi::SymMatrix4x4&   mtrx          ,
        Gaudi::Vector4&              vals          ,
        std::vector<Gaudi::Vector4>& vecs          ,
        const bool                   sorted = true )
      {
        Gaudi::Math::GSL::EigenSystem system ;
        return system.eigenVectors ( mtrx , vals , vecs , sorted ) ;
      }
      // ======================================================================
    } ;
    // ========================================================================
  } //                                             end of namespace Gaudi::Math
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
namespace
{
  // ==========================================================================
  struct LHCbMath_Instantiations
  {
    LHCbMath_Instantiations();

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
    Gaudi::Math::SVectorWithError<6,double> __sv6 ;
    Gaudi::Math::SVectorWithError<8,double> __sv8 ;
    //
    std::vector<Gaudi::Math::ValueWithError>  _dver ;
    std::vector<std::vector<Gaudi::Math::ValueWithError> > _dver2 ;
    //
    std::vector<Gaudi::Vector2>  _vct_2 ;
    std::vector<Gaudi::Vector3>  _vct_3 ;
    std::vector<Gaudi::Vector3>  _vct_4 ;
    //
    Gaudi::Math::Chi2Solution<4,2>          __cs11 ;
    Gaudi::Math::Chi2Solution<4,2>::DATA    __cs21 ;
    Gaudi::Math::Chi2Solution<4,2>::COV2    __cs31 ;
    Gaudi::Math::Chi2Solution<4,2>::CMTRX2  __cs41 ;
    Gaudi::Math::Chi2Solution<4,2>::COFF    __cs51 ;
    Gaudi::Math::Chi2Solution<4,2>::VECT    __cs61 ;

    Gaudi::Math::Chi2Solution<6,2>          __cs12 ;
    Gaudi::Math::Chi2Solution<6,2>::DATA    __cs22 ;
    Gaudi::Math::Chi2Solution<6,2>::COV2    __cs32 ;
    Gaudi::Math::Chi2Solution<6,2>::CMTRX2  __cs42 ;
    Gaudi::Math::Chi2Solution<6,2>::COFF    __cs52 ;
    Gaudi::Math::Chi2Solution<6,2>::VECT    __cs62 ;

    Gaudi::Math::Splines::DATA    _sd_1 ;
    Gaudi::Math::Splines::PAIR    _sd_2 ;
    Gaudi::Math::Splines::DATAERR _sd_3 ;
    //
    LHCb::Math::Equal_To<double>               __eq_1 ;
    LHCb::Math::Equal_To<std::vector<double> > __eq_2 ;
    LHCb::Math::Zero<double>                   __eq_3 ;
    LHCb::Math::Zero<std::vector<double> >     __eq_4 ;
    LHCb::Math::NotZero<double>                __eq_5 ;
    LHCb::Math::NotZero<std::vector<double> >  __eq_6 ;
    //
    LHCb::Math::LessOrEqual<double>     __eq_7 ;
    LHCb::Math::GreaterOrEqual<double>  __eq_8 ;
    //
  };
}

#endif // DICT_LHCBMATHDICT_H
