
#ifndef VELOPIXCLUSTERPOSITION_H 
#define VELOPIXCLUSTERPOSITION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/IVeloPixClusterPosition.h"            // Interface
#include "Kernel/VeloPixChannelID.h"
#include "Kernel/PixelPositionInfo.h"
#include "Event/StateVector.h"
#include "GaudiMath/GaudiMath.h"

/** @class VeloPixClusterPosition VeloPixClusterPosition.h
 *  
 *
 *  @author Victor Coco (based on  Tomasz Szumlak VeloClusterPosition)
 *  @date   2010-02-02
 */

class DeVeloPix;
class DeVeloPixSensor;

class VeloPixClusterPosition: public GaudiTool, virtual public IVeloPixClusterPosition{
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
  
  typedef IVeloPixClusterPosition::toolInfo toolInfo;
  typedef IVeloPixClusterPosition::Direction Direction;
  typedef std::pair<double, double> Pair;

  virtual StatusCode initialize();    ///< Tool initialization
  virtual StatusCode finalize();      ///< Tool finalization
  /// Standard constructor
  VeloPixClusterPosition( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);
  virtual ~VeloPixClusterPosition( ); ///< Destructor
  //-- public interface ----------------------------------------------
  virtual toolInfo position(const LHCb::VeloPixLiteCluster* aCluster) const;

  virtual toolInfo position(const LHCb::VeloPixLiteCluster* aCluster,
                            const Gaudi::XYZPoint& aPoint,
                            const Direction& aDirection) const;

  virtual toolInfo position(const LHCb::VeloPixLiteCluster* aCluster,
                            const LHCb::StateVector& aState) const;
  //------------------------------------------------------------------
  //-- returns value of the projected angle
  Pair projectedAngle() const;

  
protected:

  /// full/lite cluster position implementation code
  toolInfo position(const LHCb::VeloPixChannelID &centreChannel,
                    const Pair &fractionalPos) const;
  
  /// full/lite cluster position implementation code with point and direction  
  toolInfo position(const LHCb::VeloPixChannelID &centreChan,
                                          const Pair & fracPos,
                                          const Gaudi::XYZPoint& aGlobalPoint,
                    const Direction& aDirection) const;

  //-- this method can be used when full state is available
  Pair errorEstimate(const Pair projAngle, const Pair pixelSize) const;
  
  //-- calculates the value of the projected angle
  Pair projectedAngle(const DeVeloPixSensor* sensor) const;
  
private:

  DeVeloPix* m_veloPixDet;                  /// detector element
  std::vector<double> m_defaultResolution;  /// resolution para one angle
  mutable std::vector< std::pair<double,double> > m_errAnglePara;       /// angle projection para

  mutable Gaudi::XYZVector m_trackDir;      /// a track direction in global ref. frame
  mutable Gaudi::XYZPoint m_gloPoint;       /// point on sensor global ref. frame
  mutable Pair m_projectedAngle;            /// value of projected angle
  Pair m_fracPos;                           /// fractional position
    
};
#endif // VELOCLUSTERPOS_H
