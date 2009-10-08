// $Id: DeVeloPixSquareType.cpp,v 1.2 2009-10-08 08:24:40 cocov Exp $
//==============================================================================
#define VELOPIXDET_DEVELOPIXSQUARETYPE_CPP 1
//==============================================================================
// Include files 

// From Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// From LHCb
#include "LHCbMath/LHCbMath.h"
#include "Kernel/CircleTraj.h"
#include "gsl/gsl_math.h"

// From VeloPix
#include "VeloPixDet/DeVeloPixSquareType.h"
#include "Kernel/VeloPixChannelID.h"


// used to control initialization
bool DeVeloPixSquareType::m_staticDataInvalid = true;

/** @file DeVeloPixSquareType.cpp
 *
 *  Implementation of class : DeVeloPixSquareType
 *
 *  @author Victor Coco victor.coco@cern.ch
 */

//==============================================================================
/// Standard constructor
//==============================================================================
DeVeloPixSquareType::DeVeloPixSquareType(const std::string& name) : 
  DeVeloPixSensor(name),
  m_ladders(),
  m_otherSideSensor(0)
{
}
//==============================================================================
/// Destructor
//==============================================================================
DeVeloPixSquareType::~DeVeloPixSquareType()
{
}
//==============================================================================
/// Object identification
//==============================================================================
const CLID& DeVeloPixSquareType::clID() 
  const { return DeVeloPixSquareType::classID(); }
//==============================================================================
/// Initialisation method
//==============================================================================
StatusCode DeVeloPixSquareType::initialize() 
{
   // Trick from old DeVelo to set the output level
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel=0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  if( sc.isSuccess() ) sc = jobSvc->setMyProperties("DeVeloPixSquareType", pmgr);
  if( !sc ) return sc;
  if ( 0 < outputLevel ) {
    msgSvc()->setOutputLevel("DeVeloPixSquareType", outputLevel);
  }
  delete pmgr;
  MsgStream msg(msgSvc(), "DeVeloPixSquareType");
  sc = DeVeloPixSensor::initialize();
  if(!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DeVeloPixSensor" << endreq;
    return sc;
  }
  m_debug   = (msgSvc()->outputLevel("DeVeloPixSquareType") == MSG::DEBUG  ) ;
  m_verbose = (msgSvc()->outputLevel("DeVeloPixSquareType") == MSG::VERBOSE) ;

  // Fill the ladder vector with parameters from the DDDB
  for (int nl = 0 ; nl < ladderNumber() ; nl ++)
  {
    std::stringstream s;
    s << nl+1;
    //bottom left corner of the active part of ladder nl
    Gaudi::XYZPoint point(param<double>("XRef"+s.str()),param<double>("YRef"+s.str()),param<double>("ZRef"+s.str()));
    PixelLadder tmp_ladder(point,param<int>("NChip"+s.str()));
    m_ladders.push_back(tmp_ladder);
  }
  return StatusCode::SUCCESS; 
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Calculate the nearest channel to a 3-d point 
/// (assuming the DeVeloPixSquareType instance is already the correct one: z is corresponding to the sensor number)
//==============================================================================
StatusCode DeVeloPixSquareType::pointToChannel(const Gaudi::XYZPoint& point,
                                       LHCb::VeloPixChannelID& channel,
                                       double& fraction) const
{
  MsgStream msg(msgSvc(), "DeVeloPixSquareType");
  Gaudi::XYZPoint localPoint = globalToLocal(point);

  // Check that the point is in the active area of the sensor
  StatusCode sc = isInActiveArea(localPoint);
  if(!sc.isSuccess()) return sc;  
  unsigned int sensor=sensorNumber();
  // Create the associated VeloPixChannelID
  channel.setSensor(sensor);
  // Get the ladder number in which the point is
  int ladderIndex = WhichLadder(point);
  // Get the chip number in which the point is
  int chipIndex = WhichChip(point,ladderIndex);
  // Compute the proper chip number for VeloPixChanelID
  int prop_chipIndex = chipIndex;
  for(int ilad = 0 ; ilad < ladderIndex ; ilad ++){
    prop_chipIndex += m_ladders[ilad].nChip();
  }
  // Set the chip number in the VeloPixChannelID
  channel.setChip(prop_chipIndex);  
  // Get the pixel position in which the point is
  std::pair <int,int> pixelPos = WhichPixel(point,ladderIndex,chipIndex);
  // Set the pixel position in the VeloPixChannelID
  channel.setPixel_lp(pixelPos.first);
  channel.setPixel_hp(pixelPos.second);
  // Set fraction to 0 but this will need to be set correctly
  fraction = 0.;
  return StatusCode::SUCCESS;
}


//==============================================================================
/// Checks if local point is inside sensor                      
//==============================================================================
StatusCode DeVeloPixSquareType::isInActiveArea(const Gaudi::XYZPoint& point) const
{
  // Check the the point is in the active part of a ladder
  if(WhichLadder(point)>-0.5)return StatusCode::SUCCESS;
  return StatusCode::FAILURE;
}


//==============================================================================
/// Get the index of the ladder containing the point
//==============================================================================
int DeVeloPixSquareType::WhichLadder(const Gaudi::XYZPoint& point) const
{
  for ( int index = 0 ; index <(int) m_ladders.size() ; index++){
    const Gaudi::XYZPoint pointRef = m_ladders[index].ReferencePoint();
    if (
        (point.x()-pointRef.x() > 0. && point.x()-pointRef.x()<m_ladders[index].nChip()*(chipLength()+interChipDist())) 
        && (point.y()-pointRef.y() > 0. && point.y()-pointRef.y()<chipWidth())
        && (fabs(point.z()-z()-pointRef.z())<6*siliconThickness()/10)
        ) return index;
  }
  return -1;
}

//==============================================================================
/// Get the index relative to ladder of the chip containing the point
//==============================================================================
int DeVeloPixSquareType::WhichChip(const Gaudi::XYZPoint& point, int ladderIndex) const
{  
  Gaudi::XYZPoint refPoint = m_ladders[ladderIndex].ReferencePoint();
  Gaudi::XYZPoint LocalPoint(point.x()-refPoint.x(),point.y()-refPoint.y(),point.z()-refPoint.z());
  return (int)(LocalPoint.x()/(chipLength()+interChipDist()));

}

//==============================================================================
/// Get the index relative to the chip of the pixel containing the point   [to be checked for MINIPLAE TIMEPIX since the "last pixel" are face to face]
//==============================================================================
std::pair<int,int> DeVeloPixSquareType::WhichPixel(const Gaudi::XYZPoint& point, int ladderIndex, int chipIndex) const
{
  float LowPrecisionOffset = chipIndex*(chipLength()+interChipDist());
  Gaudi::XYZPoint refPoint = m_ladders[ladderIndex].ReferencePoint();
  Gaudi::XYZPoint LocalPoint(point.x()-refPoint.x()-LowPrecisionOffset,point.y()-refPoint.y(),point.z()-refPoint.z());
  std::pair< int , int > thePixel;
  int lp = (int)(LocalPoint.x()/lpSize());
  // Deal with the last pixel that is LowPrecisionSize + InterChipDist
  if (lp == nPixCol()) lp = nPixCol()-1; 
  thePixel.first=lp;  
  thePixel.second=(int)(LocalPoint.y()/hpSize());  
  return thePixel;
}

