// $Id: DeVeloPixXType.cpp,v 1.2 2009-10-08 08:24:40 cocov Exp $
//==============================================================================
#define VELOPIXDET_DEVELOPIXXTYPE_CPP 1
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
#include "VeloPixDet/DeVeloPixXType.h"
#include "Kernel/VeloPixChannelID.h"

//namespace VeloPixDet {

// used to control initialization
bool DeVeloPixXType::m_staticDataInvalid = true;

/** @file DeVeloPixXType.cpp
 *
 *  Implementation of class : DeVeloPixXType
 *
 *  @author Victor Coco  victor.coco@cern.ch
 */

//==============================================================================
/// Standard constructor
//==============================================================================
DeVeloPixXType::DeVeloPixXType(const std::string& name) : 
  DeVeloPixSensor(name),
  m_ladders(),
  m_associatedYSensor(0),
  m_otherSideXSensor(0),
  m_otherSideYSensor(0)
{
}
//==============================================================================
/// Destructor
//==============================================================================
DeVeloPixXType::~DeVeloPixXType()
{
}
//==============================================================================
/// Object identification
//==============================================================================
const CLID& DeVeloPixXType::clID() 
  const { return DeVeloPixXType::classID(); }
//==============================================================================
/// Initialisation method
//==============================================================================
StatusCode DeVeloPixXType::initialize() 
{
  // Trick from old DeVeloPix to set the output level
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel=0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  if( sc.isSuccess() ) sc = jobSvc->setMyProperties("DeVeloPixXType", pmgr);
  if( !sc ) return sc;
  if ( 0 < outputLevel ) {
    msgSvc()->setOutputLevel("DeVeloPixXType", outputLevel);
  }
  delete pmgr;
  MsgStream msg(msgSvc(), "DeVeloPixXType");
  sc = DeVeloPixSensor::initialize();
  if(!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DeVeloPixSensor" << endreq;
    return sc;
  }
  m_debug   = (msgSvc()->outputLevel("DeVeloPixXType") == MSG::DEBUG  ) ;
  m_verbose = (msgSvc()->outputLevel("DeVeloPixXType") == MSG::VERBOSE) ;

  for (int nl = 0 ; nl < ladderNumber() ; nl ++)
  {
    std::stringstream s;
    s << nl+1;
    Gaudi::XYZPoint point(param<double>("XRef"+s.str()),param<double>("YRef"+s.str()),param<double>("ZRef"+s.str()));
    PixelLadder tmp_ladder(point,param<int>("NChip"+s.str()));
    m_ladders.push_back(tmp_ladder);
  }
  return StatusCode::SUCCESS;
}
//==============================================================================
/// # Calculate the nearest channel to a 3-d point.
//==============================================================================
StatusCode DeVeloPixXType::pointToChannel(const Gaudi::XYZPoint& point,
                                       LHCb::VeloPixChannelID& channel,
                                       double& fraction) const
{
  MsgStream msg(msgSvc(), "DeVeloPixXType");
  Gaudi::XYZPoint localPoint = globalToLocal(point);

  // Check boundaries...
  StatusCode sc = isInActiveArea(localPoint);
  if(!sc.isSuccess()) return sc;  
  unsigned int sensor=sensorNumber();
  // set VeloPixChannelID....
  channel.setSensor(sensor);
  // get the ladder
  int ladderIndex = WhichLadder(point);
  int chipIndex = WhichChip(point,ladderIndex);
  // compute the proper chip number for VeloPixChanelID
  int prop_chipIndex = chipIndex;
  for(int ilad = 0 ; ilad < ladderIndex ; ilad ++){
    prop_chipIndex += m_ladders[ilad].nChip();
  }
  channel.setChip(prop_chipIndex);  
  std::pair <int,int> pixelPos = WhichPixel(point,ladderIndex,chipIndex);
  channel.setPixel_lp(pixelPos.first);
  channel.setPixel_hp(pixelPos.second);
  // Set fraction to 0 but this will need to be set correctly
  fraction = 0.;
  return StatusCode::SUCCESS;
}

//==============================================================================
/// Checks if local point is inside sensor          
//==============================================================================
StatusCode DeVeloPixXType::isInActiveArea(const Gaudi::XYZPoint& point) const
{
  if(WhichLadder(point)>-0.5)return StatusCode::SUCCESS;
  return StatusCode::FAILURE;
}

//==============================================================================
/// Get the index of the ladder containing the point
//==============================================================================
int DeVeloPixXType::WhichLadder(const Gaudi::XYZPoint& point) const
{
  for ( int index = 0 ; index <(int) m_ladders.size() ; index++){
    const Gaudi::XYZPoint pointRef = m_ladders[index].ReferencePoint();
    if (
        (point.x()-pointRef.x() > 0. && point.x()-pointRef.x()<chipWidth()) 
        && (point.y()-pointRef.y() > 0. && point.y()-pointRef.y()<m_ladders[index].nChip()*(chipLength()+interChipDist()))
        && (fabs(point.z()-z()-pointRef.z())<6*siliconThickness()/10)
        ) return index;
  }
  return -1;
}

//==============================================================================
/// Get the index relative to ladder of the chip containing the point
//==============================================================================
int DeVeloPixXType::WhichChip(const Gaudi::XYZPoint& point, int ladderIndex) const
{  
  Gaudi::XYZPoint refPoint = m_ladders[ladderIndex].ReferencePoint();
  Gaudi::XYZPoint LocalPoint(point.x()-refPoint.x(),point.y()-refPoint.y(),point.z()-refPoint.z());
  return (int)(LocalPoint.y()/(chipLength()+interChipDist()));

}

//==============================================================================
/// Get 
//==============================================================================
std::pair<int,int> DeVeloPixXType::WhichPixel(const Gaudi::XYZPoint& point, int ladderIndex, int chipIndex) const
{
  float LowPrecisionOffset = chipIndex*(chipLength()+interChipDist());
  Gaudi::XYZPoint refPoint = m_ladders[ladderIndex].ReferencePoint();
  Gaudi::XYZPoint LocalPoint(point.x()-refPoint.x(),point.y()-refPoint.y()-LowPrecisionOffset,point.z()-refPoint.z());
  std::pair< int , int > thePixel;
  int lp = (int)(LocalPoint.y()/lpSize());
  // Deal with the last pixel that is LowPrecisionSize + InterChipDist
  if (lp == nPixCol()) lp = nPixCol()-1; 
  thePixel.first=lp;  
  thePixel.second=(int)(LocalPoint.x()/hpSize());  
  return thePixel;
}


