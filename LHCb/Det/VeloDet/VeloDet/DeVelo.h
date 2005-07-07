// $Id: DeVelo.h,v 1.27 2005-07-07 16:09:14 mtobin Exp $
#ifndef       VELODET_DEVELO_H
#define       VELODET_DEVELO_H 1
// ============================================================================
/// from STL
#include <vector>

/// Local
#include "VeloDet/DeVeloSensor.h"
#include "VeloDet/DeVeloRType.h"
#include "VeloDet/DeVeloPhiType.h"

/** @class DeVelo DeVelo.h "VeloDet/DeVelo.h" 
 *
 *  Velo detector element class.
 *
 *  @author David Hutchcroft David.Hutchcroft@cern.ch
 *  @author Mark Tobin Mark.Tobin@cern.ch
 *  @date 22/4/2003
 */

static const CLID& CLID_DeVelo = 8100; 

class DeVelo: public DetectorElement {

public:
  
  /// Constructors
  DeVelo( const std::string& name    = "" );
  
  /// (virtual) Destructor
  virtual ~DeVelo() ;

  /// object identification
  static  const CLID& classID ()       { return CLID_DeVelo ; } 
  /// object identification
  virtual const CLID& clID    () const ;
  
  /// initialization method 
  virtual StatusCode initialize(); 

  /// return the number of sensors
  inline unsigned int numberSensors()  const { 
    return m_vpSensor.size(); 
  }

  /// Return the number of Velo sensors (no pile-up)
  inline unsigned int numberNonPUSensors()  const { 
    return m_nRSensors+m_nPhiSensors; 
  }

  /// return the number of PileUp sensors
  inline unsigned int numberPileUpSensors()  const{
    return m_nPileUpSensors;
  }

  /// return the number of R type sensors
  inline unsigned int numberRSensors()  const{
    return m_nRSensors;
  }

  /// return the number of Phi type sensors
  inline unsigned int numberPhiSensors()  const{
    return m_nPhiSensors;
  }

  /// return the sensor type
  inline std::string type(unsigned int sensor) const
  {
    return m_vpSensor[sensorIndex(sensor)]->type();
  }
  
  /// return true if this is an PileUp type sensor
  inline bool isPileUpSensor(unsigned int sensor) const {
    return m_vpSensor[sensorIndex(sensor)]->isPileUp();
  }

  /// return true if this is an R type sensor
  inline bool isRSensor(unsigned int sensor) const {    
    return m_vpSensor[sensorIndex(sensor)]->isR();
  }

  /// return true if this is an Phi type sensor
  inline bool isPhiSensor(unsigned int sensor) const {
    return m_vpSensor[sensorIndex(sensor)]->isPhi();
  }

  /// return true if phi sensor is second stereo (Downstream)
  inline bool isDownstreamSensor(unsigned int sensor) const {
    return m_vpSensor[sensorIndex(sensor)]->isDownstream();
  }

  /// return the sensor number for a point (global frame)
  unsigned int sensorNumber( const HepPoint3D& point ) const;
  
  /// return the sensor number for a given index
  unsigned int sensorNumber( const unsigned int index ) const{
    return m_vpSensor[index]->sensorNumber();
  };
  
  /// return the index of the sensor (assumes sensors are stored 
  unsigned int sensorIndex(unsigned int sensor) const;

  /// Return copy of vector of pointers to all sensors sorted by increasing z
  inline std::vector<DeVeloSensor*> vpSensors() const {
    return m_vpSensor;
  }

  /// Return copy vector of pointers to the R sensors sorted by increasing z
  inline std::vector<DeVeloRType*> vpRSensors() const {
    return m_vpRSensor;
  }
  
  /// Return copy of vector of pointers to the Phi sensors 
  /// sorted by increasing z
  inline std::vector<DeVeloPhiType*> vpPhiSensors() const {
    return m_vpPhiSensor;
  }
  
