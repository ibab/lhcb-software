// $Id: DeVeloPixSquareType.cpp,v 1.14 2010-04-13 10:09:43 cocov Exp $
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
#include "Kernel/LineTraj.h"
#include "gsl/gsl_math.h"

// From VeloPix
#include "VeloPixDet/DeVeloPixSquareType.h"
#include "Kernel/VeloPixChannelID.h"


// Note for later: If by design or to assume possible non orthogonal position of the sensor plane wrt. 
// the halfbox itself, the way global and halfbox position is stored should be review...

namespace VeloPixDet {
  /** This function simply provides access to a local static
   *  data which is used to initialize references in each instance
   *  of DeVeloPixSquareType.
   *  The purpose of this function is to work around
   *  a Microsoft(tm) specific extension in VC++ that makes
   *  awkward to have static data mebers accessed by inline
   *  funtions.
   *
   *  @see DeVeloPixSquareType
   */
  //double& m_PixelsSize;
  static double& deVeloPixSquareTypeStaticPixelSize()
  {
    static double s_PixelSize;
    return s_PixelSize;
  }
  /** This function simply provides access to a local static
   *  data which is used to initialize references in each instance
   *  of DeVeloPixSquareType.
   *  The purpose of this function is to work around
   *  a Microsoft(tm) specific extension in VC++ that makes
   *  awkward to have static data mebers accessed by inline
   *  funtions.
   *
   *  @see DeVeloPixSquareType
   */
  //int& m_NpixX ;
  static int& deVeloPixSquareTypeStaticNpixX()
  {
    static int s_NpixX;
    return s_NpixX;
  }
  /** This function simply provides access to a local static
   *  data which is used to initialize references in each instance
   *  of DeVeloPixSquareType.
   *  The purpose of this function is to work around
   *  a Microsoft(tm) specific extension in VC++ that makes
   *  awkward to have static data mebers accessed by inline
   *  funtions.
   *
   *  @see DeVeloPixSquareType
   */
  //double& m_ChipWidth ;
  static double& deVeloPixSquareTypeStaticChipWidth()
  {
    static double s_ChipWidth;
    return s_ChipWidth;
  }

