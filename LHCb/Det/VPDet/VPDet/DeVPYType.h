// $Id: DeVPYType.h,v 1.3 2009-10-21 11:19:28 cocov Exp $
#ifndef VPDET_DEVPYTYPE_H 
#define VPDET_DEVPYTYPE_H 1

// Include files

// Gaudi Math definitions
#include "GaudiKernel/Point3DTypes.h"

// from VPDet
#include "VPDet/DeVPSensor.h"

#include "VPDet/PixelLadder.h"

// get trajectory
#include "Kernel/Trajectory.h"

// Unique class identifier
static const CLID CLID_DeVPYType = 1008203 ;

// Forward declaration needed for link to associated sensor
class DeVPYType;

namespace LHCb {
  class VPChannelID;
}

/** @class DeVPYType DeVPYType.h VPDet/DeVPYType.h
 *  
 *
 *  @author Victor Coco
 *  @date   2009-05-14
 */

class DeVPYType : public DeVPSensor {
public:
  /// Standard constructor
  DeVPYType( const std::string& name = "" ); 

  virtual ~DeVPYType( ); ///< Destructor

  /// object identifier (static method)
  static  const CLID& classID() { return CLID_DeVPYType; }
  /// object identification
  virtual const CLID& clID()     const;

  /// Initialise the DeVPSensor from the XML
  virtual StatusCode initialize();

  /// Calculate the nearest channel to a 3-d point.
  /// Also returns the fractional difference in the channel
  /// and the local pitch.
  virtual int WhichLadder(const Gaudi::XYZPoint& point) const;
  
  virtual std::pair<int,int> WhichPixel(const Gaudi::XYZPoint& point, int ladderIndex, int chipIndex,std::pair <double, double>& fraction) const;
  
  virtual int WhichChip(const Gaudi::XYZPoint& point, int ladderIndex) const;
  

  virtual StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                                    LHCb::VPChannelID& channel,
                                    std::pair <double, double>& fraction) const;

  /// Determines if local 3-d point is inside sensor
  virtual StatusCode isInActiveArea(const Gaudi::XYZPoint& point) const;

  /// Access to the associated Y sensor on the same module
  inline const DeVPXType* associatedXSensor() const { return m_associatedXSensor; }
    
  /// Access to the x sensor on the other side of the VELO
  inline const DeVPXType* otherSideXSensor() const { return m_otherSideXSensor; }
  
  /// Access to the y sensor on the other side of the VELO
  inline const DeVPYType* otherSideYSensor() const { return m_otherSideYSensor; }
  
  /// Set the associated y sensor.  This should only be called by DeVP::initialize()
  inline void setAssociatedXSensor(const DeVPXType* ys) { m_associatedXSensor = ys; }
  
  /// Set the r sensor on the other side of the VELO.  This should only be called by DeVP::initialize()
  inline void setOtherSideXSensor(const DeVPXType* xs) { m_otherSideXSensor = xs; }

  /// Set the y sensor on the other side of the VELO.  This should only be called by DeVP::initialize()
  inline void setOtherSideYSensor(const DeVPYType* ys) { m_otherSideYSensor = ys; }


private:
  /// Store the local radius for each strip in the sensor
  //void calcStripLimits();
  
  /// Update geomtry cache when the alignment changes
  StatusCode updateGeometryCache();
    
private:
  /// list of pixel ladders
  std::vector<PixelLadder> m_ladders;

  /// pointer to associated phi sensor
  const DeVPXType* m_associatedXSensor;
  
  /// pointer to the r sensor on the other side of the VELO
  const DeVPXType* m_otherSideXSensor;
  
  /// pointer to the phi sensor on the other side of the VELO
  const DeVPYType* m_otherSideYSensor;

  // used to control initialization NEVER ACCESS THIS IN AN INLINED METHOD!
  static bool m_staticDataInvalid;
  // Set output level for message service
  bool m_debug;
  bool m_verbose;

};


/// fast cast to Y sensor, returns 0 for wrong type
inline const DeVPYType* DeVPSensor::yType() const { 
  return (m_isY ? static_cast<const DeVPYType*>(this) : 0); 
}
  

#endif // VPDET_DEVPYTYPE_H
