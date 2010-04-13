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
  static std::vector< std::pair<double,double> >& deVeloPixSquareTypeStaticPixels()
  {
    static std::vector< std::pair<double,double> > s_xyPixels;
    return s_xyPixels;
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
  static std::vector< std::pair<double,double> >& deVeloPixSquareTypeStaticPixelsSize()
  {
    static std::vector< std::pair<double,double> > s_xyPixelsSize;
    return s_xyPixelsSize;
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
  static std::vector< std::pair<double,double> >& deVeloPixSquareTypeStaticChips()
  {
    static std::vector< std::pair<double,double> > s_xyChips;
    return s_xyChips;
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
  static std::vector< bool >& deVeloPixSquareTypeStaticChipSameOrientation()
  {
    static std::vector< bool > s_xyChipSameOrientation;
    return s_xyChipSameOrientation;
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
  static std::vector< int >& deVeloPixSquareTypeStaticChipsLadder()
  {
    static std::vector< int > s_xyChipsLadder;
    return s_xyChipsLadder;
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
  m_xyPixels(VeloPixDet::deVeloPixSquareTypeStaticPixels()),
  m_PixelsSize(VeloPixDet::deVeloPixSquareTypeStaticPixelsSize()), 
  m_xyChips(VeloPixDet::deVeloPixSquareTypeStaticChips()),
  m_ChipSameOrientation(VeloPixDet::deVeloPixSquareTypeStaticChipSameOrientation()),
  m_ChipsInLadder(VeloPixDet::deVeloPixSquareTypeStaticChipsLadder()),
  //m_globalXY(m_numberOfPixels,std::pair<double,double> (0.,0.)),
  //m_halfboxXY(m_numberOfPixels,std::pair<double,double> (0.,0.)),
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
  msg() << MSG::INFO<<"USE THE CORRECT"<<endreq;
  
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
    PixelLadder tmp_ladder(point,param<int>("NChip"+s.str()));
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
  calcPixelsParam();

  // get some bits and mask field for some bit manipulation...
  // fill a channelID 
  LHCb::VeloPixChannelID refid ((int)(pow(2,32)-1));

  m_ChipBit = (unsigned int)(log(refid.chip()+1.)/log(2.));
  m_PixelLPBit = (unsigned int)(log(refid.pixel_lp()+1.)/log(2.));
  m_PixelHPBit = (unsigned int)(log(refid.pixel_hp()+1.)/log(2.));
  m_numberOfPixelsPerChipBits = (refid.pixel_lp()+1)*(refid.pixel_hp()+1);
  m_numberOfChipsBits = refid.chip()+1;
  unsigned int zeroMask = 0x00000000 ;
  m_PixelBit = m_PixelHPBit+m_PixelLPBit;
  
  m_ChipMask = (refid.chip()  << m_PixelBit ) | zeroMask;
  m_PixelLPMask = refid.pixel_lp() | zeroMask ;
  m_PixelHPMask = (refid.pixel_hp()<< m_PixelLPBit  | zeroMask);
  m_PixelMask = (refid.pixel_hp()<< m_PixelLPBit  | zeroMask)| m_PixelLPMask;
    
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
      // if there is a channel corresponding to this new point, put it in the list... here we might find a faster impemntation
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
    // that might be wrong if nChip>2... to be checked
    if (                                                     
        (point.x()-pointRef.x() > 0. && point.x()-pointRef.x()<(m_ladders[index].nChip()*(chipLength()+interChipDist())-interChipDist())) 
        && (point.y()-pointRef.y() > 0. && point.y()-pointRef.y()<chipWidth())
        && (fabs(point.z()-pointRef.z())<6*siliconThickness()/10)
        ){
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
  Gaudi::XYZPoint refPoint = m_ladders[ladderIndex].ReferencePoint();
  Gaudi::XYZPoint LocalPoint(point.x()-refPoint.x()+interChipDist()/2,point.y()-refPoint.y(),point.z()-refPoint.z());
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
void DeVeloPixSquareType::calcPixelsParam()
{
  // we only have to do this once, strip radii pitches and phi
  // limits are stored in statics, i.e. are technically the same
  // for all instances of DeVeloRType
  if (m_staticDataInvalid) {
    m_xyPixels.clear();
    m_PixelsSize.clear();
    m_xyChips.clear();
    m_ChipsInLadder.clear();
    int firstChipEdgePosition = m_ladders[0].edgeOrientation(0);
    int ladderNum = ladderNumber();
    int ntotPixel = 0;
    // Filling a big table with all the pixel position in one sensor is not possible (more than 20M per table)
    // so we cut the data into :
    //  - x,y position of pixels in first chip (vector of std::pair<double,double>)
    //  - x,y pixels size in first chip (vector of std::pair<double,double>)
    //  - x,y position of each chip (vector of std::pair<double,double>)
    //  - pixel positions orientation wrt. pixel position orientation in first chip (vector of bool)
    //  - ladder of each chip (since it contains the Z relative position wrt. the sensor z=0)
    for(int ilad = 0 ; ilad < ladderNum ; ilad ++){
      int chipNum = m_ladders[ilad].nChip();
      ntotPixel += chipNum*nPixCol()*nPixRow();
      for(int ichip = 0 ; ichip < chipNum ; ichip ++){
        m_ChipsInLadder.push_back(ilad);
        //Get the position of the chip by setting the position of the ladder reference point...
        std::pair<double,double> xyChip(m_ladders[ilad].ReferencePoint().x(),m_ladders[ilad].ReferencePoint().y());
        // ... and adding the bottom left position of the chip in the ladder
        if (ichip < 2){
          // If there is no more than 2 chips in the ladder
          xyChip.first=xyChip.first+(ichip)*(chipLength()+interChipDist()/2);
        }   
        else{ 
          //The else is not used for TIMEPIX minplane so it have to
          //be checked for other geometry (when more than 2 chip by ladder)
          xyChip.first=xyChip.first+(ichip-1)*(chipLength()+interChipDist())+(chipLength()+interChipDist()/2);
        }  
        m_xyChips.push_back(xyChip);
        // Now store the rotation to apply to the pixel coordinates:
        // if the chip edgeOrientation is 0 they we do the assumption that all the chips are similar: m_ChipSameOrientation is set to True
        // The first chip pixel positions are stored. 
        // If the edge orientation is opposite then m_ChipSameOrientation is set to False
        // If the edge orientation is the same then m_ChipSameOrientation is set to True
        int positionEdgePix =  (m_ladders[ilad]).edgeOrientation(ichip);
        bool sameOr;
        if ( positionEdgePix == firstChipEdgePosition )sameOr= true;
        else sameOr=false;
        if ( positionEdgePix == firstChipEdgePosition ) m_ChipSameOrientation.push_back(true);
        else m_ChipSameOrientation.push_back(false);
      }
    }
    //Now let's fill the informations on the pixels for first chip of first ladder
    for(unsigned int iX = 0 ; iX < (unsigned int)nPixCol() ; iX ++){
      for(unsigned int iY = 0 ; iY < (unsigned int)nPixRow() ; iY ++){
        std::pair<double,double> size (-1.,-1.);
        size.first = lpSize();
        size.second = hpSize();
        // case where the edge pixel is on the right and the channel correspond to a edge pixel
        if( ( ( firstChipEdgePosition== 1 || firstChipEdgePosition== 0 ) && iX == (unsigned int)(nPixCol()-1))
            || ( ( firstChipEdgePosition== -1 || firstChipEdgePosition== 0) && iX == 0 )){
          size.first = interchipPixSize();
        }
        std::pair<double,double> coord (0.,0.);
        double xoffset(0.);
        if (iX>0 && ( firstChipEdgePosition == 0 || firstChipEdgePosition == -1 ) ) xoffset = interchipPixSize()- lpSize();
        // Set the pixel position in the chip
        coord.first=(iX)*lpSize()+size.first/2+xoffset;
        coord.second=(iY)*hpSize()+size.second/2;
        m_xyPixels.push_back(coord);
        m_PixelsSize.push_back(size);
      }
    }
    m_staticDataInvalid = false;  // these are valid now for all instances
  } 
  //else { // statics are valid, initialize base class member only
  //Should put any initialisation of member class that are not common to all sensors
  //}
  
  if(m_debug) {
    msg() << MSG::DEBUG << "Position of first pixel is " << m_xyPixels[0]
          << " last pixel " << m_xyPixels[m_xyPixels.size()-1] << endmsg;
    msg() << MSG::DEBUG << "Size of first pixel is " << m_PixelsSize[0] << " last pixel "
          << m_PixelsSize[m_PixelsSize.size()-1] << endmsg;
  }
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