  /** This function simply provides access to a local static
   *  data which is used to initialize references in each instance
   *  of DeVeloPixSquareType.
   *  The purpose of this function is to work around
   *  a Microsoft(tm) specific extension in VC++ that makes
   *  awkward to have static data mebers accessed by inline
   *  funtions.
   *
   *  @see DeVeloPixSquareType
   */
  // std::vector< bool >& m_ChipHorizontal;
  static std::vector< bool >& deVeloPixSquareTypeStaticChipHorizontal()
  {
    static std::vector< bool > s_ChipHorizontal;
    return s_ChipHorizontal;
  }
  // 64 size at maximum
  /** This function simply provides access to a local static
   *  data which is used to initialize references in each instance
   *  of DeVeloPixSquareType.
   *  The purpose of this function is to work around
   *  a Microsoft(tm) specific extension in VC++ that makes
   *  awkward to have static data mebers accessed by inline
   *  funtions.
   *
   *  @see DeVeloPixSquareType
   */
  //std::vector< double >& m_ChipFirstPixelSize;  
  static std::vector< double >& deVeloPixSquareTypeStaticChipFirstPixelSize()
  {
    static std::vector< double > s_ChipFirstPixelSize;
    return s_ChipFirstPixelSize;
  }
  /** This function simply provides access to a local static
   *  data which is used to initialize references in each instance
   *  of DeVeloPixSquareType.
   *  The purpose of this function is to work around
   *  a Microsoft(tm) specific extension in VC++ that makes
   *  awkward to have static data mebers accessed by inline
   *  funtions.
   *
   *  @see DeVeloPixSquareType
   */
  //std::vector< double >& m_ChipLastPixelSize;  
  static std::vector< double >& deVeloPixSquareTypeStaticChipLastPixelSize()
  {
    static std::vector< double > s_ChipLastPixelSize;
    return s_ChipLastPixelSize;
  }
  /** This function simply provides access to a local static
   *  data which is used to initialize references in each instance
   *  of DeVeloPixSquareType.
   *  The purpose of this function is to work around
   *  a Microsoft(tm) specific extension in VC++ that makes
   *  awkward to have static data mebers accessed by inline
   *  funtions.
   *
   *  @see DeVeloPixSquareType
   */
  //std::vector< int >& m_ChipsInLadder;
  static std::vector< int >& deVeloPixSquareTypeStaticChipsInLadder()
  {
    static std::vector< int > s_ChipsInLadder;
    return s_ChipsInLadder;
  }
  /** This function simply provides access to a local static
   *  data which is used to initialize references in each instance
   *  of DeVeloPixSquareType.
   *  The purpose of this function is to work around
   *  a Microsoft(tm) specific extension in VC++ that makes
   *  awkward to have static data mebers accessed by inline
   *  funtions.
   *
   *  @see DeVeloPixSquareType
   */
  //std::vector< std::pair<double,double> >& m_xyChips;
  static std::vector< std::pair<double,double> >& deVeloPixSquareTypeStaticChips()
  {
    static std::vector< std::pair<double,double> > s_xyChips;
    return s_xyChips;
  }
}
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
  m_otherSideSensor(0),
  m_PixelSize(VeloPixDet::deVeloPixSquareTypeStaticPixelSize()), 
  m_NpixX(VeloPixDet::deVeloPixSquareTypeStaticNpixX()), 
  m_ChipWidth(VeloPixDet::deVeloPixSquareTypeStaticChipWidth()), 
  m_xyChips(VeloPixDet::deVeloPixSquareTypeStaticChips()),
  m_ChipFirstPixelSize(VeloPixDet::deVeloPixSquareTypeStaticChipFirstPixelSize()), 
  m_ChipLastPixelSize(VeloPixDet::deVeloPixSquareTypeStaticChipLastPixelSize()), 
  m_ChipsInLadder(VeloPixDet::deVeloPixSquareTypeStaticChipsInLadder()),
  m_ChipHorizontal(VeloPixDet::deVeloPixSquareTypeStaticChipHorizontal()),
  m_msgStream(NULL)
{
}
//==============================================================================
/// Destructor
//==============================================================================
DeVeloPixSquareType::~DeVeloPixSquareType()
{
  delete m_msgStream;
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

  sc = DeVeloPixSensor::initialize();
  if(!sc.isSuccess()) {
    msg() << MSG::ERROR << "Failed to initialise DeVeloPixSensor" << endreq;
    return sc;
  }

  m_debug   = (msgSvc()->outputLevel("DeVeloPixSquareType") == MSG::DEBUG  ) ;
  m_verbose = (msgSvc()->outputLevel("DeVeloPixSquareType") == MSG::VERBOSE) ;
  int numOfChips = 0;
  // Fill the ladder vector with parameters from the DDDB
  for (int nl = 0 ; nl < ladderNumber() ; nl ++)
  {
    std::stringstream s;
    s << nl+1;
    //bottom left corner of the active part of ladder nl
    Gaudi::XYZPoint point(param<double>("XRef"+s.str()),param<double>("YRef"+s.str()),param<double>("ZRef"+s.str()));
    bool ishorizontal_tmp = false;
    if(param<int>("Ladder"+s.str()+"_IsHorizontal")>0.5) ishorizontal_tmp = true;
    PixelLadder tmp_ladder(point,param<int>("NChip"+s.str()),ishorizontal_tmp);
    std::vector<int> tmp_interPixPos (tmp_ladder.nChip());
    numOfChips+=tmp_ladder.nChip();
    for ( int nchip = 0 ; nchip < tmp_ladder.nChip() ; nchip++)
    {
      std::stringstream sch;
      sch << nchip+1;
      tmp_interPixPos[nchip]=param<int>("PosInterChip"+sch.str()+"_Ladd"+s.str());    
    }
    tmp_ladder.setEdgesOrientation(tmp_interPixPos);
    m_ladders.push_back(tmp_ladder);
  }
  /// resize the vector to match the real number of pixels
  //m_globalXY.resize(numOfChips*nPixCol()*nPixRow());
  //m_halfboxXY.resize(numOfChips*nPixCol()*nPixRow());

  /// Calculate the pixels position and size
  sc = calcPixelsParam();
  if(!sc.isSuccess()) {
    msg() << MSG::ERROR << "Failed to store the pixel position in static" << endreq;
    return sc;
  }

  // get some bits and mask field for some bit manipulation...
  // fill a channelID 
//   LHCb::VeloPixChannelID refid ((int)(pow(2,32)-1));

//   m_ChipBit = (unsigned int)(log(refid.chip()+1.)/log(2.));
//   m_PixelHPBit = (unsigned int)(log(refid.pixel_hp()+1.)/log(2.));
//   m_numberOfPixelsPerChipBits = (refid.pixel_lp()+1)*(refid.pixel_hp()+1);
//   m_numberOfChipsBits = refid.chip()+1;
//   unsigned ich_tmpnt zeroMask = 0x00000000 ;
//   m_PixelBit = m_PixelHPBit+m_PixelLPBit;

  LHCb::VeloPixChannelID ch_tmp(0);
  m_PixelLPBit = ch_tmp.GetPixellpBits();
  m_PixelHPBit = ch_tmp.GetPixelhpBits();
  m_PixelBit =  ch_tmp.GetPixelBits();
  
  m_ChipMask = ch_tmp.GetChipMask();
  m_PixelHPMask = ch_tmp.GetPixelhpMask();
  m_PixelLPMask = ch_tmp.GetPixellpMask();
  m_PixelMask =  ch_tmp.GetPixelMask();
    
  // geometry conditions, update global position in cache
  /*updMgrSvc()->
    registerCondition(this,this->m_geometry,&DeVeloPixSquareType::updateGeometryCache);
   first update
  sc = updMgrSvc()->update(this);
  if(!sc.isSuccess()) {
    msg() << MSG::ERROR << "Failed to update geometry cache." << endreq;
    return sc;
  }*/

  
  return StatusCode::SUCCESS; 
}


