// $Id: DeOTLayer.h,v 1.11 2007-02-02 09:25:04 janos Exp $
#ifndef OTDET_DEOTLAYER_H
#define OTDET_DEOTLAYER_H 1

/// DetDesc
#include "DetDesc/DetectorElement.h"

/// Kernel
#include "Kernel/OTChannelID.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "GaudiKernel/VectorMap.h"

/// OTDet
#include "OTDet/DeOTDetector.h"

/** @class DeOTLayer DeOTLayer.h "OTDet/DeOTLayer.h"
 *
 *  This is the detector element class for an Outer Tracker Layer.
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl 
 *  @date   04-04-2003
 */

/// Forward declarations
class DeOTQuarter;
class DeOTModule;

namespace LHCb
{
  class Point3DTypes;
}

static const CLID& CLID_DeOTLayer = 8103;

class DeOTLayer : public DetectorElement {

 public:

  /** Some typedefs */
  typedef std::vector<DeOTLayer*> Container;
  typedef std::vector<DeOTQuarter*> Quarters;
  typedef GaudiUtils::VectorMap<unsigned int, DeOTQuarter*> MapIDQuarter;

  /** Constructor */
  DeOTLayer(const std::string& name = "") ;
  
  /** Destructor */
  ~DeOTLayer() ;
  
  /** Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const ;

  /** Another reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID() { return CLID_DeOTLayer; }
  
  /** Initialization method 
   * @return Status of initialisation
   */
  virtual StatusCode initialize();

  /** Return layer id 
   * @return layerID 
   */
  unsigned int layerID() const;
  
  /** Element id */
  LHCb::OTChannelID elementID() const;
  
  /** Set element id */
  void setElementID(const LHCb::OTChannelID& chanID);

  /** check contains channel
   *  @param channel
   *  @return bool
   */
  bool contains(const LHCb::OTChannelID aChannel) const;
    
  /** @return stereo angle of the layer */
  double angle() const;
  
  /** @return the quarter for a given channelID */
  DeOTQuarter* findQuarter(const LHCb::OTChannelID aChannel) const;
  
  /** @return the quarter for a given XYZ point */
  /** This method was depracated but I need it for the sensitive volume ID */
  DeOTQuarter* findQuarter(const Gaudi::XYZPoint& aPoint) const;
  
   /** plane corresponding to the layer 
  * @return the plane 
  */
  Gaudi::Plane3D plane() const;

  void cachePlane();

  /** flat vector of quarters
   * @return vector of quarters
   */
  const Quarters& quarters() const;
  
 private:
  unsigned int m_stationID;      ///< stationID
  unsigned int m_layerID;        ///< layer ID number
  LHCb::OTChannelID m_elementID; ///< element id
  double m_stereoAngle;          ///< layer stereo angle 
  Gaudi::Plane3D m_plane;        ///< plane corresponding to the layer
  Quarters m_quarters;           ///< vector of quarters
  MapIDQuarter m_mapIDQuarter;     ///< map quarter id to quarter
};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline unsigned int DeOTLayer::layerID() const {
  return m_layerID;
}

inline LHCb::OTChannelID DeOTLayer::elementID() const {
  return m_elementID;
}

inline void DeOTLayer::setElementID(const LHCb::OTChannelID& chanID) {
  m_elementID = chanID;
}

inline bool DeOTLayer::contains(const LHCb::OTChannelID aChannel) const {
  return (m_elementID.uniqueLayer() == aChannel.uniqueLayer());
}

inline double DeOTLayer::angle() const {
  return m_stereoAngle;
}

inline Gaudi::Plane3D DeOTLayer::plane() const {
  return m_plane;
}

inline const DeOTLayer::Quarters& DeOTLayer::quarters() const {
  return m_quarters;
}

#endif  // OTDET_DEOTLAYER_H
