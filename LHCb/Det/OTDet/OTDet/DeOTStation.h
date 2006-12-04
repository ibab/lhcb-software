// $Id: DeOTStation.h,v 1.6 2006-12-04 18:08:12 janos Exp $
#ifndef OTDET_DEOTSTATION_H
#define OTDET_DEOTSTATION_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"

// Kernel
#include "Kernel/OTChannelID.h"

// OTDet
#include "OTDet/DeOTDetector.h"
//#include "OTDet/DeOTBaseElement.h"

/** @class DeOTStation DeOTStation.h "OTDet/DeOTStation.h"
 *
 *  This is the detector element class for a Outer Tracker Station.
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl 
 *  @date   04-04-2003
 */

/// Forward declarations
class DeOTLayer;

namespace LHCb 
{
  class Point3DTypes;
}

static const CLID& CLID_DeOTStation = 8102;

class DeOTStation : public DetectorElement {

 public:
  
  /** Some typedefs */
  typedef std::vector<DeOTLayer*> Layers;
  
  /** Constructor */
  DeOTStation(const std::string& name = "") ;
  
  /** Destructor */
  ~DeOTStation() ;
  
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
   *  @param channel
   *  @return bool
   */
  bool contains(const LHCb::OTChannelID aChannel) const;

  /** @return the layer for a given channelID */
  DeOTLayer* findLayer(const LHCb::OTChannelID aChannel);

  /** @return the layer for a given 3D point */
  DeOTLayer* findLayer(const Gaudi::XYZPoint& aPoint);

  /** Flat vector of layers
   * @return vector of layers
   */
  const Layers& layers() const;
  
 private:
  unsigned int m_stationID;      ///< station ID number
  LHCb::OTChannelID m_elementID; ///< Element ID
  Layers m_layers;               ///< vector of layers

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

inline const DeOTStation::Layers& DeOTStation::layers() const {
  return m_layers;
}

#endif  // OTDET_DEOTSTATION_H