//==============================================================================
// Return a trajectory (for track fit) from strip + offset
//==============================================================================
std::auto_ptr<LHCb::Trajectory> DeVeloPixSquareType::trajectory(const LHCb::VeloPixChannelID& id,
                                                        const std::pair<double,double> offset) const {
  // offset is offset on R
  Gaudi::XYZPoint globalP;
  channelToPointWithFraction(id.pixel(),offset,globalP);
  Gaudi::XYZPoint globalPOff (globalP.x()+offset.first,globalP.y()+offset.second,globalP.z());
  // put into trajectory
  LHCb::Trajectory* tTraj = new LHCb::LineTraj(globalPOff ,globalPOff );

  std::auto_ptr<LHCb::Trajectory> autoTraj(tTraj);

  return autoTraj;

}



//==============================================================================
/// Calculate the nearest channel to a 3-d point 
/// (assuming the DeVeloPixSquareType instance is already the correct one: z is corresponding to the sensor number)
//==============================================================================
StatusCode DeVeloPixSquareType::pointToChannel(const Gaudi::XYZPoint& point,
                                               LHCb::VeloPixChannelID& channel,
                                               std::pair <double, double>& fraction) const
{
  //MsgStream msg(msgSvc(), "DeVeloPixSquareType");
  
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
    msg()<<MSG::INFO<<"Which Pixelfunction does not find the pixels..."<<endreq;
    return StatusCode::FAILURE;
  }
  // Set the pixel position in the VeloPixChannelID
  channel.setPixel_lp(pixelPos.first);
  channel.setPixel_hp(pixelPos.second);
  return StatusCode::SUCCESS;
}

