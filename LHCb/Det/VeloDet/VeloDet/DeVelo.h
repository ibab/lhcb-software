// $Id: DeVelo.h,v 1.38 2006-03-23 11:05:38 mtobin Exp $
#ifndef       VELODET_DEVELO_H
#define       VELODET_DEVELO_H 1
// ============================================================================
/// from STL
//#include <vector>

/// Local
#include "VeloDet/DeVeloSensor.h"
#include "VeloDet/DeVeloRType.h"
#include "VeloDet/DeVeloPhiType.h"

// get LHCbID for trajectory
#include "Kernel/LHCbID.h"

/** @class DeVelo DeVelo.h "VeloDet/DeVelo.h" 
 *
 *  Velo detector element class.
 *
 *  @author David Hutchcroft David.Hutchcroft@cern.ch
 *  @author Mark Tobin Mark.Tobin@cern.ch
 *  @author Kurt Rinnert kurt.rinnert@cern.ch
 *  @date 22/4/2003
 */

static const CLID& CLID_DeVelo = 8100; 

// forward declaration of LHCb::Trajectory
namespace LHCb{
  class Trajectory;
}

namespace DeVeloLocation{
  static const std::string& Default="/dd/Structure/LHCb/BeforeMagnetRegion/Velo";
}

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

  /** Implementation of sensitive volume identifier for a given point in the 
      global reference frame.  This is the sensor number defined in the xml.
  */
  virtual const int sensitiveVolumeID(const Gaudi::XYZPoint& globalPos) const;

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

  /// return the sensor number for a point (global frame)
  const DeVeloSensor* sensor( const Gaudi::XYZPoint& point ) const;
  
  /// Return iterator corresponding to first sensor
  inline std::vector<DeVeloSensor*>::const_iterator sensorsBegin() const {
    return m_vpSensor.begin();
  }

  /// Return iterator corresponding to last sensor
  inline std::vector<DeVeloSensor*>::const_iterator sensorsEnd() const {
    return m_vpSensor.end();
  }
  
  /// Return iterator corresponding to first non-pile up sensor
  inline std::vector<DeVeloSensor*>::const_iterator rPhiSensorsBegin() const {
    return m_vpSensor.begin()+m_nPileUpSensors;
  }

  /// Return iterator corresponding to last non-pile up sensor
  inline std::vector<DeVeloSensor*>::const_iterator rPhiSensorsEnd() const {
    return m_vpSensor.end();
  }
  
  /// Return iterator corresponding to first R sensor
  inline std::vector<DeVeloRType*>::const_iterator rSensorsBegin() const {
    return m_vpRSensor.begin();
  }

  /// Return iterator corresponding to last R sensor
  inline std::vector<DeVeloRType*>::const_iterator rSensorsEnd() const {
    return m_vpRSensor.end();
  }
  
  /// Return iterator corresponding to first Phi sensor
  inline std::vector<DeVeloPhiType*>::const_iterator phiSensorsBegin() const {
    return m_vpPhiSensor.begin();
  }

  /// Return iterator corresponding to last Phi sensor
  inline std::vector<DeVeloPhiType*>::const_iterator phiSensorsEnd() const {
    return m_vpPhiSensor.end();
  }
  
  /// Return iterator corresponding to first pile up sensor
  inline std::vector<DeVeloSensor*>::const_iterator pileUpSensorsBegin() const {
    return m_vpSensor.begin();
  }

  /// Return iterator corresponding to last pile up sensor
  inline std::vector<DeVeloSensor*>::const_iterator pileUpSensorsEnd() const {
    return m_vpSensor.begin()+m_nPileUpSensors;
  }
  
  /// Return iterator corresponding to first pile up sensor
  inline std::vector<DeVeloRType*>::const_iterator pileUpRSensorsBegin() const {
    return m_vpPUSensor.begin();
  }

  /// Return iterator corresponding to last pile up sensor
  inline std::vector<DeVeloRType*>::const_iterator pileUpRSensorsEnd() const {
    return m_vpPUSensor.end();
  }
  
  /// Return reverse iterator corresponding to first sensor
  inline std::vector<DeVeloSensor*>::const_reverse_iterator sensorsReverseBegin() const {
    return m_vpSensor.rbegin();
  }

  /// Return reverse iterator corresponding to last sensor
  inline std::vector<DeVeloSensor*>::const_reverse_iterator sensorsReverseEnd() const {
    return m_vpSensor.rend();
  }
  
  /// Return reverse iterator corresponding to first non-pile up sensor
  inline std::vector<DeVeloSensor*>::const_reverse_iterator rPhiSensorsReverseBegin() const {
    return m_vpSensor.rbegin();
  }

  /// Return reverse iterator corresponding to last non-pile up sensor
  inline std::vector<DeVeloSensor*>::const_reverse_iterator rPhiSensorsReverseEnd() const {
    return m_vpSensor.rend()+m_nPileUpSensors;
  }
  
  /// Return reverse iterator corresponding to first R sensor
  inline std::vector<DeVeloRType*>::const_reverse_iterator rSensorsReverseBegin() const {
    return m_vpRSensor.rbegin();
  }

  /// Return reverse iterator corresponding to last R sensor
  inline std::vector<DeVeloRType*>::const_reverse_iterator rSensorsReverseEnd() const {
    return m_vpRSensor.rend();
  }
  
  /// Return reverse iterator corresponding to first Phi sensor
  inline std::vector<DeVeloPhiType*>::const_reverse_iterator phiSensorsReverseBegin() const {
    return m_vpPhiSensor.rbegin();
  }

  /// Return reverse iterator corresponding to last Phi sensor
  inline std::vector<DeVeloPhiType*>::const_reverse_iterator phiSensorsReverseEnd() const {
    return m_vpPhiSensor.rend();
  }
  
  /// Return reverse iterator corresponding to first pile up sensor
  inline std::vector<DeVeloSensor*>::const_reverse_iterator pileUpSensorsReverseBegin() const {
    return m_vpSensor.rend()+m_nPileUpSensors;
  }

  /// Return reverse iterator corresponding to last pile up sensor
  inline std::vector<DeVeloSensor*>::const_reverse_iterator pileUpSensorsReverseEnd() const {
    return m_vpSensor.rend();
  }
  
  /// Return reverse iterator corresponding to first pile up sensor
  inline std::vector<DeVeloRType*>::const_reverse_iterator pileUpRSensorsReverseBegin() const {
    return m_vpPUSensor.rbegin();
  }

  /// Return reverse iterator corresponding to last pile up sensor
  inline std::vector<DeVeloRType*>::const_reverse_iterator pileUpRSensorsReverseEnd() const {
    return m_vpPUSensor.rend();
  }
  
  
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
                             Gaudi::XYZPoint& begin, 
                             Gaudi::XYZPoint& end ) const;

  /// Return a trajectory (for track fit) from strip + offset
  LHCb::Trajectory* trajectory(const LHCb::LHCbID& id, const double offset) const;
  
  /// give access to sensor for given sensor number
  const DeVeloSensor* sensor(unsigned int sensorNumber) const;

  /// give access to sensor for given sensor number
  const DeVeloRType* rSensor(unsigned int sensorNumber) const;

  /// give access to sensor for given sensor number
  const DeVeloPhiType* phiSensor(unsigned int sensorNumber) const;

  /// give access to sensor for given LHCb::VeloChannelID
  const DeVeloSensor* sensor(LHCb::VeloChannelID channel) const {
    return sensor(channel.sensor());
  }

  /// give access to sensor for given LHCb::VeloChannelID
  const DeVeloRType* rSensor(LHCb::VeloChannelID channel) const {
    return rSensor(channel.sensor());
  }

  /// give access to sensor for given LHCb::VeloChannelID
  const DeVeloPhiType* phiSensor(LHCb::VeloChannelID channel) const {
    return phiSensor(channel.sensor());
  }

  // public condition related methods 

  /** The sensor for a given TELL1 Id (cached condition).
   *  This information is based on CondDB, i.e. it can change
   *  with time.
   */
  const DeVeloSensor* sensorByTell1Id(unsigned int tell1Id) const;

  /// call back function for TELL1 to sensor mapping update
  StatusCode updateTell1ToSensorsCondition();

  /** direct access to TELL1 map condition.  
   *  This is for expert/testing purposes only.  All production 
   *  client code should use the interface to the cached conditions.  
   */
  const Condition* tell1ToSensorsCondition() const { return m_tell1ToSensorsCondition; }

   // private condition related methods 

private:
  /** registers condition call backs
   *  This has only to be done once.  Method is called once
   *  from initialize().
   */
  StatusCode registerConditionCallBacks();


  ///========================================================================
protected: 

private:

  /// Find DeVeloSensors inside DeVelo detector element tree.
  std::vector<DeVeloSensor*> findVeloSensors();

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
  
  /// return the index of the sensor (assumes sensors are stored 
  unsigned int sensorIndex(unsigned int sensor) const;

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

  std::map<unsigned int,bool> m_validSensors;//< Map of all valid sensors

  // condition caching

  std::string m_tell1ToSensorsConditionName;

  const Condition* m_tell1ToSensorsCondition;

  std::map<unsigned int, const DeVeloSensor*> m_sensorByTell1Id;
};

// ============================================================================
#endif  //    VELODET_DEVELO_H
// ============================================================================
