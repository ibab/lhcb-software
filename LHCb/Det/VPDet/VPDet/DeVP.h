#ifndef       VPDET_DEVP_H
#define       VPDET_DEVP_H 1
// ============================================================================

// Local
#include "VPDet/DeVPSensor.h"
#include "VPDet/DeVPXType.h"
#include "VPDet/DeVPYType.h"
#include "VPDet/DeVPSquareType.h"

// From LHCb Kernel
#include "Kernel/VPChannelID.h"

// get LHCbID for trajectory
#include "Kernel/LHCbID.h"
#include "Kernel/Trajectory.h"

/** @class DeVP DeVP.h "VPDet/DeVP.h" 
 *
 *  VP detector element class.
 * 
 *  @author Victor Coco Victor.Coco@cern.ch (most of it being copied from DeVelo structure)
 *  @date 20/5/2009
 */

static const CLID CLID_DeVP = 8200; 

#ifdef __INTEL_COMPILER        // Disable ICC remark
  #pragma warning(disable:177) // variable was declared but never referenced
#endif
namespace DeVPLocation{
  static const std::string& Default="/dd/Structure/LHCb/BeforeMagnetRegion/VP";
}

class DeVP: public DetectorElement {

public:

  enum { LeftHalf=0, RightHalf=1, NHalfs=2 };
    
public:
  
  /// Constructors
  DeVP( const std::string& name    = "" );
  
  /// (virtual) Destructor
  virtual ~DeVP() ;

  /// object identification
  static  const CLID& classID ()       { return CLID_DeVP ; } 
  /// object identification
  virtual const CLID& clID    () const ;
  
  /// initialization method 
  virtual StatusCode initialize(); 

  /** Implementation of sensitive volume identifier for a given point in the 
      global reference frame.  This is the sensor number defined in the xml.
  */
  virtual int sensitiveVolumeID(const Gaudi::XYZPoint& globalPos) const;

  // Get the sensor number inline from the VPChannelID
  inline  unsigned int sensorNum(LHCb::VPChannelID channel) const {
    return ((unsigned int)channel.sensor());
  }

  /// Return the number of sensors
  inline unsigned int numberSensors() const {return m_vpSensors.size();}

  /// Return the number of X type sensors
  inline unsigned int numberXSensors() const{return m_nXSensors;}

  /// Return the number of Y type sensors
  inline unsigned int numberYSensors() const{return m_nYSensors;}

  /// Return the number of sensors in the Left half of the VP
  inline unsigned int numberLeftSensors() const {return m_nLeftSensors;}

  /// Return the number of X sensors in the Left half of the VP
  inline unsigned int numberLeftXSensors() const {return m_nLeftXSensors;}

  /// Return the number of Y sensors in the Left half of the VP
  inline unsigned int numberLeftYSensors() const {return m_nLeftYSensors;}

  /// Return the number of sensors in the Right half of the VP
  inline unsigned int numberRightSensors() const {return m_nRightSensors;}

  /// Return the number of X sensors in the Right half of the VP
  inline unsigned int numberRightXSensors() const {return m_nRightXSensors;}

  /// Return the number of Y sensors in the Right half of the VP
  inline unsigned int numberRightYSensors() const {return m_nRightYSensors;}

  /// Return the sensor number for a point (global frame)
  const DeVPSensor* sensor( const Gaudi::XYZPoint& point ) const;
  
  /// Return iterator corresponding to first sensor
  inline std::vector<DeVPSensor*>::const_iterator sensorsBegin() const {
    return m_vpSensors.begin();
  }

  /// Return iterator corresponding to last sensor
  inline std::vector<DeVPSensor*>::const_iterator sensorsEnd() const {
    return m_vpSensors.end();
  }
  
  /// Return iterator corresponding to first sensor on left side
  inline std::vector<DeVPSensor*>::const_iterator leftSensorsBegin() const {
    return m_vpLeftSensors.begin();
  }

  /// Return iterator corresponding to last sensor on left side
  inline std::vector<DeVPSensor*>::const_iterator leftSensorsEnd() const {
    return m_vpLeftSensors.end();
  }
  
  /// Return iterator corresponding to first sensor on right side
  inline std::vector<DeVPSensor*>::const_iterator rightSensorsBegin() const {
    return m_vpRightSensors.begin();
  }

  /// Return iterator corresponding to last sensor on right side
  inline std::vector<DeVPSensor*>::const_iterator rightSensorsEnd() const {
    return m_vpRightSensors.end();
  }

  /// Return iterator corresponding to first X sensor
  inline std::vector<DeVPXType*>::const_iterator xSensorsBegin() const {
    return m_vpXSensors.begin();
  }

  /// Return iterator corresponding to last X sensor
  inline std::vector<DeVPXType*>::const_iterator xSensorsEnd() const {
    return m_vpXSensors.end();
  }
  
