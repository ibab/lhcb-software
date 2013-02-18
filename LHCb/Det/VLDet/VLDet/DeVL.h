#ifndef DEVL_H
#define DEVL_H 1

// Kernel/LHCbKernel
#include "Kernel/VLChannelID.h"
#include "Kernel/LHCbID.h"
#include "Kernel/Trajectory.h"
// Local 
#include "DeVLSensor.h"
#include "DeVLRSensor.h"
#include "DeVLPhiSensor.h"

/** @class DeVL DeVL.h VLDet/DeVL.h 
 *
 *  VeloLite detector element class (based on DeVelo)
 *
 */

// Unique class identifier
static const CLID CLID_DeVL = 8300; 

namespace DeVLLocation {
  static const std::string& Default = "/dd/Structure/LHCb/BeforeMagnetRegion/VL";
}

class DeVL: public DetectorElement {

public:
 
  enum {LeftHalf = 0, RightHalf = 1, NHalfs = 2};
 
  /// Constructor
  DeVL(const std::string& name = "");
  /// Destructor
  virtual ~DeVL() {}

  /// Object identification
  static const CLID& classID() {return CLID_DeVL;} 
  virtual const CLID& clID() const;
  
  /// Initialisation 
  virtual StatusCode initialize(); 
  /// Return the sensitive volume identifier for a given point in the 
  /// global reference frame.  This is the sensor number defined in the XML.
  virtual int sensitiveVolumeID(const Gaudi::XYZPoint& globalPos) const;

  /// Number of sensors
  unsigned int numberSensors() const       {return m_vpSensors.size();}
  /// Number of R type sensors
  unsigned int numberRSensors() const      {return m_nRSensors;}
  /// Number of Phi type sensors
  unsigned int numberPhiSensors() const    {return m_nPhiSensors;}

  /// Sensor number for a point in the global frame
  const DeVLSensor* sensor(const Gaudi::XYZPoint& point) const;
 
  /// Return container of pointers to all sensors  
  const std::vector<DeVLSensor*>& sensors() {
    return m_vpSensors;
  }
  /// Return container of pointers to all R sensors  
  const std::vector<DeVLRSensor*>& rSensors() {
    return m_vpRSensors;
  }
  /// Return container of pointers to all Phi sensors  
  const std::vector<DeVLPhiSensor*>& phiSensors() {
    return m_vpPhiSensors;
  }
  /// Return container of pointers to all sensors on left side 
  const std::vector<DeVLSensor*>& leftSensors() {
    return m_vpLeftSensors;
  }
  /// Return container of pointers to all sensors on right side 
  const std::vector<DeVLSensor*>& rightSensors() {
    return m_vpRightSensors;
  }
  /// Return container of pointers to all Phi sensors on left side 
  const std::vector<DeVLRSensor*>& leftRSensors() {
    return m_vpLeftRSensors;
  }
  /// Return container of pointers to all R sensors on right side 
  const std::vector<DeVLRSensor*>& rightRSensors() {
    return m_vpRightRSensors;
  }
  /// Return container of pointers to all Phi sensors on left side
  const std::vector<DeVLPhiSensor*>& leftPhiSensors() {
    return m_vpLeftPhiSensors;
  }
  /// Return container of pointers to all Phi sensors on right side  
  const std::vector<DeVLPhiSensor*>& rightPhiSensors() {
    return m_vpRightPhiSensors;
  }

  /// Return pointer to sensor for given sensor number
  const DeVLSensor* sensor(unsigned int sensorNumber) const {
    return m_sensors[sensorNumber]; 
  }
  /// Return pointer to R sensor for given sensor number
  const DeVLRSensor* rSensor(unsigned int sensorNumber) const { 
    return m_sensors[sensorNumber]->rSensor(); 
  }
  /// Return pointer to Phi sensor for given sensor number
  const DeVLPhiSensor* phiSensor(unsigned int sensorNumber) const { 
    return m_sensors[sensorNumber]->phiSensor();
  }
  /// Return pointer to sensor for given VLChannelID
  const DeVLSensor* sensor(LHCb::VLChannelID channel) const {
    return sensor(channel.sensor());
  }
  /// Return pointer to R sensor for given VLChannelID
  const DeVLRSensor* rSensor(LHCb::VLChannelID channel) const {
    return rSensor(channel.sensor());
  }
  /// Return pointer to Phi sensor for given VLChannelID
  const DeVLPhiSensor* phiSensor(LHCb::VLChannelID channel) const {
    return phiSensor(channel.sensor());
  }

