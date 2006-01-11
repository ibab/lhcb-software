// $Id: DeOTModule.h,v 1.4 2006-01-11 09:29:15 janos Exp $
#ifndef OTDET_DEOTMODULE_H
#define OTDET_DEOTMODULE_H 1

// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

// Kernel
#include "Kernel/OTChannelID.h"

// MathCore
#include "Kernel/Point3DTypes.h"


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

static const CLID& CLID_DeOTModule = 8105;

class DeOTModule : public DetectorElement {

public:
  
  /// Constructor
  DeOTModule ( const std::string& name    = "" ) ;
  
  /// Destructor
  ~DeOTModule () ;
  
  /// object identification
  const CLID& clID () const ;
  /// object identification
  static const CLID& classID () { return CLID_DeOTModule ; }
  
  /// initialization method 
  virtual StatusCode initialize();

  /// get moduleID
  unsigned int moduleID() const { return m_moduleID; };
  
  /// get quarterID
  unsigned int quarterID() const { return m_quarterID; };
  
  /// get layerID
  unsigned int layerID() const { return m_layerID; };
  
  /// get stationID
  unsigned int stationID() const { return m_stationID; };
  
  /// get unique moduleID
  unsigned int uniqueModule() const { return m_uniqueModuleID; };
  
  /// get the number of modules in this layer
  unsigned int nModules() const { return 2*m_nModules; };

  /// get the wire length
  double wireLength() const { return m_ySizeModule; };

  /// get the cell radius
  double cellRadius() const { return m_cellRadius; }

  /// get the stereo angle
  double stereoAngle() const { return m_stereoAngle; }

  /// get the sine of the stereo angle
  double sinAngle() const { return m_sinAngle; }

  /// get the cosine of the stereo angle
  double cosAngle() const { return m_cosAngle; }

  /// Check if straw is in monolayer A
  bool monoLayerA(const int straw) const;
  
  /// Check if straw is in monolayer B
  bool monoLayerB(const int straw) const;

  /// Check if module is top module
  bool topModule() const;

  /// Check if module is bottom module
  bool bottomModule() const;

  /// check if point is inside volume
  bool isInside(const Gaudi::XYZPoint& point) const;

  /// checks if this modules contains the given OTChannelID
  bool contains(LHCb::OTChannelID channel) const;

  /// return the straw to the left
  unsigned int nextLeftStraw(const unsigned int straw) const;
  
  /// return the straw to the right
  unsigned int nextRightStraw(const unsigned int straw) const;  

  /// get the number of readout channels in this module
  unsigned int nChannels() const;

  /// calculate straws which are hit
  StatusCode calculateHits(const Gaudi::XYZPoint& entryPoint, 
                           const Gaudi::XYZPoint& exitPoint,
                           std::vector<LHCb::OTChannelID>& channels, 
                           std::vector<double>& driftDistances) const;

  /// get the distance from a given vector in space to the straw
  double distanceToWire(const unsigned int straw, 
                        const Gaudi::XYZPoint& aPoint, 
                        const double tx, const double ty) const;

  /// get distance to electronics along the wire
  double distanceAlongWire(const double xHit, 
                           const double yHit) const;

  /// get the x,y,z of a given straw in global coordinates
  Gaudi::XYZPoint centerOfStraw(const unsigned int straw) const;

  /// get the global coordinate of the middle of the module
  Gaudi::XYZPoint centerOfModule() const;
  
  /// get the global z-coordinate of the module
  double z() const;

private:

  /// get the z of a given straw in local coordinates
  double localZOfStraw(const int straw) const;
  
  /// get the u (=x) of a given straw in local coordinates
  double localUOfStraw(const int straw) const;

  /// find the closest straw in monolayer A
  int hitStrawA(const double u) const;
  
  /// find the closest straw in monolayer B
  int hitStrawB(const double u) const;  

  /// check if y-coordinate is outside inefficient region of monolayer A
  bool isEfficientA(const double y) const;

