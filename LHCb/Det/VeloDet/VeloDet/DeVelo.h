// $Id: DeVelo.h,v 1.57 2009-07-27 10:36:15 jonrob Exp $
#ifndef       VELODET_DEVELO_H
#define       VELODET_DEVELO_H 1
// ============================================================================

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Local
#include "VeloDet/DeVeloSensor.h"
#include "VeloDet/DeVeloRType.h"
#include "VeloDet/DeVeloPhiType.h"

// From LHCb Kernel
#include "Kernel/VeloChannelID.h"

// get LHCbID for trajectory
#include "Kernel/LHCbID.h"
#include "Kernel/Trajectory.h"

/** @class DeVelo DeVelo.h "VeloDet/DeVelo.h" 
 *
 *  Velo detector element class.
 *
 *  @author David Hutchcroft David.Hutchcroft@cern.ch
 *  @author Mark Tobin Mark.Tobin@cern.ch
 *  @author Kurt Rinnert kurt.rinnert@cern.ch
 *  @date 22/4/2003
 */

static const CLID CLID_DeVelo = 8100; 

#ifdef __INTEL_COMPILER        // Disable ICC remark
  #pragma warning(disable:177) // variable was declared but never referenced
#endif
namespace DeVeloLocation{
  static const std::string& Default="/dd/Structure/LHCb/BeforeMagnetRegion/Velo";
}

class DeVelo: public DetectorElement {

public:

  enum { LeftHalf=0, RightHalf=1, NHalfs=2 };
    
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
  virtual int sensitiveVolumeID(const Gaudi::XYZPoint& globalPos) const;

  /// Return the number of sensors
  inline unsigned int numberSensors() const {return m_vpSensors.size();}

  /// Return the number of Velo sensors (no pile-up)
  inline unsigned int numberNonPUSensors() const {return m_nRSensors+m_nPhiSensors;}

  /// Return the number of PileUp sensors
  inline unsigned int numberPileUpSensors() const{return m_nPileUpSensors;}

  /// Return the number of R type sensors
  inline unsigned int numberRSensors() const{return m_nRSensors;}

  /// Return the number of Phi type sensors
  inline unsigned int numberPhiSensors() const{return m_nPhiSensors;}

  /// Return the number of sensors in the Left half of the Velo
  inline unsigned int numberLeftSensors() const {return m_nLeftSensors;}

  /// Return the number of Velo sensors in the Left half of the Velo (no pile-up)
  inline unsigned int numberLeftNonPUSensors() const {return m_nLeftRSensors+m_nLeftPhiSensors;}

  /// Return the number of R sensors in the Left half of the Velo
  inline unsigned int numberLeftRSensors() const {return m_nLeftRSensors;}

  /// Return the number of Phi sensors in the Left half of the Velo
  inline unsigned int numberLeftPhiSensors() const {return m_nLeftPhiSensors;}

  /// Return the number of Pile Up sensors in the Left half of the Velo
  inline unsigned int numberLeftPileUpSensors() const {return m_nLeftPUSensors;}

  /// Return the number of sensors in the Right half of the Velo
  inline unsigned int numberRightSensors() const {return m_nRightSensors;}

  /// Return the number of Velo sensors in the Right half of the Velo (no pile-up)
  inline unsigned int numberRightNonPUSensors() const {return m_nRightRSensors+m_nRightPhiSensors;}

  /// Return the number of R sensors in the Right half of the Velo
  inline unsigned int numberRightRSensors() const {return m_nRightRSensors;}

  /// Return the number of Phi sensors in the Right half of the Velo
  inline unsigned int numberRightPhiSensors() const {return m_nRightPhiSensors;}

  /// Return the number of Pile Up sensors in the Right half of the Velo
  inline unsigned int numberRightPileUpSensors() const {return m_nRightPUSensors;}

  /// Return the sensor number for a point (global frame)
  const DeVeloSensor* sensor( const Gaudi::XYZPoint& point ) const;
  
  /// Return iterator corresponding to first sensor
  inline std::vector<DeVeloSensor*>::const_iterator sensorsBegin() const {
    return m_vpSensors.begin();
  }

