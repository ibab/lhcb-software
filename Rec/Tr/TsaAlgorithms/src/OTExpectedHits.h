// $Id: OTExpectedHits.h,v 1.4 2006-09-05 15:48:51 mneedham Exp $
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

  bool insideModule(const DeOTModule* layer,
                    const Tsa::Line3D& line) const;

  
  Gaudi::XYZPoint intersection(const Tsa::Line3D& line, const DeOTModule* module,
			       const Gaudi::XYZPoint& aPoint) const;


  Gaudi::XYZPoint intersection(const Tsa::Line3D& line, const Gaudi::Plane3D& aPlane) const;

  bool correctSector(const unsigned int quart, const unsigned int iSector) const;


  DeOTDetector* m_tracker;
  //  Gaudi::XYZPoint m_entry;
  //Gaudi::XYZPoint m_exit;  

};

inline bool OTExpectedHits::correctSector(const unsigned int quart, const unsigned int iSector) const {
  //  bool correct = false;
  // if ((iSector == 0) && (quart < 2)) correct = true;
  // if ((iSector == 1) && (quart > 1)) correct = true;
  // return correct;
  return (iSector == 3 ? quart < 2 : quart > 1);
}



#endif // _OTExpectedHits_H
