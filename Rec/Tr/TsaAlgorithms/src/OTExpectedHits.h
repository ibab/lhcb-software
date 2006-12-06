// $Id: OTExpectedHits.h,v 1.5 2006-12-06 14:35:00 mneedham Exp $
#ifndef _OTExpectedHits_H
#define _OTExpectedHits_H

/** @class OTExpectedHits OTExpectedHits.h
* How many hits do we expect in in the OT
* @author M. Needham
* @date 30.05.2004
*/ 

#include "GaudiAlg/GaudiTool.h"
#include "TsaKernel/IOTExpectedHits.h"
#include "Kernel/Point3DTypes.h"
#include "TsaKernel/Line3D.h"
#include "Kernel/Plane3DTypes.h"

#include "GaudiKernel/VectorMap.h"


class DeOTDetector;
class DeOTModule;


class OTExpectedHits: public GaudiTool,
                     virtual public IOTExpectedHits {

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

  
  typedef GaudiUtils::VectorMap<unsigned int,DeOTModule*> ModuleMap;
  ModuleMap m_modMap;

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



#endif // _OTExpectedHits_H