  /// Return iterator corresponding to last sensor
  inline std::vector<DeVeloSensor*>::const_iterator sensorsEnd() const {
    return m_vpSensors.end();
  }
  
  /// Return iterator corresponding to first sensor on left side
  inline std::vector<DeVeloSensor*>::const_iterator leftSensorsBegin() const {
    return m_vpLeftSensors.begin();
  }

  /// Return iterator corresponding to last sensor on left side
  inline std::vector<DeVeloSensor*>::const_iterator leftSensorsEnd() const {
    return m_vpLeftSensors.end();
  }
  
  /// Return iterator corresponding to first sensor on right side
  inline std::vector<DeVeloSensor*>::const_iterator rightSensorsBegin() const {
    return m_vpRightSensors.begin();
  }

  /// Return iterator corresponding to last sensor on right side
  inline std::vector<DeVeloSensor*>::const_iterator rightSensorsEnd() const {
    return m_vpRightSensors.end();
  }
  
  /// Return iterator corresponding to first non-pile up sensor
  inline std::vector<DeVeloSensor*>::const_iterator rPhiSensorsBegin() const {
    return m_vpSensors.begin()+m_nPileUpSensors;
  }

  /// Return iterator corresponding to last non-pile up sensor
  inline std::vector<DeVeloSensor*>::const_iterator rPhiSensorsEnd() const {
    return m_vpSensors.end();
  }
  
  /// Return iterator corresponding to first non-pile up sensor on left side
  inline std::vector<DeVeloSensor*>::const_iterator leftRPhiSensorsBegin() const {
    return m_vpLeftSensors.begin()+m_nLeftPUSensors;
  }

  /// Return iterator corresponding to last non-pile up sensor
  inline std::vector<DeVeloSensor*>::const_iterator leftRPhiSensorsEnd() const {
    return m_vpLeftSensors.end();
  }
  
  /// Return iterator corresponding to first non-pile up sensor on right side
  inline std::vector<DeVeloSensor*>::const_iterator rightRPhiSensorsBegin() const {
    return m_vpRightSensors.begin()+m_nRightPUSensors;
  }

  /// Return iterator corresponding to last non-pile up sensor on right side
  inline std::vector<DeVeloSensor*>::const_iterator rightRPhiSensorsEnd() const {
    return m_vpRightSensors.end();
  }

  /// Return iterator corresponding to first R sensor
  inline std::vector<DeVeloRType*>::const_iterator rSensorsBegin() const {
    return m_vpRSensors.begin();
  }

  /// Return iterator corresponding to last R sensor
  inline std::vector<DeVeloRType*>::const_iterator rSensorsEnd() const {
    return m_vpRSensors.end();
  }
  
  /// Return iterator corresponding to first R sensor on left side
  inline std::vector<DeVeloRType*>::const_iterator leftRSensorsBegin() const {
    return m_vpLeftRSensors.begin();
  }

  /// Return iterator corresponding to last R sensor on left side
  inline std::vector<DeVeloRType*>::const_iterator leftRSensorsEnd() const {
    return m_vpLeftRSensors.end();
  }
  
  /// Return iterator corresponding to first R sensor on right side
  inline std::vector<DeVeloRType*>::const_iterator rightRSensorsBegin() const {
    return m_vpRightRSensors.begin();
  }

  /// Return iterator corresponding to last R sensor on right side
  inline std::vector<DeVeloRType*>::const_iterator rightRSensorsEnd() const {
    return m_vpRightRSensors.end();
  }
  
  /// Return iterator corresponding to first Phi sensor
  inline std::vector<DeVeloPhiType*>::const_iterator phiSensorsBegin() const {
    return m_vpPhiSensors.begin();
  }

  /// Return iterator corresponding to last Phi sensor
  inline std::vector<DeVeloPhiType*>::const_iterator phiSensorsEnd() const {
    return m_vpPhiSensors.end();
  }
  
  /// Return iterator corresponding to first Phi sensor on left side
  inline std::vector<DeVeloPhiType*>::const_iterator leftPhiSensorsBegin() const {
    return m_vpLeftPhiSensors.begin();
  }

  /// Return iterator corresponding to last Phi sensor on left side
  inline std::vector<DeVeloPhiType*>::const_iterator leftPhiSensorsEnd() const {
    return m_vpLeftPhiSensors.end();
  }
  
