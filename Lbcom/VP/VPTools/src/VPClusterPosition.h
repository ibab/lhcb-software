#ifndef VPCLUSTERPOSITION_H
#define VPCLUSTERPOSITION_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// LHCb
#include "TrackInterfaces/IVPClusterPosition.h"

/** @class VPClusterPosition VPClusterPosition.h
 *
 *  @author Victor Coco
 *  @date   2010-02-02
 */

class DeVP;

class VPClusterPosition : public GaudiTool, virtual public IVPClusterPosition {

 public:
  /// Constructor
  VPClusterPosition(const std::string& type, const std::string& name,
                    const IInterface* parent);
  /// Destructor
  virtual ~VPClusterPosition();

  virtual StatusCode initialize();

  virtual LHCb::VPPositionInfo position(const LHCb::VPCluster* cluster) const;
  virtual LHCb::VPPositionInfo position(const LHCb::VPCluster* cluster,
                                        const Gaudi::XYZPoint& point,
                                        const double& tx,
                                        const double& ty) const;

 private:
  /// Pointer to detector element
  DeVP* m_det;

  /// Cosine squared of rotation angle for each sensor.
  std::vector<double> m_c2;
  /// Sine squared of rotation angle for each sensor.
  std::vector<double> m_s2; 
  /// Average error for single-pixel clusters
  double m_errorSinglePixel;
  /// Average error for two-pixel clusters
  double m_errorTwoPixel;
  /// Average error (all cluster sizes)
  double m_errorAverage;

};

#endif
