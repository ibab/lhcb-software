// $Id: STChargeSharingTool.h,v 1.5 2008-02-15 13:29:46 cattanem Exp $
#ifndef STCHARGESHARINGTOOL_H
#define STCHARGESHARINGTOOL_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiMath/GaudiMath.h"

// From LHCbKernel
#include "Kernel/ISharingTool.h"

/** @class STChargeSharingTool STChargeSharingTool.h
 *
 *  Tool for charge sharing ST strip
 *
 *  @author M.Needham
 *  @date   14/3/2002
 */

class STChargeSharingTool : public GaudiTool, 
                            virtual public ISharingTool {

public: 

  STChargeSharingTool( const std::string& type, const std::string& name,
                       const IInterface* parent );

  /// destructer
  virtual ~STChargeSharingTool();

  virtual StatusCode initialize();

  /// calculate charge fraction as function of the relative distance
  double sharing(const double relDist) const;

private:

  /// Spline function to fit to the charge sharing function
  GaudiMath::SimpleSpline* m_responseSpline;

  // Job option:
  /// Fraction of charge on strip as funtion of the relative distance
  std::vector<double> m_sharingFunction; 

};

#endif // STCHARGESHARINGTOOL_H
