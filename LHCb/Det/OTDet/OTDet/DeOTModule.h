#ifndef OTDET_DEOTMODULE_H
#define OTDET_DEOTMODULE_H 1

#include "boost/container/static_vector.hpp"
/// DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

/// Kernel
#include "Kernel/OTChannelID.h"
#include "Kernel/LineTraj.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "LHCbMath/LineTypes.h"

/// OTDet
#include "OTDet/RtRelation.h"
#include "OTDet/WalkRelation.h"

namespace LHCb
{
  class Trajectory;
  class OTWireTraj ;
}

class MsgStream;

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
 *  @verbatim
                                                                ^ Y
          Quarter 3                      Quarter 2              |
     __________________________     _________________________   |
    |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  |  |
    |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  |  |
    |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  |  |
    | 1| 2| 3| 4| 5| 6| 7| 8|9 |   |9| 8| 7| 6| 5| 4| 3| 2| 1|  |
    |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  |  |
    |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  |  |
    |  |  |  |  |  |  |  |  |__|   |_|  |  |  |  |  |  |  |  |  |
    |  |  |  |  |  |  |  |__|        |__|  |  |  |  |  |  |  |  |
    |--|--|--|--|--|--|--|__   IT     __|--|--|--|--|--|--|--|  |
    |  |  |  |  |  |  |  |  |__     _|  |  |  |  |  |  |  |  |  |
    |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  |  |
    |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  |  |
    |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  |  |
    | 1| 2| 3| 4| 5| 6| 7| 8| 9|   |9| 8| 7| 6| 5| 4| 3| 2| 1|  |
    |  |  |  |  |  |  |  |  |  |   | |  |  |  |  |  |  |  |  |  |
    |__|__|__|__|__|__|__|__|__|   |_|__|__|__|__|__|__|__|__|  |
                                                                |
           Quarter 1                      Quarter 0             |
                                                                |
   <------------------------------------------------------------
   X

 *  @endverbatim
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl
 *  @date   05-03-2003
 */

namespace LHCb{ class Trajectory; }

static const CLID CLID_DeOTModule = 8105;

class DeOTModule : public DetectorElement {

public:

  /// some handy typedefs
  typedef std::vector<DeOTModule*> Container;
  enum { MAXNUMCHAN = 128 } ;
  typedef boost::container::static_vector<unsigned, MAXNUMCHAN> Straws;
  enum ChannelStatus { Good = 0, Dead = 1, Noisy = 2, Unknown = 99 } ;

  /** Constructor */
  DeOTModule(const std::string& name = "");

  /** Destructor */
  ~DeOTModule() override;

  /** Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const override;

  /** Another reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID() { return CLID_DeOTModule; };

  /** Initialization method
   * @return Status of initialisation
   */
  StatusCode initialize() override;

  /** @return moduleID */
  unsigned int moduleID() const;

  /** @return quarterID */
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
   *  @param aChannel The channel to check
   *  @return bool
   */
  bool contains(const LHCb::OTChannelID aChannel) const;

  /** Check if module is top module
   * @return bool
   */
  bool topModule() const;

  /** Check if module is bottom module
   * @return bool
   */
  bool bottomModule() const;

  /** Check if module is long module (from 1-7)
   * @return bool
   */
  bool longModule() const;

  /** Check if module is S1 module (8)
   * @return bool
   */
  bool s1Module() const;

  /** Check if module is S2 module (9 Left)
   * @return bool
   */
  bool s2Module() const;

  /** Check if module is S3 module (9 right)
   * @return bool
   */
  bool s3Module() const;

  /** @return the number of modules in a layer */
  unsigned int nModules() const;

  /** @return the number of readout channels in a module */
  unsigned int nChannels() const;

  /** Check if straw is in monolayer A
   * @param aStraw The straw to check
   * @return bool
   */
  bool monoLayerA(const unsigned int aStraw) const;

  /** Check if straw is in monolayer B
   * @param aStraw the straw to check
   * @return bool
   */
  bool monoLayerB(const unsigned int aStraw) const;

