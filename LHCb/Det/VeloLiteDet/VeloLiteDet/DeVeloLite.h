#ifndef DEVELOLITE_H
#define DEVELOLITE_H 1

// Kernel/LHCbKernel
#include "Kernel/VeloLiteChannelID.h"
#include "Kernel/LHCbID.h"
#include "Kernel/Trajectory.h"
// Local 
#include "DeVeloLiteSensor.h"
#include "DeVeloLiteRType.h"
#include "DeVeloLitePhiType.h"

/** @class DeVeloLite DeVeloLite.h "VeloLiteDet/DeVeloLite.h" 
 *
 *  VeloLite detector element class (based on DeVelo)
 *
 */

// Unique class identifier
static const CLID CLID_DeVeloLite = 8300; 

namespace DeVeloLiteLocation {
  static const std::string& Default = "/dd/Structure/LHCb/BeforeMagnetRegion/VeloLite";
}

class DeVeloLite: public DetectorElement {

public:
 
  enum {LeftHalf = 0, RightHalf = 1, NHalfs = 2};
 
  /// Constructor
  DeVeloLite(const std::string& name = "");
  /// Destructor
  virtual ~DeVeloLite() {}

  /// Object identification
  static const CLID& classID() {return CLID_DeVeloLite;} 
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
  /// Number of sensors in the left half
  unsigned int numberLeftSensors() const        {return m_nLeftSensors;}
  /// Number of R sensors in the left half
  unsigned int numberLeftRSensors() const       {return m_nLeftRSensors;}
  /// Number of Phi sensors in the left half
  unsigned int numberLeftPhiSensors() const     {return m_nLeftPhiSensors;}
  /// Number of sensors in the right half
  unsigned int numberRightSensors() const       {return m_nRightSensors;}
  /// Number of R sensors in the right half
  unsigned int numberRightRSensors() const      {return m_nRightRSensors;}
  /// Number of Phi sensors in the right half
  unsigned int numberRightPhiSensors() const    {return m_nRightPhiSensors;}
  /// Number of pile-up sensors
  unsigned int numberPileUpSensors() const      {return m_nPileUpSensors;}
  /// Number of pile-up sensors in the left half
  unsigned int numberLeftPileUpSensors() const  {return m_nLeftPUSensors;}
  /// Number of pile-up sensors in the right half
  unsigned int numberRightPileUpSensors() const {return m_nRightPUSensors;}
  /// Number of sensors excluding pile-up
  unsigned int numberNonPUSensors() const  {
    return m_nRSensors + m_nPhiSensors;
  }
  /// Number of sensors in the left half excluding pile-up
  unsigned int numberLeftNonPUSensors() const {
    return m_nLeftRSensors + m_nLeftPhiSensors;
  }
  /// Number of sensors in the right half excluding pile-up
  unsigned int numberRightNonPUSensors() const {
    return m_nRightRSensors + m_nRightPhiSensors;
  }

  /// Sensor number for a point in the global frame
  const DeVeloLiteSensor* sensor(const Gaudi::XYZPoint& point) const;
  
