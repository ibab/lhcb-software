// $Id: DeOTModule.h,v 1.9 2006-03-31 14:22:40 janos Exp $
#ifndef OTDET_DEOTMODULE_H
#define OTDET_DEOTMODULE_H 1

/// DetDesc
#include "DetDesc/DetectorElement.h"

/// Kernel
#include "Kernel/OTChannelID.h"

namespace LHCb
{
  class Point3DTypes;
  class Trajectory;
}

/** @class DeOTModule DeOTModule.h "OTDet/DeOTModule.h"
 *
 *  This is the detector element class for a Outer Tracker Module. It 
 *  gets the geometry from the XML.
 *  An OT station (e.g. T1) contains 4 layers (x,u,v,x). The layers
 *  contain 4 quarters each. The quarters have 9 modules each.
 *  The long modules are split in an upper and 
 *  lower side. In total this makes 36 modules per layer. 
 *
 *  This class has the calculateHits-method which calculates the closest 
 *  wires (=channels) and the distances from an entry- and exit-point. 
 *  Another method, distanceAlongWire, calculates the distance 
 *  between the hit and the read-out. These and some other functions 
 *  are used by the OT digitization as well as the track reconstruction.
 *  The numbering scheme for the OT modules in the digitization is:
 *
 *         Quarter 3                      Quarter 2
 *    __________________________     _________________________
 *   |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  |
 *   |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  |
 *   |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  |
 *   | 1| 2| 3| 4| 5| 6| 7| 8|9 |   |9| 8| 7| 6| 5| 4| 3| 2| 1|
 *   |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  | 
 *   |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  |
 *   |  |  |  |  |  |  |  |  |__|   |_|  |  |  |  |  |  |  |  |
 *   |  |  |  |  |  |  |  |__|        |__|  |  |  |  |  |  |  |
 *   |--|--|--|--|--|--|--|__   IT     __|--|--|--|--|--|--|--|
 *   |  |  |  |  |  |  |  |  |__     _|  |  |  |  |  |  |  |  |
 *   |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  | 
 *   |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  | 
 *   |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  |
 *   | 1| 2| 3| 4| 5| 6| 7| 8| 9|   |9| 8| 7| 6| 5| 4| 3| 2| 1|
 *   |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  |
 *   |__|__|__|__|__|__|__|__|__|   |_|__|__|__|__|__|__|__|__|
 *
 *          Quarter 1                      Quarter 0
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl 
 *  @date   05-03-2003
 */

namespace LHCb{ class Trajectory; }

static const CLID& CLID_DeOTModule = 8105;

class DeOTModule : public DetectorElement {

 public:
  
  /** Constructor */
  DeOTModule(const std::string& name = "");
  
  /** Destructor */
  ~DeOTModule();
  
  /** Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const;
  
  /** Another reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID() { return CLID_DeOTModule; };
    
  /** Initialization method 
   * @return Status of initialisation
   */ 
  virtual StatusCode initialize();

  /** @return moduleID */
  unsigned int moduleID() const;
  
  /** @retrun quarterID */
  unsigned int quarterID() const;
  
  /** @return  layerID */
  unsigned int layerID() const;
  
  /** @return stationID */
  unsigned int stationID() const;
  
  /** Element id */
  LHCb::OTChannelID elementID() const;
  
  /** Set element id */
  void setElementID(const LHCb::OTChannelID& chanID);

  /** @return unique moduleID */
  unsigned int uniqueModule() const;
  
  /** Check contains channel
   *  @param channel
   *  @return bool
   */
  bool contains(const LHCb::OTChannelID channel) const;

  /** Check if module is top module
   * @return bool
   */ 
  bool topModule() const;

  /** Check if module is bottom module
   * @return bool
   */
  bool bottomModule() const;

  /** @return the number of modules in a layer */
  unsigned int nModules() const;

  /** @return the number of readout channels in a module */
  unsigned int nChannels() const;

  /** Check if straw is in monolayer A
   * @param straw
   * @return bool
   */
  bool monoLayerA(const int aStraw) const;
  
  /** Check if straw is in monolayer B
   * @param straw
   * @return bool
   */
  bool monoLayerB(const int aStraw) const;

  /** @return the straw to the left of a given straw */
  unsigned int nextLeftStraw(const unsigned int aStraw) const;
  