  /// Return vector of pointers to the Pile Up sensors sorted by increasing z
  inline std::vector<DeVeloRType*> vpPileUpSensors() const {
    return m_vpPUSensor;
  }
  
  /** Gives the VeloChannelID and offset (in fraction of a pitch width) 
      associated to a 3D position. with pitch width in mm
      Sign convention is offset is +- 0.5 
      with +ve in increasing stip number  (global frame) */
  StatusCode pointToChannel(const HepPoint3D &point, 
                            VeloChannelID &channel,
                            double &localOffset,
                            double &pitch) const;

  /// pointToChannel if sensor known (global frame)
  StatusCode pointToChannel(const HepPoint3D &point, 
                            const unsigned int &sensor,
                            VeloChannelID &channel,
                            double &localOffset,
                            double &pitch) const;


  /** Residual of 3D point to a VeloChannelID
      returns offset in mm from closest point on channel */
  StatusCode residual(const HepPoint3D &point, 
                      const VeloChannelID &channel,
                      double &residual,
                      double &chi2) const;

  /** Residual of 3D point to a VeloChannelID + offset in fraction of a channel
      and width of the cluster in channel widths (for the chi2)
      returns offset in mm, and chi^2 from position to point */
  StatusCode residual(const HepPoint3D &point, 
                      const VeloChannelID &channel,
                      const double &localOffset,
                      const double &width,
                      double &residual,
                      double &chi2) const;

  /// Convert global 3D Point to local 3D point in frame of the sensor 
  StatusCode globalToLocal(const unsigned int &sensorNumber,
                           const HepPoint3D &global,
                           HepPoint3D &local) ;
  
  /// Convert local 3D Point in the sensor fram to a global 3D point 
  StatusCode localToGlobal(const unsigned int &sensorNumber,
                           const HepPoint3D &local,
                           HepPoint3D &global) const;

  /// Get the nth (signed) neighbour strip to a given VeloChannelID
  StatusCode neighbour(const VeloChannelID &startChannel,
                       const int &Offset,
                       VeloChannelID &channel) const;

  /// Check the distance in strips between two channelIDs
  StatusCode channelDistance(const VeloChannelID &startChannel,
                             const VeloChannelID &endChannel,
                             int &Offset) const;

  /** Returns the vector of sensor numbers one can match with the 
      specified sensor number, r and pileup sensors match to phi, phi to R 
      Returns a list (0,1,2) of sensor numbers (same +/- x side) 
      with the closest in Z listed first */
  StatusCode sensorAssociated( unsigned int sensor, 
                               std::vector<unsigned int> &assocSensor ) const;
  
  /** Returns the offical (not alignment corrected) z position of this 
      Velo sensor */
  inline double zSensor( unsigned int sensor )  const { 
    return m_sensorZ[sensorIndex(sensor)]; 
  }
  
  /// returns the number of zones in this sensor
  unsigned int numberOfZones( unsigned int sensor) const;

  /// returns the phi "zone" of the r strip or r zone of phi strip
  unsigned int zoneOfStrip( VeloChannelID strip ) const;

  /// returns the phi "zone" of the r strip or r zone of phi strip
  unsigned int zoneOfStrip( unsigned int strip, unsigned int sensor ) const;
  
  /// Number of strips in each zone
  unsigned int stripsInZone( unsigned int sensor, unsigned int zone ) const;

  /// returns the local radius of the strip
  double rOfStrip( VeloChannelID channel ) const;

  /// returns the local radius of the strip+fractional distance to strip
  double rOfStrip(VeloChannelID channel, double fraction) const;

  /// returns the R pitch at the given channelID
  double rPitch( VeloChannelID channel) const;

  /// returns the R pitch at the given channelID +/- fraction of channel
  double rPitch( VeloChannelID channel, double fraction) const;

  /// returns the R pitch at a given radius
  double rPitchAtLocalR( VeloChannelID channel, double radius) const;