  /** return the monolayer for a straw (0 or 1)
   * @param aStraw the straw to check
   * @return unsigned char
   */
  unsigned char mono(const unsigned int aStraw) const;

  /** @return the straw to the left of a given straw */
  unsigned int nextLeftStraw(const unsigned int aStraw) const;

  /** @return the straw to the right of a given straw */
  unsigned int nextRightStraw(const unsigned int aStraw) const;

  /** @return thickness of sensitive volume */
  double sensThickness() const;

  /** @return wire length */
  double wireLength(const LHCb::OTChannelID aChan) const;

  /** @return cell radius */
  double cellRadius() const;

  /** @return stereo angle */
  double angle() const;

  /** @return sin of stereo angle */
  double sinAngle() const;

  /** @return cosine of stereo angle */
  double cosAngle() const;

  /** @return local from global */
  Gaudi::XYZPoint toLocal(const Gaudi::XYZPoint& aPoint) const;

   /** @return local point from xyz */
  Gaudi::XYZPoint localPoint(const double x, const double y, const double z) const;

  /** @return global from local */
  Gaudi::XYZPoint toGlobal(const Gaudi::XYZPoint& aPoint) const;

  /** @return global point from xyz */
  Gaudi::XYZPoint globalPoint(const double x, const double y, const double z) const;

  /** Calculate straws which are hit
   * @param  entryPoint  entry point
   * @param  exitPoint   exit point
   * @param  chanAndDist vector of pairs of channel and drift distance
   * @return void
   */
  void calculateHits(const Gaudi::XYZPoint& entryPoint,
		     const Gaudi::XYZPoint& exitPoint,
		     std::vector<std::pair<LHCb::OTChannelID, double> >& chanAndDist) const;

  /** Calculate the distance from a given vector in space to the straw
   * @param aStraw straw
   * @param aPoint point
   * @param tx     dx/dz
   * @param ty     dy/dz
   * @return distance
   */
  double distanceToWire(const unsigned int aStraw,
                        const Gaudi::XYZPoint& aPoint,
                        const double tx, const double ty) const;

  /** Calculate the coordinate in the mono-layer plane in terms of the
   * pitch. The z-coordinate is the middle of the mono-layer.
   */
  void monoLayerIntersection(int monolayer,
			     const Gaudi::XYZPoint& aPoint,
			     const double tx, const double ty,
			     double& straw, double& yfrac) const ;

  /** @return distance to electronics along the wire */
  double distanceAlongWire(const double xHit,
                           const double yHit) const;

  /** @return Global XYZ of the center of a given straw */
  Gaudi::XYZPoint centerOfStraw(const unsigned int aStraw) const;

  /** @return Global XYZ of the center of a module */
  Gaudi::XYZPoint centerOfModule() const;

  /** @return the global z-coordinate of a module */
  double z() const;

    /** plane corresponding to the module
  * @return the plane
  */
  Gaudi::Plane3D plane() const;

  /** plane corresponding to the module entrance
  * @return the plane
  */
  Gaudi::Plane3D entryPlane() const;


  /** plane corresponding to the module exit
  * @return the plane
  */
  Gaudi::Plane3D exitPlane() const;

  /** Get trajectory representing the most left wire in (first=0) monolayer
   * @return trajectory
   */
  std::unique_ptr<LHCb::Trajectory> trajectoryFirstWire(int monolayer=0) const;

  /** Get trajectory representing the most right wire in (second=1) monolayer
   * @return trajectory
   */
  std::unique_ptr<LHCb::Trajectory> trajectoryLastWire(int monolayer=1) const;

  /** Get trajectory representing the wire identified by the LHCbID.
   * The offset is zero for all OT Trajectories
   * @return trajectory
   */
  std::unique_ptr<LHCb::Trajectory> trajectory( const LHCb::OTChannelID& aChan,
                                                double = 0 /*offset*/ ) const;


  /** Trajectory parameterized along y-axis */
  void trajectory(unsigned int aStraw, double& dxdy, double& dzdy,
		  double& xAtYEq0, double& zAtYEq0, double& ybegin, double& yend) const ;