  /** @return the straw to the right of a given straw */
  unsigned int nextRightStraw(const unsigned int aStraw) const;  

  /** @return wire length */
  double wireLength() const;

  /** @return cell radius */
  double cellRadius() const;

  /** @return stereo angle */
  double angle() const;

  /** @return sin of stereo angle */
  double sinAngle() const;

  /** @return cosine of stereo angle */
  double cosAngle() const;

  /** Calculate straws which are hit 
   * @retrun status code
   */
  StatusCode calculateHits(const Gaudi::XYZPoint& entryPoint, 
                           const Gaudi::XYZPoint& exitPoint,
                           std::vector<LHCb::OTChannelID>& channels, 
                           std::vector<double>& driftDistances) const;

  /** Calculate the distance from a given vector in space to the straw 
   * @param straw
   * @param point
   * @param dx/dz
   * @param dy/dz
   * @return distance
   */
  double distanceToWire(const unsigned int straw, 
                        const Gaudi::XYZPoint& aPoint, 
                        const double tx, const double ty) const;
  
  
  
  /** @return distance to electronics along the wire */
  double distanceAlongWire(const double xHit, 
                           const double yHit) const;

  /** @return Global XYZ of the center of a given straw */
  Gaudi::XYZPoint centerOfStraw(const unsigned int aStraw) const;

  /** @return Global XYZ of the center of a module */
  Gaudi::XYZPoint centerOfModule() const;
  
  /** @return the global z-coordinate of a module */
  double z() const;
  
  /// Returns a Trajectory representing the wire identified by the LHCbID
  /// The offset is zero for all OT Trajectories
  LHCb::Trajectory* trajectory( const LHCb::OTChannelID& aChan,
                                const double = 0 /*offset*/ ) const;

 private:
  
  /** Find range of straws that might contain hits */
  void findStraws(const Gaudi::XYZPoint& entryPoint, 
		  const Gaudi::XYZPoint& exitPoint, 
		  std::vector<unsigned int>& straws) const;
    
  /** Find the DOCA paramters mu of a wire and lambda of a track. 
   * Where w = w_bottom + mu*e_w 
   * and p = p_entry + lambda*e_p.
   */
  StatusCode findDocaParams(const Gaudi::XYZPoint& entryPoint,
			    const Gaudi::XYZVector& pUnit,
			    const Gaudi::XYZPoint& wireBottom,
			    const Gaudi::XYZVector& wUnit,
			    double& lambda, double& mu) const;

  /** Return distance of closest approach to wire */
  double docaWire(const Gaudi::XYZPoint& entryPoint,
		  const Gaudi::XYZVector& pUnit,
		  const Gaudi::XYZPoint& wireBottom,
		  const Gaudi::XYZVector& wUnit,
		  double lambda,
		  double mu ) const;

  /** Find the straws that are hit */
  /* void findHitStraws(...) const; */
    
  /** @return local Z of a given straw */
  double localZOfStraw(const int aStraw) const;
  
  /** @return local U (=X) of a given straw */
  double localUOfStraw(const int aStraw) const;
  
  /** @return the straw in monolayer A closest to the hit */ 
  int hitStrawA(const double u) const;
  
  /** @return the straw in monolayer B closest to the hit */
  int hitStrawB(const double u) const;  

  /** Check if Y is inside efficient region of monolayer A
   * @param Y coordinate
   * @return bool
   */
  bool isEfficientA(const double y) const;

  /** Check if Y is inside efficient region of monolayer B
   * @param Y coordinate
   * @return bool
   */
  bool isEfficientB(const double y) const;

  /** Calculates zc,uc of the center of a circle and radius 
      rc of the circle (special assumptions) */
  void sCircle(const double z1, const double u1, const double z2, 
               const double u2, const double dz,
               double& zc, double& uc, double& rc) const;