  /// Iterator pointing to the first sensor
  std::vector<DeVeloLiteSensor*>::const_iterator sensorsBegin() const {
    return m_vpSensors.begin();
  }
  /// Iterator pointing to the last sensor
  std::vector<DeVeloLiteSensor*>::const_iterator sensorsEnd() const {
    return m_vpSensors.end();
  }
  /// Iterator pointing to the first sensor on the left side
  std::vector<DeVeloLiteSensor*>::const_iterator leftSensorsBegin() const {
    return m_vpLeftSensors.begin();
  }
  /// Iterator pointing to the last sensor on the left side
  std::vector<DeVeloLiteSensor*>::const_iterator leftSensorsEnd() const {
    return m_vpLeftSensors.end();
  }
  /// Iterator pointing to the first sensor on the right side
  std::vector<DeVeloLiteSensor*>::const_iterator rightSensorsBegin() const {
    return m_vpRightSensors.begin();
  }
  /// Iterator pointing to the last sensor on the right side
  std::vector<DeVeloLiteSensor*>::const_iterator rightSensorsEnd() const {
    return m_vpRightSensors.end();
  }
  /// Iterator pointing to the first non pile-up sensor
  std::vector<DeVeloLiteSensor*>::const_iterator rPhiSensorsBegin() const {
    return m_vpSensors.begin() + m_nPileUpSensors;
  }
  /// Iterator pointing to the last non pile-up sensor
  std::vector<DeVeloLiteSensor*>::const_iterator rPhiSensorsEnd() const {
    return m_vpSensors.end();
  }
  /// Iterator pointing to the first non-pile up sensor on the left side
  std::vector<DeVeloLiteSensor*>::const_iterator leftRPhiSensorsBegin() const {
    return m_vpLeftSensors.begin() + m_nLeftPUSensors;
  }
  /// Iterator pointing to the last non-pile up sensor on the left side
  std::vector<DeVeloLiteSensor*>::const_iterator leftRPhiSensorsEnd() const {
    return m_vpLeftSensors.end();
  }
  /// Iterator pointing to the first non-pile up sensor on the right side
  std::vector<DeVeloLiteSensor*>::const_iterator rightRPhiSensorsBegin() const {
    return m_vpRightSensors.begin() + m_nRightPUSensors;
  }
  /// Iterator pointing to the last non-pile up sensor on the right side
  std::vector<DeVeloLiteSensor*>::const_iterator rightRPhiSensorsEnd() const {
    return m_vpRightSensors.end();
  }
  /// Iterator pointing to the first R sensor
  std::vector<DeVeloLiteRType*>::const_iterator rSensorsBegin() const {
    return m_vpRSensors.begin();
  }
  /// Iterator pointing to the last R sensor
  std::vector<DeVeloLiteRType*>::const_iterator rSensorsEnd() const {
    return m_vpRSensors.end();
  }
  /// Iterator pointing to the first R sensor on the left side
  std::vector<DeVeloLiteRType*>::const_iterator leftRSensorsBegin() const {
    return m_vpLeftRSensors.begin();
  }
  /// Iterator pointing to the last R sensor on the left side
  std::vector<DeVeloLiteRType*>::const_iterator leftRSensorsEnd() const {
    return m_vpLeftRSensors.end();
  }
  /// Iterator pointing to the first R sensor on the right side
  std::vector<DeVeloLiteRType*>::const_iterator rightRSensorsBegin() const {
    return m_vpRightRSensors.begin();
  }
  /// Iterator pointing to the last R sensor on the right side
  std::vector<DeVeloLiteRType*>::const_iterator rightRSensorsEnd() const {
    return m_vpRightRSensors.end();
  }
  /// Iterator pointing to the first Phi sensor
  std::vector<DeVeloLitePhiType*>::const_iterator phiSensorsBegin() const {
    return m_vpPhiSensors.begin();
  }
  /// Iterator pointing to the last Phi sensor
  std::vector<DeVeloLitePhiType*>::const_iterator phiSensorsEnd() const {
    return m_vpPhiSensors.end();
  }
  /// Iterator pointing to the first Phi sensor on the left side
  std::vector<DeVeloLitePhiType*>::const_iterator leftPhiSensorsBegin() const {
    return m_vpLeftPhiSensors.begin();
  }
  /// Iterator pointing to the last Phi sensor on the left side
  std::vector<DeVeloLitePhiType*>::const_iterator leftPhiSensorsEnd() const {
    return m_vpLeftPhiSensors.end();
  }
  /// Iterator pointing to the first Phi sensor on the right side
  std::vector<DeVeloLitePhiType*>::const_iterator rightPhiSensorsBegin() const {
    return m_vpRightPhiSensors.begin();
  }
  /// Iterator pointing to the last Phi sensor on the right side
  std::vector<DeVeloLitePhiType*>::const_iterator rightPhiSensorsEnd() const {
    return m_vpRightPhiSensors.end();
  }
  /// Iterator pointing to the first pile-up sensor
  std::vector<DeVeloLiteSensor*>::const_iterator pileUpSensorsBegin() const {
    return m_vpSensors.begin();
  }
  /// Iterator pointing to the last pile-up sensor
  std::vector<DeVeloLiteSensor*>::const_iterator pileUpSensorsEnd() const {
    return m_vpSensors.begin() + m_nPileUpSensors - 1;
  }
  /// Iterator pointing to the first pile-up sensor on the left side
  std::vector<DeVeloLiteSensor*>::const_iterator leftPileUpSensorsBegin() const {
    return m_vpLeftSensors.begin();
  }
  /// Iterator pointing to the last pile-up sensor on the left side
  std::vector<DeVeloLiteSensor*>::const_iterator leftPileUpSensorsEnd() const {
    return m_vpLeftSensors.begin() + m_nLeftPUSensors - 1;
  }
  /// Iterator pointing to the first pile-up sensor on the right side
  std::vector<DeVeloLiteSensor*>::const_iterator rightPileUpSensorsBegin() const {
    return m_vpRightSensors.begin();
  }
  /// Iterator pointing to the last pile-up sensor on the right side
  std::vector<DeVeloLiteSensor*>::const_iterator rightPileUpSensorsEnd() const {
    return m_vpRightSensors.begin() + m_nRightPUSensors - 1;
  }
  /// Iterator pointing to the first pile-up sensor
  std::vector<DeVeloLiteRType*>::const_iterator pileUpRSensorsBegin() const {
    return m_vpPUSensors.begin();
  }
  /// Iterator pointing to the last pile-up sensor
  std::vector<DeVeloLiteRType*>::const_iterator pileUpRSensorsEnd() const {
    return m_vpPUSensors.end();
  }
  /// Iterator pointing to the first pile-up sensor on the left side
  std::vector<DeVeloLiteRType*>::const_iterator leftPileUpRSensorsBegin() const {
    return m_vpLeftPUSensors.begin();
  }
  /// Iterator pointing to the last pile-up sensor on the left side
  std::vector<DeVeloLiteRType*>::const_iterator leftPileUpRSensorsEnd() const {
    return m_vpLeftPUSensors.end();
  }
  /// Iterator pointing to the first pile-up sensor on the right side
  std::vector<DeVeloLiteRType*>::const_iterator rightPileUpRSensorsBegin() const {
    return m_vpRightPUSensors.begin();
  }
  /// Iterator pointing to the last pile-up sensor on the right side
  std::vector<DeVeloLiteRType*>::const_iterator rightPileUpRSensorsEnd() const {
    return m_vpRightPUSensors.end();
  }