  /** Set the status flags for all straws in this module. The vector
      can have 3 different lengths:
      - if the length is nChannels() or MAXNUMCHANNELS, then it contains one status
        flag per channel
      - if the length is 2 or 4, then it contains 1 status flag per otis
      - if the length is 1, it contains one satus flag for the entire module.
      Note: the flags are actually internally stored as 'unsigned chars',
      so don't use more than 8 bits.
  */
  StatusCode setStrawStatus( const std::vector< int >& statusflags );

  /** Get the vector of straw status flags from the condition */
  const std::vector< int >& strawStatus() const;

  /** Status flag for a straw */
  int strawStatus(unsigned int istraw) const { return m_strawStatus[istraw-1] ; }

  /** Set the t0 for a straw in this module */
  StatusCode setStrawT0s( const std::vector< double >& tzeros );

  /** Get the vector of straw t0s from the condition */
  const std::vector< double >& getStrawT0s() const;

  /** Time offset correction for a straw */
  double strawT0(unsigned int iStraw) const ;

  /** Reference time-of-flight correction for straw (c=1 particle from origin)*/
  double strawReferenceTimeOfFlight(unsigned int iStraw) const ;

  /** Set the rt-relation for all straws in this module. !! Only for experts
      or people who know what their doing. !! */
  //void setRtRelation(const OTDet::RtRelation& rtr) ;
  StatusCode setRtRelation(const OTDet::RtRelation& rtr) ;

  /** Rt-realtion for  all straws in this module */
  const OTDet::RtRelation& rtRelation() const ;

  /** Drift distance for given drift time */
  double driftRadius(double drifttime ) const ;

  /** Drift radius plus resolution for given drift time */
  OTDet::RadiusWithError driftRadiusWithError( double drifttime ) const ;

  /** Drift distance for given drift time. Extrapolate outside cell if
      time is outside [tmin,tmax]. Usefull for pattern recognition,
      but kind of slow. */
  double untruncatedDriftRadius( double drifttime ) const ;

  /** Drift time plus resolution for given drift radius */
  OTDet::DriftTimeWithError driftTimeWithError( double radius ) const ;

  /** Maximum drift time */
  double maxDriftTime() const ;

  /** Drifttime resolution for given drift time */
  double driftTimeResolution( double radius ) const ;

  /** Propagation velocity */
  double propagationVelocity() const ;

  /** Propagation velocity in y direction. Takes in account the
      readoutside f the wire. (A positive value is toward the readout
      side.) */
  double propagationVelocityY() const ;

  /** @return pointer to Calibration condition. !! Only for experts or people who think
       they know what their doing. !! */
  const Condition* calibrationCondition() const;

  /** @return pointer to Status condition. !! Only for experts or people who think
       they know what their doing. !! */
  const Condition* statusCondition() const;

  /** @return pointer to mono alignment condition. !! Only for experts or people who think
       they know what their doing. !! */
  const Condition* monoalignCondition() const ;

  /// return pitch of straws in one mono layer
  double xPitch() const { return m_xPitch ; }

  /** Set parameters for mono layer alignment **/
  StatusCode setMonoAlignment( const std::vector<double>& pars ) ;
  
  /** Get parameters for mono layer alignment **/
  void getMonoAlignment( std::vector<double>& pars ) const ;
  
  /// Private member methods
private:

  /// Not allowed to copy
  DeOTModule(const DeOTModule&);
  DeOTModule& operator=(const DeOTModule&);

  StatusCode cacheInfo();

  StatusCode calibrationCallback();

  StatusCode statusCallback();

  StatusCode monoalignCallback();

  /** Only for backwards compatibility with DC06 */
  void fallbackDefaults();

  /** Return range of hit straws for a given local entry and exit point.
   * @param entryPoint local entry point
   * @param exitPoint  local exit point
   * @param straws     hit straws
   */
  void findStraws(const Gaudi::XYZPoint& entryPoint,
                  const Gaudi::XYZPoint& exitPoint,
                  Straws& straws) const;