//==============================================================================
/// Calculate the position of a fraction in the given channel in global coordinates
//==============================================================================
StatusCode DeVeloPixSquareType::channelToPointWithFraction( const LHCb::VeloPixChannelID& channel,
                                                        const std::pair<double,double> offset,
                                                        Gaudi::XYZPoint& point) const
{
  MsgStream msg(msgSvc(), "DeVeloPixSquareType");
  Gaudi::XYZPoint LocalPoint(0.,0.,0.);
  int ladderIndex = WhichLadder(channel.chip());
  std::pair<double,double> point2d = xyOfPixel(channel.pixel(),offset);
  LocalPoint.SetX(point2d.first);
  LocalPoint.SetY(point2d.second);
  LocalPoint.SetZ(m_ladders[ladderIndex].ReferencePoint().z());
  point = localToGlobal(LocalPoint);
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
  int ladderIndex = WhichLadder(channel.chip());
  std::pair<double,double> point2d = xyOfPixel(channel.pixel());
  LocalPoint.SetX(point2d.first);
  LocalPoint.SetY(point2d.second);
  LocalPoint.SetZ(m_ladders[ladderIndex].ReferencePoint().z());
  point = localToGlobal(LocalPoint);
  return StatusCode::SUCCESS;
}

//==============================================================================
/// Calculate the nearest 3x3 list of channel to a 3-d point 
/// If it needs to be used online maybe it might first be rethinked in order not
/// to use 8 time pointToChannel function (search directly by channelID)
//==============================================================================
StatusCode  DeVeloPixSquareType::pointTo3x3Channels(const Gaudi::XYZPoint& point,
                                                    std::vector <LHCb::VeloPixChannelID>& channels) const
{
  //MsgStream msg(msgSvc(), "DeVeloPixSquareType");
  // Get the channel corresponding to the central point 
  LHCb::VeloPixChannelID  channelCentral;
  std::pair <double, double> fraction;
  StatusCode sc = pointToChannel( point, channelCentral, fraction);
  Gaudi::XYZPoint loc_point = globalToLocal(point);
  // Initialise the size and if there is a channel at the central point get its size
  std::pair <double, double> size (0.,0.);
  if( sc.isSuccess()) size = PixelSize(channelCentral);


  // Loop over the possible position in the 3x3 cluster
  int pixY = (channelCentral).pixel_hp();
  int pixX = (channelCentral).pixel_lp();
  int chip = (channelCentral).chip();
  int ladderIndex = WhichLadder(loc_point);
  int NchipInLad =m_ladders[ladderIndex].nChip();
      
  int minY(-1),maxY(2);
  int minX(-1),maxX(2);
  int chipoffset (0);
  // if it is the central position and it was corresponding to a channel, put it in the list
  if( sc.isSuccess() ) {
    

    channels.push_back(channelCentral);
    //bool diff = false;
    if (pixX == 0 || pixX == nPixCol()-1 || pixY==0 || pixY==nPixRow()-1){
      //diff = true;
      // if pixel is fisrt row, start loop at 0
      if (pixY == 0 )minY=0;
      // if pixel is last row, end loop at 0
      if (pixY == nPixRow()-1 )maxY=1;
      // if pixel is first col and this is first chip of ladder, start loop at 0
      if (pixX == 0 && chip%NchipInLad == 0)minX=0;
      // if pixel is last col and this is lasr chip of ladder, end loop at 0
      if (pixX == nPixRow()-1 && chip%NchipInLad == NchipInLad-1)maxX=1;
      // if pixel is first col and this is not first chip of ladder, start loop at 0
      if (pixX == 0 && chip%NchipInLad != 0)chipoffset = -1;
      // if pixel is last col and this is lasr chip of ladder, end loop at 0
      if (pixX == nPixRow()-1 && chip%NchipInLad != NchipInLad-1)chipoffset = 1;
    }
    
    //if (diff)msg() << MSG::INFO<<"Central Channel "<<(channelCentral).sensor()<<" "<<(channelCentral).chip()<<" "<<(channelCentral).pixel_hp()<<" "<<(channelCentral).pixel_lp()<<" "<<size<<" x,y,zloc "<<loc_point<<endreq;
    for (int x = minX ; x < maxX ; x ++){
      for (int y = minY ; y < maxY ; y ++){
	LHCb::VeloPixChannelID neig_channel(channelCentral);
	if (x == 0 && y == 0)continue;
	neig_channel.setChip(neig_channel.chip()+chipoffset);
	neig_channel.setPixel_lp((neig_channel.pixel_lp()+x)%256);
	neig_channel.setPixel_hp(neig_channel.pixel_hp()+y);
	channels.push_back(neig_channel);
	//if (diff) msg() << MSG::INFO<<"       x,y "<<x<<" "<<y<<" minX,Y "<<minX<<" "<<minY<<" maxX,Y"<<maxX<<" "<<maxY<<" "<<(neig_channel).sensor()<<" "<<(neig_channel).chip()<<" "<<(neig_channel).pixel_hp()<<" "<<(neig_channel).pixel_lp()<<endreq;
      }
    }
  }
  else if(  !sc.isSuccess() ) {
    //msg() << MSG::INFO<<"Central Channel does not exist.... "<<" x,y,zloc "<<loc_point<<endreq;
    std::vector< std::pair< int,int > > thePairPix;
    for (int x = -1 ; x < 2 ; x ++){
      for (int y = -1 ; y < 2 ; y ++){
	Gaudi::XYZPoint tmpPoint(point.x()+x*hpSize(),point.y()+y*hpSize(),point.z());
	LHCb::VeloPixChannelID  channeltmp;
	std::pair <double, double> fractiontmp;
	StatusCode sc = pointToChannel( tmpPoint, channeltmp, fractiontmp);
	if (sc.isSuccess()){
	  bool used = false;
	  // prevent reuse of long pixel (since here the space is checked in small pixel size)
	  for (std::vector <LHCb::VeloPixChannelID>::iterator ich = channels.begin();channels.end()!=ich;ich++){
	    if (channeltmp.channelID()==(*ich).channelID()){
	      used=true;
	      continue;
	    }
	  }
	  if(!used)channels.push_back(channeltmp);
	}
      }
    }
  }

  return StatusCode::SUCCESS;
}


