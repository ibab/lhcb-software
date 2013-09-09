#ifndef VPDET_DEVP_H
#define VPDET_DEVP_H 1

// Kernel/LHCbKernel
#include "Kernel/VPChannelID.h"

// Local
#include "VPDet/DeVPSensor.h"

/** @class DeVP DeVP.h "VPDet/DeVP.h" 
 *
 *  VP detector element class.
 * 
 *  @author Victor Coco Victor.Coco@cern.ch 
 *  @date 20/5/2009
 */

static const CLID CLID_DeVP = 8200; 

#ifdef __INTEL_COMPILER        // Disable ICC remark
  #pragma warning(disable:177) // variable was declared but never referenced
#endif
namespace DeVPLocation {
  static const std::string& Default = "/dd/Structure/LHCb/BeforeMagnetRegion/VP";
}

class DeVP : public DetectorElement {

public:

  /// Constructor
  DeVP(const std::string& name = "");
  /// Destructor
  virtual ~DeVP();

  /// Object identification
  static  const CLID& classID() {return CLID_DeVP;} 
  virtual const CLID& clID() const;
  
  /// Initialization method 
  virtual StatusCode initialize(); 

  /// Return sensitive volume identifier for a given point in the global frame.
  virtual int sensitiveVolumeID(const Gaudi::XYZPoint& point) const;

  /// Return the number of sensors.
  unsigned int numberSensors() const {return m_nSensors;}
  /// Return the number of sensors in the left half.
  unsigned int numberLeftSensors() const {return m_nLeftSensors;}
  /// Return the number of sensors in the right half.
  unsigned int numberRightSensors() const {return m_nRightSensors;}

  /// Return iterator corresponding to first sensor.
  std::vector<DeVPSensor*>::const_iterator sensorsBegin() const {
    return m_vpSensors.begin();
  }
  /// Return iterator corresponding to last sensor.
  std::vector<DeVPSensor*>::const_iterator sensorsEnd() const {
    return m_vpSensors.end();
  }
  /// Return iterator corresponding to first sensor on left side.
  std::vector<DeVPSensor*>::const_iterator leftSensorsBegin() const {
    return m_leftSensors.begin();
  }
  /// Return iterator corresponding to last sensor on left side.
  std::vector<DeVPSensor*>::const_iterator leftSensorsEnd() const {
    return m_leftSensors.end();
  }
  /// Return iterator corresponding to first sensor on right side.
  std::vector<DeVPSensor*>::const_iterator rightSensorsBegin() const {
    return m_rightSensors.begin();
  }
  /// Return iterator corresponding to last sensor on right side.
  std::vector<DeVPSensor*>::const_iterator rightSensorsEnd() const {
    return m_rightSensors.end();
  }

  /// Return pointer to sensor for a given point in the global frame.
  const DeVPSensor* sensor(const Gaudi::XYZPoint& point) const;
  /// Return pointer to sensor for a given channel ID.
  const DeVPSensor* sensorOfChannel(LHCb::VPChannelID channel) const {
    unsigned int sensorNumber = channel.module();
    if (!m_old) {
      // TODO: to be checked!
      const int ladder = int(channel.chip() / 3);
      sensorNumber = 4 * sensorNumber + ladder;
    } 
    return sensor(sensorNumber);
  }
  /// Return pointer to sensor for a given sensor number.
  const DeVPSensor* sensor(unsigned int sensorNumber) const {
    return m_sensors[sensorNumber];
  }

private:

  /// Find sensors inside detector element tree.
  void findSensors(IDetectorElement* det, 
                   std::vector<DeVPSensor*>& sensors);
  
  /// List of pointers to all sensors.
  std::vector<DeVPSensor*> m_vpSensors;
  /// List of pointers to sensors on the left side.
  std::vector<DeVPSensor*> m_leftSensors;
  /// List of pointers to sensors on the right side.
  std::vector<DeVPSensor*> m_rightSensors;

  /// Number of sensors
  unsigned int m_nSensors;
  /// Number of sensors on the left side
  unsigned int m_nLeftSensors;
  /// Number of sensors on the righ side
  unsigned int m_nRightSensors;

  mutable std::vector<DeVPSensor*> m_sensors;

  /// Custom operator for sorting sensors by z position.
  struct less_Z {
    bool operator()(DeVPSensor* const &x, DeVPSensor* const &y) {
      return (x->z() < y->z());
    }
  };
  /// Custom operator for sorting sensors by sensor number.
  struct less_sensor {
    bool operator()(DeVPSensor* const &x, DeVPSensor* const &y) {
      return (x->sensorNumber() < y->sensorNumber());
    }
  };

  // Temporary flag for distinguishing between old and new geometry.
  bool m_old;

  bool m_debug;

};

#endif