  /** Return distance of closest approach to wire i.e. drift distance
   * @param doca
   * @return ambiguity*doca
   */
  double driftDistance(const Gaudi::XYZVector& doca) const;

  /** @return local Z of a given straw */
  double localZOfStraw(const unsigned int aStraw) const;

  /** @return local U (=X) of a given straw */
  double localUOfStraw(const unsigned int aStraw) const;

  /** @return the straw in monolayer A closest to the hit */
  unsigned int hitStrawA(const double u) const;

  /** @return the straw in monolayer B closest to the hit */
  unsigned int hitStrawB(const double u) const;

  /** Check if Y is inside efficient region of monolayer A
   * @param  y    Y coordinate to check
   * @return bool
   */
  bool isEfficientA(const double y) const;

  /** Check if Y is inside efficient region of monolayer B
   * @param  y    Y coordinate to check
   * @return bool
   */
  bool isEfficientB(const double y) const;

  /** Calculates zc,uc of the center of a circle and radius
      rc of the circle (special assumptions) */
  void sCircle(const double z1, const double u1, const double z2,
               const double u2, const double dz,
               double& zc, double& uc, double& rc) const;

  void checkRtRelation(MsgStream& msg) const;
  void checkWalkRelation(MsgStream& msg) const;

  /// Private members
private :
  unsigned int m_stationID;      ///< station ID number
  unsigned int m_layerID;        ///< layer ID number
  unsigned int m_quarterID;      ///< quarter ID number
  unsigned int m_moduleID;       ///< module ID number
  LHCb::OTChannelID m_elementID; ///< element ID
  unsigned int m_uniqueModuleID; ///< module ID number
  unsigned int m_nStraws;        ///< half the number of straws inside module
  double m_angle;                ///< stereo angle of the layer
  double m_sinAngle;             ///< sine of stereo angle
  double m_cosAngle;             ///< cosine of stereo angle
  double m_xPitch;               ///< pitch in x between straws
  double m_zPitch;               ///< pitch in z bewteen straws
  double m_cellRadius;           ///< radius of a cell
  double m_inefficientRegion;    ///< size of the inefficient region
  double m_sensThickness;        ///< Thickness of sensitive volume
  unsigned int m_nModules;       ///< half the number of read-out modules in layer
  double m_ySizeModule;          ///< size of the module in y
  double m_xMinLocal;                           ///< local x min of module
  double m_xMaxLocal;                           ///< local x max of module
  double m_yMinLocal;                           ///< local y min of module
  double m_yMaxLocal;                           ///< local y max of module
  bool m_xInverted;                             ///< swap x min and x max
  bool m_yInverted;                             ///< swap y min and y max
  Gaudi::Plane3D m_plane;                       ///< plane through center of module
  Gaudi::Plane3D m_entryPlane;                  ///< entry plane
  Gaudi::Plane3D m_exitPlane;                   ///< exit plane
  Gaudi::XYZPoint m_centerModule;               ///< center of module
  std::unique_ptr<LHCb::OTWireTraj> m_trajFirstWire[2] ; ///< trajectory of first wire in monolayer
  double m_dxdy[2] ;                            ///< dx/dy along straw
  double m_dzdy[2] ;                            ///< dx/dz along straw
  double m_dy[2] ;                              ///< difference in y coordinates of straw end points
  Gaudi::XYZVector m_dp0di ;                    ///< vector with change in straw position per straw index
  Gaudi::XYZPoint  m_p0[2] ;                    ///< position of first straw
  double m_strawt0[MAXNUMCHAN] ;                ///< vector with t0 for every straw
  double m_strawdefaulttof[MAXNUMCHAN] ;        ///< vector with default tof correction for straw
  OTDet::RtRelation m_rtrelation ;              ///< rt-relation
  double m_propagationVelocity ;                ///< propagation velocity
  double m_propagationVelocityY ;               ///< propagation velocity in y-direction (cached for speed)
  double m_halfXPitch;                          ///< Half of the pitch in x (needed for staggering)
  double m_monoXZero[2];                        ///< offset of staggering in each monolayer
  // Calibration and status CONDDB stuff
  std::string           m_calibrationName;      ///< Name of calibration condition
  SmartRef< Condition > m_calibration;          ///< Calibration condition
  std::string           m_statusName;           ///< Name of calibration condition
  SmartRef< Condition > m_status;               ///< Status condition
  unsigned char m_strawStatus[MAXNUMCHAN] ;     ///< vector of channel statuses
private:
  OTDet::WalkRelation m_walkrelation ;          ///< walk-relation
  std::vector<double> m_monoDx ;
  static const std::string m_monoalignConditionName ;
  static const std::string m_monoalignParametersName ;
public:
  /**
   * Set the walk-relation for all straws in this module.
   */
  StatusCode setWalkRelation(const OTDet::WalkRelation& walkRelation);

