#ifndef VL_CLUSTER_POSITION_H 
#define VL_CLUSTER_POSITION_H 1

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiMath/GaudiMath.h"
// Tr/TrackInterfaces
#include "TrackInterfaces/IVLClusterPosition.h"
// Kernel/LHCbKernel 
#include "Kernel/VLChannelID.h"
#include "Kernel/SiPositionInfo.h"
#include "Event/StateVector.h"

/** @class VLClusterPosition VLClusterPosition.h
 *  
 */

class DeVL;
class DeVLSensor;

class VLClusterPosition: public GaudiTool, virtual public IVLClusterPosition {

public:
  /// Standard constructor
  VLClusterPosition(const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);
  /// Destructor
  virtual ~VLClusterPosition() {}
  
  typedef IVLClusterPosition::toolInfo toolInfo;

  virtual StatusCode initialize();    ///< Tool initialization
  virtual StatusCode finalize();      ///< Tool finalization

  virtual toolInfo position(const LHCb::VLCluster* cluster) const {
    return position(cluster->channelID(), fraction(cluster));
  }
  virtual toolInfo position(const LHCb::VLLiteCluster* cluster) const {
    return position(cluster->channelID(), cluster->interStripFraction());
  }
  virtual toolInfo position(const LHCb::VLCluster* cluster,
                            const Gaudi::XYZPoint& point,
                            const double tx, const double ty) const {
    return position(cluster->channelID(), fraction(cluster), point, tx, ty);
  }
  virtual toolInfo position(const LHCb::VLLiteCluster* cluster,
                            const Gaudi::XYZPoint& point,
                            const double tx, const double ty) const {
    return position(cluster->channelID(), cluster->interStripFraction(),
                    point, tx, ty);
  }
  virtual toolInfo position(const LHCb::VLCluster* cluster,
                            const LHCb::StateVector& state) const;
  virtual toolInfo position(const LHCb::VLLiteCluster* cluster,
                            const LHCb::StateVector& state) const;
  
private:
  /// Calculate position without track information. 
  toolInfo position(const LHCb::VLChannelID& channel,
		    const double& fraction) const;
  /// Calculate position with track information.
  toolInfo position(const LHCb::VLChannelID& channel,
		    const double& fraction,
		    const Gaudi::XYZPoint& point,
		    const double tx, const double ty) const;
  // Calculate error estimate.
  double errorEstimate(double angle, const double pitch) const;
  // Calculate projected angle.
  void projectedAngle(const DeVLSensor* sensor,
                      const LHCb::VLChannelID channel,
                      double tx, double ty, Gaudi::XYZPoint point,
                      double& angle, double& pitch) const;
  double fraction(const LHCb::VLCluster* cluster) const;
  double angleOfTrack(const double tx, const double ty,
                      Gaudi::XYZVector& parallel2Track) const;
  
  /// Pointer to detector element
  DeVL* m_det;                       
  /// Flag to use exact calculation of projected angles or not.
  bool m_exactProjAngle;
  /// Flag to use angle bins (true) or pitch bins (false) for interpolation. 
  bool m_angleBins;

  double m_p0Default;
  double m_p1Default;
  /// Spline interpolation of constant term
  GaudiMath::SimpleSpline* m_p0;          
  /// Spline interpolation of slope
  GaudiMath::SimpleSpline* m_p1;            
  /// Type of spline
  std::string m_splineType; 
  /// Angular range
  double m_minAngle;                        
  double m_maxAngle;                        
 
};
#endif 
