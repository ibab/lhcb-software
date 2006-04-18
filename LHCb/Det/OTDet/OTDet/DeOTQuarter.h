// $Id: DeOTQuarter.h,v 1.8 2006-04-18 18:57:37 janos Exp $
#ifndef OTDET_DEOTQUARTER_H
#define OTDET_DEOTQUARTER_H 1

/// DetDesc
#include "DetDesc/DetectorElement.h"

/// Kernel
#include "Kernel/OTChannelID.h"

/** @class DeOTQuarter DeOTQuarter.h "OTDet/DeOTQuarter.h"
 *
 *  This is the detector element class for a Outer Tracker Quarter.
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl 
 *  @date   04-04-2003
 */

/// Forward declarations
class DeOTModule;

namespace LHCb
{
  class Point3DTypes;
}

static const CLID& CLID_DeOTQuarter = 8104;

class DeOTQuarter : public DetectorElement {

 public:
  /** Some typedefs */
  typedef std::vector<DeOTModule*> Modules;
  
  /** Constructor */
  DeOTQuarter( const std::string& name = "");
  
  /** Destructor */
  ~DeOTQuarter();
  
  /** Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const;

  /** Another reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID() { return CLID_DeOTQuarter; };
    
  /** Initialization method 
   * @return Status of initialisation
   */ 
  virtual StatusCode initialize();

  /** @return quarterID */
  unsigned int quarterID() const;

  /** Element id */
  LHCb::OTChannelID elementID() const;
  
  /** Set element id */
  void setElementID(const LHCb::OTChannelID& chanID);
  
  /** Check contains channel
   *  @param channel
   *  @return bool
   */
  bool DeOTQuarter::contains(const LHCb::OTChannelID aChannel) const;
  
  // FIXME: isInsideEfficient is really efficient. So efficient that it's throwing
  //        away hits :-(
  /** Check if a point is inside the quarter 
   * @parma a point
   * @return bool
   */
  /*  bool DeOTQuarter::isInsideEfficient(const Gaudi::XYZPoint& aPoint) const; */

  /** @return stereo angle of the layer */
  double angle() const;

  /** @return the module for a given channelID */
  DeOTModule* findModule(const LHCb::OTChannelID aChannel) const;

  /** @return the module for a given XYZ point */
  DeOTModule* findModule(const Gaudi::XYZPoint& aPoint) const;

  /** Flat vector of all OT modules
   * @return vector of modules
   */
  const Modules& modules() const;
  
 private:
  unsigned int m_stationID;      ///< station ID number
  unsigned int m_layerID;        ///< layer ID number
  unsigned int m_quarterID;      ///< quarter ID number
  LHCb::OTChannelID m_elementID; ///< element id
  double m_stereoAngle;          ///< layer stereo angle 
  /*   double m_xMin;                 ///< Minimum x of the cover */
  /*   double m_yMin;                 ///< Minimum y of the cover */
  /*   double m_zMin;                 ///< Minimum z of the cover */
  /*   double m_xMax;                 ///< Maximum x of the cover */
  /*   double m_yMax;                 ///< Maximum y of the cover */
  /*   double m_zMax;                 ///< Maximum z of the cover */
  Modules m_modules;             ///< vector of modules

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline unsigned int DeOTQuarter::quarterID() const {
  return m_quarterID;
}

inline LHCb::OTChannelID DeOTQuarter::elementID() const {
  return m_elementID;
}

inline void DeOTQuarter::setElementID(const LHCb::OTChannelID& chanID) {
  m_elementID = chanID;
}

inline bool DeOTQuarter::contains(const LHCb::OTChannelID aChannel) const {
  return (quarterID() == aChannel.quarter());
}

/// I'll keep this untill it's in DetDesc
// FIXME: isInsideEfficient is really efficient. So efficient that it's throwing
//        away hits :-(
/* inline bool DeOTQuarter::isInsideEfficient(const Gaudi::XYZPoint& aPoint) const { */
/*   double aPointX = aPoint.x(); */
/*   double aPointY = aPoint.y(); */
/*   double aPointZ = aPoint.z(); */
/*   return (aPointX > m_xMin && aPointX < m_xMax && */
/* 	  aPointY > m_yMin && aPointY < m_yMax && */
/* 	  aPointZ > m_zMin && aPointZ < m_zMax); */
/* } */

inline double DeOTQuarter::angle() const {
  return m_stereoAngle;
}

inline const DeOTQuarter::Modules& DeOTQuarter::modules() const {
  return m_modules;
}

#endif  // OTDET_DEOTQUARTER_H
