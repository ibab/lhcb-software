// $Id: DeOTStation.h,v 1.12 2007-07-23 09:59:04 cattanem Exp $
#ifndef OTDET_DEOTSTATION_H
#define OTDET_DEOTSTATION_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"

// GaudiKernel
#include "GaudiKernel/VectorMap.h"

// Kernel
#include "Kernel/OTChannelID.h"

// Local
#include "OTDet/DeOTLayer.h"
#include "OTDet/IndexToDetElementMap.h"

/** @class DeOTStation DeOTStation.h "OTDet/DeOTStation.h"
 *
 *  This is the detector element class for a Outer Tracker Station.
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl 
 *  @date   04-04-2003
 */

static const CLID CLID_DeOTStation = 8102;

class DeOTStation : public DetectorElement {

 public:
  
  /** Some typedefs */
  typedef std::vector<DeOTStation*> Container;
  typedef std::vector<DeOTLayer*> Layers;

  /** Constructor */
  DeOTStation(const std::string& name = "") ;
  
  /** Destructor */
  ~DeOTStation() = default;
  
  /** Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const ;
  
  /** Another reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID() { return CLID_DeOTStation; };
    
  /** Initialization method 
   * @return Status of initialisation
   */
  virtual StatusCode initialize();

  /** @return stationID */
  unsigned int stationID() const;
  
  /** Element id */
  LHCb::OTChannelID elementID() const;
  
  /** Set element id */
  void setElementID(const LHCb::OTChannelID& chanID);
  
  /** Check contains channel
   *  @param  aChannel The channel to check
   *  @return bool
   */
  bool contains(const LHCb::OTChannelID aChannel) const;

  /** Const method to return the layer for a given channel id
   * @param  aChannel  an OT channel id
   * @return pointer to detector element
   */
  const DeOTLayer* findLayer(const LHCb::OTChannelID& aChannel) const;

  /** Non const method to return the layer for a given channel id
   * @param  aChannel  an OT channel id
   * @return pointer to detector element
   */
  DeOTLayer* findLayer(const LHCb::OTChannelID& aChannel);

  /** Const method to return the layer for a given XYZ point
   * @param  aPoint the given point
   * @return const pointer to detector element
   */
  const DeOTLayer* findLayer(const Gaudi::XYZPoint& aPoint) const;

  /** Non const method to return the layer for a given XYZ point
   * @param  aPoint the given point
   * @return const pointer to detector element
   */
  DeOTLayer* findLayer(const Gaudi::XYZPoint& aPoint);

  /** Flat vector of layers
   * @return vector of layers
   */
  const Layers& layers() const;
  
 private:
  /// 4 layers; starting from 0
  typedef OT::IndexToDetElementMap<DeOTLayer, 4, 0> MapLayers;

  unsigned int m_stationID = 0u;      ///< station ID number
  LHCb::OTChannelID m_elementID = 0u; ///< Element ID
  Layers m_layers;               ///< vector of layers
  MapLayers m_mapLayers;         ///< map of layers
};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline unsigned int DeOTStation::stationID() const {
  return m_stationID;
}

inline LHCb::OTChannelID DeOTStation::elementID() const {
  return m_elementID;
}

inline void DeOTStation::setElementID(const LHCb::OTChannelID& chanID) {
  m_elementID = chanID;
}

inline bool DeOTStation::contains(const LHCb::OTChannelID aChannel) const {
  return (m_stationID == aChannel.station());
}

/// Find layer methods
inline const DeOTLayer* DeOTStation::findLayer(const LHCb::OTChannelID& aChannel) const {
  return m_mapLayers[aChannel.layer()]; 
}

inline DeOTLayer* DeOTStation::findLayer(const LHCb::OTChannelID& aChannel) {
  return m_mapLayers[aChannel.layer()]; 
}

inline DeOTLayer* DeOTStation::findLayer(const Gaudi::XYZPoint& aPoint) {
  return const_cast<DeOTLayer*>(static_cast<const DeOTStation&>(*this).findLayer(aPoint));
}

inline const DeOTStation::Layers& DeOTStation::layers() const {
  return m_layers;
}

#endif  // OTDET_DEOTSTATION_H