  /// returns the phi of the strip at the specified radius 
  /// in the local frame of the sensor.
  double phiOfStrip( VeloChannelID channel, double radius) const;

  /// returns the phi of the strip+fractional distance to strip
  /// at the specified radius in the local frame of sensor.
  double phiOfStrip( VeloChannelID channel, double fraction, double radius) const;
                      
  /// returns the angle of the strip wrt the x axis in the local frame for
  /// the strip+fractional distance to strip
  double angleOfStrip( VeloChannelID channel, double fraction=0.) const;

  /** The stereo angle of the phi strips in radians,
      signed so that positive indicates phi increases with radius */
  double phiStereo( VeloChannelID channel, double radius) const;

  /// returns the Phi pitch (in mm) at the given radius (sensor local)
  double phiPitch( VeloChannelID channel, double radius ) const;

  /// returns the Phi pitch (in mm) at the given radius (sensor local)
  double phiPitch( VeloChannelID channel ) const;

  /// Return the distance to the origin for a phi strip
  double distToOrigin(VeloChannelID channel) const;

  /// return the minimum sensitive radius of an R wafer, local frame
  double rMin(unsigned int sensor) const;

  /// return the maximum sensitive radius of an R wafer, local frame
  double rMax(unsigned int sensor) const;

  /** return the minimum sensitive radius of an R wafer in a zone, local frame
      4 zones (different phi) for R sensors and 2 zones 
      (different R and stereo) for the phi sensors */
  double rMin(unsigned int sensor, unsigned int zone) const; 

  /** return the maximum sensitive radius of an R wafer in a zone, local frame
      4 zones (different phi) for R sensors and 2 zones 
      (different R and stereo) for the phi sensors */
  double rMax(unsigned int sensor, unsigned int zone) const;

  /// Smallest Phi at R (local frame) of the R strips in a zone
  double phiMin(unsigned int sensor, unsigned int zone) const;
  
  /// Largest Phi (local frame) of the R strips in a zone
  double phiMax(unsigned int sensor, unsigned int zone) const;

  // minimum phi at R (overlap in x) for a given zone
  double phiMin(unsigned int sensor, unsigned int zone, double radius) const;
  
  // maximum phi at R (overlap in x) for a given zone
  double phiMax(unsigned int sensor, unsigned int zone, double radius) const;
  
  /// returns the silicon thickness
  double siliconThickness ( unsigned int sensor ) const;

  /// returns the number of strips per sensor.
  unsigned int numberStrips(unsigned int sensor) const;
    
  /// returns the capacitance of the strip.
  double stripCapacitance(VeloChannelID channel) const;

  /** Access to a strip's geometry, for Panoramix
      from strip number and R sensor number, returns Z, R and a phi range.
      in local frame */
  StatusCode stripLimitsR( unsigned int sensor, 
                           unsigned int strip,
                           double& z, 
                           double& radius, 
                           double& phiMin, 
                           double& phiMax ) const ;
  
  /** from strip number and phi sensor number, returns the two end points
      in local frame */
  StatusCode stripLimitsPhi( unsigned int sensor, 
                             unsigned int strip,
                             HepPoint3D& begin, 
                             HepPoint3D& end ) const;

  /// Return the side of the detector. (+1 for +ve x, -1 for -ve x.)
  inline int xSide(unsigned int sensor)  const
  {
    return m_vpSensor[sensorIndex(sensor)]->xSide();
  }
  
  /// Returns true if sensor is in right side (-ve x) of detector
  inline bool isRightSensor(unsigned int sensor) const
  {
    return m_vpSensor[sensorIndex(sensor)]->isRight();
  }

  /// Set accessor to member m_zVertex, used to compute phi by extrapolating
  /// between R and Phi sensors.
  inline void setZVertex(double zVertex)
  {
    m_zVertex = zVertex;
  }