  /// Rev. iterator pointing to the first sensor
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator sensorsReverseBegin() const {
    return m_vpSensors.rbegin();
  }
  /// Rev. iterator pointing to the last sensor
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator sensorsReverseEnd() const {
    return m_vpSensors.rend();
  }
  /// Rev. iterator pointing to the first sensor on the left side
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator leftSensorsReverseBegin() const {
    return m_vpLeftSensors.rbegin();
  }
  /// Rev. iterator pointing to the last sensor on the left side
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator leftSensorsReverseEnd() const {
    return m_vpLeftSensors.rend();
  }
  /// Rev. iterator pointing to the first sensor on the right side
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator rightSensorsReverseBegin() const {
    return m_vpRightSensors.rbegin();
  }
  /// Rev. iterator pointing to the last sensor on the right side
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator rightSensorsReverseEnd() const {
    return m_vpRightSensors.rend();
  }
  /// Rev. iterator pointing to the first non-pile up sensor
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator rPhiSensorsReverseBegin() const {
    return m_vpSensors.rbegin();
  }
  /// Rev. iterator pointing to the last non-pile up sensor
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator rPhiSensorsReverseEnd() const {
    return m_vpSensors.rend()-m_nPileUpSensors;
  }
  /// Rev. iterator pointing to the first non-pile up sensor on the left side
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator leftRPhiSensorsReverseBegin() const {
    return m_vpLeftSensors.rbegin();
  }
  /// Rev. iterator pointing to the last non-pile up sensor on the left side
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator leftRPhiSensorsReverseEnd() const {
    return m_vpLeftSensors.rend() - m_nLeftPUSensors;
  }
  /// Rev. iterator pointing to the first non-pile up sensor on the right side
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator rightRPhiSensorsReverseBegin() const {
    return m_vpRightSensors.rbegin();
  }
  /// Rev. iterator pointing to the last non-pile up sensor on the right side
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator rightRPhiSensorsReverseEnd() const {
    return m_vpRightSensors.rend() - m_nRightPUSensors;
  }
  /// Rev. iterator pointing to the first R sensor
  std::vector<DeVeloLiteRType*>::const_reverse_iterator rSensorsReverseBegin() const {
    return m_vpRSensors.rbegin();
  }
  /// Rev. iterator pointing to the last R sensor
  std::vector<DeVeloLiteRType*>::const_reverse_iterator rSensorsReverseEnd() const {
    return m_vpRSensors.rend();
  }
  /// Rev. iterator pointing to the first R sensor on the left side
  std::vector<DeVeloLiteRType*>::const_reverse_iterator leftRSensorsReverseBegin() const {
    return m_vpLeftRSensors.rbegin();
  }
  /// Rev. iterator pointing to the last R sensor on the left side
  std::vector<DeVeloLiteRType*>::const_reverse_iterator leftRSensorsReverseEnd() const {
    return m_vpLeftRSensors.rend();
  }
  /// Rev. iterator pointing to the first R sensor on the right side
  std::vector<DeVeloLiteRType*>::const_reverse_iterator rightRSensorsReverseBegin() const {
    return m_vpRightRSensors.rbegin();
  }
  /// Rev. iterator pointing to the last R sensor on the right side
  std::vector<DeVeloLiteRType*>::const_reverse_iterator rightRSensorsReverseEnd() const {
    return m_vpRightRSensors.rend();
  }
  /// Rev. iterator pointing to the first Phi sensor
  std::vector<DeVeloLitePhiType*>::const_reverse_iterator phiSensorsReverseBegin() const {
    return m_vpPhiSensors.rbegin();
  }
  /// Rev. iterator pointing to the last Phi sensor
  std::vector<DeVeloLitePhiType*>::const_reverse_iterator phiSensorsReverseEnd() const {
    return m_vpPhiSensors.rend();
  }
  /// Rev. iterator pointing to the first Phi sensor on the left side
  std::vector<DeVeloLitePhiType*>::const_reverse_iterator leftPhiSensorsReverseBegin() const {
    return m_vpLeftPhiSensors.rbegin();
  }
  /// Rev. iterator pointing to the last Phi sensor on the left side
  std::vector<DeVeloLitePhiType*>::const_reverse_iterator leftPhiSensorsReverseEnd() const {
    return m_vpLeftPhiSensors.rend();
  }
  /// Rev. iterator pointing to the first Phi sensor on the right side
  std::vector<DeVeloLitePhiType*>::const_reverse_iterator rightPhiSensorsReverseBegin() const {
    return m_vpRightPhiSensors.rbegin();
  }
  /// Rev. iterator pointing to the last Phi sensor on the right side
  std::vector<DeVeloLitePhiType*>::const_reverse_iterator rightPhiSensorsReverseEnd() const {
    return m_vpRightPhiSensors.rend();
  }
  /// Rev. iterator pointing to the first pile up sensor
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator pileUpSensorsReverseBegin() const {
    return m_vpSensors.rend()-m_nPileUpSensors;
  }
  /// Rev. iterator pointing to the last pile up sensor
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator pileUpSensorsReverseEnd() const {
    return m_vpSensors.rend();
  }
  /// Rev. iterator pointing to the first pile up sensor on the left side
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator leftPileUpSensorsReverseBegin() const {
    return m_vpLeftSensors.rend()-m_nLeftPUSensors;
  }
  /// Rev. iterator pointing to the last pile up sensor on the left side
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator leftPileUpSensorsReverseEnd() const {
    return m_vpLeftSensors.rend();
  }
  /// Rev. iterator pointing to the first pile up sensor on the right side
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator rightPileUpSensorsReverseBegin() const {
    return m_vpRightSensors.rend()-m_nRightPUSensors;
  }
  /// Rev. iterator pointing to the last pile up sensor on the right side
  std::vector<DeVeloLiteSensor*>::const_reverse_iterator rightPileUpSensorsReverseEnd() const {
    return m_vpRightSensors.rend();
  }
  /// Rev. iterator pointing to the first pile up sensor
  std::vector<DeVeloLiteRType*>::const_reverse_iterator pileUpRSensorsReverseBegin() const {
    return m_vpPUSensors.rbegin();
  }
  /// Rev. iterator pointing to the last pile up sensor
  std::vector<DeVeloLiteRType*>::const_reverse_iterator pileUpRSensorsReverseEnd() const {
    return m_vpPUSensors.rend();
  }
  /// Rev. iterator pointing to the first pile up sensor on the left side
  std::vector<DeVeloLiteRType*>::const_reverse_iterator leftPileUpRSensorsReverseBegin() const {
    return m_vpLeftPUSensors.rbegin();
  }
  /// Rev. iterator pointing to the last pile up sensor on the left side
  std::vector<DeVeloLiteRType*>::const_reverse_iterator leftPileUpRSensorsReverseEnd() const {
    return m_vpLeftPUSensors.rend();
  }
  /// Rev. iterator pointing to the first pile up sensor on the right side
  std::vector<DeVeloLiteRType*>::const_reverse_iterator rightPileUpRSensorsReverseBegin() const {
    return m_vpRightPUSensors.rbegin();
  }
  /// Rev. iterator pointing to the last pile up sensor on the right side
  std::vector<DeVeloLiteRType*>::const_reverse_iterator rightPileUpRSensorsReverseEnd() const {
    return m_vpRightPUSensors.rend();
  }

