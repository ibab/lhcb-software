// $Id: LHCbMathDict.h,v 1.2 2008-06-24 12:24:27 jpalac Exp $
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

    Gaudi::Math::XYZLine       __lineXYZ;
    Gaudi::Math::Polar3DLine   __linePolar3D;
    Gaudi::Math::RhoEtaPhiLine __lineRhoEtaPhi;
    Gaudi::Math::RhoZPhiLine   __lineRhoZPhi;

    Gaudi::Math::XYZLineF       __lineXYZF;
    Gaudi::Math::Polar3DLineF   __linePolar3DF;
    Gaudi::Math::RhoEtaPhiLineF __lineRhoEtaPhiF;
    Gaudi::Math::RhoZPhiLineF   __lineRhoZPhiF;

    Gaudi::Math::XYZGeomFun __geomFunXYZ;
    
  };
}

#endif // DICT_LHCBMATHDICT_H
