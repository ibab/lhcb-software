// $Id: DeOTDetector.h,v 1.33 2007-02-02 09:25:04 janos Exp $
#ifndef OTDET_DEOTDETECTOR_H
#define OTDET_DEOTDETECTOR_H 1

/// GaudiKernel
#include "GaudiKernel/VectorMap.h"

/// DetDesc
#include "DetDesc/DetectorElement.h"

/// Kernel
#include "Kernel/OTChannelID.h"

/** @class DeOTDetector DeOTDetector.h "OTDet/DeOTDetector.h"
 *
 *  This is the detector element class for the Outer Tracker. It 
 *  is able to get the geometry from the XML. Many geometry parameters are
 *  stored inside it grand-children, which are derived from DetectorElement
 *  as well: DeOTModule. DeOTDetector is used by the OT digitization and 
 *  the track reconstruction.
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl 
 *  @date   26-05-2002
 */

/// Forward declarations
class DeOTStation;
class DeOTLayer;
class DeOTQuarter;
class DeOTModule;

namespace LHCb
{
  class Point3DTypes;
  class LHCbID;
  class Trajectory;
}

namespace DeOTDetectorLocation{
  /// Outer Tracker location in transient detector store
  static const std::string& Default="/dd/Structure/LHCb/AfterMagnetRegion/T/OT";
};

static const CLID& CLID_DeOTDetector = 8101;

class DeOTDetector : public DetectorElement {

public:
  
  /** Some typedefs */
  typedef std::vector<DeOTStation*> Stations;
  typedef std::vector<DeOTLayer*> Layers;
  typedef std::vector<DeOTQuarter*> Quarters;
  typedef std::vector<DeOTModule*> Modules;
  typedef GaudiUtils::VectorMap<unsigned int, DeOTStation*> MapIDStation;
  typedef GaudiUtils::VectorMap<unsigned int, DeOTLayer*> MapIDLayer;
  typedef GaudiUtils::VectorMap<unsigned int, DeOTQuarter*> MapIDQuarter;
  typedef GaudiUtils::VectorMap<unsigned int, DeOTModule*> MapIDModule;
  
  /** Constructor */
  DeOTDetector(const std::string& name = "");
  
  /** Destructor */
  ~DeOTDetector();
    
  /** Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const;
  
  /** Another reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID() { return CLID_DeOTDetector ; }
  
  /** Initialization method 
   * @return Status of initialisation
   */
  virtual StatusCode initialize();
  
  /** @return number of first station */
  unsigned int firstStation() const; 
  
  /** @return number of last station */
  unsigned int lastStation() const;

  /** @return number of stations */
  unsigned int nStation() const; 
  
  /** @return the station for a given channelID */
  DeOTStation* findStation(const LHCb::OTChannelID aChannel) const;
  
  /** @return the station for a given XYZ point */
  DeOTStation* findStation(const Gaudi::XYZPoint& aPoint) const;

  /** Check contains channel
   *  @param channel
   *  @return bool
   */
  bool contains(const LHCb::OTChannelID aChannel) const;

  /** Check channel number is valid 
   * @param channel
   * @return bool
   */
  /* bool isValid(const LHCb::OTChannelID aChannel); */
  
  /** @return the layer for a given channelID */
  DeOTLayer* findLayer(const LHCb::OTChannelID aChannel) const;

  /** @return the layer for a given XYZ point */
  DeOTLayer* findLayer(const Gaudi::XYZPoint& aPoint) const;
  
  /** @return the layer for a given channelID/XYZ point */
  /*  template <typename T> DeOTLayer* DeOTDetector::findLayer(const T& t)  const; */

  /** @return the quarter for a given channelID */
  DeOTQuarter* findQuarter(const LHCb::OTChannelID aChannel) const;

  /** @return the quarter for a given XYZ point */
  DeOTQuarter* findQuarter(const Gaudi::XYZPoint& aPoint) const;

  /** @return the module for a given channel ID */
  DeOTModule* findModule(const LHCb::OTChannelID aChannel) const;

  /** @return the module for a given XYZ point */
  DeOTModule* findModule(const Gaudi::XYZPoint& aPoint) const;

