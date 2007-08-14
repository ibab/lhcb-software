// $Id: OTExpectedHits.h,v 1.1.1.1 2007-08-14 13:50:47 jonrob Exp $
#ifndef _OTExpectedHits_H
#define _OTExpectedHits_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"

// Tsa Kernel
#include "TsaKernel/IOTExpectedHits.h"
#include "TsaKernel/Parabola.h"
#include "TsaKernel/Line.h"
#include "TsaKernel/Line3D.h"

// Geometry
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTQuarter.h"
#include "OTDet/DeOTModule.h"

// Kernel
#include "Kernel/OTChannelID.h"
#include "LHCbMath/GeomFun.h"

namespace Tf
{
  namespace Tsa
  {

    /** @class OTExpectedHits OTExpectedHits.h
     * How many hits do we expect in in the OT detector
     * @author M. Needham
     * @date 30.05.2004
     */

    class OTExpectedHits: public GaudiTool,
                          virtual public IOTExpectedHits
    {

    public:

      /// constructer
      OTExpectedHits(const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

      // destructer
      virtual ~OTExpectedHits();

      /// init
      virtual StatusCode initialize();

      // execute method
      virtual StatusCode collect(const Tsa::Parabola& parab,
                                 const Tsa::Line& line,
                                 const LHCb::OTChannelID& aChan,
                                 std::vector<IOTExpectedHits::OTPair>& hits,
                                 const unsigned int iSector ) const;

    private:

      bool insideModule(const DeOTModule* layer,
                        const Tsa::Line3D& line) const;


      Gaudi::XYZPoint intersection(const Tsa::Line3D& line, const DeOTModule* module,
                                   const Gaudi::XYZPoint& aPoint) const;


      Gaudi::XYZPoint intersection(const Tsa::Line3D& line, const Gaudi::Plane3D& aPlane) const;

      bool correctSector(const unsigned int quart, const unsigned int iSector) const;

      DeOTModule* findModule(const Tsa::Parabola& parab,
                             const Tsa::Line& line,
                             const LHCb::OTChannelID& aChan,
                             const unsigned int iSector) const;

      DeOTDetector* m_tracker;

    };

    inline bool OTExpectedHits::correctSector(const unsigned int quart, const unsigned int iSector) const {

      return (iSector == 3 ? quart < 2 : quart > 1);
    }

  }
}

#endif // _OTExpectedHits_H