//==============================================================================
/// Get the 8 channel (if they exist) arround a given seed channel
/// If one want to use it online, same remark than for pointTo3x3Channels
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
  int pixY = (seedChannel).pixel_hp();
  int pixX = (seedChannel).pixel_lp();
  int chip = (seedChannel).chip();
  int ladderIndex = WhichLadder(loc_point);
  int NchipInLad =m_ladders[ladderIndex].nChip();
  
  int minY(-1),maxY(2);
  int minX(-1),maxX(2);
  int chipoffset (0);
  if (pixX == 0 || pixX == nPixCol()-1 || pixY==0 || pixY==nPixRow()-1){
    // if pixel is fisrt row, start loop at 0
    if (pixY == 0 )minY=0;
    // if pixel is last row, end loop at 0
    else if (pixY == nPixRow()-1 )maxY=1;
    // if pixel is first col and this is first chip of ladder, start loop at 0
    if (pixX == 0 && chip%NchipInLad == 0)minX=0;
    // if pixel is last col and this is lasr chip of ladder, end loop at 0
    else if (pixX == nPixRow()-1 && chip%NchipInLad == NchipInLad-1)maxX=1;
    // if pixel is first col and this is not first chip of ladder, start loop at 0
    else if (pixX == 0 && chip%NchipInLad != 0)chipoffset = -1;
    // if pixel is last col and this is lasr chip of ladder, end loop at 0
    else if (pixX == nPixRow()-1 && chip%NchipInLad == NchipInLad-1)chipoffset = 1;
  }
  for (int x = minX ; x < maxX ; x ++){
    for (int y = minY ; y < maxY ; y ++){
      LHCb::VeloPixChannelID neig_channel(seedChannel);
      if (x == 0 && y == 0)continue;
      neig_channel.setChip(neig_channel.chip()+chipoffset);
      neig_channel.setPixel_lp((neig_channel.pixel_lp()+x)%256);
      neig_channel.setPixel_hp(neig_channel.pixel_hp()+y);
      channels.push_back(neig_channel);
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
        ( 
         m_ladders[index].isHorizontal() && (
                                             (pointRef.x()-point.x() > 0. 
                                              && pointRef.x()-point.x()
                                              <(m_ladders[index].nChip()*(chipLength()+interChipDist())-interChipDist()))
                                             &&
                                             (point.y()-pointRef.y() > 0. && point.y()-pointRef.y()<chipWidth())
                                             &&
                                             (fabs(point.z()-pointRef.z())<6*siliconThickness()/10)
                                             )
         ) || (
               ! m_ladders[index].isHorizontal() && (
                                                     (point.y()-pointRef.y() > 0. 
                                                      && point.y()-pointRef.y()
                                                      <(m_ladders[index].nChip()*(chipLength()+interChipDist())-interChipDist()))
                                                     &&
                                                     (pointRef.x()-point.x() > 0. && pointRef.x()-point.x() <chipWidth())
                                                     &&
                                                     (fabs(point.z()-pointRef.z())<6*siliconThickness()/10)
                                                     )
               ) 
        )
    {
      return index;
    }
  }
  return -1;
}