  ///  Returns m_zVertex, used to compute phi by extrapolating
  /// between R and Phi sensors.
  inline double zVertex() const
  {
    return m_zVertex;
  }

  /// Construct 3d point from R/phi channels (rFrac is fractional distance 
  /// to strip (+/-0.5))
  StatusCode makeSpacePoint( VeloChannelID rChan, 
                             double rFrac,
                             VeloChannelID phiChan,
                             double phiFrac,
                             HepPoint3D& point, 
                             double&  rPitch,
                             double&  phiPitch ) const;

  ///  Return true if the two zones are matching for R sensors. 
  ///  Also returns true for neighbouring phi zones
  bool matchingZones(unsigned int zone1, unsigned int zone2) const;

  ///=========================================================================
  /// REPLICATE OLD DeVelo Code with added rotations asumning perfect geometry
  ///=========================================================================
  ///  Returns a range of strip matching the point, and the conversion factors
  void trgPhiMatchingStrips( int sensor, 
                             double radius, 
                             int rSensor, 
                             int zone,
                             double angularTol,
                             double& stripMin, 
                             double& stripMax, 
                             double& pitch, 
                             double& offset ) const;
  /// returns the phi of the strip at the specified radius 
  double trgPhiOfStrip( VeloChannelID channel,
                        double radius ) const;
  /// returns the phi of the strip+fractional distance to strip
  double trgPhiOfStrip( VeloChannelID channel, 
                        double fraction, 
                        double radius ) const;
  /// returns the angle of the strip+frac. distance to strip wrt the x axis 
  double trgPhiDirectionOfStrip( VeloChannelID channel, 
                                 double fraction=0. ) const;
  ///========================================================================
protected: 

private:

  /// Find DeVeloSensors inside DeVelo detector element tree.
  std::vector<DeVeloSensor*> getVeloSensors();

  /// Navigate DeVelo detector element tree recursively.
  /// Store DeVeloSensors in "sensors" wherever they might be found.
  void scanDetectorElement(IDetectorElement* detElem, 
                           std::vector<DeVeloSensor*>& sensors);
  
  /// pointers to all sensors sorted by increasing z
  std::vector<DeVeloSensor*> m_vpSensor;

  /// vector of pointers to the R sensors (excluding Pile Up) 
  /// sorted by increasing z
  std::vector<DeVeloRType*> m_vpRSensor;
  
  /// vector of pointers to the Phi sensors sorted by increasing z
  std::vector<DeVeloPhiType*> m_vpPhiSensor;
  
  /// vector of pointers to the Pile Up sensors sorted by increasing z
  std::vector<DeVeloRType*> m_vpPUSensor;
  
  /// Number of R sensors
  unsigned int m_nRSensors;

  /// Number of Phi sensors
  unsigned int m_nPhiSensors;

  /// Number of Pile Up sensors
  unsigned int m_nPileUpSensors;

  /// Z of stations 
  std::vector<double> m_sensorZ; 

  /// Sensors associated to each sensor
  std::vector< std::vector< unsigned int > > m_AssocSensors;
    
  /// Indices of R, Phi and Pile Up sensors in list of all sensors sorted by z
  std::vector<unsigned int> m_RIndex;
  std::vector<unsigned int> m_PhiIndex;
  std::vector<unsigned int> m_PUIndex;

  /// Custom operator for sorting sensors in terms of z position
  struct less_Z {
    bool operator()(DeVeloSensor * const &x, DeVeloSensor * const &y) {
      return ( x->z() < y->z() );
    }
  };

  /// Custom operator for sorting sensors in terms of sensor number
  struct less_sensor {
    bool operator()(DeVeloSensor * const &x, DeVeloSensor * const &y) {
      return ( x->sensorNumber() < y->sensorNumber() );
    }
  };

  ///
  double m_zVertex;
};

// ============================================================================
#endif  //    VELODET_DEVELO_H
// ============================================================================