  /// Return iterator corresponding to first Phi sensor on right side
  inline std::vector<DeVeloPhiType*>::const_iterator rightPhiSensorsBegin() const {
    return m_vpRightPhiSensors.begin();
  }

  /// Return iterator corresponding to last Phi sensor on right side
  inline std::vector<DeVeloPhiType*>::const_iterator rightPhiSensorsEnd() const {
    return m_vpRightPhiSensors.end();
  }
  
  /// Return iterator corresponding to first pile up sensor
  inline std::vector<DeVeloSensor*>::const_iterator pileUpSensorsBegin() const {
    return m_vpSensors.begin();
  }

  /// Return iterator corresponding to last pile up sensor
  inline std::vector<DeVeloSensor*>::const_iterator pileUpSensorsEnd() const {
    return m_vpSensors.begin()+m_nPileUpSensors;
  }
  
  /// Return iterator corresponding to first pile up sensor on left side
  inline std::vector<DeVeloSensor*>::const_iterator leftPileUpSensorsBegin() const {
    return m_vpLeftSensors.begin();
  }

  /// Return iterator corresponding to last pile up sensor on left side
  inline std::vector<DeVeloSensor*>::const_iterator leftPileUpSensorsEnd() const {
    return m_vpLeftSensors.begin()+m_nLeftPUSensors;
  }
  
  /// Return iterator corresponding to first pile up sensor on right side
  inline std::vector<DeVeloSensor*>::const_iterator rightPileUpSensorsBegin() const {
    return m_vpRightSensors.begin();
  }

  /// Return iterator corresponding to last pile up sensor on right side
  inline std::vector<DeVeloSensor*>::const_iterator rightPileUpSensorsEnd() const {
    return m_vpRightSensors.begin()+m_nRightPUSensors;
  }
  
  /// Return iterator corresponding to first pile up sensor
  inline std::vector<DeVeloRType*>::const_iterator pileUpRSensorsBegin() const {
    return m_vpPUSensors.begin();
  }

  /// Return iterator corresponding to last pile up sensor
  inline std::vector<DeVeloRType*>::const_iterator pileUpRSensorsEnd() const {
    return m_vpPUSensors.end();
  }
  
  /// Return iterator corresponding to first pile up sensor on left side
  inline std::vector<DeVeloRType*>::const_iterator leftPileUpRSensorsBegin() const {
    return m_vpLeftPUSensors.begin();
  }

  /// Return iterator corresponding to last pile up sensor on left side
  inline std::vector<DeVeloRType*>::const_iterator leftPileUpRSensorsEnd() const {
    return m_vpLeftPUSensors.end();
  }
  
  /// Return iterator corresponding to first pile up sensor on right side
  inline std::vector<DeVeloRType*>::const_iterator rightPileUpRSensorsBegin() const {
    return m_vpRightPUSensors.begin();
  }

  /// Return iterator corresponding to last pile up sensor on right side
  inline std::vector<DeVeloRType*>::const_iterator rightPileUpRSensorsEnd() const {
    return m_vpRightPUSensors.end();
  }
  
