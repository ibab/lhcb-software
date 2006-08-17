// $Id: ITExpectedHits.h,v 1.2 2006-08-17 08:36:07 mneedham Exp $
#ifndef _ITExpectedHits_H
#define _ITExpectedHits_H

/** @class ITExpectedHits ITExpectedHits.h
* How many hits do we expect in in the OT
* @author M. Needham
* @date 30.05.2004
*/ 

#include "GaudiAlg/GaudiTool.h"
#include "TsaKernel/IITExpectedHits.h"
#include "Kernel/Point3DTypes.h"
#include "TsaKernel/Line3D.h"



class DeSTDetector;
class DeSTSector;


class ITExpectedHits: public GaudiTool,
                     virtual public IITExpectedHits {

public:

  /// constructer
  ITExpectedHits(const std::string& type,
                const std::string& name,
                const IInterface* parent);

  // destructer
  virtual ~ITExpectedHits();

  /// init
  virtual StatusCode initialize();

  // execute method
  virtual StatusCode collect(const Tsa::Parabola& parab,
                             const Tsa::Line& line,
                             const LHCb::STChannelID& aChan,
                             std::vector<IITExpectedHits::ITPair>& hits ,
                             const unsigned int iSector) const;

private:

  bool insideSector(const DeSTSector* sector,
                    const Tsa::Line3D& line) const;

  
  Gaudi::XYZPoint intersection(const Tsa::Line3D& line, const DeSTSector* sector,
			       const Gaudi::XYZPoint& aPoint) const;

  DeSTDetector* m_tracker;
  
};

#endif // _ITExpectedHits_H
