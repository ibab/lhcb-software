// $Id: DeVeloPix.h,v 1.1.1.1 2009-09-17 16:07:20 cocov Exp $
#ifndef       VELOPIXDET_DEVELOPIX_H
#define       VELOPIXDET_DEVELOPIX_H 1
// ============================================================================

// Local
#include "VeloPixDet/DeVeloPixSensor.h"
#include "VeloPixDet/DeVeloPixXType.h"
#include "VeloPixDet/DeVeloPixYType.h"
#include "VeloPixDet/DeVeloPixSquareType.h"

// From LHCb Kernel
#include "Kernel/VeloPixChannelID.h"

// get LHCbID for trajectory
#include "Kernel/LHCbID.h"
#include "Kernel/Trajectory.h"

/** @class DeVeloPix DeVeloPix.h "VeloPixDet/DeVeloPix.h" 
 *
 *  VeloPix detector element class.
 * 
 *  @author Victor Coco Victor.Coco@cern.ch (most of it being copied from DeVelo structure)
 *  @date 20/5/2009
 */

static const CLID CLID_DeVeloPix = 8200; 

#ifdef __INTEL_COMPILER        // Disable ICC remark
  #pragma warning(disable:177) // variable was declared but never referenced
#endif
namespace DeVeloPixLocation{
  static const std::string& Default="/dd/Structure/LHCb/BeforeMagnetRegion/VeloPix";
}

class DeVeloPix: public DetectorElement {

public:

  enum { LeftHalf=0, RightHalf=1, NHalfs=2 };
    
public:
  
  /// Constructors
  DeVeloPix( const std::string& name    = "" );
  
  /// (virtual) Destructor
  virtual ~DeVeloPix() ;

  /// object identification
  static  const CLID& classID ()       { return CLID_DeVeloPix ; } 
  /// object identification
  virtual const CLID& clID    () const ;
  
  /// initialization method 
  virtual StatusCode initialize(); 

  /** Implementation of sensitive volume identifier for a given point in the 
      global reference frame.  This is the sensor number defined in the xml.
  */
  virtual int sensitiveVolumeID(const Gaudi::XYZPoint& globalPos) const;

  // Get the sensor number inline from the VeloPixChannelID
  inline  unsigned int sensorNum(LHCb::VeloPixChannelID channel) const {
    return ((unsigned int)channel.sensor());
  }

  /// Return the number of sensors
  inline unsigned int numberSensors() const {return m_vpSensors.size();}

  /// Return the number of X type sensors
  inline unsigned int numberXSensors() const{return m_nXSensors;}

  /// Return the number of Y type sensors
  inline unsigned int numberYSensors() const{return m_nYSensors;}

  /// Return the number of sensors in the Left half of the VeloPix
  inline unsigned int numberLeftSensors() const {return m_nLeftSensors;}

  /// Return the number of X sensors in the Left half of the VeloPix
  inline unsigned int numberLeftXSensors() const {return m_nLeftXSensors;}

  /// Return the number of Y sensors in the Left half of the VeloPix
  inline unsigned int numberLeftYSensors() const {return m_nLeftYSensors;}

  /// Return the number of sensors in the Right half of the VeloPix
  inline unsigned int numberRightSensors() const {return m_nRightSensors;}

  /// Return the number of X sensors in the Right half of the VeloPix
  inline unsigned int numberRightXSensors() const {return m_nRightXSensors;}

  /// Return the number of Y sensors in the Right half of the VeloPix
  inline unsigned int numberRightYSensors() const {return m_nRightYSensors;}

  /// Return the sensor number for a point (global frame)
  const DeVeloPixSensor* sensor( const Gaudi::XYZPoint& point ) const;
  
  /// Return iterator corresponding to first sensor
  inline std::vector<DeVeloPixSensor*>::const_iterator sensorsBegin() const {
    return m_vpSensors.begin();
  }

  /// Return iterator corresponding to last sensor
  inline std::vector<DeVeloPixSensor*>::const_iterator sensorsEnd() const {
    return m_vpSensors.end();
  }
  
  /// Return iterator corresponding to first sensor on left side
  inline std::vector<DeVeloPixSensor*>::const_iterator leftSensorsBegin() const {
    return m_vpLeftSensors.begin();
  }

  /// Return iterator corresponding to last sensor on left side
  inline std::vector<DeVeloPixSensor*>::const_iterator leftSensorsEnd() const {
    return m_vpLeftSensors.end();
  }
  
  /// Return iterator corresponding to first sensor on right side
  inline std::vector<DeVeloPixSensor*>::const_iterator rightSensorsBegin() const {
    return m_vpRightSensors.begin();
  }

