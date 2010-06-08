// $Id: STChargeSharingTool.h,v 1.2 2010-04-07 09:27:38 mneedham Exp $
#ifndef STCHARGESHARINGTOOL_H
#define STCHARGESHARINGTOOL_H 1

// Gaudi
#include "GaudiMath/GaudiMath.h"

#include "Kernel/STToolBase.h"

// local interface
#include "ISTChargeSharingTool.h"

/** @class STChargeSharingTool STChargeSharingTool.h
 *
 *  Tool for charge sharing ST strip
 *
 *  @author M.Needham
 *  @date   14/3/2002
 */

class STChargeSharingTool : public ST::ToolBase, 
                            virtual public ISTChargeSharingTool {

public: 

  STChargeSharingTool( const std::string& type, const std::string& name,
                       const IInterface* parent );

  /// destructer
  virtual ~STChargeSharingTool();

  virtual StatusCode initialize();

  /// calculate charge fraction as function of the relative distance
  double sharing(const double relDist) const;

  /// return thickness of the corresponding sensor
  double thickness() const;

private:

  /// Spline function to fit to the charge sharing function
  GaudiMath::SimpleSpline* m_responseSpline;

  // Job options:
  /// Fraction of charge on strip as funtion of the relative distance
  std::vector<double> m_sharingFunction; 
  bool m_useAnalyticErf; ///< Flag to switch to using the analytic error functn 
  double m_erfWidth;     ///< Width of the charge sharing (in terms of pitch).
  double m_thickness;    ///< Sensor thickness corresponding to the parameters

};

#endif // STCHARGESHARINGTOOL_H
