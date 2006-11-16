// $Id: VeloDetChecker.h,v 1.2 2006-11-16 21:58:25 mtobin Exp $
#ifndef VELODETCHECKER_H 
#define VELODETCHECKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

/** @class VeloDetChecker VeloDetChecker.h
 *  
 *
 *  @author Mark TOBIN
 *  @date   2006-06-12
 */
class VeloDetChecker : public GaudiTupleAlg {
public: 
  /// Standard constructor
  VeloDetChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloDetChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  /// Print out a table with numbers of sensors of each type
  void PrintNumberOfSensors();

  /// Test contents of all sensor containers
  void TestAllSensorContainers();

  /// Test contents of all sensor type containers using forward iterators
  void TestAllSensorForwardIterators();

  /// Test contents of all DeVeloSensor containers using forward iterators
  void TestSensorForwardIterators(std::vector<DeVeloSensor*>::const_iterator sensorsBegin,
                   std::vector<DeVeloSensor*>::const_iterator sensorsEnd);

  /// Test contents of all DeVeloRType containers using forward iterators
  void TestRSensorForwardIterators(std::vector<DeVeloRType*>::const_iterator rSensorsBegin,
                    std::vector<DeVeloRType*>::const_iterator rSensorsEnd);

  /// Test contents of all DeVeloPhiType containers using forward iterators
  void TestPhiSensorForwardIterators(std::vector<DeVeloPhiType*>::const_iterator phiSensorsBegin,
                                     std::vector<DeVeloPhiType*>::const_iterator phiSensorsEnd);

  /// Test contents of all sensor type containers using reverse iterators
  void TestAllSensorReverseIterators();

  /// Test contents of all DeVeloSensor containers using reverse iterators
  void TestSensorReverseIterators(std::vector<DeVeloSensor*>::const_reverse_iterator sensorsReverseBegin,
                   std::vector<DeVeloSensor*>::const_reverse_iterator sensorsReverseEnd);

  /// Test contents of all DeVeloRType containers using reverse iterators
  void TestRSensorReverseIterators(std::vector<DeVeloRType*>::const_reverse_iterator rSensorsReverseBegin,
                    std::vector<DeVeloRType*>::const_reverse_iterator rSensorsReverseEnd);

  /// Test contents of all DeVeloPhiType containers using reverse iterators
  void TestPhiSensorReverseIterators(std::vector<DeVeloPhiType*>::const_reverse_iterator phiSensorsReverseBegin,
                                     std::vector<DeVeloPhiType*>::const_reverse_iterator phiSensorsReverseEnd);

  /** Look at trajectory of first and last strips in each zone of the sensors for all 
      sensor */
  void TestTrajectories();

  /** Print the start and end co-ordinates of the trajectory for a given sensor
      and VeloChannelID */ 
  void PrintTrajectory(std::vector<DeVeloSensor*>::const_iterator iSens, 
                       LHCb::VeloChannelID channel);

  /// Test sensors (geometry and readout)
  void CheckSensorGeometry();


  /// Test all sensors (geometry and readout)
  void CheckAllSensorGeometry();

  /// Test R sensors (geometry and readout)
  void CheckRSensorGeometry();

  /// Test phi sensors (geometry and readout)
  void CheckPhiSensorGeometry();

  /** Scan the surface of x-y plane to check point to channel and residual 
      calculations work. **/
  void ScanXYPlaneOfSensors();

  void ScanXYPlaneOfSensor(const DeVeloSensor* sensor);

protected:

private:
  DeVelo* m_velo;
  bool m_check;//< Controls whether checker runs
  bool m_r;//< Check R sensors
  bool m_phi;//< Check Phi sensors
  bool m_pileUp;//< Check Pile Up sensors
  bool m_allTypes;//< Check all sensor types
  bool m_printNSens;//< Print number of each sensor type
  bool m_containers;//< Check contents of the containers
  bool m_forwardIt;//< Check forward iterators
  bool m_reverseIt;//< Check reverse iterators
  bool m_geometry;//< Look at sensor geometry
  bool m_scan;//< Scan over sensor in plane
  int m_scale;//< Scale the default range of histograms for the XY scan
};
#endif // VELODETCHECKER_H
