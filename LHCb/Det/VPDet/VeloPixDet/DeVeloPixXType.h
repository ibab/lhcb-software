// $Id: DeVeloPixXType.h,v 1.3 2009-10-21 11:19:28 cocov Exp $
#ifndef VELOPIXDET_DEVELOPIXXTYPE_H 
#define VELOPIXDET_DEVELOPIXXTYPE_H 1

// Include files

// Gaudi Math definitions
#include "GaudiKernel/Point3DTypes.h"

// from VeloPixDet
#include "VeloPixDet/DeVeloPixSensor.h"

#include "VeloPixDet/PixelLadder.h"

// get trajectory
#include "Kernel/Trajectory.h"

// Unique class identifier
static const CLID CLID_DeVeloPixXType = 1008202 ;

// Forward declaration needed for link to associated sensor
class DeVeloPixYType;

namespace LHCb {
  class VeloPixChannelID;
}

/** @class DeVeloPixXType DeVeloPixXType.h VeloPixDet/DeVeloPixXType.h
 *  
 *
 *  @author Victor Coco
 *  @date   2009-05-14
 */

class DeVeloPixXType : public DeVeloPixSensor {
public:
  /// Standard constructor
  DeVeloPixXType( const std::string& name = "" ); 

  virtual ~DeVeloPixXType( ); ///< Destructor

  /// object identifier (static method)
  static  const CLID& classID() { return CLID_DeVeloPixXType; }
  /// object identification
  virtual const CLID& clID()     const;

  /// Initialise the DeVeloPixSensor from the XML
  virtual StatusCode initialize();

  /// Calculate the nearest channel to a 3-d point.
  /// Also returns the fractional difference in the channel
  /// and the local pitch.
  virtual int WhichLadder(const Gaudi::XYZPoint& point) const;
  
  virtual std::pair<int,int> WhichPixel(const Gaudi::XYZPoint& point, int ladderIndex, int chipIndex,std::pair <double, double>& fraction) const;
  
  virtual int WhichChip(const Gaudi::XYZPoint& point, int ladderIndex) const;

  virtual StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                                    LHCb::VeloPixChannelID& channel,
                                    std::pair <double, double>& fraction) const = 0;

  /// Determines if local 3-d point is inside sensor
  virtual StatusCode isInActiveArea(const Gaudi::XYZPoint& point) const;

  /// Access to the associated Y sensor on the same module
  inline const DeVeloPixYType* associatedYSensor() const { return m_associatedYSensor; }
    
  /// Access to the x sensor on the other side of the VELO
  inline const DeVeloPixXType* otherSideXSensor() const { return m_otherSideXSensor; }
  
  /// Access to the y sensor on the other side of the VELO
  inline const DeVeloPixYType* otherSideYSensor() const { return m_otherSideYSensor; }
  
  /// Set the associated y sensor.  This should only be called by DeVeloPix::initialize()
  inline void setAssociatedYSensor(const DeVeloPixYType* ys) { m_associatedYSensor = ys; }
  
  /// Set the r sensor on the other side of the VELO.  This should only be called by DeVeloPix::initialize()
  inline void setOtherSideXSensor(const DeVeloPixXType* xs) { m_otherSideXSensor = xs; }

  /// Set the y sensor on the other side of the VELO.  This should only be called by DeVeloPix::initialize()
  inline void setOtherSideYSensor(const DeVeloPixYType* ys) { m_otherSideYSensor = ys; }


private:
  /// Store the local radius for each strip in the sensor
  //void calcStripLimits();
  
  /// Update geomtry cache when the alignment changes
  StatusCode updateGeometryCache();
    
private:
  /// list of pixel ladders
  std::vector<PixelLadder> m_ladders;

  /// pointer to associated phi sensor
  const DeVeloPixYType* m_associatedYSensor;
  
  /// pointer to the r sensor on the other side of the VELO
  const DeVeloPixXType* m_otherSideXSensor;
  
  /// pointer to the phi sensor on the other side of the VELO
  const DeVeloPixYType* m_otherSideYSensor;

  // used to control initialization NEVER ACCESS THIS IN AN INLINED METHOD!
  static bool m_staticDataInvalid;
  // Set output level for message service
  bool m_debug;
  bool m_verbose;

};

/// fast cast to X sensor, returns 0 for wrong type
inline const DeVeloPixXType* DeVeloPixSensor::xType() const { 
  return (m_isX ? static_cast<const DeVeloPixXType*>(this) : 0); 
}


#endif // VELOPIXELDET_DEVELOPIXELXTYPE_H
