// $Id: DeVelo.h,v 1.19 2004-02-17 21:36:50 mtobin Exp $
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
 *  @Date 22/4/2003
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

  /// return the number of PileUp sensors
  inline unsigned int numberPileUpSensors()  const{
    return m_indexPileUp.size();
  }

  /// return the number of R type sensors
  inline unsigned int numberRSensors()  const{
    return m_indexRType.size();
  }

  /// return the number of Phi type sensors
  inline unsigned int numberPhiSensors()  const{
    return m_indexPhiType.size();
  }

  /// return the sensor type
  inline std::string type(unsigned int sensor) 
  {
    return m_vpSensor[sensorIndex(sensor)]->type();
  }
  
  /// return true if this is an PileUp type sensor
  inline bool isPileUpSensor(unsigned int sensor) {
    return m_vpSensor[sensorIndex(sensor)]->isPileUp();
  }

  /// return true if this is an R type sensor
  inline bool isRSensor(unsigned int sensor) {    
    return m_vpSensor[sensorIndex(sensor)]->isR();
  }

  /// return true if this is an Phi type sensor
  inline bool isPhiSensor(unsigned int sensor) {
    return m_vpSensor[sensorIndex(sensor)]->isPhi();
  }

  /// return true if phi sensor is second stereo (Downstream)
  inline bool isDownstream(unsigned int sensor) {
    return m_vpSensor[sensorIndex(sensor)]->isDownstream();
  }

  /// index PileUp type sensors, indexed in increasing Z
  std::vector<unsigned int> indexOfPileUpSensors() const{
    return m_indexPileUp;
  }

  /// index of R type sensors, indexed in increasing Z
  std::vector<unsigned int> indexOfRSensors() const{
    return m_indexRType;
  }

  /// index of phi type sensors, indexed in increasing Z
  std::vector<unsigned int> indexOfPhiSensors() const{
    return m_indexPhiType;
  }

  /// return the sensor number for a point (global frame)
  unsigned int sensorNumber( const HepPoint3D& point );
  
  /// return the sensor number for a given index
  unsigned int sensorNumber( const unsigned int index ) const{
    return m_vpSensor[index]->sensorNumber();
  };
  
  /// return the index of the sensor
  unsigned int sensorIndex(unsigned int sensor);

  /// Return the number of Pile Up sensors
  int nbPuSensor()  const { return m_nPileUpType; };

  /// Return the number of Velo sensors (no pile-up)
  unsigned int nbSensor()  const { return m_nRType+m_nPhiType; };

  /// Return pointer to DeVeloRType for a given sensor
  inline DeVeloRType* pRSensor(VeloChannelID channel) {
   unsigned int index=sensorIndex(channel.sensor());
   return dynamic_cast<DeVeloRType*>(m_vpSensor[index]);
  }
  
  /// Return pointer to Phi sensor
  inline DeVeloPhiType* pPhiSensor(VeloChannelID channel) {
   unsigned int index=sensorIndex(channel.sensor());
   return dynamic_cast<DeVeloPhiType*>(m_vpSensor[index]);
  }
  
  /// Returns a vector of pointers to the R/Phi sensors
  inline std::vector<DeVeloSensor*> vpSensors() const {
    return m_vpSensor;
  }

  /** Gives the VeloChannelID and offset (in fraction of a pitch width) 
      associated to a 3D position. with pitch width in mm
      Sign convention is offset is +- 0.5 
      with +ve in increasing stip number  (global frame) */
  StatusCode pointToChannel(const HepPoint3D &point, 
                            VeloChannelID &channel,
                            double &localOffset,
                            double &pitch) ;

  /// pointToChannel if sensor known (global frame)
  StatusCode pointToChannel(const HepPoint3D &point, 
                            const unsigned int &sensor,
                            VeloChannelID &channel,
                            double &localOffset,
                            double &pitch) ;


  /** Residual of 3D point to a VeloChannelID
      returns offset in mm from closest point on channel */
  StatusCode residual(const HepPoint3D &point, 
                      const VeloChannelID &channel,
                      double &residual,
                      double &chi2) ;

  /** Residual of 3D point to a VeloChannelID + offset in fraction of a channel
      and width of the cluster in channel widths (for the chi2)
      returns offset in mm, and chi^2 from position to point */
  StatusCode residual(const HepPoint3D &point, 
                      const VeloChannelID &channel,
                      const double &localOffset,
                      const double &width,
                      double &residual,
                      double &chi2) ;

  /// Convert global 3D Point to local 3D point in frame of the sensor 
  StatusCode globalToLocal(const unsigned int &sensorNumber,
                           const HepPoint3D &global,
                           HepPoint3D &local) ;
  
  /// Convert local 3D Point in the sensor fram to a global 3D point 
  StatusCode localToGlobal(const unsigned int &sensorNumber,
                           const HepPoint3D &local,
                           HepPoint3D &global) ;

  /// Get the nth (signed) neighbour strip to a given VeloChannelID
  StatusCode neighbour(const VeloChannelID &startChannel,
                       const int &Offset,
                       VeloChannelID &channel) ;

  /// Check the distance in strips between two channelIDs
  StatusCode channelDistance(const VeloChannelID &startChannel,
                             const VeloChannelID &endChannel,
                             int &Offset) ;

  /** Returns the vector of sensor numbers one can match with the 
      specified sensor number, r and pileup sensors match to phi, phi to R 
      Returns a list (0,1,2) of sensor numbers (same +/- x side) 
      with the closest in Z listed first */
  StatusCode sensorAssociated( unsigned int sensor, 
                               std::vector<unsigned int> &assocSensor ) ;
  
  /** Returns the offical (not alignment corrected) z position of this 
      Velo sensor */
  inline double zSensor( unsigned int sensor )  { 
    return m_sensorZ[sensorIndex(sensor)]; 
  }
  
  /// returns the number of zones in this sensor
  unsigned int numberOfZones( unsigned int sensor) ;

  /// returns the phi "zone" of the r strip or r zone of phi strip
  unsigned int zoneOfStrip( VeloChannelID strip ) ;

  /// returns the phi "zone" of the r strip or r zone of phi strip
  unsigned int zoneOfStrip( unsigned int strip, unsigned int sensor ) ;
  
  /// Number of strips in each zone
  unsigned int stripsInZone( unsigned int sensor, unsigned int zone ) ;

  /// returns the local radius of the strip
  StatusCode rOfStrip( VeloChannelID channel, 
                              double &radius ) ;

  /// returns the local radius of the strip+fractional distance to strip
  StatusCode rOfStrip(VeloChannelID channel, double fraction, 
                      double & radius) ;

  /// returns the R pitch at the given channelID
  StatusCode rPitch( VeloChannelID channel, double &rPitch ) ;

  /// returns the R pitch at the given channelID +/- fraction of channel
  StatusCode rPitch( VeloChannelID channel, double fraction, double &rPitch ) ;

  /// returns the R pitch at a given radiusb
  StatusCode rPitchAtR( VeloChannelID channel, double radius, double &rPitch);

  /// returns the phi of the strip at the specified radius for this sensor.
  StatusCode phiOfStrip( VeloChannelID channel,
                                double radius, double &phiOfStrip ) ;

  /// returns the phi of the strip+fractional distance to strip
  /// at the specified radius for this sensor.
  StatusCode phiOfStrip( VeloChannelID channel, double fraction,
                                double radius, double &phiOfStrip ) ;

  /// returns the angle of the strip wrt the x axis for the strip
  StatusCode angleOfStrip( VeloChannelID channel, double &angleOfStrip ) ;
                               
  /// returns the angle of the strip wrt the x axis for
  /// the strip+fractional distance to strip
  StatusCode angleOfStrip( VeloChannelID channel, double fraction,
                           double &angleOfStrip ) ;

  /** The stereo angle of the phi strips in radians,
      signed so that positive indicates phi increases with radius */
  StatusCode phiStereo( VeloChannelID channel, double radius,
                               double &phiStereo ) ;

  /// returns the Phi pitch (in mm) at the given radius (sensor local)
  StatusCode phiPitch( VeloChannelID channel, double radius, 
                       double &phiPitch) ;

  /// returns the Phi pitch (in mm) at the given radius (sensor local)
  StatusCode phiPitch( VeloChannelID channel, 
                       double &phiPitch) ;

  /// Return the distance to the origin for a phi strip
  StatusCode distToOrigin(VeloChannelID channel, double &distance) ;

  /// return the minimum sensitive radius of an R wafer, local frame
  double rMin(unsigned int sensor) ;
  /// return the maximum sensitive radius of an R wafer, local frame
  double rMax(unsigned int sensor) ;

  /** return the minimum sensitive radius of an R wafer in a zone, local frame
      4 zones (different phi) for R sensors and 2 zones 
      (different R and stereo) for the phi sensors */
  double rMin(unsigned int sensor, unsigned int zone) ; 
  /** return the maximum sensitive radius of an R wafer in a zone, local frame
      4 zones (different phi) for R sensors and 2 zones 
      (different R and stereo) for the phi sensors */
  double rMax(unsigned int sensor, unsigned int zone) ;

  /// Smallest Phi at R (local frame) of the R or phi strips
  StatusCode phiMin(unsigned int sensor, unsigned int zone, 
                    double &phiMin) ;
  
  /// Largest Phi (local frame) of the R or phi strips
  StatusCode phiMax(unsigned int sensor, unsigned int zone, 
                    double &phiMax) ;

  /// Smallest Phi at R (local frame) of the R or phi strips
  StatusCode phiMin(unsigned int sensor, double r, double &phiMin) ;
  
  /// Largest Phi (local frame) of the R or phi strips
  StatusCode phiMax(unsigned int sensor, double r, double &phiMax) ;

  // minimum phi at R (overlap in x) for a given zone
  StatusCode phiMin(unsigned int sensor, unsigned int zone,
                    double radius, double &phiMin) ;
  
  // maximum phi at R (overlap in x) for a given zone
  StatusCode phiMax(unsigned int sensor, unsigned int zone,
                    double radius, double &phiMax) ;
  
  /// returns the silicon thickness
  double siliconThickness ( unsigned int sensor ) ;

  /// returns the number of strips per sensor.
  unsigned int numberStrips(unsigned int sensor) ;
    
  /// returns the capacitance of the strip.
  double stripCapacitance(VeloChannelID channel) ;

  /** Access to a strip's geometry, for Panoramix
      from strip number and R sensor number, returns Z, R and a phi range.
      in local frame */
  StatusCode stripLimitsR( unsigned int sensor, unsigned int strip,
                     double& z, double& radius, 
                     double& phiMin, double& phiMax ) ;
  
  /** from strip number and phi sensor number, returns the two end points
      in local frame */
  StatusCode stripLimitsPhi( unsigned int sensor, unsigned int strip,
                       HepPoint3D& begin, HepPoint3D& end ) ;

  /// Return the side of the detector. (+1 for +ve x, -1 for -ve x.)
  inline int xSide(unsigned int sensor)  
  {
    return m_vpSensor[sensorIndex(sensor)]->xSide();
  }
  
  /// Returns true if sensor is in right side (-ve x) of detector
  inline bool isRight(unsigned int sensor) 
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
  inline double zVertex() 
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
                             double&  phiPitch ) ;

  ///  Returns a range of strip matching the point, and the conversion factors
  void phiMatchingStrips( int sensor, 
                          double radius, 
                          int rSensor, 
                          int zone,
                          double angularTol,
                          double& stripMin, 
                          double& stripMax, 
                          double& pitch, 
                          double& offset );

  
protected: 

private:

  // Local storage for geometry computation

  /// pointers to R/Phi Sensors
  std::vector<DeVeloSensor *> m_vpSensor;

  /// index to the pileup type sensors
  std::vector<unsigned int> m_indexPileUp;

  /// index to the R type sensors
  std::vector<unsigned int> m_indexRType;

  /// Number of R sensors
  unsigned int m_nRType;

  /// index to the Phi type sensors
  std::vector<unsigned int> m_indexPhiType;

  /// Number of Phi sensors
  unsigned int m_nPhiType;

  /// index to the Phi type sensors
  std::vector<unsigned int> m_indexPileUpType;

  /// Number of Pile Up sensors
  unsigned int m_nPileUpType;

  /// Z of stations 
  std::vector<double> m_sensorZ; 

  /// Sensors associated to each sensor
  std::vector< std::vector< unsigned int > > m_AssocSensors;
    
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