//==============================================================================
/// Get the index of the ladder containing the channelID chip number
//==============================================================================
int DeVeloPixSquareType::WhichLadder(int chipNum) const
{
  int ladderNum = ladderNumber();
  int ntotChip = 0;
  for(int ilad = 0 ; ilad < ladderNum ; ilad ++){
    int ntotChip_tmp = ntotChip;
    ntotChip += m_ladders[ilad].nChip();
    if ( chipNum < ntotChip && ( chipNum > ntotChip_tmp ||  chipNum == ntotChip_tmp ) ) return ilad;
  }
  return -1;
}

//==============================================================================
/// Get the index relative to ladder of the chip containing the point
//==============================================================================
int DeVeloPixSquareType::WhichChip(const Gaudi::XYZPoint& point, int ladderIndex) const
{  
  // Get the ladder reference point (ie x,y,z of the bottom left corner)
  Gaudi::XYZPoint refPoint = m_ladders[ladderIndex].ReferencePoint();
  
  //Local position in the ladder contains +interChipDist()/2 because we assume the 
  // pixel at the reference point is a normal one... if PosInterChip1_Laddn!=1 then this might change...
  double extraDist = interChipDist()/2;
  if ( m_ladders[ladderIndex].edgeOrientation(0)!=1){
    msg() << MSG::ERROR << "In the calculation of the pixel position, the case where first chip have an external readout edge is not forseen..." << endreq;
    return -1;
  }
  // compute the local position of the point with respect to the reference point, along the chip alignment direction.
  double LocalPosition = -100.;
  if (m_ladders[ladderIndex].isHorizontal()) {
    LocalPosition = refPoint.x()-point.x()+extraDist;
  }
  else{
    LocalPosition = point.y()-refPoint.y()+extraDist;
  }
  // get the chip number
  int ChipNum = (int)(LocalPosition/(chipLength()+interChipDist()));
  return ChipNum;
}