  /// Return iterator corresponding to last sensor on right side
  inline std::vector<DeVeloPixSensor*>::const_iterator rightSensorsEnd() const {
    return m_vpRightSensors.end();
  }

  /// Return iterator corresponding to first X sensor
  inline std::vector<DeVeloPixXType*>::const_iterator xSensorsBegin() const {
    return m_vpXSensors.begin();
  }

  /// Return iterator corresponding to last X sensor
  inline std::vector<DeVeloPixXType*>::const_iterator xSensorsEnd() const {
    return m_vpXSensors.end();
  }
  
  /// Return iterator corresponding to first X sensor on left side
  inline std::vector<DeVeloPixXType*>::const_iterator leftXSensorsBegin() const {
    return m_vpLeftXSensors.begin();
  }
  

  /// Return iterator corresponding to last X sensor on left side
  inline std::vector<DeVeloPixXType*>::const_iterator leftXSensorsEnd() const {
    return m_vpLeftXSensors.end();
  }
  
  /// Return iterator corresponding to first X sensor on right side
  inline std::vector<DeVeloPixXType*>::const_iterator rightXSensorsBegin() const {
    return m_vpRightXSensors.begin();
  }

  /// Return iterator corresponding to last X sensor on right side
  inline std::vector<DeVeloPixXType*>::const_iterator rightXSensorsEnd() const {
    return m_vpRightXSensors.end();
  }
  
  /// Return iterator corresponding to first Y sensor
  inline std::vector<DeVeloPixYType*>::const_iterator ySensorsBegin() const {
    return m_vpYSensors.begin();
  }

  /// Return iterator corresponding to last Y sensor
  inline std::vector<DeVeloPixYType*>::const_iterator ySensorsEnd() const {
    return m_vpYSensors.end();
  }
  
  /// Return iterator corresponding to first Y sensor on left side
  inline std::vector<DeVeloPixYType*>::const_iterator leftYSensorsBegin() const {
    return m_vpLeftYSensors.begin();
  }

  /// Return iterator corresponding to last Y sensor on left side
  inline std::vector<DeVeloPixYType*>::const_iterator leftYSensorsEnd() const {
    return m_vpLeftYSensors.end();
  }
  
  /// Return iterator corresponding to first Y sensor on right side
  inline std::vector<DeVeloPixYType*>::const_iterator rightYSensorsBegin() const {
    return m_vpRightYSensors.begin();
  }

  /// Return iterator corresponding to last Y sensor on right side
  inline std::vector<DeVeloPixYType*>::const_iterator rightYSensorsEnd() const {
    return m_vpRightYSensors.end();
  }
  
