// $Id: DeOTQuarter.h,v 1.10 2007-02-02 09:25:04 janos Exp $
#ifndef OTDET_DEOTQUARTER_H
#define OTDET_DEOTQUARTER_H 1

/// GaudiKernel
#include "GaudiKernel/VectorMap.h"

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
  typedef GaudiUtils::VectorMap<unsigned int, DeOTModule*> MapIDModule;

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
  Modules m_modules;             ///< vector of modules
  MapIDModule m_mapIDModule;       ///< map module id to module
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
  return (m_elementID.uniqueQuarter() == aChannel.uniqueQuarter());
}

inline double DeOTQuarter::angle() const {
  return m_stereoAngle;
}

inline const DeOTQuarter::Modules& DeOTQuarter::modules() const {
  return m_modules;
}

#endif  // OTDET_DEOTQUARTER_H