  /**
   * Walk-relation for all straws in this module.
   */
  const OTDet::WalkRelation& walkRelation() const;

  double propagationTime(const LHCb::OTChannelID& channel, double arclen) const;
  double propagationTimeFromY(const LHCb::OTChannelID& channel, double globalY) const;
};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

#include "Kernel/Trajectory.h"

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

inline bool DeOTModule::contains(LHCb::OTChannelID aChannel) const {
  return (m_elementID.uniqueModule() == aChannel.uniqueModule());
}

inline bool DeOTModule::topModule() const {
  return m_quarterID > 1u;
}

inline bool DeOTModule::bottomModule() const {
  return m_quarterID < 2u;
}

/** Long modules go from 1-7 */
inline bool DeOTModule::longModule() const {
  return m_moduleID < 8u;
}

inline bool DeOTModule::s1Module() const {
  return (m_moduleID == 8u);
}

inline bool DeOTModule::s2Module() const {
  return ((m_quarterID == 1u || m_quarterID == 3u) && m_moduleID == 9u);
}

inline bool DeOTModule::s3Module() const {
  return ((m_quarterID == 0u || m_quarterID == 2u) && m_moduleID == 9u);
}

inline unsigned int DeOTModule::nModules() const {
  return 2*m_nModules;
}

inline unsigned int DeOTModule::nChannels() const {
  return 2*m_nStraws;
}

inline bool DeOTModule::monoLayerA(const unsigned int aStraw) const {
  return (aStraw <= m_nStraws);
}

inline bool DeOTModule::monoLayerB(const unsigned int aStraw) const {
  return (aStraw > m_nStraws);
}

inline unsigned char DeOTModule::mono(const unsigned int aStraw) const {
  return (aStraw-1)/m_nStraws ;
}

inline unsigned int DeOTModule::nextLeftStraw(const unsigned int aStraw) const {
  return (aStraw <= 1u || aStraw == m_nStraws+1u) ? 0u : aStraw-1u;
}

inline unsigned int DeOTModule::nextRightStraw(const unsigned int aStraw) const {
  return (aStraw == m_nStraws || aStraw >= 2*m_nStraws) ? 0u : aStraw+1u;
}

inline double DeOTModule::sensThickness() const {
  return m_sensThickness;
}

inline double DeOTModule::wireLength(const LHCb::OTChannelID aChan) const {

  double wireLength = m_ySizeModule;

  /// check if it is a long module goes from 1 to 7
  if ( aChan.module() < 8u) {
    /// check if it is top module
    if (aChan.quarter() > 1u) {
      /// check if it is the first or second monolayer
      wireLength = ((aChan.straw() <= m_nStraws)?m_ySizeModule-m_inefficientRegion:m_ySizeModule);
      /// check if it is bottom module
    } else if (aChan.quarter() < 2u) {
      /// check if it is the first or second monolayer
      wireLength = ((aChan.straw() <= m_nStraws)?m_ySizeModule:m_ySizeModule-m_inefficientRegion);
    }
  }

  return wireLength;
}

inline double DeOTModule::cellRadius() const {
  return m_cellRadius;
}

inline double DeOTModule::angle() const {
  return m_angle;
}

