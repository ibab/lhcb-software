// $Id: DeVPYType.cpp,v 1.5 2009-10-28 13:02:28 cocov Exp $
//==============================================================================
#define VPDET_DEVPYTYPE_CPP 1
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

// From VP
#include "VPDet/DeVPYType.h"
#include "Kernel/VPChannelID.h"

//namespace VPDet {

// used to control initialization
bool DeVPYType::m_staticDataInvalid = true;

/** @file DeVPYType.cpp
 *
 *  Implementation of class : DeVPYType
 *
 *  @author Victor Coco  victor.coco@cern.ch
 */

//==============================================================================
/// Standard constructor
//==============================================================================
DeVPYType::DeVPYType(const std::string& name) : 
  DeVPSensor(name),
  m_ladders(),
  m_associatedXSensor(0),
  m_otherSideXSensor(0),
  m_otherSideYSensor(0)
{
}
//==============================================================================
/// Destructor
//==============================================================================
DeVPYType::~DeVPYType()
{
}
//==============================================================================
/// Object identification
//==============================================================================
const CLID& DeVPYType::clID() 
  const { return DeVPYType::classID(); }
//==============================================================================
/// Initialisation method
//==============================================================================
StatusCode DeVPYType::initialize() 
{
  // Trick from old DeVP to set the output level
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel=0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  if( sc.isSuccess() ) sc = jobSvc->setMyProperties("DeVPYType", pmgr);
  if ( 0 < outputLevel ) {
    msgSvc()->setOutputLevel("DeVPYType", outputLevel);
  }
  delete pmgr;
  if( !sc ) return sc;
  MsgStream msg(msgSvc(), "DeVPYType");
  sc = DeVPSensor::initialize();
  if(!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DeVPSensor" << endreq;
    return sc;
  }
  m_debug   = (msgSvc()->outputLevel("DeVPYType") == MSG::DEBUG  ) ;
  m_verbose = (msgSvc()->outputLevel("DeVPYType") == MSG::VERBOSE) ;

  /*  for (int nl = 0 ; nl < ladderNumber() ; nl ++)
  {
    std::stringstream s;
    s << nl+1;
    Gaudi::XYZPoint point(param<double>("XRef"+s.str()),param<double>("YRef"+s.str()),param<double>("ZRef"+s.str()));
    PixelLadder tmp_ladder(point,param<int>("NChip"+s.str()));
    m_ladders.push_back(tmp_ladder);
    }*/
  return StatusCode::SUCCESS;
}
//==============================================================================
/// # Calculate the nearest channel to a 3-d point.
//==============================================================================

StatusCode DeVPYType::pointToChannel(const Gaudi::XYZPoint& point,
                                       LHCb::VPChannelID& channel,
                                       std::pair <double, double>& fraction) const
{
  MsgStream msg(msgSvc(), "DeVPYType");
  Gaudi::XYZPoint localPoint = globalToLocal(point);

  // Check that the point is in the active area of the sensor
  StatusCode sc = isInActiveArea(localPoint);
  if(!sc.isSuccess()) return sc;  
  unsigned int sensor=sensorNumber();
  // Create the associated VPChannelID
  channel.setSensor(sensor);
  // Get the ladder number in which the point is
  int ladderIndex = WhichLadder(point);
  // Get the chip number in which the point is
  int chipIndex = WhichChip(point,ladderIndex);
  // Compute the proper chip number for VPChanelID
  int prop_chipIndex = chipIndex;
  for(int ilad = 0 ; ilad < ladderIndex ; ilad ++){
    prop_chipIndex += m_ladders[ilad].nChip();
  }
  // Set the chip number in the VPChannelID
  channel.setChip(prop_chipIndex);  
  // Get the pixel position in which the point is
  std::pair <int,int> pixelPos = WhichPixel(point,ladderIndex,chipIndex,fraction);
  if( pixelPos.first < 0. || pixelPos.second < 0. ){
    return StatusCode::FAILURE;
  }
  // Set the pixel position in the VPChannelID
  channel.setPixel_lp(pixelPos.first);
  channel.setPixel_hp(pixelPos.second);
  // Set fraction to 0 but this will need to be set correctly
  return StatusCode::SUCCESS;
}


//==============================================================================
/// Checks if local point is inside sensor          
//==============================================================================
StatusCode DeVPYType::isInActiveArea(const Gaudi::XYZPoint& point) const
{
  if(WhichLadder(point)>-0.5)return StatusCode::SUCCESS;
  return StatusCode::FAILURE;
}

//==============================================================================
/// Get the index of the ladder containing the point
//==============================================================================
int DeVPYType::WhichLadder(const Gaudi::XYZPoint& point) const
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
int DeVPYType::WhichChip(const Gaudi::XYZPoint& point, int ladderIndex) const
{  
  Gaudi::XYZPoint refPoint = m_ladders[ladderIndex].ReferencePoint();
  Gaudi::XYZPoint LocalPoint(point.x()-refPoint.x(),point.y()-refPoint.y(),point.z()-refPoint.z());
  return (int)(LocalPoint.x()/(chipLength()+interChipDist()));

}

//==============================================================================
/// Get 
//==============================================================================

std::pair<int,int> DeVPYType::WhichPixel(const Gaudi::XYZPoint& point, int ladderIndex, int chipIndex, std::pair<double,double>& fraction) const
{
  double alongAxisOffset = 0.;
  if( chipIndex>0)  alongAxisOffset = chipIndex*(chipLength()+interChipDist()/2)+(chipIndex-1)*interChipDist()/2;

  Gaudi::XYZPoint refPoint = m_ladders[ladderIndex].ReferencePoint();
  Gaudi::XYZPoint LocalPoint(point.x()-refPoint.x()-alongAxisOffset,point.y()-refPoint.y(),point.z()-refPoint.z());

  int positionEdgePix =  (m_ladders[ladderIndex]).edgeOrientation(chipIndex);
  std::pair< int , int > thePixel;

  // Set the size of the edges (left/right) pixels
  double interchipPixSizeLEFT = lpSize();
  double interchipPixSizeRIGHT = lpSize();
  if ( positionEdgePix== 1 || positionEdgePix== 0 )interchipPixSizeRIGHT = interchipPixSize();
  if ( positionEdgePix== -1 || positionEdgePix== 0 )interchipPixSizeLEFT = interchipPixSize();
  
  // correct for the extra length of the left pixel
  double newx = LocalPoint.x() - (interchipPixSizeLEFT-lpSize());
  // set the results for most of the case
  thePixel.first = (int)(newx/lpSize());
  fraction.first = newx/lpSize() - (int)(newx/lpSize());
  //modify in the case where it is on the left edge
  if (newx < lpSize()  && newx > - (interchipPixSizeLEFT-lpSize())){
    thePixel.first=0; // in case it would have been negative
      fraction.first = LocalPoint.x()/interchipPixSizeLEFT;
  }
  else if (newx <((nPixCol()-1)*lpSize())+interchipPixSizeRIGHT && newx >((nPixCol()-2)*lpSize())){
    fraction.first = (newx-(nPixCol()-1)*lpSize())/interchipPixSizeRIGHT;
  }
  else{
    thePixel.first = -1;
    fraction.first = -1; 
  }
  thePixel.second=(int)(LocalPoint.y()/hpSize());  
  fraction.second=LocalPoint.y()/hpSize()-thePixel.second;  
  return thePixel;
}