  /// check if y-coordinate is outside inefficient region of monolayer B
  bool isEfficientB(const double y) const;

  /// calculates of zc,uc of the center and radius 
  ///  rc of the circle (special assumptions)
  void sCircle(const double z1, const double u1, const double z2, 
               const double u2, const double dz,
               double& zc, double& uc, double& rc) const;

  unsigned int m_moduleID;     ///< module ID number
  unsigned int m_quarterID;    ///< quarter ID number
  unsigned int m_layerID;      ///< layer ID number
  unsigned int m_stationID;    ///< station ID number
  unsigned int m_uniqueModuleID;///< module ID number
  unsigned int m_nStraws;      ///< half the number of straws inside module
  bool m_longModule;           ///< is module a long module?
  double m_stereoAngle;        ///< stereo angle of the layer
  double m_sinAngle;           ///< sine of stereo angle
  double m_cosAngle;           ///< cosine of stereo angle
  double m_pitch;              ///< pitch between straws
  double m_cellRadius;         ///< radius of a cell
  double m_inefficientRegion;  ///< size of the inefficient region
  unsigned int m_nModules;     ///< half the number of read-out modules in layer
  double m_ySizeModule;        ///< size of the module in y

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------


inline bool DeOTModule::monoLayerA(const int straw) const
{
  return (straw <= (int) m_nStraws);
}

inline bool DeOTModule::monoLayerB(const int straw) const
{
  return (straw > (int) m_nStraws);
}

inline bool DeOTModule::topModule() const
{
  return m_quarterID > 1;
}

inline bool DeOTModule::bottomModule() const
{
  return m_quarterID < 2;
}

inline bool DeOTModule::isInside(const Gaudi::XYZPoint& point) const
{
  return this->geometry()->isInside(point);
}

inline bool DeOTModule::contains(LHCb::OTChannelID channel) const
{
  return (channel.station() == m_stationID && 
          channel.layer() == m_layerID && 
          channel.quarter() == m_quarterID && 
          channel.module() == m_moduleID);
}

inline unsigned int DeOTModule::nextLeftStraw(const unsigned int straw) const
{
  return (straw <= 1 || straw == m_nStraws+1) ? 0 : straw-1;
}

inline unsigned int DeOTModule::nextRightStraw(const unsigned int straw) const
{
  return (straw == m_nStraws || straw >= 2*m_nStraws) ? 0 : straw+1;
}

inline unsigned int DeOTModule::nChannels() const
{
  return 2*m_nStraws;
}

inline double DeOTModule::localZOfStraw(const int straw) const
{
  return (this->monoLayerA(straw)) ? -0.5*m_pitch : 0.5*m_pitch;
}

inline int DeOTModule::hitStrawA(const double u) const
{
  double dU = u - localUOfStraw(1);
  int strawA = (int) ( dU/m_pitch + 1.5 );
  if (strawA < 1) strawA = 1;
  if (strawA > (int) m_nStraws) strawA = m_nStraws;
  return strawA;
}

inline int DeOTModule::hitStrawB(const double u) const
{
  double dU = u - localUOfStraw(m_nStraws+1);
  int strawB = (int) ( dU/m_pitch + 1.5);
  strawB += m_nStraws;
  if (strawB <= (int) m_nStraws) strawB = m_nStraws + 1;
  if (strawB > (int) (2*m_nStraws)) strawB = 2*m_nStraws;
  return strawB;
}

inline bool DeOTModule::isEfficientA(const double y) const
{
  // check if hit is not inside the inefficient region
  return (m_longModule && this->bottomModule() &&
          (0.5*m_ySizeModule - y) < m_inefficientRegion) ? false : true;
}

inline bool DeOTModule::isEfficientB(const double y) const
{
  // check if hit is not inside the inefficient region
  return (m_longModule && this->topModule() &&
          (0.5*m_ySizeModule + y) < m_inefficientRegion) ? false : true;
}

#endif  // OTDET_DEOTMODULE_H