  /// Return iterator corresponding to first X sensor on left side
  inline std::vector<DeVPXType*>::const_iterator leftXSensorsBegin() const {
    return m_vpLeftXSensors.begin();
  }
  

  /// Return iterator corresponding to last X sensor on left side
  inline std::vector<DeVPXType*>::const_iterator leftXSensorsEnd() const {
    return m_vpLeftXSensors.end();
  }
  
  /// Return iterator corresponding to first X sensor on right side
  inline std::vector<DeVPXType*>::const_iterator rightXSensorsBegin() const {
    return m_vpRightXSensors.begin();
  }

  /// Return iterator corresponding to last X sensor on right side
  inline std::vector<DeVPXType*>::const_iterator rightXSensorsEnd() const {
    return m_vpRightXSensors.end();
  }
  
  /// Return iterator corresponding to first Y sensor
  inline std::vector<DeVPYType*>::const_iterator ySensorsBegin() const {
    return m_vpYSensors.begin();
  }

  /// Return iterator corresponding to last Y sensor
  inline std::vector<DeVPYType*>::const_iterator ySensorsEnd() const {
    return m_vpYSensors.end();
  }
  
  /// Return iterator corresponding to first Y sensor on left side
  inline std::vector<DeVPYType*>::const_iterator leftYSensorsBegin() const {
    return m_vpLeftYSensors.begin();
  }

  /// Return iterator corresponding to last Y sensor on left side
  inline std::vector<DeVPYType*>::const_iterator leftYSensorsEnd() const {
    return m_vpLeftYSensors.end();
  }
  
  /// Return iterator corresponding to first Y sensor on right side
  inline std::vector<DeVPYType*>::const_iterator rightYSensorsBegin() const {
    return m_vpRightYSensors.begin();
  }

  /// Return iterator corresponding to last Y sensor on right side
  inline std::vector<DeVPYType*>::const_iterator rightYSensorsEnd() const {
    return m_vpRightYSensors.end();
  }
  