  /// Return a trajectory (for track fit) from strip + offset
  std::auto_ptr<LHCb::Trajectory> trajectory(const LHCb::LHCbID& id, 
                                             const double offset) const {
    if (!id.isVeloLite()) {
      throw GaudiException("The LHCbID is not a VeloLiteChannelID", "DeVeloLite", 
                           StatusCode::FAILURE);
    }
    return sensor(id.veloLiteID())->trajectory(id.veloLiteID(), offset);
  }

  /// Return pointer to sensor for given sensor number
  const DeVeloLiteSensor* sensor(unsigned int sensorNumber) const {
    return m_sensors[sensorNumber]; 
  }
  /// Return pointer to R sensor for given sensor number
  const DeVeloLiteRType* rSensor(unsigned int sensorNumber) const { 
    return m_sensors[sensorNumber]->rType(); 
  }
  /// Return pointer to Phi sensor for given sensor number
  const DeVeloLitePhiType* phiSensor(unsigned int sensorNumber) const { 
    return m_sensors[sensorNumber]->phiType();
  }
  /// Return pointer to sensor for given VeloLiteChannelID
  const DeVeloLiteSensor* sensor(LHCb::VeloLiteChannelID channel) const {
    return sensor(channel.sensor());
  }
  /// Return pointer to R sensor for given VeloLiteChannelID
  const DeVeloLiteRType* rSensor(LHCb::VeloLiteChannelID channel) const {
    return rSensor(channel.sensor());
  }
  /// Return pointer to Phi sensor for given VeloLiteChannelID
  const DeVeloLitePhiType* phiSensor(LHCb::VeloLiteChannelID channel) const {
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

  /// Navigate detector element tree recursively and store DeVeloLiteSensors.
  void findVeloLiteSensors(IDetectorElement* detElem, 
                           std::vector<DeVeloLiteSensor*>& sensors);

  /// Vector of pointers to all sensors
  std::vector<DeVeloLiteSensor*> m_vpSensors;
  /// Vector of pointers to the R sensors (excluding pile-up)
  std::vector<DeVeloLiteRType*> m_vpRSensors;
  /// Vector of pointers to the Phi sensors
  std::vector<DeVeloLitePhiType*> m_vpPhiSensors;
  /// Vector of pointers to the pile-up sensors
  std::vector<DeVeloLiteRType*> m_vpPUSensors;
  
  /// Vector of pointers to all sensors on the left side
  std::vector<DeVeloLiteSensor*> m_vpLeftSensors;
  /// Vector of pointers to all sensors on the right side
  std::vector<DeVeloLiteSensor*> m_vpRightSensors;
  /// Vector of pointers to all left side R/Phi sensors (excluding pile-up)
  std::vector<DeVeloLiteSensor*> m_vpLeftRPhiSensors;
  /// Vector of pointers to all right side R/Phi sensors (excluding pile-up)
  std::vector<DeVeloLiteSensor*> m_vpRightRPhiSensors;
  /// Vector of pointers to all left side R Type sensors (excluding pile-up)
  std::vector<DeVeloLiteRType*> m_vpLeftRSensors;
  /// Vector of pointers to all right side R Type sensors (excluding pile-up)
  std::vector<DeVeloLiteRType*> m_vpRightRSensors;
  /// Vector of pointers to all left side Phi sensors
  std::vector<DeVeloLitePhiType*> m_vpLeftPhiSensors;
  /// Vector of pointers to all right side Phi sensors
  std::vector<DeVeloLitePhiType*> m_vpRightPhiSensors;
  /// Vector of pointers to all pile-up sensors on the left side
  std::vector<DeVeloLiteRType*> m_vpLeftPUSensors;
  /// Vector of pointers to all pile-up sensors on the right side
  std::vector<DeVeloLiteRType*> m_vpRightPUSensors;

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
  mutable std::vector<DeVeloLiteSensor*> m_sensors;

  /// Custom operator for sorting sensors by z position
  struct less_Z {
    bool operator()(DeVeloLiteSensor* const &x, DeVeloLiteSensor* const &y) {
      return (x->z() < y->z());
    }
  };
  /// Custom operator for sorting sensors by sensor number
  struct less_sensor {
    bool operator()(DeVeloLiteSensor* const &x, DeVeloLiteSensor* const &y) {
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
