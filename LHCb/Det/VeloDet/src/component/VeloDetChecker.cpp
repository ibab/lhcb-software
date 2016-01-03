// Include files 

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// from Detector
#include "VeloDet/DeVelo.h" 

#include "Kernel/LHCbID.h"
#include <boost/lexical_cast.hpp>

// local
#include "VeloDetChecker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloDetChecker
//
// 2006-06-12 : Mark TOBIN
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloDetChecker )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloDetChecker::VeloDetChecker( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
  , m_velo(NULL)
{
  declareProperty("VeloDet", m_check = true);
  declareProperty("RSensors",m_r = true);
  declareProperty("PhiSensors",m_phi = true);
  declareProperty("PileUpSensors",m_pileUp = true);
  declareProperty("PrintNumberSensors", m_printNSens = true);
  declareProperty("Containers", m_containers = true);
  declareProperty("ForwardIterators", m_forwardIt = true);
  declareProperty("ReverseIterators", m_reverseIt = true);
  declareProperty("Geometry",m_geometry=true);
  declareProperty("ScanSensors",m_scan=true);
  declareProperty("ScaleXYGrid",m_scale=1);
}
//=============================================================================
// Destructor
//=============================================================================
VeloDetChecker::~VeloDetChecker() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloDetChecker::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTupleAlg
  debug() << "==> Initialize" << endmsg;
  m_velo = getDet<DeVelo>( DeVeloLocation::Default );
  if(m_r && m_phi && m_pileUp) m_allTypes=true;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloDetChecker::execute() {

  if(!m_check) return StatusCode::SUCCESS;
  debug() << "==> Execute" << endmsg;
  if(m_printNSens) PrintNumberOfSensors();
  if(m_containers) TestAllSensorContainers();
  TestTrajectories();
  CheckSensorGeometry();
  if(m_scan) ScanXYPlaneOfSensors();
  m_check=false;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloDetChecker::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}
//==============================================================================
// Test contents of all sensor containers using forward iterators
//==============================================================================
void VeloDetChecker::TestAllSensorContainers() {
  debug() << "==> TestAllSensorContainers" << endmsg;
  if(m_forwardIt) TestAllSensorForwardIterators();
  if(m_reverseIt) TestAllSensorReverseIterators();
}
//==============================================================================
// Test contents of all sensor containers using forward iterators
//==============================================================================
void VeloDetChecker::TestAllSensorForwardIterators() {
  debug() << "==> TestAllSensorForwardIterators" << endmsg;
  if(m_allTypes) {
    debug() << "==============================================================================" << endmsg;
    debug() << "Test containers in DeVelo which have all sensors" << endmsg;
    debug() << "==============================================================================" << endmsg;
    debug() << "===> Testing DeVeloSensor container" << endmsg;
    TestSensorForwardIterators(m_velo->sensorsBegin(),m_velo->sensorsEnd());
    debug() << "===> Testing Left DeVeloSensor container" << endmsg;
    TestSensorForwardIterators(m_velo->leftSensorsBegin(),m_velo->leftSensorsEnd());
    debug() << "===> Testing Right DeVeloSensor container" << endmsg;
    TestSensorForwardIterators(m_velo->rightSensorsBegin(),m_velo->rightSensorsEnd());
  }
  if(m_r && m_phi) {
    debug() << "==============================================================================" << endmsg;
    debug() << "Test containers in DeVelo which contain all R/Phi sensors excluding Pile Up" << endmsg;
    debug() << "==============================================================================" << endmsg;
    debug() << "===> Testing R/Phi DeVeloSensor containers" << endmsg;
    TestSensorForwardIterators(m_velo->rPhiSensorsBegin(),m_velo->rPhiSensorsEnd());
    debug() << "===> Testing Left R/Phi DeVeloSensor container" << endmsg;
    TestSensorForwardIterators(m_velo->leftRPhiSensorsBegin(),m_velo->leftRPhiSensorsEnd());
    debug() << "===> Testing Right R/Phi DeVeloSensor container" << endmsg;
    TestSensorForwardIterators(m_velo->rightRPhiSensorsBegin(),m_velo->rightRPhiSensorsEnd());
  }
  if(m_r) {
    debug() << "==============================================================================" << endmsg;
    debug() << "Test containers in DeVelo which have only R sensors" << endmsg;
    debug() << "==============================================================================" << endmsg;
    debug() << "===> Testing R DeVeloRType container" << endmsg;
    TestRSensorForwardIterators(m_velo->rSensorsBegin(),m_velo->rSensorsEnd());
    debug() << "===> Testing Left R DeVeloRType container" << endmsg;
    TestRSensorForwardIterators(m_velo->leftRSensorsBegin(),m_velo->leftRSensorsEnd());
    debug() << "===> Testing Right R DeVeloRType container" << endmsg;
    TestRSensorForwardIterators(m_velo->rightRSensorsBegin(),m_velo->rightRSensorsEnd());
  }
  if(m_phi) {
    debug() << "==============================================================================" << endmsg;
    debug() << "Test containers in DeVelo which have only Phi sensors" << endmsg; 
    debug() << "==============================================================================" << endmsg;
    debug() << "===> Testing Phi DeVeloPhiType container" << endmsg;
    TestPhiSensorForwardIterators(m_velo->phiSensorsBegin(),m_velo->phiSensorsEnd());
    debug() << "===> Testing Left Phi DeVeloPhiType container" << endmsg;
    TestPhiSensorForwardIterators(m_velo->leftPhiSensorsBegin(),m_velo->leftPhiSensorsEnd());
    debug() << "===> Testing Right Phi DeVeloPhiType container" << endmsg;
    TestPhiSensorForwardIterators(m_velo->rightPhiSensorsBegin(),m_velo->rightPhiSensorsEnd());
  }
  if(m_pileUp) {
    debug() << "==============================================================================" << endmsg;
    debug() << "Test containers in DeVelo which have Pile Up sensors" << endmsg; 
    debug() << "==============================================================================" << endmsg;
    debug() << "===> Testing Pile Up DeVeloSensor containers" << endmsg;
    TestSensorForwardIterators(m_velo->pileUpSensorsBegin(),m_velo->pileUpSensorsEnd());
    debug() << "===> Testing Left Pile Up DeVeloSensor container" << endmsg;
    TestSensorForwardIterators(m_velo->leftPileUpSensorsBegin(),m_velo->leftPileUpSensorsEnd());
    debug() << "===> Testing Right Pile Up DeVeloSensor container" << endmsg;
    TestSensorForwardIterators(m_velo->rightPileUpSensorsBegin(),m_velo->rightPileUpSensorsEnd());
    debug() << "===> Testing Pile Up DeVeloRType container" << endmsg;
    TestRSensorForwardIterators(m_velo->pileUpRSensorsBegin(),m_velo->pileUpRSensorsEnd());
    debug() << "===> Testing Left Pile Up DeVeloRType container" << endmsg;
    TestRSensorForwardIterators(m_velo->leftPileUpRSensorsBegin(),m_velo->leftPileUpRSensorsEnd());
    debug() << "===> Testing Right Pile Up DeVeloRType container" << endmsg;
    TestRSensorForwardIterators(m_velo->rightPileUpRSensorsBegin(),m_velo->rightPileUpRSensorsEnd());
  }
}
//==============================================================================
// Generic method to test contents of sensor containers
//==============================================================================
void VeloDetChecker::TestSensorForwardIterators(std::vector<DeVeloSensor*>::const_iterator sensorsBegin,
                                   std::vector<DeVeloSensor*>::const_iterator sensorsEnd) {
  debug() << "TestSensorForwardIterators" << endmsg;
  for(std::vector<DeVeloSensor*>::const_iterator iSens=sensorsBegin; iSens != sensorsEnd; ++iSens) {
    debug() << format("Sensor Number=%d, R=%i, Phi=%i, PU=%i, Left=%i, Right=%i, Down=%i, z=%f",
                      (*iSens)->sensorNumber(),(*iSens)->isR(),(*iSens)->isPhi(),(*iSens)->isPileUp(),
                      (*iSens)->isLeft(),(*iSens)->isRight(),(*iSens)->isDownstream(),
                      (*iSens)->z()) 
            << endmsg;
  }
}
//==============================================================================
// Generic method to test contents of R sensor containers
//==============================================================================
void VeloDetChecker::TestRSensorForwardIterators(std::vector<DeVeloRType*>::const_iterator rSensorsBegin,
                                  std::vector<DeVeloRType*>::const_iterator rSensorsEnd) {
  debug() << "TestRSensorForwardIterators" << endmsg;
  for(std::vector<DeVeloRType*>::const_iterator iSens=rSensorsBegin; iSens != rSensorsEnd; ++iSens) {
    debug() << format("Sensor Number=%d, R=%i, Phi=%i, PU=%i, Left=%i, Right=%i, Down=%i, z=%f",
                      (*iSens)->sensorNumber(),(*iSens)->isR(),(*iSens)->isPhi(),(*iSens)->isPileUp(),
                      (*iSens)->isLeft(),(*iSens)->isRight(),(*iSens)->isDownstream(),
                      (*iSens)->z()) 
            << endmsg;
  }
}
//==============================================================================
// Generic method to test contents of Phi sensor containers
//==============================================================================
void VeloDetChecker::TestPhiSensorForwardIterators(std::vector<DeVeloPhiType*>::const_iterator phiSensorsBegin,
                                    std::vector<DeVeloPhiType*>::const_iterator phiSensorsEnd) {
  debug() << "TestPhiSensorForwardIterators" << endmsg;
  for(std::vector<DeVeloPhiType*>::const_iterator iSens=phiSensorsBegin; iSens != phiSensorsEnd; ++iSens) {
    debug() << format("Sensor Number=%d, R=%i, Phi=%i, PU=%i, Left=%i, Right=%i, Down=%i, z=%f",
                      (*iSens)->sensorNumber(),(*iSens)->isR(),(*iSens)->isPhi(),(*iSens)->isPileUp(),
                      (*iSens)->isLeft(),(*iSens)->isRight(),(*iSens)->isDownstream(),
                      (*iSens)->z()) 
            << endmsg;
  }
}
//==============================================================================
// Test contents of all sensor containers using reverse iterators
//==============================================================================
void VeloDetChecker::TestAllSensorReverseIterators() {
  debug() << "==> TestAllSensorReverseIterators" << endmsg;
  if(m_allTypes) {
    debug() << "==============================================================================" << endmsg;
    debug() << "Test containers in DeVelo which have all sensors" << endmsg;
    debug() << "==============================================================================" << endmsg;
    debug() << "===> Testing DeVeloSensor container" << endmsg;
    TestSensorReverseIterators(m_velo->sensorsReverseBegin(),m_velo->sensorsReverseEnd());
    debug() << "===> Testing Left DeVeloSensor container" << endmsg;
    TestSensorReverseIterators(m_velo->leftSensorsReverseBegin(),m_velo->leftSensorsReverseEnd());
    debug() << "===> Testing Right DeVeloSensor container" << endmsg;
    TestSensorReverseIterators(m_velo->rightSensorsReverseBegin(),m_velo->rightSensorsReverseEnd());
  }
  if(m_r && m_phi) {
    debug() << "==============================================================================" << endmsg;
    debug() << "Test containers in DeVelo which contain all R/Phi sensors excluding Pile Up" << endmsg;
    debug() << "==============================================================================" << endmsg;
    debug() << "===> Testing R/Phi DeVeloSensor containers" << endmsg;
    TestSensorReverseIterators(m_velo->rPhiSensorsReverseBegin(),m_velo->rPhiSensorsReverseEnd());
    debug() << "===> Testing Left R/Phi DeVeloSensor container" << endmsg;
    TestSensorReverseIterators(m_velo->leftRPhiSensorsReverseBegin(),m_velo->leftRPhiSensorsReverseEnd());
    debug() << "===> Testing Right R/Phi DeVeloSensor container" << endmsg;
    TestSensorReverseIterators(m_velo->rightRPhiSensorsReverseBegin(),m_velo->rightRPhiSensorsReverseEnd());
  }
  if(m_r) {
    debug() << "==============================================================================" << endmsg;
    debug() << "Test containers in DeVelo which have only R sensors" << endmsg;
    debug() << "==============================================================================" << endmsg;
    debug() << "===> Testing R DeVeloRType container" << endmsg;
    TestRSensorReverseIterators(m_velo->rSensorsReverseBegin(),m_velo->rSensorsReverseEnd());
    debug() << "===> Testing Left R DeVeloRType container" << endmsg;
    TestRSensorReverseIterators(m_velo->leftRSensorsReverseBegin(),m_velo->leftRSensorsReverseEnd());
    debug() << "===> Testing Right R DeVeloRType container" << endmsg;
    TestRSensorReverseIterators(m_velo->rightRSensorsReverseBegin(),m_velo->rightRSensorsReverseEnd());
  }
  if(m_phi) {
    debug() << "==============================================================================" << endmsg;
    debug() << "Test containers in DeVelo which have only Phi sensors" << endmsg; 
    debug() << "==============================================================================" << endmsg;
    debug() << "===> Testing Phi DeVeloPhiType container" << endmsg;
    TestPhiSensorReverseIterators(m_velo->phiSensorsReverseBegin(),m_velo->phiSensorsReverseEnd());
    debug() << "===> Testing Left Phi DeVeloPhiType container" << endmsg;
    TestPhiSensorReverseIterators(m_velo->leftPhiSensorsReverseBegin(),m_velo->leftPhiSensorsReverseEnd());
    debug() << "===> Testing Right Phi DeVeloPhiType container" << endmsg;
    TestPhiSensorReverseIterators(m_velo->rightPhiSensorsReverseBegin(),m_velo->rightPhiSensorsReverseEnd());
  }
  if(m_pileUp) {
    debug() << "==============================================================================" << endmsg;
    debug() << "Test containers in DeVelo which have Pile Up sensors" << endmsg; 
    debug() << "==============================================================================" << endmsg;
    debug() << "===> Testing Pile Up DeVeloSensor containers" << endmsg;
    TestSensorReverseIterators(m_velo->pileUpSensorsReverseBegin(),m_velo->pileUpSensorsReverseEnd());
    debug() << "===> Testing Left Pile Up DeVeloSensor container" << endmsg;
    TestSensorReverseIterators(m_velo->leftPileUpSensorsReverseBegin(),m_velo->leftPileUpSensorsReverseEnd());
    debug() << "===> Testing Right Pile Up DeVeloSensor container" << endmsg;
    TestSensorReverseIterators(m_velo->rightPileUpSensorsReverseBegin(),m_velo->rightPileUpSensorsReverseEnd());
    debug() << "===> Testing Pile Up DeVeloRType container" << endmsg;
    TestRSensorReverseIterators(m_velo->pileUpRSensorsReverseBegin(),m_velo->pileUpRSensorsReverseEnd());
    debug() << "===> Testing Left Pile Up DeVeloRType container" << endmsg;
    TestRSensorReverseIterators(m_velo->leftPileUpRSensorsReverseBegin(),m_velo->leftPileUpRSensorsReverseEnd());
    debug() << "===> Testing Right Pile Up DeVeloRType container" << endmsg;
    TestRSensorReverseIterators(m_velo->rightPileUpRSensorsReverseBegin(),m_velo->rightPileUpRSensorsReverseEnd());
  }
}
//==============================================================================
// Generic method to test contents of sensor containers
//==============================================================================
void VeloDetChecker::TestSensorReverseIterators(std::vector<DeVeloSensor*>::const_reverse_iterator sensorsReverseBegin,
                                   std::vector<DeVeloSensor*>::const_reverse_iterator sensorsReverseEnd) {
  debug() << "TestSensorReverseIterators" << endmsg;
  for(std::vector<DeVeloSensor*>::const_reverse_iterator iSens=sensorsReverseBegin; iSens != sensorsReverseEnd; ++iSens) {
    debug() << format("Sensor Number=%d, R=%i, Phi=%i, PU=%i, Left=%i, Right=%i, Down=%i, z=%f",
                      (*iSens)->sensorNumber(),(*iSens)->isR(),(*iSens)->isPhi(),(*iSens)->isPileUp(),
                      (*iSens)->isLeft(),(*iSens)->isRight(),(*iSens)->isDownstream(),
                      (*iSens)->z()) 
            << endmsg;
  }
}
//==============================================================================
// Generic method to test contents of R sensor containers
//==============================================================================
void VeloDetChecker::TestRSensorReverseIterators(std::vector<DeVeloRType*>::const_reverse_iterator rSensorsReverseBegin,
                                  std::vector<DeVeloRType*>::const_reverse_iterator rSensorsReverseEnd) {
  debug() << "TestRSensorReverseIterators" << endmsg;
  for(std::vector<DeVeloRType*>::const_reverse_iterator iSens=rSensorsReverseBegin; iSens != rSensorsReverseEnd; ++iSens) {
    debug() << format("Sensor Number=%d, R=%i, Phi=%i, PU=%i, Left=%i, Right=%i, Down=%i, z=%f",
                      (*iSens)->sensorNumber(),(*iSens)->isR(),(*iSens)->isPhi(),(*iSens)->isPileUp(),
                      (*iSens)->isLeft(),(*iSens)->isRight(),(*iSens)->isDownstream(),
                      (*iSens)->z()) 
            << endmsg;
  }
}
//==============================================================================
// Generic method to test contents of Phi sensor containers
//==============================================================================
void VeloDetChecker::TestPhiSensorReverseIterators(std::vector<DeVeloPhiType*>::const_reverse_iterator phiSensorsReverseBegin,
                                    std::vector<DeVeloPhiType*>::const_reverse_iterator phiSensorsReverseEnd) {
  debug() << "TestPhiSensorReverseIterators" << endmsg;
  for(std::vector<DeVeloPhiType*>::const_reverse_iterator iSens=phiSensorsReverseBegin; iSens != phiSensorsReverseEnd; ++iSens) {
    debug() << format("Sensor Number=%d, R=%i, Phi=%i, PU=%i, Left=%i, Right=%i, Down=%i, z=%f",
                      (*iSens)->sensorNumber(),(*iSens)->isR(),(*iSens)->isPhi(),(*iSens)->isPileUp(),
                      (*iSens)->isLeft(),(*iSens)->isRight(),(*iSens)->isDownstream(),
                      (*iSens)->z()) 
            << endmsg;
  }
}
//==============================================================================
// Print out a table with numbers of sensors of each type
//==============================================================================
void VeloDetChecker::PrintNumberOfSensors() {
  debug() << "==============================================================================" << endmsg;
  debug() << "Count the number of various types of VeLo sensors" << endmsg;
  debug() << "==============================================================================" << endmsg;
  debug() << "----------------> All sensors" << endmsg;
  debug() << format("Number of sensors is:               %d",(m_velo->numberSensors())) << endmsg;
  debug() << format("Number of non Pile Up sensors is:   %d",(m_velo->numberNonPUSensors())) << endmsg;
  debug() << format("Number of R sensors is:             %d",(m_velo->numberRSensors())) << endmsg;
  debug() << format("Number of Phi sensors is:           %d",(m_velo->numberPhiSensors())) << endmsg;
  debug() << format("Number of Pile Up sensors is:       %d",(m_velo->numberPileUpSensors())) << endmsg;
  debug() << "----------------> Left sensors" << endmsg;
  debug() << format("Number of sensors is:               %d",(m_velo->numberLeftSensors())) << endmsg;
  debug() << format("Number of non Pile Up sensors is:   %d",(m_velo->numberLeftNonPUSensors())) << endmsg;
  debug() << format("Number of R sensors is:             %d",(m_velo->numberLeftRSensors())) << endmsg;
  debug() << format("Number of Phi sensors is:           %d",(m_velo->numberLeftPhiSensors())) << endmsg;
  debug() << format("Number of Pile Up sensors is:       %d",(m_velo->numberLeftPileUpSensors())) << endmsg;
  debug() << "----------------> Right sensors" << endmsg;
  debug() << format("Number of sensors is:               %d",(m_velo->numberRightSensors())) << endmsg;
  debug() << format("Number of non Pile Up sensors is:   %d",(m_velo->numberRightNonPUSensors())) << endmsg;
  debug() << format("Number of R sensors is:             %d",(m_velo->numberRightRSensors())) << endmsg;
  debug() << format("Number of Phi sensors is:           %d",(m_velo->numberRightPhiSensors())) << endmsg;
  debug() << format("Number of Pile Up sensors is:       %d",(m_velo->numberRightPileUpSensors())) << endmsg;
}
//==============================================================================
// Look at trajectory of first and last strips in each zone of the sensors
//==============================================================================
void VeloDetChecker::TestTrajectories() {
  debug() << "==============================================================================" << endmsg;
  debug() << " Look at trajectory of first and last strips in each zone of the sensors" << endmsg;
  debug() << "==============================================================================" << endmsg;

  for(std::vector<DeVeloSensor*>::const_iterator iSens=m_velo->sensorsBegin(); 
      iSens != m_velo->sensorsEnd(); ++iSens) {
    unsigned int sensor=(*iSens)->sensorNumber();
    debug() << format("==> Sensor Number=%d, R=%i, Phi=%i, PU=%i, Left=%i, Right=%i, Down=%i, z=%f",
                      (*iSens)->sensorNumber(),(*iSens)->isR(),(*iSens)->isPhi(),(*iSens)->isPileUp(),
                      (*iSens)->isLeft(),(*iSens)->isRight(),(*iSens)->isDownstream(),
                      (*iSens)->z()) 
            << endmsg;
    for(unsigned int zone=0; zone < (*iSens)->numberOfZones(); ++zone) {
      unsigned int nStrips=(*iSens)->stripsInZone(zone);
      debug() << format("--> Sensor number=%d, zone=%d",sensor,zone) << endmsg;
      PrintTrajectory(iSens,LHCb::VeloChannelID(sensor,zone*nStrips));
      PrintTrajectory(iSens,LHCb::VeloChannelID(sensor,zone*nStrips+nStrips-1));
    }
  }
}
//==============================================================================
// Print the start and end co-ordinates of the trajectory for a given sensor
// and VeloChannelID
//==============================================================================
void VeloDetChecker::PrintTrajectory(std::vector<DeVeloSensor*>::const_iterator iSens, 
                                     LHCb::VeloChannelID channel) {
  auto traj=(*iSens)->trajectory(channel,0.);
  Gaudi::XYZPoint begin=traj->beginPoint(); 
  Gaudi::XYZPoint end=traj->endPoint(); 
  debug() << format("Strip=%d, Begin: x=%f,y=%f,z=%f",channel.strip(),begin.x(),begin.y(),begin.z()) << endmsg;
  debug() << format("Strip=%d, End: x=%f,y=%f,z=%f",channel.strip(),end.x(),end.y(),end.z()) << endmsg;
  if((*iSens)->isR() || (*iSens)->isPileUp()) {
    debug() << format("Strip=%d, Phi: Begin=%f, End=%f",channel.strip(),begin.phi()/Gaudi::Units::degree,
                      end.phi()/Gaudi::Units::degree) << endmsg;
  }
}
//==============================================================================
// Steering routine to look at sensors (geometry and read-out)
//==============================================================================
void VeloDetChecker::CheckSensorGeometry() {
  debug() << "==> TestSensors" << endmsg;
  if(m_r) {
    CheckRSensorGeometry();
  } 
  if(m_phi) {
    CheckPhiSensorGeometry();
  }
  if(m_r && m_phi) {
    CheckAllSensorGeometry();
  }
}
//==============================================================================
// Produce ntuple containing x,y,z co-ordinates for all strips and readout info
//==============================================================================
void VeloDetChecker::CheckAllSensorGeometry() {
  debug() << "==> TestAllSensors" << endmsg;
  for(std::vector<DeVeloSensor*>::const_iterator iSens=m_velo->sensorsBegin(); 
      iSens != m_velo->sensorsEnd(); ++iSens) {
    const DeVeloSensor* sensor = (*iSens);
    for (unsigned int strip=0; strip != sensor->numberOfStrips(); ++strip){
      Tuple sensorTuple = nTuple( "allStrips", "Every channel in the VeLo" );
      sensorTuple->column( "sensor",sensor->sensorNumber());
      sensorTuple->column( "strip",strip);
      sensorTuple->column( "VeloChannelID",LHCb::VeloChannelID((sensor->sensorNumber()),strip));
      sensorTuple->column( "isLeft",sensor->isLeft());
      sensorTuple->column( "isDownstream",sensor->isDownstream());
      sensorTuple->column( "isR",sensor->isR());
      sensorTuple->column( "isPhi",sensor->isPhi());
      sensorTuple->column( "isPileUp",sensor->isPileUp());
      sensorTuple->column( "routLine",sensor->StripToRoutingLine(strip));
      sensorTuple->column( "chipChan",sensor->StripToChipChannel(strip));
      sensorTuple->column( "stripNoise",sensor->stripNoise(strip));
      std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> stripLimits;
      // Strip co-ordinates in the global frame
      stripLimits=sensor->globalStripLimits(strip);
      std::vector<double> globalBeginXYZ;
      globalBeginXYZ.push_back(stripLimits.first.x());
      globalBeginXYZ.push_back(stripLimits.first.y());
      globalBeginXYZ.push_back(stripLimits.first.z());
      sensorTuple->farray("globalBeginXYZ", globalBeginXYZ, "n", 3 );
      std::vector<double> globalEndXYZ;
      globalEndXYZ.push_back(stripLimits.second.x());
      globalEndXYZ.push_back(stripLimits.second.y());
      globalEndXYZ.push_back(stripLimits.second.z());
      sensorTuple->farray("globalEndXYZ", globalEndXYZ, "n", 3 );
      // Strip co-ordinates in the local frame
      stripLimits=sensor->localStripLimits(strip);
      std::vector<double> localBeginXYZ;
      localBeginXYZ.push_back(stripLimits.first.x());
      localBeginXYZ.push_back(stripLimits.first.y());
      localBeginXYZ.push_back(stripLimits.first.z());
      sensorTuple->farray("localBeginXYZ", localBeginXYZ, "n", 3 );
      std::vector<double> localEndXYZ;
      localEndXYZ.push_back(stripLimits.second.x());
      localEndXYZ.push_back(stripLimits.second.y());
      localEndXYZ.push_back(stripLimits.second.z());
      sensorTuple->farray("localEndXYZ", localEndXYZ, "n", 3 );
      sensorTuple->write();
    }
  }
}
//==============================================================================
// Produce ntuple containing all geometry and readout info for R sensors
//==============================================================================
void VeloDetChecker::CheckRSensorGeometry() {
  debug() << "==> TestRSensors" << endmsg;
  const DeVeloRType* rSensor = (*m_velo->rSensorsBegin());
  for (unsigned int strip=0; strip != rSensor->numberOfStrips(); ++strip){
    Tuple rTuple = nTuple( "rStrips", "R sensor" );
    rTuple->column( "strip",strip);
    rTuple->column( "sensor",rSensor->sensorNumber());
    rTuple->column( "z",rSensor->z());
    rTuple->column( "routLine",rSensor->StripToRoutingLine(strip));
    rTuple->column( "chipChan",rSensor->StripToChipChannel(strip));
    rTuple->column( "r", rSensor->rOfStrip(strip));
    rTuple->column( "pitch", rSensor->rPitch(strip));
    rTuple->column( "phiMin",rSensor->phiMinStrip(strip));
    rTuple->column( "phiMax",rSensor->phiMaxStrip(strip));
    rTuple->column( "length",rSensor->stripLength(strip));
    std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> stripLimits=rSensor->globalStripLimits(strip);
    std::vector<double> beginXYZ;
    beginXYZ.push_back(stripLimits.first.x());
    beginXYZ.push_back(stripLimits.first.y());
    beginXYZ.push_back(stripLimits.first.z());
    rTuple->farray("beginXYZ", beginXYZ, "n", 3 );
    std::vector<double> endXYZ;
    endXYZ.push_back(stripLimits.second.x());
    endXYZ.push_back(stripLimits.second.y());
    endXYZ.push_back(stripLimits.second.z());
    rTuple->farray("endXYZ", endXYZ, "n", 3 );
    rTuple->write();
  }
}
//==============================================================================
// Produce ntuple containing all geometry and readout info for R sensors
//==============================================================================
void VeloDetChecker::CheckPhiSensorGeometry() {
  debug() << "==> TestPhiSensors" << endmsg;
  const DeVeloPhiType* phiSensor = (*m_velo->phiSensorsBegin());
  for (unsigned int strip=0; strip != phiSensor->numberOfStrips(); ++strip){
    Tuple phiTuple = nTuple( "phiStrips", "Phi sensor" );
    phiTuple->column( "strip",strip);
    phiTuple->column( "sensor",phiSensor->sensorNumber());
    phiTuple->column( "z",phiSensor->z());
    phiTuple->column( "routLine",phiSensor->StripToRoutingLine(strip));
    phiTuple->column( "chipChan",phiSensor->StripToChipChannel(strip));
    phiTuple->column( "idealPhiOfStrip",phiSensor->idealPhiOfStrip(strip));
    phiTuple->column( "angleOfStrip",phiSensor->angleOfStrip(strip));
    phiTuple->column( "trgPhiDirectionOfStrip",phiSensor->trgPhiDirectionOfStrip(strip));
    phiTuple->column( "length",phiSensor->stripLength(strip));
    std::pair<Gaudi::XYZPoint,Gaudi::XYZPoint> stripLimits=phiSensor->globalStripLimits(strip);
    std::vector<double> beginXYZ;
    beginXYZ.push_back(stripLimits.first.x());
    beginXYZ.push_back(stripLimits.first.y());
    beginXYZ.push_back(stripLimits.first.z());
    phiTuple->farray("beginXYZ", beginXYZ, "n", 3 );
    std::vector<double> endXYZ;
    endXYZ.push_back(stripLimits.second.x());
    endXYZ.push_back(stripLimits.second.y());
    endXYZ.push_back(stripLimits.second.z());
    phiTuple->farray("endXYZ", endXYZ, "n", 3 );
    phiTuple->write();
  }
}
//==============================================================================
// Scan the surface of x-y plane to check point to channel and residual 
// calculations work.
//==============================================================================
void VeloDetChecker::ScanXYPlaneOfSensors() {
  if(m_r) {
    ScanXYPlaneOfSensor((*m_velo->rSensorsBegin()));
    ScanXYPlaneOfSensor((*(m_velo->rSensorsBegin()+1)));
    ScanXYPlaneOfSensor((*(m_velo->rSensorsBegin()+2)));
    ScanXYPlaneOfSensor((*(m_velo->rSensorsBegin()+3)));
  }
  if(m_phi) {
    ScanXYPlaneOfSensor((*m_velo->phiSensorsBegin()));
    ScanXYPlaneOfSensor((*(m_velo->phiSensorsBegin()+1)));
    ScanXYPlaneOfSensor((*(m_velo->phiSensorsBegin()+2)));
    ScanXYPlaneOfSensor((*(m_velo->phiSensorsBegin()+3)));
  }

}
//==============================================================================
// Scan over surface of detectors
//==============================================================================
void VeloDetChecker::ScanXYPlaneOfSensor(const DeVeloSensor* sensor){
  verbose() << "Scan XY Plane Of Sensor " << sensor->sensorNumber() << endmsg;
  std::string sens=boost::lexical_cast<std::string>(sensor->sensorNumber());
  std::string scanTitle="ScanXYPlaneSensor" + sens;
  int range=50*m_scale;
  int nBins=2*range+1;
  for(int ix=-range; ix<range; ix++){
    double x=ix/static_cast<double>(m_scale);
    for(int iy=-range; iy<range; iy++){
      double y=iy/static_cast<double>(m_scale);
      double z=sensor->z();
      LHCb::VeloChannelID vcID;
      double offset,pitch;
      StatusCode sc=sensor->pointToChannel(Gaudi::XYZPoint(x,y,z),vcID,offset,pitch);
      verbose() << "x=" << x << ",y=" << y << ",z=" << z << ",sc=" << sc << endmsg;
      if(sc.isSuccess()) {
        plot2D(x,y,scanTitle,"Scan map of sensor "+sens,-50.,50.,-50.,50,nBins,nBins);
      }
    }
  }
}
