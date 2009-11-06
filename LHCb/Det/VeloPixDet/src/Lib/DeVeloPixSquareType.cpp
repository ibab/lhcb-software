// $Id: DeVeloPixSquareType.cpp,v 1.8 2009-11-06 11:05:05 cocov Exp $
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
    std::vector<int> tmp_interPixPos (tmp_ladder.nChip());
    for ( int nchip = 0 ; nchip < tmp_ladder.nChip() ; nchip++)
    {
      std::stringstream sch;
      sch << nchip+1;
      tmp_interPixPos[nchip]=param<int>("PosInterChip"+sch.str()+"_Ladd"+s.str());    
    }
    tmp_ladder.setEdgesOrientation(tmp_interPixPos);
    m_ladders.push_back(tmp_ladder);
  }
  return StatusCode::SUCCESS; 
}



//==============================================================================
/// Calculate the nearest channel to a 3-d point 
/// (assuming the DeVeloPixSquareType instance is already the correct one: z is corresponding to the sensor number)
//==============================================================================
StatusCode DeVeloPixSquareType::pointToChannel(const Gaudi::XYZPoint& point,
                                       LHCb::VeloPixChannelID& channel,
                                       std::pair <double, double>& fraction) const
{
  MsgStream msg(msgSvc(), "DeVeloPixSquareType");
  Gaudi::XYZPoint localPoint = globalToLocal(point);

  // Check that the point is in the active area of the sensor
  StatusCode sc = isInActiveArea(localPoint);
  if(!sc.isSuccess())return sc;  
  unsigned int sensor=sensorNumber();
  // Create the associated VeloPixChannelID
  channel.setSensor(sensor);
  // Get the ladder number in which the point is
  int ladderIndex = WhichLadder(localPoint);
  // Get the chip number in which the point is
  int chipIndex = WhichChip(localPoint,ladderIndex);
  // Compute the proper chip number for VeloPixChanelID
  int prop_chipIndex = chipIndex;
  for(int ilad = 0 ; ilad < ladderIndex ; ilad ++){
    prop_chipIndex += m_ladders[ilad].nChip();
  }
  // Set the chip number in the VeloPixChannelID
  channel.setChip(prop_chipIndex);  
  // Get the pixel position in which the point is
  std::pair <int,int> pixelPos = WhichPixel(localPoint,ladderIndex,chipIndex,fraction);
  if( pixelPos.first < 0. || pixelPos.second < 0. ){
    return StatusCode::FAILURE;
  }
  // Set the pixel position in the VeloPixChannelID
  channel.setPixel_lp(pixelPos.first);
  channel.setPixel_hp(pixelPos.second);
  return StatusCode::SUCCESS;
}

//==============================================================================
/// Calculate the center of the pixel from a given channel
//==============================================================================
StatusCode DeVeloPixSquareType::channelToPoint( const LHCb::VeloPixChannelID& channel,
                                                Gaudi::XYZPoint& point) const
{
  MsgStream msg(msgSvc(), "DeVeloPixSquareType");
  Gaudi::XYZPoint LocalPoint(0.,0.,0.);
  int chipNum = channel.chip();  
  int ladderNum = ladderNumber();
  int ladderIndex = 0;
  int ntotChip = 0;
  int chipInLadd = 0;
  for(int ilad = 0 ; ilad < ladderNum ; ilad ++){
    int ntotChip_tmp = ntotChip;
    ntotChip += m_ladders[ilad].nChip();
    if ( chipNum < ntotChip && ( chipNum > ntotChip_tmp ||  chipNum == ntotChip_tmp ) ) {
      // Get the ladder
      ladderIndex = ilad;
      // get the chip
      chipInLadd = chipNum-(ntotChip-m_ladders[ilad].nChip());
      // Set the position in the pixel
      std::pair <double, double> size = PixelSize(channel);
      // Set the offset due to left edge pixel (except when it is the first pixel)
      double xoffset = 0.;
      
      int positionEdgePix =  (m_ladders[ladderIndex]).edgeOrientation(chipInLadd);
      if (channel.pixel_lp()>0 && ( positionEdgePix == 0 || positionEdgePix == -1 ) ) xoffset = interchipPixSize()- lpSize();
      LocalPoint.SetX((channel.pixel_lp())*lpSize()+size.first/2+xoffset);
      LocalPoint.SetY((channel.pixel_hp())*hpSize()+size.second/2);
    }
  }
  // Add the bottom left position of the chip in the ladder
  if (chipInLadd < 2)LocalPoint.SetX((chipInLadd)*(chipLength()+interChipDist()/2)+LocalPoint.x());
  // the else is not used for TIMEPIX minplane and all this have to be checked for othe geometry
  else LocalPoint.SetX((chipInLadd-1)*(chipLength()+interChipDist())+(chipLength()+interChipDist()/2)+LocalPoint.x());

  // Add the postition of the reference LocalPoint of the ladder
  LocalPoint.SetX(m_ladders[ladderIndex].ReferencePoint().x()+LocalPoint.x());
  LocalPoint.SetY(m_ladders[ladderIndex].ReferencePoint().y()+LocalPoint.y());
  LocalPoint.SetZ(m_ladders[ladderIndex].ReferencePoint().z());
  point = localToGlobal(LocalPoint);
  
  return StatusCode::SUCCESS;
}