  /** @return the channel left from a given channel 
   * @param channel
   */
  LHCb::OTChannelID nextChannelLeft(const LHCb::OTChannelID aChannel) const;

  /** @return the channel right from a given channel 
   * @param channel
   */
  LHCb::OTChannelID nextChannelRight(const LHCb::OTChannelID aChannel) const;

  /** @return sentive volume id for a given global XYZ point */
  const int sensitiveVolumeID(const Gaudi::XYZPoint& aPoint) const;
  
  /** Calculate the distance along a wire for a given channel and XY position
   * @param channel
   * @param x coordinate
   * @param y coordinate
   * @return distance
   */
  double distanceAlongWire(const LHCb::OTChannelID aChannel, 
                           double xHit, double yHit) const;

  /** Get the straw resolution
   * @return straw resolution
   */   
  double resolution() const;
    
  /** Get B-field corrected resolution
   * @param By
   * @return resolution
   */
  double resolution(const double by) const;

  /** @return propagation delay (ns/mm) */
  double propagationDelay() const;

  /** Calculate the propagation delay along a wire
   * @param channel
   * @param x-coordinate
   * @param y-coordinate 
   */
  double propagationTime(const LHCb::OTChannelID aChannel, 
                         const double x, const double y ) const;
  
  /** Calculate max drift as function of By
   * @param By
   * @return Max drift time correction
   */
  double maxDriftTimeFunc(const double by) const;
  
  /** @return r-t relation */
  double driftTime(const double driftDist) const;
  
  /** @return B-field corrected r-t relation */
  double driftTime(const double driftDist, const double by) const;
  
  /** @return inverse r-t relation */
  double driftDistance( const double driftTime) const;

  /** @return B-field corrected inverse r-t relation */
  double driftDistance( const double driftTime, const double by ) const;
  
  /** @return drift delay (ns/mm) */
  double driftDelay() const;

  /** @return dead time (ns) */
  double deadTime() const;
  
  /** Flat vector of OT station
   * @return vector of station
   */
  const Stations& stations() const;

  /** Flat vector of OT layers
   * @return vector of layers
   */
  const Layers& layers() const;
  
  /** Flat vector of OT quarters
   * @return vector of quarters
   */
  const Quarters& quarters() const;

  /** Flat vector of OT modules
   * @return vector of modules
   */
  const Modules& modules() const;
  
  /** @return the total number of readout channels */
  unsigned int nChannels();
  
  /** @return the maximum number of channels in a module */
  unsigned int nMaxChanInModule();

  /** Get trajectory representing the most left wire in (first=0) monolayer  
   * @return trajectory
   */
  std::auto_ptr<LHCb::Trajectory> trajectoryFirstWire(const LHCb::LHCbID& id, 
						      int monolayer=0) const;
  
  /** Get trajectory representing the most right wire in (second=0) monolayer
   * @return trajectory
   */
  std::auto_ptr<LHCb::Trajectory> trajectoryLastWire(const LHCb::LHCbID& id, 
						     int monolayer=1) const;

  /** Get trajectory representing the wire identified by the LHCbID
   * The offset is zero for all OT Trajectories
   * @return trajecory
   */ 
  std::auto_ptr<LHCb::Trajectory> trajectory(const LHCb::LHCbID& id, 
					     const double =0 /** offset */) const;
  
private:

  /** set the first station */
  void setFirstStation(const unsigned int iStation);

  Stations m_stations;             ///< flat vector of stations
  Layers m_layers;                 ///< flat vector of layers
  Quarters m_quarters;             ///< flat vector of quarters
  Modules m_modules;               ///< flat vector of modules 
  MapIDStation m_mapIDStation;     ///< map station id to station
  MapIDLayer m_mapIDLayer;         ///< map layer id to layer
  MapIDQuarter m_mapIDQuarter;     ///< map quarter id to quarter
  MapIDModule m_mapIDModule;       ///< map module id to module
  unsigned int m_firstStation;     ///< number of first station
  unsigned int m_nChannels;        ///< total number of channels in OT
  unsigned int m_nMaxChanInModule; ///< the maximum # channels in 1 module

