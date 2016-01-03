// $Id: DeOTQuarter.h,v 1.16 2008-10-22 11:12:50 cattanem Exp $
#ifndef OTDET_DEOTQUARTER_H
#define OTDET_DEOTQUARTER_H 1

/// GaudiKernel
#include "GaudiKernel/VectorMap.h"

/// DetDesc
#include "DetDesc/DetectorElement.h"

/// Kernel
#include "Kernel/OTChannelID.h"

// Local
#include "OTDet/DeOTModule.h"
#include "OTDet/IndexToDetElementMap.h"

/** @class DeOTQuarter DeOTQuarter.h "OTDet/DeOTQuarter.h"
 *
 *  This is the detector element class for a Outer Tracker Quarter.
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl
 *  @date   04-04-2003
 */

static const CLID CLID_DeOTQuarter = 8104;

class DeOTQuarter : public DetectorElement {

 public:
  /** Some typedefs */
  typedef std::vector<DeOTModule*> Modules;

  /** Constructor */
  DeOTQuarter( const std::string& name = "");

  /** Destructor */
  ~DeOTQuarter() = default;

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
   *  @param  aChannel The channel to check
   *  @return bool
   */
  bool contains(const LHCb::OTChannelID aChannel) const;

  /** Const method to return the module for a given channel id
   * @param  aChannel  an OT channel id
   * @return pointer to detector element
   */
  const DeOTModule* findModule(const LHCb::OTChannelID& aChannel) const;

  /** Non const method to return the module for a given channel id
   * @param  aChannel  an OT channel id
   * @return pointer to detector element
   */
  DeOTModule* findModule(const LHCb::OTChannelID& aChannel);

  /** Const method to return the module for a given XYZ point
   * @param  aPoint the given point
   * @return const pointer to detector element
   */
  const DeOTModule* findModule(const Gaudi::XYZPoint& aPoint) const;

  /** Non const method to return the module for a given XYZ point
   * @param  aPoint the given point
   * @return const pointer to detector element
   */
  DeOTModule* findModule(const Gaudi::XYZPoint& aPoint);

  /** @return stereo angle of the layer */
  double angle() const;

  /** Flat vector of all OT modules
   * @return vector of modules
   */
  const Modules& modules() const;

 private:
   /// 9 modules; starting from 1
  typedef OT::IndexToDetElementMap<DeOTModule, 9, 1> MapModules;

  unsigned int m_stationID = 0u;      ///< station ID number
  unsigned int m_layerID = 0u;        ///< layer ID number
  unsigned int m_quarterID = 0u;      ///< quarter ID number
  LHCb::OTChannelID m_elementID = 0u; ///< element id
  double m_stereoAngle = 0.;          ///< layer stereo angle
  Modules m_modules;             ///< vector of modules
  MapModules m_mapModules;       ///< map module id to module
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

/// Find module methods
inline const DeOTModule* DeOTQuarter::findModule(const LHCb::OTChannelID& aChannel) const {
  return  m_mapModules[aChannel.module()];
}

inline DeOTModule* DeOTQuarter::findModule(const LHCb::OTChannelID& aChannel) {
    return  m_mapModules[aChannel.module()];
}

inline DeOTModule* DeOTQuarter::findModule(const Gaudi::XYZPoint& aPoint) {
  return const_cast<DeOTModule*>(static_cast<const DeOTQuarter&>(*this).findModule(aPoint));
}

inline double DeOTQuarter::angle() const {
  return m_stereoAngle;
}

inline const DeOTQuarter::Modules& DeOTQuarter::modules() const {
  return m_modules;
}

#endif  // OTDET_DEOTQUARTER_H
