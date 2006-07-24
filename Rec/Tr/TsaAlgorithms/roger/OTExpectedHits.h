// $Id: OTExpectedHits.h,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $
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

class DeOTDetector;
class DeOTModule;

namespace Tsa{
  class Line3D;
}

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
                             std::vector<IOTExpectedHits::OTPair>& hits ) const;

private:

  bool insideModule(const DeOTModule* layer,
                    const Tsa::Line3D& line) const;

  
  Gaudi::XYZPoint intersection(const Tsa::Line3D& line, const DeOTModule* module,
			       const Gaudi::XYZPoint& aPoint) const;

  DeOTDetector* m_tracker;
  
};

#endif // _OTExpectedHits_H