  /// Return reverse iterator corresponding to first sensor
  inline std::vector<DeVPSensor*>::const_reverse_iterator sensorsReverseBegin() const {
    return m_vpSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last sensor
  inline std::vector<DeVPSensor*>::const_reverse_iterator sensorsReverseEnd() const {
    return m_vpSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first sensor on left side
  inline std::vector<DeVPSensor*>::const_reverse_iterator leftSensorsReverseBegin() const {
    return m_vpLeftSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last sensor on left side
  inline std::vector<DeVPSensor*>::const_reverse_iterator leftSensorsReverseEnd() const {
    return m_vpLeftSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first sensor on right side
  inline std::vector<DeVPSensor*>::const_reverse_iterator rightSensorsReverseBegin() const {
    return m_vpRightSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last sensor on right side
  inline std::vector<DeVPSensor*>::const_reverse_iterator rightSensorsReverseEnd() const {
    return m_vpRightSensors.rend();
  }

  /// Return reverse iterator corresponding to first X sensor
  inline std::vector<DeVPXType*>::const_reverse_iterator xSensorsReverseBegin() const {
    return m_vpXSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last X sensor
  inline std::vector<DeVPXType*>::const_reverse_iterator xSensorsReverseEnd() const {
    return m_vpXSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first X sensor on left side
  inline std::vector<DeVPXType*>::const_reverse_iterator leftXSensorsReverseBegin() const {
    return m_vpLeftXSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last X sensor on left side
  inline std::vector<DeVPXType*>::const_reverse_iterator leftXSensorsReverseEnd() const {
    return m_vpLeftXSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first X sensor on right side
  inline std::vector<DeVPXType*>::const_reverse_iterator rightXSensorsReverseBegin() const {
    return m_vpRightXSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last X sensor on right side
  inline std::vector<DeVPXType*>::const_reverse_iterator rightXSensorsReverseEnd() const {
    return m_vpRightXSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first Y sensor
  inline std::vector<DeVPYType*>::const_reverse_iterator ySensorsReverseBegin() const {
    return m_vpYSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last Y sensor
  inline std::vector<DeVPYType*>::const_reverse_iterator ySensorsReverseEnd() const {
    return m_vpYSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first Y sensor on left side
  inline std::vector<DeVPYType*>::const_reverse_iterator leftYSensorsReverseBegin() const {
    return m_vpLeftYSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last Y sensor on left side
  inline std::vector<DeVPYType*>::const_reverse_iterator leftYSensorsReverseEnd() const {
    return m_vpLeftYSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first Y sensor on right side
  inline std::vector<DeVPYType*>::const_reverse_iterator rightYSensorsReverseBegin() const {
    return m_vpRightYSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last Y sensor on right side
  inline std::vector<DeVPYType*>::const_reverse_iterator rightYSensorsReverseEnd() const {
    return m_vpRightYSensors.rend();
  }

  /// give access to sensor for given sensor number
  const DeVPSensor* sensor(unsigned int sensorNumber) const {
    return m_sensors[sensorNumber]; 
  }

  /// give access to sensor for given sensor number
  const DeVPSquareType* squareSensor(unsigned int sensorNumber) const { 
    return m_sensors[sensorNumber]->squareType(); 
  }

  /// give access to sensor for given sensor number
  const DeVPXType* xSensor(unsigned int sensorNumber) const { 
    return m_sensors[sensorNumber]->xType(); 
  }

  /// give access to sensor for given sensor number
  const DeVPYType* ySensor(unsigned int sensorNumber) const { 
    return m_sensors[sensorNumber]->yType();
  }

  /// give access to sensor for given LHCb::VPChannelID
  const DeVPSensor* sensor(LHCb::VPChannelID channel) const {
    return sensor(sensorNum(channel));
  }

  /// give access to sensor for given LHCb::VPChannelID
  const DeVPXType* xSensor(LHCb::VPChannelID channel) const {
    return xSensor(sensorNum(channel));
  }

  /// give access to sensor for given LHCb::VPChannelID
  const DeVPYType* ySensor(LHCb::VPChannelID channel) const {
    return ySensor(sensorNum(channel));
  }
  /// Update the left half box offset vector
  StatusCode updateLeftHalfBoxOffset();
  
  /// Update the right half box offset vector
  StatusCode updateRightHalfBoxOffset();
  
   // private condition related methods 

private:


  ///========================================================================
protected: 

private:

  /// Find DeVPSensors inside DeVP detector element tree.
  std::vector<DeVPSensor*> findVPSensors();

  /// Navigate DeVP detector element tree recursively.
  /// Store DeVPSensors in "sensors" wherever they might be found.
  void scanDetectorElement(IDetectorElement* detElem, 
                           std::vector<DeVPSensor*>& sensors);
  
  /// pointers to all sensors sorted by increasing z
  std::vector<DeVPSensor*> m_vpSensors;

  /** vector of pointers to the X sensors 
   * sorted by increasing z */
  std::vector<DeVPXType*> m_vpXSensors;
  
  /// vector of pointers to the Y sensors sorted by increasing z
  std::vector<DeVPYType*> m_vpYSensors;  

  /// vector of pointers to all sensors on the left side of the detector sorted by increasing z
  std::vector<DeVPSensor*> m_vpLeftSensors;

  /// vector of pointers to all sensors on the right side of the detector sorted by increasing z
  std::vector<DeVPSensor*> m_vpRightSensors;

  /** vector of pointers to all X Type sensors ( 
   * on the left side of the detector sorted by increasing z */
  std::vector<DeVPXType*> m_vpLeftXSensors;

  /** vector of pointers to all X Type sensors
   * on the right side of the detector sorted by increasing z */
  std::vector<DeVPXType*> m_vpRightXSensors;

  /// vector of pointers to all Y sensors on the left side of the detector sorted by increasing z
  std::vector<DeVPYType*> m_vpLeftYSensors;

  /// vector of pointers to all Y sensors on the right side of the detector sorted by increasing z
  std::vector<DeVPYType*> m_vpRightYSensors;

  /// Number of sensors
  unsigned int m_nSensors;

  /// Number of X sensors
  unsigned int m_nXSensors;

  /// Number of Y sensors
  unsigned int m_nYSensors;

  /// Number of Left sensors
  unsigned int m_nLeftSensors;
  
  /// Number of Right sensors
  unsigned int m_nRightSensors;

  /// Number of Left X sensors
  unsigned int m_nLeftXSensors;

  /// Number of Right X sensors
  unsigned int m_nRightXSensors;

  /// Number of Left Y sensors
  unsigned int m_nLeftYSensors;

  /// Number of Right Y sensors
  unsigned int m_nRightYSensors;

  /// Indices of X and Y sensors in list of all sensors sorted by z
  mutable std::vector<DeVPSensor*> m_sensors;

  /// Custom operator for sorting sensors in terms of z position
  struct less_Z {
    bool operator()(DeVPSensor * const &x, DeVPSensor * const &y) {
      return ( x->z() < y->z() );
    }
  };

  /// Custom operator for sorting sensors in terms of sensor number
  struct less_sensor {
    bool operator()(DeVPSensor * const &x, DeVPSensor * const &y) {
      return ( x->sensorNumber() < y->sensorNumber() );
    }
  };

  Gaudi::XYZPoint m_halfBoxOffsets[NHalfs];

  // Set output level for message service
  bool m_debug;

  double m_sensVolCut;
};

// ============================================================================
#endif  //    VPDET_DEVP_H
// ============================================================================
