// $Id: LHCbMathDict.h,v 1.1 2008-06-24 08:12:05 jpalac Exp $
#ifndef DICT_LHCBMATHDICT_H 
#define DICT_LHCBMATHDICT_H 1

// Include files

/** @file LHCbMathDict.h dict/LHCbMathDict.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-06-20
 */
#include "LHCbMath/GeomFun.h"
#include "LHCbMath/LineTypes.h"
#include "GaudiKernel/Plane3DTypes.h"
namespace Gaudi
{
  namespace Math 
  {
    template <typename aPoint, typename aLine, typename aPlane>
    struct GF
    {
      static double impactParameter(const aPoint&  point ,
                                    const aLine&   line  ) 
      {
        return Gaudi::Math::impactParameter<aPoint,aLine>(point, line);
      }
      
    };
    typedef GF<XYZPoint, XYZLine, Plane3D> XYZGeomFun;
  }
  
}


namespace
{

  struct __Instantiations
  {
    __Instantiations();

    Gaudi::XYZLine       __lineXYZ;
    Gaudi::Polar3DLine   __linePolar3D;
    Gaudi::RhoEtaPhiLine __lineRhoEtaPhi;
    Gaudi::RhoZPhiLine   __lineRhoZPhi;

    Gaudi::XYZLineF       __lineXYZF;
    Gaudi::Polar3DLineF   __linePolar3DF;
    Gaudi::RhoEtaPhiLineF __lineRhoEtaPhiF;
    Gaudi::RhoZPhiLineF   __lineRhoZPhiF;

    Gaudi::Math::XYZGeomFun __geomFunXYZ;

    //    double 
    //    Gaudi::Math::intersection<Gaudi::XYZPoint,Gaudi::XYZLine>(const Gaudi::XYZPoint& point,
    //                                                              const Gaudi::XYZLine line);
    
  };
}

#endif // DICT_LHCBMATHDICT_H