  /// Return reverse iterator corresponding to first sensor
  inline std::vector<DeVeloSensor*>::const_reverse_iterator sensorsReverseBegin() const {
    return m_vpSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last sensor
  inline std::vector<DeVeloSensor*>::const_reverse_iterator sensorsReverseEnd() const {
    return m_vpSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first sensor on left side
  inline std::vector<DeVeloSensor*>::const_reverse_iterator leftSensorsReverseBegin() const {
    return m_vpLeftSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last sensor on left side
  inline std::vector<DeVeloSensor*>::const_reverse_iterator leftSensorsReverseEnd() const {
    return m_vpLeftSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first sensor on right side
  inline std::vector<DeVeloSensor*>::const_reverse_iterator rightSensorsReverseBegin() const {
    return m_vpRightSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last sensor on right side
  inline std::vector<DeVeloSensor*>::const_reverse_iterator rightSensorsReverseEnd() const {
    return m_vpRightSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first non-pile up sensor
  inline std::vector<DeVeloSensor*>::const_reverse_iterator rPhiSensorsReverseBegin() const {
    return m_vpSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last non-pile up sensor
  inline std::vector<DeVeloSensor*>::const_reverse_iterator rPhiSensorsReverseEnd() const {
    return m_vpSensors.rend()-m_nPileUpSensors;
  }
  
  /// Return reverse iterator corresponding to first non-pile up sensor on left side
  inline std::vector<DeVeloSensor*>::const_reverse_iterator leftRPhiSensorsReverseBegin() const {
    return m_vpLeftSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last non-pile up sensor on left side
  inline std::vector<DeVeloSensor*>::const_reverse_iterator leftRPhiSensorsReverseEnd() const {
    return m_vpLeftSensors.rend()-m_nLeftPUSensors;
  }
  
  /// Return reverse iterator corresponding to first non-pile up sensor on right side
  inline std::vector<DeVeloSensor*>::const_reverse_iterator rightRPhiSensorsReverseBegin() const {
    return m_vpRightSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last non-pile up sensor on right side
  inline std::vector<DeVeloSensor*>::const_reverse_iterator rightRPhiSensorsReverseEnd() const {
    return m_vpRightSensors.rend()-m_nRightPUSensors;
  }
  
  /// Return reverse iterator corresponding to first R sensor
  inline std::vector<DeVeloRType*>::const_reverse_iterator rSensorsReverseBegin() const {
    return m_vpRSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last R sensor
  inline std::vector<DeVeloRType*>::const_reverse_iterator rSensorsReverseEnd() const {
    return m_vpRSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first R sensor on left side
  inline std::vector<DeVeloRType*>::const_reverse_iterator leftRSensorsReverseBegin() const {
    return m_vpLeftRSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last R sensor on left side
  inline std::vector<DeVeloRType*>::const_reverse_iterator leftRSensorsReverseEnd() const {
    return m_vpLeftRSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first R sensor on right side
  inline std::vector<DeVeloRType*>::const_reverse_iterator rightRSensorsReverseBegin() const {
    return m_vpRightRSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last R sensor on right side
  inline std::vector<DeVeloRType*>::const_reverse_iterator rightRSensorsReverseEnd() const {
    return m_vpRightRSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first Phi sensor
  inline std::vector<DeVeloPhiType*>::const_reverse_iterator phiSensorsReverseBegin() const {
    return m_vpPhiSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last Phi sensor
  inline std::vector<DeVeloPhiType*>::const_reverse_iterator phiSensorsReverseEnd() const {
    return m_vpPhiSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first Phi sensor on left side
  inline std::vector<DeVeloPhiType*>::const_reverse_iterator leftPhiSensorsReverseBegin() const {
    return m_vpLeftPhiSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last Phi sensor on left side
  inline std::vector<DeVeloPhiType*>::const_reverse_iterator leftPhiSensorsReverseEnd() const {
    return m_vpLeftPhiSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first Phi sensor on right side
  inline std::vector<DeVeloPhiType*>::const_reverse_iterator rightPhiSensorsReverseBegin() const {
    return m_vpRightPhiSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last Phi sensor on right side
  inline std::vector<DeVeloPhiType*>::const_reverse_iterator rightPhiSensorsReverseEnd() const {
    return m_vpRightPhiSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first pile up sensor
  inline std::vector<DeVeloSensor*>::const_reverse_iterator pileUpSensorsReverseBegin() const {
    return m_vpSensors.rend()-m_nPileUpSensors;
  }

  /// Return reverse iterator corresponding to last pile up sensor
  inline std::vector<DeVeloSensor*>::const_reverse_iterator pileUpSensorsReverseEnd() const {
    return m_vpSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first pile up sensor on left side
  inline std::vector<DeVeloSensor*>::const_reverse_iterator leftPileUpSensorsReverseBegin() const {
    return m_vpLeftSensors.rend()-m_nLeftPUSensors;
  }

  /// Return reverse iterator corresponding to last pile up sensor on left side
  inline std::vector<DeVeloSensor*>::const_reverse_iterator leftPileUpSensorsReverseEnd() const {
    return m_vpLeftSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first pile up sensor on right side
  inline std::vector<DeVeloSensor*>::const_reverse_iterator rightPileUpSensorsReverseBegin() const {
    return m_vpRightSensors.rend()-m_nRightPUSensors;
  }

  /// Return reverse iterator corresponding to last pile up sensor on right side
  inline std::vector<DeVeloSensor*>::const_reverse_iterator rightPileUpSensorsReverseEnd() const {
    return m_vpRightSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first pile up sensor
  inline std::vector<DeVeloRType*>::const_reverse_iterator pileUpRSensorsReverseBegin() const {
    return m_vpPUSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last pile up sensor
  inline std::vector<DeVeloRType*>::const_reverse_iterator pileUpRSensorsReverseEnd() const {
    return m_vpPUSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first pile up sensor on left side
  inline std::vector<DeVeloRType*>::const_reverse_iterator leftPileUpRSensorsReverseBegin() const {
    return m_vpLeftPUSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last pile up sensor on left side
  inline std::vector<DeVeloRType*>::const_reverse_iterator leftPileUpRSensorsReverseEnd() const {
    return m_vpLeftPUSensors.rend();
  }
  
  /// Return reverse iterator corresponding to first pile up sensor on right side
  inline std::vector<DeVeloRType*>::const_reverse_iterator rightPileUpRSensorsReverseBegin() const {
    return m_vpRightPUSensors.rbegin();
  }

  /// Return reverse iterator corresponding to last pile up sensor on right side
  inline std::vector<DeVeloRType*>::const_reverse_iterator rightPileUpRSensorsReverseEnd() const {
    return m_vpRightPUSensors.rend();
  }
  
  /// Return a trajectory (for track fit) from strip + offset
  std::unique_ptr<LHCb::Trajectory> trajectory(const LHCb::LHCbID& id, const double offset) const {
    if ( !id.isVelo()){
      throw GaudiException( "The LHCbID is not a VeloChannelID", "DeVelo",StatusCode::FAILURE );
    }
    return sensor(id.veloID())->trajectory(id.veloID(),offset);
  }

  /// give access to sensor for given sensor number
  const DeVeloSensor* sensor(unsigned int sensorNumber) const {
    return m_sensors[sensorNumber]; 
  }

  /// give access to sensor for given sensor number
  const DeVeloRType* rSensor(unsigned int sensorNumber) const { 
    return m_sensors[sensorNumber]->rType(); 
  }

  /// give access to sensor for given sensor number
  const DeVeloPhiType* phiSensor(unsigned int sensorNumber) const { 
    return m_sensors[sensorNumber]->phiType();
  }

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

  /// Access half box offset vectors (don't cache, depends on the geometry condition)
  const Gaudi::XYZPoint& halfBoxOffset(unsigned int half) { return m_halfBoxOffsets[half]; }

  // public condition related methods 

  /** The sensor for a given TELL1 Id (cached condition).
   *  This information is based on CondDB, i.e. it can change
   *  with time.
   */
  const DeVeloSensor* sensorByTell1Id(unsigned int tell1Id) const;

  /** The TELL1 Id for a given sensor number (cached condition)
   *  This information is based on CondDB, i.e. it can change
   *  with time.
   *
   *  @param sensorNumber input, the sensor number for which you want to know the TELL1 Id
   *  @param tell1Id output, the TELL1 Id associated with the sensor number, undefined if not mapped
   *  @return true if mapping exists, false otherwise
   */
  bool tell1IdBySensorNumber(unsigned int sensorNumber, unsigned int& tell1Id) const;

  /// call back function for TELL1 to sensor mapping update
  StatusCode updateTell1ToSensorsCondition();

  /** direct access to TELL1 map condition.  
   *  This is for expert/testing purposes only.  All production 
   *  client code should use the interface to the cached conditions.  
   */
  const Condition* tell1ToSensorsCondition() const { return m_tell1ToSensorsCondition; }

  /// Update the left half box offset vector
  StatusCode updateLeftHalfBoxOffset();
  
  /// Update the right half box offset vector
  StatusCode updateRightHalfBoxOffset();
  
   // private condition related methods 

private:

  /** registers condition call backs
   *  This has only to be done once.  Method is called once
   *  from initialize().
   */
  StatusCode registerConditionCallBacks();

  /// On demand access to MsgStream object
  inline MsgStream & msg() const
  {
    if ( !m_msgStream ) m_msgStream = new MsgStream( msgSvc(), "DeVelo" );
    return *m_msgStream; 
  }

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
  std::vector<DeVeloSensor*> m_vpSensors;

  /** vector of pointers to the R sensors (excluding Pile Up) 
   * sorted by increasing z */
  std::vector<DeVeloRType*> m_vpRSensors;
  
  /// vector of pointers to the Phi sensors sorted by increasing z
  std::vector<DeVeloPhiType*> m_vpPhiSensors;
  
  /// vector of pointers to the Pile Up sensors sorted by increasing z
  std::vector<DeVeloRType*> m_vpPUSensors;
  
  /// vector of pointers to all sensors on the left side of the detector sorted by increasing z
  std::vector<DeVeloSensor*> m_vpLeftSensors;

  /// vector of pointers to all sensors on the right side of the detector sorted by increasing z
  std::vector<DeVeloSensor*> m_vpRightSensors;

  /** vector of pointers to all R/Phi sensors (excluding pile up) on the left side 
   * of the detector sorted by increasing z */
  std::vector<DeVeloSensor*> m_vpLeftRPhiSensors;

  /** vector of pointers to all R/Phi sensors (excluding pile up) on the right side 
   * of the detector sorted by increasing z */
  std::vector<DeVeloSensor*> m_vpRightRPhiSensors;

  /** vector of pointers to all R Type sensors (excluding pile up) 
   * on the left side of the detector sorted by increasing z */
  std::vector<DeVeloRType*> m_vpLeftRSensors;

  /** vector of pointers to all R Type sensors (excluding pile up)
   * on the right side of the detector sorted by increasing z */
  std::vector<DeVeloRType*> m_vpRightRSensors;

  /// vector of pointers to all phi sensors on the left side of the detector sorted by increasing z
  std::vector<DeVeloPhiType*> m_vpLeftPhiSensors;

  /// vector of pointers to all phi sensors on the right side of the detector sorted by increasing z
  std::vector<DeVeloPhiType*> m_vpRightPhiSensors;

  /// vector of pointers to all phi sensors on the left side of the detector sorted by increasing z
  std::vector<DeVeloRType*> m_vpLeftPUSensors;

  /// vector of pointers to all phi sensors on the right side of the detector sorted by increasing z
  std::vector<DeVeloRType*> m_vpRightPUSensors;

  /// Number of sensors
  unsigned int m_nSensors;

  /// Number of R sensors
  unsigned int m_nRSensors;

  /// Number of Phi sensors
  unsigned int m_nPhiSensors;

  /// Number of Pile Up sensors
  unsigned int m_nPileUpSensors;

  /// Number of Left sensors
  unsigned int m_nLeftSensors;
  
  /// Number of Right sensors
  unsigned int m_nRightSensors;

  /// Number of Left R sensors
  unsigned int m_nLeftRSensors;

  /// Number of Right R sensors
  unsigned int m_nRightRSensors;

  /// Number of Left Phi sensors
  unsigned int m_nLeftPhiSensors;

  /// Number of Right Phi sensors
  unsigned int m_nRightPhiSensors;

  /// Number of Left Pile Up sensors
  unsigned int m_nLeftPUSensors;

  /// Number of Right Pile Up sensors
  unsigned int m_nRightPUSensors;

  /// Indices of R, Phi and Pile Up sensors in list of all sensors sorted by z
  mutable std::vector<DeVeloSensor*> m_sensors;

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

  // condition caching

  std::string m_tell1ToSensorsConditionName;

  const Condition* m_tell1ToSensorsCondition;

  std::map<unsigned int, const DeVeloSensor*> m_sensorByTell1Id;
  std::map<unsigned int, unsigned int> m_tell1IdBySensorNumber;

  Gaudi::XYZPoint m_halfBoxOffsets[NHalfs];

  // Set output level for message service
  bool m_debug;
  bool m_verbose;

  /// cached Message Stream object
  mutable MsgStream * m_msgStream;

  double m_sensVolCut;
};

// ============================================================================
#endif  //    VELODET_DEVELO_H
// ============================================================================
