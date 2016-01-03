// $Id: DeOTDetector.h,v 1.39 2008-10-28 12:53:40 cattanem Exp $
#ifndef OTDET_DEOTDETECTOR_H
#define OTDET_DEOTDETECTOR_H 1

/// GaudiKernel
#include "GaudiKernel/VectorMap.h"

/// DetDesc
#include "DetDesc/DetectorElement.h"

/// Kernel
#include "Kernel/OTChannelID.h"

/// Local
#include "OTDet/DeOTStation.h"
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTQuarter.h"
#include "OTDet/DeOTModule.h"
#include "OTDet/IndexToDetElementMap.h"

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

namespace LHCb
{
  class LHCbID;
  class Trajectory;
}

namespace DeOTDetectorLocation{
  /// Outer Tracker location in transient detector store
  static const std::string& Default="/dd/Structure/LHCb/AfterMagnetRegion/T/OT";
}

static const CLID CLID_DeOTDetector = 8101;

class DeOTDetector : public DetectorElement {

public:
  
  /** Some typedefs */
  typedef std::vector<DeOTStation*> Stations;
  typedef std::vector<DeOTLayer*> Layers;
  typedef std::vector<DeOTQuarter*> Quarters;
  typedef std::vector<DeOTModule*> Modules;
  
  /** Constructor */
  DeOTDetector(const std::string& name = "");
  
  /** Destructor */
  ~DeOTDetector() = default;
    
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
  
  /** Check if station  contains channel
   *  @param  aChannel an OT channel id
   *  @return bool True if channel is contained
   */
  bool contains(const LHCb::OTChannelID aChannel) const;

  /** Check channel number is valid 
   * @param aChannel
   * @return bool
   */
  /* bool isValid(const LHCb::OTChannelID aChannel); */

  /** Const method to return the station for a given channel id
   * @param  aChannel  an OT channel id
   * @return const pointer to detector element
   */
  const DeOTStation* findStation(const LHCb::OTChannelID& aChannel) const;

  /** Non const method to return the station for a given channel id
   * @param  aChannel  an OT channel id
   * @return pointer to detector element
   */
  DeOTStation* findStation(const LHCb::OTChannelID& aChannel);

  /** Const method to return the station for a given XYZ point
   * @param  aPoint the given point
   * @return const pointer to detector element
   */
  const DeOTStation* findStation(const Gaudi::XYZPoint& aPoint) const;

  /** Non onst method to return the station for a given XYZ point
   * @param  aPoint the given point
   * @return const pointer to detector element
   */
  DeOTStation* findStation(const Gaudi::XYZPoint& aPoint);

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

  /** @return the channel left from a given channel 
   *  @param aChannel The given channel
   */
  LHCb::OTChannelID nextChannelLeft(const LHCb::OTChannelID aChannel) const;

  /** @return the channel right from a given channel 
   *  @param aChannel The given channel
   */
  LHCb::OTChannelID nextChannelRight(const LHCb::OTChannelID aChannel) const;

  /** @return sentive volume id for a given global XYZ point */
  int sensitiveVolumeID(const Gaudi::XYZPoint& aPoint) const;
  
  /** Calculate the distance along a wire for a given channel and XY position
   * @param aChannel The given channel
   * @param xHit     x coordinate
   * @param yHit     y coordinate
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
  double resolution(const double By) const;

  /** @return propagation delay (ns/mm) */
  double propagationDelay() const;

  /** Calculate the propagation delay along a wire
   * @param aChannel The given channel
   * @param x        x coordinate
   * @param y        y coordinate
   */
  double propagationTime(const LHCb::OTChannelID aChannel, 
                         const double x, const double y ) const;
  
  /** Calculate max drift as function of By
   * @param By
   * @return Max drift time correction
   */
  double maxDriftTimeFunc(const double By) const;
  
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
  std::unique_ptr<LHCb::Trajectory> trajectoryFirstWire(const LHCb::LHCbID& id, 
                                                        int monolayer=0) const;
  
  /** Get trajectory representing the most right wire in (second=0) monolayer
   * @return trajectory
   */
  std::unique_ptr<LHCb::Trajectory> trajectoryLastWire(const LHCb::LHCbID& id, 
                                                       int monolayer=1) const;

  /** Get trajectory representing the wire identified by the LHCbID
   * The offset is zero for all OT Trajectories
   * @return trajecory
   */ 
  std::unique_ptr<LHCb::Trajectory> trajectory(const LHCb::LHCbID& id, 
                                               double =0 /** offset */) const;


 public:
  /// Nested class to communicate to the updatesvc for the
  /// calibration. Advantage of object is that DeOTModule can also
  /// register to it.
  struct Calibration
  {
    Calibration( SmartRef<Condition> c ) : m_condition(c) {}
    virtual ~Calibration() {}
    StatusCode callback() ;
    SmartRef<Condition> m_condition ;
    double m_globalT0 ;
  } ;
  
