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
  static const std::string& Default = "/dd/Structure/LHCb/BeforeMagnetRegion/VeloLite";
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
  unsigned int numberSensors() const            {return m_vpSensors.size();}
  /// Number of R type sensors
  unsigned int numberRSensors() const           {return m_nRSensors;}
  /// Number of Phi type sensors
  unsigned int numberPhiSensors() const         {return m_nPhiSensors;}
  /// Number of pile-up sensors
  unsigned int numberPileUpSensors() const      {return m_nPileUpSensors;}

  /// Sensor number for a point in the global frame
  const DeVLSensor* sensor(const Gaudi::XYZPoint& point) const;
  
  /// Iterator pointing to the first sensor
  std::vector<DeVLSensor*>::const_iterator sensorsBegin() const {
    return m_vpSensors.begin();
  }
  /// Iterator pointing to the last sensor
  std::vector<DeVLSensor*>::const_iterator sensorsEnd() const {
    return m_vpSensors.end();
  }
  /// Iterator pointing to the first sensor on the left side
  std::vector<DeVLSensor*>::const_iterator leftSensorsBegin() const {
    return m_vpLeftSensors.begin();
  }
  /// Iterator pointing to the last sensor on the left side
  std::vector<DeVLSensor*>::const_iterator leftSensorsEnd() const {
    return m_vpLeftSensors.end();
  }
  /// Iterator pointing to the first sensor on the right side
  std::vector<DeVLSensor*>::const_iterator rightSensorsBegin() const {
    return m_vpRightSensors.begin();
  }
  /// Iterator pointing to the last sensor on the right side
  std::vector<DeVLSensor*>::const_iterator rightSensorsEnd() const {
    return m_vpRightSensors.end();
  }
  /// Iterator pointing to the first non pile-up sensor
  std::vector<DeVLSensor*>::const_iterator rPhiSensorsBegin() const {
    return m_vpSensors.begin() + m_nPileUpSensors;
  }
  /// Iterator pointing to the last non pile-up sensor
  std::vector<DeVLSensor*>::const_iterator rPhiSensorsEnd() const {
    return m_vpSensors.end();
  }
  /// Iterator pointing to the first non-pile up sensor on the left side
  std::vector<DeVLSensor*>::const_iterator leftRPhiSensorsBegin() const {
    return m_vpLeftSensors.begin() + m_nLeftPUSensors;
  }
  /// Iterator pointing to the last non-pile up sensor on the left side
  std::vector<DeVLSensor*>::const_iterator leftRPhiSensorsEnd() const {
    return m_vpLeftSensors.end();
  }
  /// Iterator pointing to the first non-pile up sensor on the right side
  std::vector<DeVLSensor*>::const_iterator rightRPhiSensorsBegin() const {
    return m_vpRightSensors.begin() + m_nRightPUSensors;
  }
  /// Iterator pointing to the last non-pile up sensor on the right side
  std::vector<DeVLSensor*>::const_iterator rightRPhiSensorsEnd() const {
    return m_vpRightSensors.end();
  }
  /// Iterator pointing to the first R sensor
  std::vector<DeVLRSensor*>::const_iterator rSensorsBegin() const {
    return m_vpRSensors.begin();
  }
  /// Iterator pointing to the last R sensor
  std::vector<DeVLRSensor*>::const_iterator rSensorsEnd() const {
    return m_vpRSensors.end();
  }
  /// Iterator pointing to the first R sensor on the left side
  std::vector<DeVLRSensor*>::const_iterator leftRSensorsBegin() const {
    return m_vpLeftRSensors.begin();
  }
  /// Iterator pointing to the last R sensor on the left side
  std::vector<DeVLRSensor*>::const_iterator leftRSensorsEnd() const {
    return m_vpLeftRSensors.end();
  }
  /// Iterator pointing to the first R sensor on the right side
  std::vector<DeVLRSensor*>::const_iterator rightRSensorsBegin() const {
    return m_vpRightRSensors.begin();
  }
  /// Iterator pointing to the last R sensor on the right side
  std::vector<DeVLRSensor*>::const_iterator rightRSensorsEnd() const {
    return m_vpRightRSensors.end();
  }
  /// Iterator pointing to the first Phi sensor
  std::vector<DeVLPhiSensor*>::const_iterator phiSensorsBegin() const {
    return m_vpPhiSensors.begin();
  }
  /// Iterator pointing to the last Phi sensor
  std::vector<DeVLPhiSensor*>::const_iterator phiSensorsEnd() const {
    return m_vpPhiSensors.end();
  }
  /// Iterator pointing to the first Phi sensor on the left side
  std::vector<DeVLPhiSensor*>::const_iterator leftPhiSensorsBegin() const {
    return m_vpLeftPhiSensors.begin();
  }
  /// Iterator pointing to the last Phi sensor on the left side
  std::vector<DeVLPhiSensor*>::const_iterator leftPhiSensorsEnd() const {
    return m_vpLeftPhiSensors.end();
  }
  /// Iterator pointing to the first Phi sensor on the right side
  std::vector<DeVLPhiSensor*>::const_iterator rightPhiSensorsBegin() const {
    return m_vpRightPhiSensors.begin();
  }
  /// Iterator pointing to the last Phi sensor on the right side
  std::vector<DeVLPhiSensor*>::const_iterator rightPhiSensorsEnd() const {
    return m_vpRightPhiSensors.end();
  }
  /// Iterator pointing to the first pile-up sensor
  std::vector<DeVLSensor*>::const_iterator pileUpSensorsBegin() const {
    return m_vpSensors.begin();
  }
  /// Iterator pointing to the last pile-up sensor
  std::vector<DeVLSensor*>::const_iterator pileUpSensorsEnd() const {
    return m_vpSensors.begin() + m_nPileUpSensors - 1;
  }
  /// Iterator pointing to the first pile-up sensor on the left side
  std::vector<DeVLSensor*>::const_iterator leftPileUpSensorsBegin() const {
    return m_vpLeftSensors.begin();
  }
  /// Iterator pointing to the last pile-up sensor on the left side
  std::vector<DeVLSensor*>::const_iterator leftPileUpSensorsEnd() const {
    return m_vpLeftSensors.begin() + m_nLeftPUSensors - 1;
  }
  /// Iterator pointing to the first pile-up sensor on the right side
  std::vector<DeVLSensor*>::const_iterator rightPileUpSensorsBegin() const {
    return m_vpRightSensors.begin();
  }
  /// Iterator pointing to the last pile-up sensor on the right side
  std::vector<DeVLSensor*>::const_iterator rightPileUpSensorsEnd() const {
    return m_vpRightSensors.begin() + m_nRightPUSensors - 1;
  }
  /// Iterator pointing to the first pile-up sensor
  std::vector<DeVLRSensor*>::const_iterator pileUpRSensorsBegin() const {
    return m_vpPUSensors.begin();
  }
  /// Iterator pointing to the last pile-up sensor
  std::vector<DeVLRSensor*>::const_iterator pileUpRSensorsEnd() const {
    return m_vpPUSensors.end();
  }
  /// Iterator pointing to the first pile-up sensor on the left side
  std::vector<DeVLRSensor*>::const_iterator leftPileUpRSensorsBegin() const {
    return m_vpLeftPUSensors.begin();
  }
  /// Iterator pointing to the last pile-up sensor on the left side
  std::vector<DeVLRSensor*>::const_iterator leftPileUpRSensorsEnd() const {
    return m_vpLeftPUSensors.end();
  }
  /// Iterator pointing to the first pile-up sensor on the right side
  std::vector<DeVLRSensor*>::const_iterator rightPileUpRSensorsBegin() const {
    return m_vpRightPUSensors.begin();
  }
  /// Iterator pointing to the last pile-up sensor on the right side
  std::vector<DeVLRSensor*>::const_iterator rightPileUpRSensorsEnd() const {
    return m_vpRightPUSensors.end();
  }

  /// Return a trajectory (for track fit) from strip + offset
  std::auto_ptr<LHCb::Trajectory> trajectory(const LHCb::LHCbID& id, 
                                             const double offset) const {
    if (!id.isVL()) {
      throw GaudiException("The LHCbID is not a VLChannelID", "DeVL", 
                           StatusCode::FAILURE);
    }
    return sensor(id.vlID())->trajectory(id.vlID(), offset);
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
  /// Vector of pointers to the R sensors (excluding pile-up)
  std::vector<DeVLRSensor*> m_vpRSensors;
  /// Vector of pointers to the Phi sensors
  std::vector<DeVLPhiSensor*> m_vpPhiSensors;
  /// Vector of pointers to the pile-up sensors
  std::vector<DeVLRSensor*> m_vpPUSensors;
  
  /// Vector of pointers to all sensors on the left side
  std::vector<DeVLSensor*> m_vpLeftSensors;
  /// Vector of pointers to all sensors on the right side
  std::vector<DeVLSensor*> m_vpRightSensors;
  /// Vector of pointers to all left side R/Phi sensors (excluding pile-up)
  std::vector<DeVLSensor*> m_vpLeftRPhiSensors;
  /// Vector of pointers to all right side R/Phi sensors (excluding pile-up)
  std::vector<DeVLSensor*> m_vpRightRPhiSensors;
  /// Vector of pointers to all left side R Type sensors (excluding pile-up)
  std::vector<DeVLRSensor*> m_vpLeftRSensors;
  /// Vector of pointers to all right side R Type sensors (excluding pile-up)
  std::vector<DeVLRSensor*> m_vpRightRSensors;
  /// Vector of pointers to all left side Phi sensors
  std::vector<DeVLPhiSensor*> m_vpLeftPhiSensors;
  /// Vector of pointers to all right side Phi sensors
  std::vector<DeVLPhiSensor*> m_vpRightPhiSensors;
  /// Vector of pointers to all pile-up sensors on the left side
  std::vector<DeVLRSensor*> m_vpLeftPUSensors;
  /// Vector of pointers to all pile-up sensors on the right side
  std::vector<DeVLRSensor*> m_vpRightPUSensors;

  /// Number of sensors
  unsigned int m_nSensors;
  /// Number of R type sensors
  unsigned int m_nRSensors;
  /// Number of Phi type sensors
  unsigned int m_nPhiSensors;
  /// Number of pile-up sensors
  unsigned int m_nPileUpSensors;
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
  /// Number of left side pile-up sensors
  unsigned int m_nLeftPUSensors;
  /// Number of right side pile-up sensors
  unsigned int m_nRightPUSensors;

  /// Indices of R, Phi and pile-up sensors in list of all sensors sorted by z
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