  /// Return reverse iterator corresponding to first sensor
  inline std::vector<DeVeloPixSensor*>::const_reverse_iterator sensorsReverseBegin() const {
    return m_vpSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last sensor
  inline std::vector<DeVeloPixSensor*>::const_reverse_iterator sensorsReverseEnd() const {
    return m_vpSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first sensor on left side
  inline std::vector<DeVeloPixSensor*>::const_reverse_iterator leftSensorsReverseBegin() const {
    return m_vpLeftSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last sensor on left side
  inline std::vector<DeVeloPixSensor*>::const_reverse_iterator leftSensorsReverseEnd() const {
    return m_vpLeftSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first sensor on right side
  inline std::vector<DeVeloPixSensor*>::const_reverse_iterator rightSensorsReverseBegin() const {
    return m_vpRightSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last sensor on right side
  inline std::vector<DeVeloPixSensor*>::const_reverse_iterator rightSensorsReverseEnd() const {
    return m_vpRightSensors.rend();
  }

  /// Return reverse iterator corresponding to first X sensor
  inline std::vector<DeVeloPixXType*>::const_reverse_iterator xSensorsReverseBegin() const {
    return m_vpXSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last X sensor
  inline std::vector<DeVeloPixXType*>::const_reverse_iterator xSensorsReverseEnd() const {
    return m_vpXSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first X sensor on left side
  inline std::vector<DeVeloPixXType*>::const_reverse_iterator leftXSensorsReverseBegin() const {
    return m_vpLeftXSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last X sensor on left side
  inline std::vector<DeVeloPixXType*>::const_reverse_iterator leftXSensorsReverseEnd() const {
    return m_vpLeftXSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first X sensor on right side
  inline std::vector<DeVeloPixXType*>::const_reverse_iterator rightXSensorsReverseBegin() const {
    return m_vpRightXSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last X sensor on right side
  inline std::vector<DeVeloPixXType*>::const_reverse_iterator rightXSensorsReverseEnd() const {
    return m_vpRightXSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first Y sensor
  inline std::vector<DeVeloPixYType*>::const_reverse_iterator ySensorsReverseBegin() const {
    return m_vpYSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last Y sensor
  inline std::vector<DeVeloPixYType*>::const_reverse_iterator ySensorsReverseEnd() const {
    return m_vpYSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first Y sensor on left side
  inline std::vector<DeVeloPixYType*>::const_reverse_iterator leftYSensorsReverseBegin() const {
    return m_vpLeftYSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last Y sensor on left side
  inline std::vector<DeVeloPixYType*>::const_reverse_iterator leftYSensorsReverseEnd() const {
    return m_vpLeftYSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first Y sensor on right side
  inline std::vector<DeVeloPixYType*>::const_reverse_iterator rightYSensorsReverseBegin() const {
    return m_vpRightYSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last Y sensor on right side
  inline std::vector<DeVeloPixYType*>::const_reverse_iterator rightYSensorsReverseEnd() const {
    return m_vpRightYSensors.rend();
  }

  /// give access to sensor for given sensor number
  const DeVeloPixSensor* sensor(unsigned int sensorNumber) const {
    return m_sensors[sensorNumber]; 
  }

  /// give access to sensor for given sensor number
  const DeVeloPixSquareType* squareSensor(unsigned int sensorNumber) const { 
    return m_sensors[sensorNumber]->squareType(); 
  }

  /// give access to sensor for given sensor number
  const DeVeloPixXType* xSensor(unsigned int sensorNumber) const { 
    return m_sensors[sensorNumber]->xType(); 
  }

  /// give access to sensor for given sensor number
  const DeVeloPixYType* ySensor(unsigned int sensorNumber) const { 
    return m_sensors[sensorNumber]->yType();
  }

  /// give access to sensor for given LHCb::VeloPixChannelID
  const DeVeloPixSensor* sensor(LHCb::VeloPixChannelID channel) const {
    return sensor(sensorNum(channel));
  }

  /// give access to sensor for given LHCb::VeloPixChannelID
  const DeVeloPixXType* xSensor(LHCb::VeloPixChannelID channel) const {
    return xSensor(sensorNum(channel));
  }

  /// give access to sensor for given LHCb::VeloPixChannelID
  const DeVeloPixYType* ySensor(LHCb::VeloPixChannelID channel) const {
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

  /// Find DeVeloPixSensors inside DeVeloPix detector element tree.
  std::vector<DeVeloPixSensor*> findVeloPixSensors();

  /// Navigate DeVeloPix detector element tree recursively.
  /// Store DeVeloPixSensors in "sensors" wherever they might be found.
  void scanDetectorElement(IDetectorElement* detElem, 
                           std::vector<DeVeloPixSensor*>& sensors);
  
  /// pointers to all sensors sorted by increasing z
  std::vector<DeVeloPixSensor*> m_vpSensors;

  /** vector of pointers to the X sensors 
   * sorted by increasing z */
  std::vector<DeVeloPixXType*> m_vpXSensors;
  
  /// vector of pointers to the Y sensors sorted by increasing z
  std::vector<DeVeloPixYType*> m_vpYSensors;  

  /// vector of pointers to all sensors on the left side of the detector sorted by increasing z
  std::vector<DeVeloPixSensor*> m_vpLeftSensors;

  /// vector of pointers to all sensors on the right side of the detector sorted by increasing z
  std::vector<DeVeloPixSensor*> m_vpRightSensors;

  /** vector of pointers to all X Type sensors ( 
   * on the left side of the detector sorted by increasing z */
  std::vector<DeVeloPixXType*> m_vpLeftXSensors;

  /** vector of pointers to all X Type sensors
   * on the right side of the detector sorted by increasing z */
  std::vector<DeVeloPixXType*> m_vpRightXSensors;

  /// vector of pointers to all Y sensors on the left side of the detector sorted by increasing z
  std::vector<DeVeloPixYType*> m_vpLeftYSensors;

  /// vector of pointers to all Y sensors on the right side of the detector sorted by increasing z
  std::vector<DeVeloPixYType*> m_vpRightYSensors;

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
  mutable std::vector<DeVeloPixSensor*> m_sensors;

  /// Custom operator for sorting sensors in terms of z position
  struct less_Z {
    bool operator()(DeVeloPixSensor * const &x, DeVeloPixSensor * const &y) {
      return ( x->z() < y->z() );
    }
  };

  /// Custom operator for sorting sensors in terms of sensor number
  struct less_sensor {
    bool operator()(DeVeloPixSensor * const &x, DeVeloPixSensor * const &y) {
      return ( x->sensorNumber() < y->sensorNumber() );
    }
  };

  Gaudi::XYZPoint m_halfBoxOffsets[NHalfs];

  // Set output level for message service
  bool m_debug;
  bool m_verbose;

  double m_sensVolCut;
};

// ============================================================================
#endif  //    VELOPIXDET_DEVELOPIX_H
// ============================================================================