inline double DeOTModule::sinAngle() const {
  return m_sinAngle;
}

inline double DeOTModule::cosAngle() const {
  return m_cosAngle;
}

inline double DeOTModule::driftDistance(const Gaudi::XYZVector& doca) const {
  int ambiguity = (doca.x() > 0 ? 1 : -1);
  return ambiguity*doca.r();
}

inline Gaudi::XYZPoint DeOTModule::toLocal(const Gaudi::XYZPoint& aPoint) const {
  return this->geometry()->toLocal(aPoint);
}

inline Gaudi::XYZPoint DeOTModule::localPoint(const double x,
					      const double y,
					      const double z) const {
  return toLocal(Gaudi::XYZPoint(x, y, z));
}

inline Gaudi::XYZPoint DeOTModule::toGlobal(const Gaudi::XYZPoint& aPoint) const {
  return this->geometry()->toGlobal(aPoint);
}

inline Gaudi::XYZPoint DeOTModule::globalPoint(const double x,
					       const double y,
					       const double z) const {
  return toGlobal(Gaudi::XYZPoint(x, y, z));
}

/// This gives you the x position of the wire
inline double DeOTModule::localUOfStraw(const unsigned int aStraw) const {
  // why is this so terribly complicated?
  unsigned int tmpStraw = (aStraw-1u)%m_nStraws ;
  unsigned int mono = this->mono(aStraw) ;
  //double uLeftStraw = (!monoLayerB(aStraw)?(-(0.5*m_nStraws-0.25)+0.5)
  //                     :-(0.5*m_nStraws-0.25))*m_xPitch;
  double uLeftStraw = (double( m_nStraws ) + m_monoXZero[mono])*-m_halfXPitch;
  return uLeftStraw + tmpStraw * m_xPitch ;
}

inline double DeOTModule::localZOfStraw(const unsigned int aStraw) const {
  return ( mono(aStraw) - 0.5 )*m_zPitch;
}

inline double DeOTModule::distanceAlongWire(const double xHit,
					    const double yHit) const {
  // For the upper modules of the station the readout is above.
  return ((m_quarterID > 1u)?m_yMaxLocal-localPoint(xHit, yHit, 0).y()
	  :m_yMaxLocal+localPoint(xHit, yHit, 0).y());
}

inline Gaudi::XYZPoint DeOTModule::centerOfModule() const {
  /// get the global coordinate of the middle of the module
  return m_centerModule;
}

inline double DeOTModule::z() const {
  /// Get the global z-coordinate of the module
  return centerOfModule().z();
}

inline Gaudi::Plane3D DeOTModule::plane() const {
  return m_plane;
}

inline Gaudi::Plane3D DeOTModule::entryPlane() const {
  return m_entryPlane;
}

inline Gaudi::Plane3D DeOTModule::exitPlane() const {
  return m_exitPlane;
}

inline unsigned int DeOTModule::hitStrawA(const double u) const {
  double dU = u - localUOfStraw(1);
  // this can be negative!
  int strawA = int(dU/m_xPitch + 1.5);
  return std::min(std::max(1,strawA), int(m_nStraws));
}

inline unsigned int DeOTModule::hitStrawB(const double u) const {
  double dU = u - localUOfStraw(m_nStraws+1);
  // this can be negative
  int strawB = int(m_nStraws) + int(dU/m_xPitch + 1.5);
  return std::min(std::max(int(m_nStraws) + 1,strawB),int(2*m_nStraws));
}

/// See LHCb note: 2003-019
inline bool DeOTModule::isEfficientA(const double y) const {
  // check if hit is not inside the inefficient region
//   return !((m_moduleID < 8u) && (m_quarterID > 1u) &&
//           ((m_yMaxLocal + y) < m_inefficientRegion));
  /// In local frame
  bool topeven   = ( m_quarterID > 1u && m_layerID%2 == 0 ) && ( ( m_yMaxLocal + y ) < m_inefficientRegion );
  bool bottomodd = ( m_quarterID < 2u && m_layerID%2 != 0 ) && ( ( m_yMaxLocal - y ) < m_inefficientRegion );
  return !( m_moduleID < 8u  && ( topeven || bottomodd ) );

}