  /** get the calibrationholder */
  const Calibration* globalCalibration() const {
    return m_calibration.get() ;
  }
  
  /** get the global T0 offset */
  double globalT0() const { return m_calibration ? m_calibration->m_globalT0 : 0 ; }

  /** set the global T0 offset */
  void setGlobalT0(double t0) ;
  
private:
  /// three stations; starting from 1
  typedef OT::IndexToDetElementMap<DeOTStation, 3, 1> MapStations;

  /** set the first station */
  void setFirstStation(const unsigned int iStation);

  Stations m_stations;             ///< flat vector of stations
  Layers m_layers;                 ///< flat vector of layers
  Quarters m_quarters;             ///< flat vector of quarters
  Modules m_modules;               ///< flat vector of modules
  MapStations m_mapStations;       ///< map of stations
  unsigned int m_firstStation = 0u;     ///< number of first station

  /** General Outer Tracker pramaters */
  unsigned int m_nChannels = 0u;        ///< total number of channels in OT
  unsigned int m_nMaxChanInModule = 0u; ///< the maximum # channels in 1 module
  double m_cellRadius = 0.;             ///< cell radius
  double m_resolution = 0.;             ///< straw resolution
  double m_propagationDelay = 0.;       ///< Propagation time delay
  double m_maxDriftTime = 0.;           ///< maximum drift time
  double m_maxDriftTimeCor = 0.;        ///< magn. correction on maximum drift time
  double m_deadTime = 0.;               ///< deadtime
  std::unique_ptr<Calibration> m_calibration;     ///< holder for global calibration parameters (e.g. t0)
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

inline bool DeOTDetector::contains(const LHCb::OTChannelID aChannel) const {
  return ((aChannel.station() >= firstStation()) && (aChannel.station() <= lastStation()));
}

/* inline bool DeOTDetector::isValid(const LHCb::OTChannelID aChannel) { */
/*   DeOTDetector::Modules::iterator iter = m_modules.begin(); */
/*   while((iter != m_modules.end()) && !((*iter)->contains(aChannel))) { */
/*     ++iter; */
/*   } // iter */
/*   return (iter != m_modules.end() ? (*iter)->isWire(aChannel.wire()) : false ); */
/* } */

/// Find station methods
inline const DeOTStation* DeOTDetector::findStation(const LHCb::OTChannelID& aChannel) const {
  return m_mapStations[aChannel.station()];
}

inline DeOTStation* DeOTDetector::findStation(const LHCb::OTChannelID& aChannel) {
  return m_mapStations[aChannel.station()];
}

inline DeOTStation* DeOTDetector::findStation(const Gaudi::XYZPoint& aPoint) {
  return const_cast<DeOTStation*>(static_cast<const DeOTDetector&>(*this).findStation(aPoint));
}

/// Find layer methods
inline const DeOTLayer* DeOTDetector::findLayer(const LHCb::OTChannelID& aChannel) const {
  const DeOTStation* s = findStation(aChannel);
  return s ? s->findLayer(aChannel) : 0; 
}

inline DeOTLayer* DeOTDetector::findLayer(const LHCb::OTChannelID& aChannel) {
  DeOTStation* s = findStation(aChannel);
  return s ? s->findLayer(aChannel) : 0; 
}

inline DeOTLayer* DeOTDetector::findLayer(const Gaudi::XYZPoint& aPoint) {
  return const_cast<DeOTLayer*>(static_cast<const DeOTDetector&>(*this).findLayer(aPoint));
}

/// Find quarter methods
inline const DeOTQuarter* DeOTDetector::findQuarter(const LHCb::OTChannelID& aChannel) const {
  const DeOTLayer* l = findLayer(aChannel);
  return l ? l->findQuarter(aChannel) : 0; 
}

inline DeOTQuarter* DeOTDetector::findQuarter(const LHCb::OTChannelID& aChannel) {
  DeOTLayer* l = findLayer(aChannel);
  return l ? l->findQuarter(aChannel) : 0;
}

inline DeOTQuarter* DeOTDetector::findQuarter(const Gaudi::XYZPoint& aPoint) {
  return const_cast<DeOTQuarter*>(static_cast<const DeOTDetector&>(*this).findQuarter(aPoint));
}

/// Find module methods
inline const DeOTModule* DeOTDetector::findModule(const LHCb::OTChannelID& aChannel) const {
  const DeOTQuarter* q = findQuarter(aChannel) ;
  return q ? q->findModule(aChannel) : 0; 
}

inline DeOTModule* DeOTDetector::findModule(const LHCb::OTChannelID& aChannel) {
  DeOTQuarter* q = findQuarter(aChannel) ;
  return q ? q->findModule(aChannel) : 0;
}

inline DeOTModule* DeOTDetector::findModule(const Gaudi::XYZPoint& aPoint) {
  return const_cast<DeOTModule*>(static_cast<const DeOTDetector&>(*this).findModule(aPoint));
}

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
