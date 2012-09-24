
#ifndef VPCLUSTERPOSITION_H 
#define VPCLUSTERPOSITION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/IVPClusterPosition.h"            // Interface
#include "Kernel/VPChannelID.h"
#include "Kernel/PixelPositionInfo.h"
#include "Event/StateVector.h"
#include "GaudiMath/GaudiMath.h"

/** @class VPClusterPosition VPClusterPosition.h
 *  
 *
 *  @author Victor Coco (based on  Tomasz Szumlak VeloClusterPosition)
 *  @date   2010-02-02
 */

class DeVP;
class DeVPSensor;

class VPClusterPosition: public GaudiTool, virtual public IVPClusterPosition{
public:

  // throughout the code LA stands for Linear Approximation
  // typedefs for object returned by tool

  enum conv{
    RAD_TO_DEG=180
  };
  //-- type of the error parametrization
  enum paraType{
    PITCH_PARA=1,
    ANGLE_PARA
  };
  
  typedef IVPClusterPosition::toolInfo toolInfo;
  typedef IVPClusterPosition::Direction Direction;
  typedef std::pair<double, double> Pair;

  virtual StatusCode initialize();    ///< Tool initialization
  virtual StatusCode finalize();      ///< Tool finalization
  /// Standard constructor
  VPClusterPosition( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);
  virtual ~VPClusterPosition( ); ///< Destructor
  //-- public interface ----------------------------------------------
  virtual toolInfo position(const LHCb::VPLiteCluster* aCluster) const;

  virtual toolInfo position(const LHCb::VPLiteCluster* aCluster,
                            const Gaudi::XYZPoint& aPoint,
                            const Direction& aDirection) const;

  virtual toolInfo position(const LHCb::VPLiteCluster* aCluster,
                            const LHCb::StateVector& aState) const;
  //------------------------------------------------------------------
  //-- returns value of the projected angle
  Pair projectedAngle() const;

  
protected:

  /// full/lite cluster position implementation code
  toolInfo position(const LHCb::VPChannelID &centreChannel,
                    const Pair &fractionalPos) const;
  
  /// full/lite cluster position implementation code with point and direction  
  toolInfo position(const LHCb::VPChannelID &centreChan,
                                          const Pair & fracPos,
                                          const Gaudi::XYZPoint& aGlobalPoint,
                    const Direction& aDirection) const;

  //-- this method can be used when full state is available
  Pair errorEstimate(const Pair projAngle, const Pair pixelSize) const;
  
  //-- calculates the value of the projected angle
  Pair projectedAngle(const DeVPSensor* sensor) const;
  
private:

  DeVP* m_vPDet;                  /// detector element
  std::vector<double> m_defaultResolution;  /// resolution para one angle
  mutable std::vector< std::pair<double,double> > m_errAnglePara;       /// angle projection para

  mutable Gaudi::XYZVector m_trackDir;      /// a track direction in global ref. frame
  mutable Gaudi::XYZPoint m_gloPoint;       /// point on sensor global ref. frame
  mutable Pair m_projectedAngle;            /// value of projected angle
  Pair m_fracPos;                           /// fractional position
    
};
#endif // VELOCLUSTERPOS_H