  unsigned int m_stationID;      ///< station ID number
  unsigned int m_layerID;        ///< layer ID number
  unsigned int m_quarterID;      ///< quarter ID number
  unsigned int m_moduleID;       ///< module ID number
  LHCb::OTChannelID m_elementID; ///< element ID
  unsigned int m_uniqueModuleID; ///< module ID number
  unsigned int m_nStraws;        ///< half the number of straws inside module
  bool m_longModule;             ///< is module a long module?
  double m_stereoAngle;          ///< stereo angle of the layer
  double m_sinAngle;             ///< sine of stereo angle
  double m_cosAngle;             ///< cosine of stereo angle
  double m_xPitch;               ///< pitch in x between straws
  double m_zPitch;               ///< pitch in z bewteen straws
  double m_cellRadius;           ///< radius of a cell
  double m_inefficientRegion;    ///< size of the inefficient region
  unsigned int m_nModules;       ///< half the number of read-out modules in layer
  double m_ySizeModule;          ///< size of the module in y
  double m_yHalfModule;          ///< size of the module in y
};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline unsigned int DeOTModule::moduleID() const {
  return m_moduleID;
}

inline unsigned int DeOTModule::quarterID() const {
  return m_quarterID;
}

inline unsigned int DeOTModule::layerID() const {
  return m_layerID;
}

inline unsigned int DeOTModule::stationID() const {
  return m_stationID;
}

inline LHCb::OTChannelID DeOTModule::elementID() const {
  return m_elementID;
}

inline void DeOTModule::setElementID(const LHCb::OTChannelID& chanID) {
  m_elementID = chanID;
}

inline unsigned int DeOTModule::uniqueModule() const {
  return m_uniqueModuleID;
}

inline bool DeOTModule::contains(LHCb::OTChannelID channel) const {
  return (channel.station() == m_stationID && 
          channel.layer() == m_layerID && 
          channel.quarter() == m_quarterID && 
          channel.module() == m_moduleID);
}

inline bool DeOTModule::topModule() const {
  return m_quarterID > 1;
}

inline bool DeOTModule::bottomModule() const {
  return m_quarterID < 2;
}

inline unsigned int DeOTModule::nModules() const {
  return 2*m_nModules;
}

inline unsigned int DeOTModule::nChannels() const {
  return 2*m_nStraws;
}

inline bool DeOTModule::monoLayerA(const int aStraw) const {
  return (aStraw <= (int) m_nStraws);
}

inline bool DeOTModule::monoLayerB(const int aStraw) const {
  return (aStraw > (int) m_nStraws);
}

inline unsigned int DeOTModule::nextLeftStraw(const unsigned int aStraw) const {
  return (aStraw <= 1 || aStraw == m_nStraws+1) ? 0 : aStraw-1;
}

inline unsigned int DeOTModule::nextRightStraw(const unsigned int aStraw) const {
  return (aStraw == m_nStraws || aStraw >= 2*m_nStraws) ? 0 : aStraw+1;
}

inline double DeOTModule::wireLength() const {
  return m_ySizeModule;
};

inline double DeOTModule::cellRadius() const {
  return m_cellRadius;
}

inline double DeOTModule::angle() const {
  return m_stereoAngle;
}

inline double DeOTModule::sinAngle() const {
  return m_sinAngle;
}

inline double DeOTModule::cosAngle() const {
  return m_cosAngle;
}

inline double DeOTModule::localZOfStraw(const int aStraw) const {
  return (this->monoLayerA(aStraw)) ? -0.5*m_zPitch : 0.5*m_zPitch;
}

inline int DeOTModule::hitStrawA(const double u) const {
  double dU = u - localUOfStraw(1);
  int strawA = (int) ( dU/m_xPitch + 1.5 );
  if (strawA < 1) strawA = 1;
  if (strawA > (int) m_nStraws) strawA = m_nStraws;
  return strawA;
}

inline int DeOTModule::hitStrawB(const double u) const {
  double dU = u - localUOfStraw(m_nStraws+1);
  int strawB = (int) ( dU/m_xPitch + 1.5);
  strawB += m_nStraws;
  if (strawB <= (int) m_nStraws) strawB = m_nStraws + 1;
  if (strawB > (int) (2*m_nStraws)) strawB = 2*m_nStraws;
  return strawB;
}

inline bool DeOTModule::isEfficientA(const double y) const {
  // check if hit is not inside the inefficient region
  return (m_longModule && this->bottomModule() &&
          (0.5*m_ySizeModule - y) < m_inefficientRegion) ? false : true;
}

inline bool DeOTModule::isEfficientB(const double y) const {
  // check if hit is not inside the inefficient region
  return (m_longModule && this->topModule() &&
          (0.5*m_ySizeModule + y) < m_inefficientRegion) ? false : true;
}

#endif  // OTDET_DEOTMODULE_H
