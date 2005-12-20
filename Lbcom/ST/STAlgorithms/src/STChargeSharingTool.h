// $Id: STChargeSharingTool.h,v 1.2 2005-12-20 15:50:25 cattanem Exp $
#ifndef _STCHARGESHARINGTOOL_H
#define _STCHARGESHARINGTOOL_H

#include <string>
#include <vector>

#include "GaudiAlg/GaudiTool.h"
#include "ISTChargeSharingTool.h"

class GSLSpline;

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

  GSLSpline* m_responseSpline; 
 

};

#endif // _STCHARGESHARINGTOOL_H



