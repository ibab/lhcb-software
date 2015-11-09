#pragma once

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Point3DTypes.h"
// Det/VPDet
#include "VPDet/DeVP.h"

static const InterfaceID IID_VPRadiationDamageTool ("VPRadiationDamageTool", 1, 0);

/** @class VPRadiationDamageTool VPRadiationDamageTool.h
 *
 */

class VPRadiationDamageTool : public GaudiTool {

public: 
  /// Return the interface ID
  static const InterfaceID& interfaceID() {return IID_VPRadiationDamageTool;}

  /// Standard constructor
  VPRadiationDamageTool(const std::string& type, const std::string& name,
                        const IInterface* parent);
  /// Destructor
  virtual ~VPRadiationDamageTool();

  virtual StatusCode initialize();

  /// Fluence as function of position and integrated luminosity (in fb-1).
  double fluence(const Gaudi::XYZPoint& point, const double lint);
  /// Charge collection efficiency as function of 1 MeV n eq. fluence and bias.
  double chargeCollectionEfficiency(const double f, const double v);

private:
  DeVP* m_det;
  std::vector<double> m_a;
  std::vector<double> m_k;
};