//==============================================================================
/// Calculate the nearest 3x3 list of channel to a 3-d point 
//==============================================================================
StatusCode  DeVeloPixSquareType::pointTo3x3Channels(const Gaudi::XYZPoint& point,
                                       std::vector <LHCb::VeloPixChannelID>& channels) const
{
  MsgStream msg(msgSvc(), "DeVeloPixSquareType");
  // Get the channel corresponding to the central point 
  LHCb::VeloPixChannelID  channelCentral;
  std::pair <double, double> fraction;
  StatusCode sc = pointToChannel( point, channelCentral, fraction);
  Gaudi::XYZPoint loc_point = globalToLocal(point);
  // Initialise the size and if there is a channel at the central point get its size
  std::pair <double, double> size (0.,0.);
  if( sc.isSuccess()) size = PixelSize(channelCentral);

  // Loop over the possible position in the 3x3 cluster
  for (int x = -1 ; x < 2 ; x ++){
    for (int y = -1 ; y < 2 ; y ++){
      
      // if it is the central position and it was corresponding to a channel, put it in the list
      if( x == 0 && y == 0 && sc.isSuccess() ) {
        channels.push_back(channelCentral);
        continue;
      }
      // but if is the central position and it does not correspond to any channel skip it
      else if( x == 0 && y == 0 && !sc.isSuccess() )  continue;
      double relx = 0;
      double rely = 0;
      // compute the relx,rely distances to which one might find an other pixel for the different x,y    
      if (x < 0) relx = - fraction.first*size.first - lpSize()/2;
      if (x > 0) relx = (1.- fraction.first)*size.first + lpSize()/2;
      if (y < 0) rely = - fraction.second*size.second - hpSize()/2;
      if (y > 0) rely = (1.- fraction.second)*size.second + hpSize()/2;
      if (!sc.isSuccess()){
      // in case fraction is undefined (central position does not correspond to any channel), move by nominal size
        relx = x*lpSize();
        rely = y*hpSize();
      }
      
      Gaudi::XYZPoint neigh_point (loc_point.x()+relx,loc_point.y()+rely,loc_point.z());  
      Gaudi::XYZPoint glob_neigh_point = localToGlobal(neigh_point);
      LHCb::VeloPixChannelID neig_channel;
      std::pair <double,double> frac (0.,0.);
      // if there is a channel corresponding to this new point, put it in the list
      if ( pointToChannel( glob_neigh_point, neig_channel, frac).isSuccess()){
        channels.push_back(neig_channel);
      }
    }
  }
  return StatusCode::SUCCESS;
}


