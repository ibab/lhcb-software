// $Id: DeOTLayer.h,v 1.18 2009-09-24 11:45:32 wouter Exp $
#ifndef OTDET_DEOTLAYER_H
#define OTDET_DEOTLAYER_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"

// Kernel
#include "Kernel/OTChannelID.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "GaudiKernel/VectorMap.h"

// Local
#include "OTDet/DeOTQuarter.h"
#include "OTDet/IndexToDetElementMap.h"

/** @class DeOTLayer DeOTLayer.h "OTDet/DeOTLayer.h"
 *
 *  This is the detector element class for an Outer Tracker Layer.
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl 
 *  @date   04-04-2003
 */

static const CLID CLID_DeOTLayer = 8103;

class DeOTLayer : public DetectorElement {

 public:

  /** Some typedefs */
  typedef std::vector<DeOTLayer*> Container;
  typedef std::vector<DeOTQuarter*> Quarters;

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
   *  @param  aChannel The channel to check
   *  @return bool
   */
  bool contains(const LHCb::OTChannelID aChannel) const;

  /** Const method to return the quarter for a given channel id
   * @param  aChannel  an OT channel id
   * @return pointer to detector element
   */
  const DeOTQuarter* findQuarter(const LHCb::OTChannelID& aChannel) const;

  /** Non const method to return the quarter for a given channel id
   * @param  aChannel  an OT channel id
   * @return pointer to detector element
   */
  DeOTQuarter* findQuarter(const LHCb::OTChannelID& aChannel);

  /** Const method to return the quarter for a given XYZ point
   * @param  aPoint the given point
   * @return const pointer to detector element
   */
  const DeOTQuarter* findQuarter(const Gaudi::XYZPoint& aPoint) const;

  /** Non const method to return the quarter for a given XYZ point
   * @param  aPoint the given point
   * @return const pointer to detector element
   */
  DeOTQuarter* findQuarter(const Gaudi::XYZPoint& aPoint);
  
  /** @return stereo angle of the layer */
  double angle() const;
  
  /** plane corresponding to the layer 
  * @return the plane 
  */
  const Gaudi::Plane3D& plane() const;
  StatusCode cachePlane();

  /** flat vector of quarters
   * @return vector of quarters
   */
  const Quarters& quarters() const;
  
 private:
   /// 4 quarters; starting from 0
  typedef OT::IndexToDetElementMap<DeOTQuarter, 4, 0> MapQuarters;

  unsigned int m_stationID;       ///< stationID
  unsigned int m_layerID;         ///< layer ID number
  LHCb::OTChannelID m_elementID;  ///< element id
  double m_stereoAngle;           ///< layer stereo angle 
  Gaudi::Plane3D m_plane;         ///< plane corresponding to the layer
  Quarters m_quarters;            ///< vector of quarters
  MapQuarters m_mapQuarters;      ///< map quarters
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

/// Find quarter methods
inline const DeOTQuarter* DeOTLayer::findQuarter(const LHCb::OTChannelID& aChannel) const {
  return m_mapQuarters[aChannel.quarter()];
}

inline DeOTQuarter* DeOTLayer::findQuarter(const LHCb::OTChannelID& aChannel) {
  return m_mapQuarters[aChannel.quarter()];
}

inline DeOTQuarter* DeOTLayer::findQuarter(const Gaudi::XYZPoint& aPoint) {
  return const_cast<DeOTQuarter*>(static_cast<const DeOTLayer&>(*this).findQuarter(aPoint));
}

inline double DeOTLayer::angle() const {
  return m_stereoAngle;
}

inline const Gaudi::Plane3D& DeOTLayer::plane() const {
  return m_plane;
}

inline const DeOTLayer::Quarters& DeOTLayer::quarters() const {
  return m_quarters;
}

#endif  // OTDET_DEOTLAYER_H
