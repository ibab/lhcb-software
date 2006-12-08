// $Id: STChargeSharingTool.h,v 1.3 2006-12-08 09:54:23 mneedham Exp $
#ifndef _STCHARGESHARINGTOOL_H
#define _STCHARGESHARINGTOOL_H

#include <string>
#include <vector>

#include "GaudiAlg/GaudiTool.h"
#include "ISTChargeSharingTool.h"
#include "GaudiMath/GaudiMath.h"


/** @class STChargeSharingTool STChargeSharingTool.h STAlgorithms/STChargeSharingTool.h
 *
 *  Interface Class for charge sharing ST strip
 *
 *  @author M.Needham
 *  @date   14/3/2002
 */

// Declaration of the interface ID ( interface id, major version, minor version) 

class STChargeSharingTool : public GaudiTool, virtual public ISTChargeSharingTool {

public: 

  STChargeSharingTool(const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  /// destructer
  virtual ~STChargeSharingTool();

  virtual StatusCode initialize();

  /// calc rel fraction
  double sharing(const double relDist) const;

private:
 
  std::vector<double> m_sharingFunction;
  std::vector<double> m_binCenters;

  GaudiMath::SimpleSpline* m_responseSpline;

};

#endif // _STCHARGESHARINGTOOL_H