  /// Return a trajectory (for track fit) for given strip and offset
  std::auto_ptr<LHCb::Trajectory> trajectory(const LHCb::LHCbID& id, 
                                             const double offset) const {
    if (!id.isVL()) {
      throw GaudiException("The LHCbID is not a VLChannelID", "DeVL", 
                           StatusCode::FAILURE);
    }
    return sensor(id.vlID())->trajectory(id.vlID(), offset);
  }

  /// Return half box offset vectors (depends on geometry condition)
  const Gaudi::XYZPoint& halfBoxOffset(unsigned int half) {
    return m_halfBoxOffsets[half];
  }

  // Public condition related methods 
  /// Update the left half box offset vector
  StatusCode updateLeftHalfBoxOffset();
  /// Update the right half box offset vector
  StatusCode updateRightHalfBoxOffset();
 
protected:

  virtual StatusCode registerConditionCallBacks(); 

private:

  /// Navigate detector element tree recursively and store DeVLSensors.
  void findSensors(IDetectorElement* detElem, 
                   std::vector<DeVLSensor*>& sensors);

  /// Vector of pointers to all sensors
  std::vector<DeVLSensor*> m_vpSensors;
  /// Vector of pointers to the R sensors 
  std::vector<DeVLRSensor*> m_vpRSensors;
  /// Vector of pointers to the Phi sensors
  std::vector<DeVLPhiSensor*> m_vpPhiSensors;
  
  /// Vector of pointers to all sensors on the left side
  std::vector<DeVLSensor*> m_vpLeftSensors;
  /// Vector of pointers to all sensors on the right side
  std::vector<DeVLSensor*> m_vpRightSensors;
  /// Vector of pointers to all left side R sensors
  std::vector<DeVLRSensor*> m_vpLeftRSensors;
  /// Vector of pointers to all right side R sensors
  std::vector<DeVLRSensor*> m_vpRightRSensors;
  /// Vector of pointers to all left side Phi sensors
  std::vector<DeVLPhiSensor*> m_vpLeftPhiSensors;
  /// Vector of pointers to all right side Phi sensors
  std::vector<DeVLPhiSensor*> m_vpRightPhiSensors;

  /// Number of sensors
  unsigned int m_nSensors;
  /// Number of R type sensors
  unsigned int m_nRSensors;
  /// Number of Phi type sensors
  unsigned int m_nPhiSensors;
  /// Number of left side sensors
  unsigned int m_nLeftSensors;
  /// Number of right side sensors
  unsigned int m_nRightSensors;
  /// Number of left side R type sensors
  unsigned int m_nLeftRSensors;
  /// Number of right side R type sensors
  unsigned int m_nRightRSensors;
  /// Number of left side Phi type sensors
  unsigned int m_nLeftPhiSensors;
  /// Number of right side Phi type sensors
  unsigned int m_nRightPhiSensors;

  /// Indices of sensors in list of all sensors sorted by z
  mutable std::vector<DeVLSensor*> m_sensors;

  /// Custom operator for sorting sensors by z position
  struct lessZ {
    bool operator()(DeVLSensor* const &x, DeVLSensor* const &y) {
      return (x->z() < y->z());
    }
  };
  /// Custom operator for sorting sensors by sensor number
  struct lessSensor {
    bool operator()(DeVLSensor* const &x, DeVLSensor* const &y) {
      return (x->sensorNumber() < y->sensorNumber());
    }
  };

  /// Cached half box offsets
  Gaudi::XYZPoint m_halfBoxOffsets[NHalfs];

  /// Sensitive volume cut
  double m_sensVolCut;

  /// Output level for message service
  bool m_debug;
  bool m_verbose;
  
};

#endif