inline bool DeOTModule::isEfficientB(const double y) const {
  // check if hit is not inside the inefficient region
//   return !((m_moduleID < 8u) && (m_quarterID < 2u) &&
//   	   ((m_yMaxLocal - y) < m_inefficientRegion));
  bool bottomeven = ( m_quarterID < 2u && m_layerID%2 == 0 ) && ( ( m_yMaxLocal - y ) < m_inefficientRegion );
  bool topodd     = ( m_quarterID > 1u && m_layerID%2 != 0 ) && ( ( m_yMaxLocal + y ) < m_inefficientRegion );
  return !( m_moduleID < 8u  && ( bottomeven || topodd ) );
}

inline void DeOTModule::trajectory(unsigned int aStraw,
				   double& dxdy, double& dzdy,
				   double& xAtYEq0, double& zAtYEq0,
				   double& ybegin, double& yend) const
{
  unsigned int mono = this->mono(aStraw) ;
  unsigned int tmpstraw = (aStraw - 1)%m_nStraws ;
  dxdy    = m_dxdy[mono] ;
  dzdy    = m_dzdy[mono] ;
  ybegin  = m_p0[mono].y() + tmpstraw * m_dp0di.y() ;
  yend    = ybegin + m_dy[mono] ;
  xAtYEq0 = m_p0[mono].x() + tmpstraw * m_dp0di.x() - ybegin * dxdy ;
  zAtYEq0 = m_p0[mono].z() + tmpstraw * m_dp0di.z() - ybegin * dzdy ;
}

inline double DeOTModule::driftRadius( double drifttime ) const {
  return m_rtrelation.radius(drifttime) ;
}

inline double DeOTModule::untruncatedDriftRadius( double drifttime ) const {
  return m_rtrelation.extrapolatedradius(drifttime) ;
}

inline OTDet::RadiusWithError DeOTModule::driftRadiusWithError( double drifttime ) const {
  return m_rtrelation.radiusWithError(drifttime) ;
}

inline OTDet::DriftTimeWithError DeOTModule::driftTimeWithError( double radius ) const {
  return m_rtrelation.drifttimeWithError(radius) ;
}

inline double DeOTModule::maxDriftTime() const {
  return m_rtrelation.tmax() ;
}

inline double DeOTModule::driftTimeResolution( double radius ) const {
  return m_rtrelation.drifttimeError( radius ) ;
}

inline double DeOTModule::propagationVelocity() const {
  return m_propagationVelocity ;
}

inline double DeOTModule::propagationVelocityY() const {
  return m_propagationVelocityY ;
}

inline double DeOTModule::strawT0( unsigned int istraw ) const {
  return m_strawt0[istraw-1] ;
}

inline double DeOTModule::strawReferenceTimeOfFlight(unsigned int istraw) const {
  return m_strawdefaulttof[istraw-1] ;
}

//inline void DeOTModule::setStrawT0( unsigned int istraw, double t0 ) {
//  m_strawt0[istraw-1] = t0 ;
//}

inline const OTDet::RtRelation& DeOTModule::rtRelation() const {
  return m_rtrelation ;
}

//inline void DeOTModule::setRtRelation( const OTDet::RtRelation& rtr) {
//  m_rtrelation = rtr ;
//}

inline const Condition* DeOTModule::calibrationCondition() const {
  return hasCondition( m_calibrationName ) ? m_calibration.target() : static_cast< const Condition* >( 0 );
}

inline const Condition* DeOTModule::statusCondition() const {
  return hasCondition( m_statusName ) ? m_status.target() : static_cast< const Condition* >( 0 );
}

inline const Condition* DeOTModule::monoalignCondition() const {
  return hasCondition( m_monoalignConditionName ) ? condition( m_monoalignConditionName ).target() : 0 ;
}

inline const OTDet::WalkRelation& DeOTModule::walkRelation() const {
  return m_walkrelation;
}

#endif  // OTDET_DEOTMODULE_H
