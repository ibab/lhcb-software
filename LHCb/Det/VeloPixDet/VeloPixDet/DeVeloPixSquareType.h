// $Id: DeVeloPixSquareType.h,v 1.1.1.1 2009-09-17 16:07:20 cocov Exp $
#ifndef VELOPIXDET_DEVELOPIXSQUARETYPE_H 
#define VELOPIXDET_DEVELOPIXSQUARETYPE_H 1

// Include files

// Gaudi Math definitions
#include "GaudiKernel/Point3DTypes.h"

// from VeloPixDet
#include "VeloPixDet/DeVeloPixSensor.h"
#include "VeloPixDet/PixelLadder.h"

// Unique class identifier
static const CLID CLID_DeVeloPixSquareType = 1008204 ;

namespace LHCb {
  class VeloPixChannelID;
}

/** @class DeVeloPixSquareType DeVeloPixSquareType.h VeloPixDet/DeVeloPixSquareType.h
 *  
 *  Detector element for square (symetric) pixel type
 *  contains all the function specific to this type of pixel sensor
 *
 *  @author Victor Coco
 *  @date   2009-05-14
 */

class DeVeloPixSquareType : public DeVeloPixSensor {
public:
  /// Standard constructor
  DeVeloPixSquareType( const std::string& name = "" ); 

  virtual ~DeVeloPixSquareType( ); ///< Destructor

  /// object identifier (static method)
  static  const CLID& classID() { return CLID_DeVeloPixSquareType; }
  /// object identification
  virtual const CLID& clID()     const;

  /// Initialise the DeVeloPixSensor from the XML
  virtual StatusCode initialize();

  /// Calculate the nearest channel to a 3-d point.
  /// Also returns the fractional difference in the channel
  /// and the local pitch.
  virtual StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                                    LHCb::VeloPixChannelID& channel,
                                    double& fraction) const;

  /// Determines if local 3-d point is inside sensor
  virtual StatusCode isInActiveArea(const Gaudi::XYZPoint& point) const;

  /// Determines in which ladder is local 3-d point
  virtual int WhichLadder(const Gaudi::XYZPoint& point) const;

  /// Determines in which chip of a given ladder is local 3-d point 
  virtual int WhichChip(const Gaudi::XYZPoint& point, int ladderIndex) const;
  
  /// Determines in which pixel of a given chip and a given ladder is local 3-d point
  virtual std::pair<int,int> WhichPixel(const Gaudi::XYZPoint& point, int ladderIndex, int chipIndex) const;
    
  /// Access to the sensor on the other side of the VeloPix
  inline const DeVeloPixSquareType* otherSideSensor() const { return m_otherSideSensor; }
  
  /// Set the sensor on the other side of the VeloPix.  This should only be called by DeVeloPix::initialize()
  inline void setOtherSideSensor(const DeVeloPixSquareType* s) { m_otherSideSensor = s; }
    
private:
  // vector of Ladder constituing the sensor
  std::vector<PixelLadder> m_ladders;
  // associated sensor on the other side of the velo
  const DeVeloPixSquareType* m_otherSideSensor;
  
  // used to control initialization NEVER ACCESS THIS IN AN INLINED METHOD!
  static bool m_staticDataInvalid;
  // Set output level for message service
  bool m_debug;
  bool m_verbose;
};

/// fast cast to Square sensor, returns 0 for wrong type
inline const DeVeloPixSquareType* DeVeloPixSensor::squareType() const { 
  return (m_isSquare ? static_cast<const DeVeloPixSquareType*>(this) : 0); 
}
  

#endif // VELOPIXDET_DEVELOPIXSQUARETYPE_H