//==============================================================================
/// Get the index relative to the chip of the pixel containing the point 
//==============================================================================
std::pair<int,int> DeVeloPixSquareType::WhichPixel(const Gaudi::XYZPoint& point, int ladderIndex, int chipIndex, std::pair<double,double>& fraction) const
{
  //step 1 local coordinates
  Gaudi::XYZPoint refPoint = m_ladders[ladderIndex].ReferencePoint();
  Gaudi::XYZPoint Point(point.x(),point.y(),point.z());
  
  //step 2 ladders coordinates
  Gaudi::XYZPoint LocalPoint_Ladd(refPoint.x()-Point.x(),Point.y()-refPoint.y(),Point.z()-refPoint.z());

  
  //step 3 horizontal ladder coordinates
  if (! m_ladders[ladderIndex].isHorizontal()) {
    double refx = LocalPoint_Ladd.y();
    double refy = chipWidth() - LocalPoint_Ladd.x();
    LocalPoint_Ladd.SetX(refx);
    LocalPoint_Ladd.SetY(refy);
  }

  // step 4 go to the chip coordinates
  double alongAxisOffset = 0.;
  if( chipIndex>0) alongAxisOffset = chipIndex*(chipLength()+interChipDist()/2)+(chipIndex-1)*interChipDist()/2;

  // Local Point is the point in the chip refernetial (ie, x<chipLenght and y<chipWidth)
  Gaudi::XYZPoint LocalPoint(LocalPoint_Ladd.x()-alongAxisOffset,LocalPoint_Ladd.y(),LocalPoint_Ladd.z());

  int positionEdgePix =  (m_ladders[ladderIndex]).edgeOrientation(chipIndex);
  std::pair< int , int > thePixel;

  // Set the size of the edges (left/right) pixels
  double interchipPixSizeLEFT = lpSize();
  double interchipPixSizeRIGHT = lpSize();
  if ( positionEdgePix== 1 || positionEdgePix== 0 )interchipPixSizeRIGHT = interchipPixSize();
  if ( positionEdgePix== -1 || positionEdgePix== 0 )interchipPixSizeLEFT = interchipPixSize();

  // BECAREFULL x is now not anymore X but the local ladder x ie. the dimension along which the chips are aligned
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
  else if (newx <((nPixCol()-1)*lpSize())+interchipPixSizeRIGHT && newx >((nPixCol()-1)*lpSize())){
    thePixel.first=(nPixCol()-1);
    fraction.first = (newx-(nPixCol()-1)*lpSize())/interchipPixSizeRIGHT;
  }
  else if (newx < - (interchipPixSizeLEFT-lpSize()) && newx >((nPixCol()-1)*lpSize())+interchipPixSizeRIGHT ){
    thePixel.first = -1;
    fraction.first = -1; 
  }
  thePixel.second=(int)(LocalPoint.y()/hpSize());  
  fraction.second=LocalPoint.y()/hpSize()-thePixel.second;  
  // fraction should be given in global X,Y
  if (! m_ladders[ladderIndex].isHorizontal()) {
    double fracX = 1-fraction.second;
    double fracY = fraction.first;
    fraction.first = fracX;
    fraction.second = fracY;
  }
  return thePixel;
}

//==============================================================================
/// The function that retruns the size of a pixel from channelID                 
//==============================================================================
std::pair<double,double> DeVeloPixSquareType::PixelSize(LHCb::VeloPixChannelID channel) const
{
  return PixelSize(channel.pixel());
}
//==============================================================================
/// The function that retruns true if channel is a long pixel              
//==============================================================================
bool DeVeloPixSquareType::isLong( LHCb::VeloPixChannelID channel ) const
{
  return isLong(channel.pixel());
}

