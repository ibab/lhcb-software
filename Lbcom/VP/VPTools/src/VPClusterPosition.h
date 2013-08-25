#ifndef VPCLUSTERPOSITION_H 
#define VPCLUSTERPOSITION_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiMath/GaudiMath.h"

// LHCb
#include "Kernel/VPChannelID.h"
#include "Kernel/PixelPositionInfo.h"
#include "Event/StateVector.h"
#include "TrackInterfaces/IVPClusterPosition.h"

/** @class VPClusterPosition VPClusterPosition.h
 *  
 *  @author Victor Coco (based on  Tomasz Szumlak VeloClusterPosition)
 *  @date   2010-02-02
 */

class DeVP;
class DeVPSensor;

class VPClusterPosition: public GaudiTool, virtual public IVPClusterPosition {

public:
  /// Constructor
  VPClusterPosition(const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);
  /// Destructor
  virtual ~VPClusterPosition();

  // Throughout the code LA stands for Linear Approximation
  // typedefs for object returned by tool
  typedef IVPClusterPosition::toolInfo toolInfo;
  typedef IVPClusterPosition::Direction Direction;
  typedef std::pair<double, double> Pair;

  virtual StatusCode initialize();

  virtual toolInfo position(const LHCb::VPLiteCluster* cluster) const;
  virtual toolInfo position(const LHCb::VPLiteCluster* cluster,
                            const Gaudi::XYZPoint& point,
                            const Direction& direction) const;
  virtual toolInfo position(const LHCb::VPLiteCluster* cluster,
                            const LHCb::StateVector& state) const;
  
private:

  toolInfo position(const LHCb::VPChannelID& channel, const Pair& frac,
                    const Gaudi::XYZPoint& point,
                    const Direction& direction) const;

  // Error estimate when full state is available
  Pair errorEstimate(const Pair projAngle, const Pair pixelSize) const;
  // Calculate the projected angles
  Pair projectedAngles(const DeVPSensor* sensor, Gaudi::XYZVector track,
                       Gaudi::XYZPoint globalPoint) const;
  
  /// Detector element
  DeVP* m_det;
  /// Parameters for polynomial fit of resolution as function of proj. angle
  std::vector<double> m_p; 
    
};

#endif