//==============================================================================
/// Get the 8 channel (if they exist) arround a given seed channel
//==============================================================================
StatusCode  DeVeloPixSquareType::channelToNeighbours( const LHCb::VeloPixChannelID& seedChannel,
                                       std::vector <LHCb::VeloPixChannelID>& channels) const
{
  MsgStream msg(msgSvc(), "DeVeloPixSquareType");
  // Get the point corresponding to the seedChannel 
  std::pair <double, double> fraction(0.5,0.5);
  Gaudi::XYZPoint point;
  StatusCode sc = channelToPoint (seedChannel, point);
  if (!sc.isSuccess()) return sc;
  
  Gaudi::XYZPoint loc_point = globalToLocal(point);

  // Initialise the size of the seed pixel
  std::pair <double, double> size (0.,0.);
  size = PixelSize(seedChannel);

  // Loop over the possible position in the 3x3 cluster
  for (int x = -1 ; x < 2 ; x ++){
    for (int y = -1 ; y < 2 ; y ++){

      if( x == 0 && y == 0  )  continue;
      double relx = 0;
      double rely = 0;
      // compute the relx,rely distances to which one might find an other pixel for the different x,y    
      if (x < 0) relx = - fraction.first*size.first - lpSize()/2;
      if (x > 0) relx = (1.- fraction.first)*size.first + lpSize()/2;
      if (y < 0) rely = - fraction.second*size.second - hpSize()/2;
      if (y > 0) rely = (1.- fraction.second)*size.second + hpSize()/2;
      if (!sc.isSuccess()){
      // in case fraction is undefined (central position does not correspond to any channel), move by nominal size
        relx = x*lpSize();
        rely = y*hpSize();
      }
      
      Gaudi::XYZPoint neigh_point (loc_point.x()+relx,loc_point.y()+rely,loc_point.z());  
      Gaudi::XYZPoint glob_neigh_point = localToGlobal(neigh_point);
      LHCb::VeloPixChannelID neig_channel;
      std::pair <double,double> frac (0.,0.);
      // if there is a channel corresponding to this new point, put it in the list
      if ( pointToChannel( glob_neigh_point, neig_channel, frac).isSuccess()){
        channels.push_back(neig_channel);
      }
    }
  }
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
        && (fabs(point.z()-pointRef.z())<6*siliconThickness()/10)
        ){
      return index;
    }
  }
  return -1;
}

//==============================================================================
/// Get the index relative to ladder of the chip containing the point
//==============================================================================
int DeVeloPixSquareType::WhichChip(const Gaudi::XYZPoint& point, int ladderIndex) const
{  
  Gaudi::XYZPoint refPoint = m_ladders[ladderIndex].ReferencePoint();
  Gaudi::XYZPoint LocalPoint(point.x()-refPoint.x()-interChipDist()/2,point.y()-refPoint.y(),point.z()-refPoint.z());
  int ChipNum = (int)(LocalPoint.x()/(chipLength()+interChipDist()));
  return ChipNum;

}

//==============================================================================
/// Get the index relative to the chip of the pixel containing the point 
//==============================================================================
std::pair<int,int> DeVeloPixSquareType::WhichPixel(const Gaudi::XYZPoint& point, int ladderIndex, int chipIndex, std::pair<double,double>& fraction) const
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
  else if (newx < - (interchipPixSizeLEFT-lpSize()) && newx >((nPixCol()-1)*lpSize())+interchipPixSizeRIGHT ){
    thePixel.first = -1;
    fraction.first = -1; 
  }
  thePixel.second=(int)(LocalPoint.y()/hpSize());  
  fraction.second=LocalPoint.y()/hpSize()-thePixel.second;  
  return thePixel;
}

std::pair<double,double> DeVeloPixSquareType::PixelSize(LHCb::VeloPixChannelID channel) const
{
  int ladderIndex = -1;
  int ladderNum = ladderNumber();
  int chipNum = channel.chip();
  int ntotChip = 0;
  for(int ilad = 0 ; ilad < ladderNum ; ilad ++){
    ntotChip += m_ladders[ilad].nChip();
    if ( chipNum < ntotChip ) {
      ladderIndex = ilad;
      continue;
    }
  }
  std::pair<double,double> size (-1.,-1.);
  if (ladderIndex == -1)return size;
  size.first = lpSize();
  size.second = hpSize();
  int positionEdgePix =  (m_ladders[ladderIndex]).edgeOrientation(chipNum);
  // case where the edge pixel is on the right and the channel correspond to a edge pixel
  if ( ( positionEdgePix== 1 || positionEdgePix== 0 ) && channel.pixel_lp() == (unsigned int)(nPixCol()-1)){
    size.first = interchipPixSize();
  }
  if ( ( positionEdgePix== -1 || positionEdgePix== 0) && channel.pixel_lp() == 0 ){
    size.first = interchipPixSize();
  }
  return size;
  
}