  /** General Outer Tracker pramaters */
  double m_cellRadius;             ///< cell radius
  double m_resolution;             ///< straw resolution
  double m_propagationDelay;       ///< Propagation time delay
  double m_maxDriftTime;           ///< maximum drift time
  double m_maxDriftTimeCor;        ///< magn. correction on maximum drift time
  double m_deadTime;               ///< deadtime
};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

inline unsigned int DeOTDetector::firstStation() const {
  return m_firstStation;
}

inline unsigned int DeOTDetector::lastStation() const {
  return m_firstStation + m_stations.size() - 1u;
}

inline unsigned int DeOTDetector::nStation() const {
  return m_stations.size();
}

inline bool DeOTDetector::contains(const LHCb::OTChannelID aChannel) const{
  return ((aChannel.station() >= firstStation()) && (aChannel.station() <= lastStation()));
}

/* inline bool DeOTDetector::isValid(const LHCb::OTChannelID aChannel) { */
/*   DeOTDetector::Modules::iterator iter = m_modules.begin(); */
/*   while((iter != m_modules.end()) && !((*iter)->contains(aChannel))) { */
/*     ++iter; */
/*   } // iter */
/*   return (iter != m_modules.end() ? (*iter)->isWire(aChannel.wire()) : false ); */
/* } */

inline double DeOTDetector::resolution() const {  
  return m_resolution;
}

inline double DeOTDetector::resolution(const double /*by */) const {  
  /// Calculate resolution
  /// The form is p1+p2*B^2 (empirical fit to the testbeam results)
  /// p1, p2 vary for different gas mixes
  /// The parameterization is only valid for Bx<1.4T (or there abouts)

  return m_resolution;
}

inline double DeOTDetector::propagationDelay() const {
  return m_propagationDelay;
}

inline double DeOTDetector::maxDriftTimeFunc(const double by) const {
  /// Calculate max drift as function of B 
  /// The form is p1+p2*B^2 (empirical fit to the testbeam results)
  /// p1, p2 vary for different gas mixes
  /// The parameterization is only valid for Bx<1.4T (or there abouts)

  return (m_maxDriftTime + (m_maxDriftTimeCor * by * by) );
}
  
inline double DeOTDetector::driftTime(const double driftDist) const {
  /// r-t relation without correction for the magnetic field
  /// convert r to t - hack as drift dist can > rCell
  return (fabs(driftDist) < m_cellRadius?(fabs(driftDist) / m_cellRadius) : 1)*m_maxDriftTime;
}

inline double DeOTDetector::driftTime(const double driftDist, const double by) const
{
  /// r-t relation with correction for the magnetic field
  /// get max drift time 
  double maxDriftTime = maxDriftTimeFunc( by );
  /// convert r to t - hack as drift dist can > rCell
  double driftTime = (fabs(driftDist) < m_cellRadius ? fabs(driftDist)/m_cellRadius : 1)*maxDriftTime;
  return driftTime;
}

inline double DeOTDetector::driftDistance( const double driftTime) const {
  /// inverse r-t relation without correction for the magnetic field
  return (driftTime * m_cellRadius) / m_maxDriftTime;
}

inline double DeOTDetector::driftDistance( const double driftTime, const double by ) const {
  /// inverse r-t relation with correction for the magnetic field
  /// get max drift time with correction for magnetic field
  double maxDriftTime = maxDriftTimeFunc(by);

  /// inverse r-t relation
  return driftTime * m_cellRadius / maxDriftTime;  
}

inline double DeOTDetector::driftDelay() const {
  return m_maxDriftTime/m_cellRadius;
}

inline double DeOTDetector::deadTime() const {
  return m_deadTime;
}

inline const DeOTDetector::Stations& DeOTDetector::stations() const {
  return m_stations;
}

inline const DeOTDetector::Layers& DeOTDetector::layers() const {
  return m_layers;
}

inline const DeOTDetector::Quarters& DeOTDetector::quarters() const {
  return m_quarters;
}

inline const DeOTDetector::Modules& DeOTDetector::modules() const {
  return m_modules;
}

inline unsigned int DeOTDetector::nChannels() {
  return m_nChannels;
}

inline unsigned int DeOTDetector::nMaxChanInModule() {
  return m_nMaxChanInModule;
}

#endif  // OTDET_DEOTDETECTOR_H