//==============================================================================
/// The function that fill the members in intialisation                  
//==============================================================================
StatusCode DeVeloPixSquareType::calcPixelsParam()
{
  // we only have to do this once, x and y position of pixel in chips 
  // are stored in statics, i.e. are technically the same
  // for all instances of DeVeloRType
  if (m_staticDataInvalid) {
    m_ChipFirstPixelSize.clear();
    m_xyChips.clear();
    m_ChipsInLadder.clear();
    m_ChipHorizontal.clear();
    m_ChipLastPixelSize.clear();
    int ladderNum = ladderNumber();
    int ntotPixel = 0;
    // Filling a big table with all the pixel position in one sensor is not possible (more than 20M per table)
    // so we cut the data into :
    //  - m_PixelSize | standard pixel size
    //  - m_NpixX     | number of chips along the x axis
    //  - m_ChipWidth | chip width needed to transform x to y
    //  - m_xyChips | bottom left X,Y position of 1st pixel in chip (size NChip*std::pair<double,double>)
    //  - m_xyPixels | x,y position of each pixel in chip assuming all pix similar (size NPixX*NPixY*std::pair<double,double>) (maybe not needed)
    //  - m_ChipFirstPixelSize | x size of first pixel (vector of double) size NChip*double
    //  - m_ChipLastPixelSize | x size of last pixel (vector of double) size NChip*double
    //  - m_ChipsInLadder  | ladder of each chip (since it contains the Z relative position wrt. the sensor z=0) size  NChip*int
    //  - m_ChipHorizontal | chip orientation (for the moment Horizontal or vertial --> vector of bool) size  NChip*bool
    // Note that X,Y are in the sensor coordinates while  x,y are in the ladder coordinate
    m_PixelSize = lpSize();
    m_NpixX = nPixCol();
    m_ChipWidth = chipWidth();

    for(int ilad = 0 ; ilad < ladderNum ; ilad ++){
      int chipNum = m_ladders[ilad].nChip();
      ntotPixel += chipNum*nPixCol()*nPixRow();
      for(int ichip = 0 ; ichip < chipNum ; ichip ++){
        m_ChipsInLadder.push_back(ilad);
        bool isHorizontal = m_ladders[ilad].isHorizontal();
        m_ChipHorizontal.push_back(isHorizontal);
        
        //Get the position of the chip by setting the position of the ladder reference point...
        std::pair<double,double> xyChip(m_ladders[ilad].ReferencePoint().x(),m_ladders[ilad].ReferencePoint().y());

        // ... and adding the bottom left position of the chip in the ladder
        if (ichip < 2){
          // If there is no more than 2 chips in the ladder
          if ( isHorizontal ) xyChip.first=xyChip.first-((ichip)*(chipLength()+interChipDist()/2));
           else xyChip.second=xyChip.second+(ichip)*(chipLength()+interChipDist()/2);
        }   
        else{ 
          //The else is not used for TIMEPIX minplane so it have to
          //be checked for other geometry (when more than 2 chip by ladder)
          if ( isHorizontal ) xyChip.first=xyChip.first-((ichip-1)*(chipLength()+interChipDist())+(chipLength()+interChipDist()/2));
          else xyChip.second=xyChip.second+(ichip-1)*(chipLength()+interChipDist())+(chipLength()+interChipDist()/2);
        }  
        m_xyChips.push_back(xyChip);
        

        if (m_ladders[ilad].edgeOrientation(ichip)== 0 || m_ladders[ilad].edgeOrientation(ichip)== -1){
          m_ChipFirstPixelSize.push_back(interchipPixSize());
        }
        if (m_ladders[ilad].edgeOrientation(ichip)== 1 ){
          m_ChipFirstPixelSize.push_back(lpSize());
        }

        if ( m_ladders[ilad].edgeOrientation(ichip)== -1){
          m_ChipLastPixelSize.push_back(lpSize());
        }
        if (m_ladders[ilad].edgeOrientation(ichip)== 0 || m_ladders[ilad].edgeOrientation(ichip)== 1 ){
          m_ChipLastPixelSize.push_back(interchipPixSize());
        }
      }
    }
    m_staticDataInvalid = false;  // these are valid now for all instances
  } 
  //else { // statics are valid, initialize base class member only
  //Should put any initialisation of member class that are not common to all sensors
  //}
  
  if(m_debug) {
    msg() << MSG::DEBUG << "Position of first chip is " << m_xyChips[0]
          << " last chip is " << m_xyChips[m_xyChips.size()-1] << endmsg;
  }
  
  return StatusCode::SUCCESS;
}


//==============================================================================
/// Update the Cache Geometry                
//==============================================================================

StatusCode DeVeloPixSquareType::updateGeometryCache()
{
  StatusCode sc = updatePixelSquareCache();
  if(!sc.isSuccess()) {
    msg() << MSG::ERROR << "Failed to update pixel square cache." << endreq;
    return sc;
  }
  return StatusCode::SUCCESS;
}

//==============================================================================
/// Update the Cache for Pixel position in the different frame             
//==============================================================================

StatusCode DeVeloPixSquareType::updatePixelSquareCache()
{
  // that's the place where, if we find a good way to store the global and halfbox info it shoulmd be done
  return StatusCode::SUCCESS;
}


