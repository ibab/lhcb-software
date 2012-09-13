// $Id: DeVPSquareType.h,v 1.10 2010-04-13 10:09:43 cocov Exp $
#ifndef VPDET_DEVPSQUARETYPE_H 
#define VPDET_DEVPSQUARETYPE_H 1

// Include files

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Gaudi Math definitions
#include "GaudiKernel/Point3DTypes.h"

// from VPDet
#include "VPDet/DeVPSensor.h"
#include "VPDet/PixelLadder.h"

// get trajectory
#include "Kernel/Trajectory.h"


// Unique class identifier
static const CLID CLID_DeVPSquareType = 1008204 ;

namespace LHCb {
  class VPChannelID;
}

/** @class DeVPSquareType DeVPSquareType.h VPDet/DeVPSquareType.h
 *  
 *  Detector element for square (symetric) pixel type
 *  contains all the function specific to this type of pixel sensor
 *
 *  @author Victor Coco
 *  @date   2009-05-14
 */

class DeVPSquareType : public DeVPSensor {
public:
  
  typedef std::pair<double,double> pixelCoord;
  
  /// Standard constructor
  DeVPSquareType( const std::string& name = "" ); 

  virtual ~DeVPSquareType( ); ///< Destructor

  /// object identifier (static method)
  static  const CLID& classID() { return CLID_DeVPSquareType; }
  /// object identification
  virtual const CLID& clID()     const;

  /// Initialise the DeVPSensor from the XML
  virtual StatusCode initialize();


  /// Return a trajectory (for track fit) from pixel + fraction
  virtual std::auto_ptr<LHCb::Trajectory> trajectory(const LHCb::VPChannelID& id
                                                     , const std::pair<double,double> offset) const;

  //::::::::::::: Some classes to deal with channelID to real world

  /// Calculate the nearest channel to a 3-d point.
  /// Also returns the fractional x-y position IN the pixel
  virtual StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                                    LHCb::VPChannelID& channel,
                                    std::pair <double, double>& fraction) const;
  /// Calculate the XYZ center of a pixel
  virtual StatusCode channelToPoint( const LHCb::VPChannelID& channel,
                                                Gaudi::XYZPoint& point) const;

  virtual StatusCode channelToPointWithFraction( const LHCb::VPChannelID& channel,
                                                        const std::pair<double,double> offset,
                                                 Gaudi::XYZPoint& point) const;
  
  

  /// Get the list of VPChannelID forming the 3x3 cluster of pixel centered on point
  virtual StatusCode pointTo3x3Channels(const Gaudi::XYZPoint& point,
                                       std::vector <LHCb::VPChannelID>& channels) const;

  /// Get the 8 channel (if they exist) arround a given seed channel
  virtual StatusCode  channelToNeighbours( const LHCb::VPChannelID& seedChannel, 
                                       std::vector <LHCb::VPChannelID>& channels) const;
  
  /// Determines if local 3-d point is inside sensor
  virtual StatusCode isInActiveArea(const Gaudi::XYZPoint& point) const;

  /// Determines in which ladder is local 3-d point
  virtual int WhichLadder(const Gaudi::XYZPoint& point) const;
  virtual int WhichLadder(int chipNum) const;
  

  /// Determines in which chip of a given ladder is local 3-d point 
  virtual int WhichChip(const Gaudi::XYZPoint& point, int ladderIndex) const;
  
  /// Determines in which pixel of a given chip and a given ladder is local 3-d point
  virtual std::pair<int,int> WhichPixel(const Gaudi::XYZPoint& point, int ladderIndex,
                                        int chipIndex, std::pair <double, double>& fraction) const;

  virtual std::pair<double,double> PixelSize( LHCb::VPChannelID channel) const;

  virtual bool isLong( LHCb::VPChannelID channel ) const;

  //::::::::::::: Some classes to deal with the sensors
    
  /// Access to the sensor on the other side of the VP
  inline const DeVPSquareType* otherSideSensor() const { return m_otherSideSensor; }
  
  /// Set the sensor on the other side of the VP.  This should only be called by DeVP::initialize()
  inline void setOtherSideSensor(const DeVPSquareType* s) { m_otherSideSensor = s; }


  //::::::::::::: Some classes to deal with coordinates in the different frame

  /// Return the X,Y size of a pixel
  inline std::pair<double,double> PixelSize(unsigned long pixel)const
  {
    int chipNum =( pixel & m_ChipMask) >> m_PixelBit  ;
    int pixX = (pixel & m_PixelLPMask) >> m_PixelHPBit ;
    if ( m_ChipHorizontal[chipNum]){
      if ( pixX == 0 ){
        return std::make_pair(  m_ChipFirstPixelSize[chipNum] ,m_PixelSize   ); 

      }
      else if ( pixX < (m_NpixX-1) && pixX!=0 ){
        return std::make_pair( m_PixelSize ,m_PixelSize   );  
      }
      else {
        return std::make_pair( m_ChipLastPixelSize[chipNum] , m_PixelSize );  
      }
    }
    else {
      if ( pixX == 0 ){
        return std::make_pair( m_PixelSize ,  m_ChipFirstPixelSize[chipNum]   ); 

      }
      else if ( pixX < (m_NpixX-1) && pixX!=0 ){
        return std::make_pair( m_PixelSize ,m_PixelSize   );  
      }
      else {
        return std::make_pair( m_PixelSize , m_ChipLastPixelSize[chipNum] );  
      }
    }
  }

  /// Return the X,Y  position of the pixel
  inline pixelCoord xyOfPixel(const unsigned long pixel) const {
    int chipNum =( pixel & m_ChipMask) >> m_PixelBit  ;
    
    int pixX = (pixel & m_PixelLPMask) >> m_PixelHPBit ;
    int pixY =  pixel & m_PixelHPMask  ;
    if ( m_ChipHorizontal[chipNum]){
      if ( pixX == 0 ){
        return std::make_pair(  m_xyChips[chipNum].first -( m_ChipFirstPixelSize[chipNum]/2. ),
                                m_xyChips[chipNum].second + m_PixelSize/2. + pixY*m_PixelSize   );  
      }
      else if ( pixX < (m_NpixX-1) && pixX != 0 ){
        return std::make_pair(  m_xyChips[chipNum].first -( m_ChipFirstPixelSize[chipNum]+ (double)m_PixelSize/2. 
                                                            + (double)(pixX-1)*m_PixelSize),
                                m_xyChips[chipNum].second + m_PixelSize/2. + pixY*m_PixelSize   );  
      }
      else {
        return std::make_pair(  m_xyChips[chipNum].first -( m_ChipFirstPixelSize[chipNum] + (double)(pixX-1)*m_PixelSize
                                                            + m_ChipLastPixelSize[chipNum]/2.),
                                m_xyChips[chipNum].second + m_PixelSize/2. + pixY*m_PixelSize   );  
      }
    }
    else {
      if ( pixX == 0 ){
        return std::make_pair(  m_xyChips[chipNum].first - ( chipWidth() - ( m_PixelSize/2. + pixY*m_PixelSize ) ),
                                m_xyChips[chipNum].second + m_ChipFirstPixelSize[chipNum]/2.  );
      }
      else if ( pixX < ( m_NpixX - 1 ) && pixX != 0 ){
        return std::make_pair(  m_xyChips[chipNum].first - ( chipWidth()  - ( m_PixelSize/2. + pixY*m_PixelSize )  )   ,
                                m_xyChips[chipNum].second + ( m_ChipFirstPixelSize[chipNum]+ (double)m_PixelSize/2. 
                                                              + (double)(pixX-1)*m_PixelSize)  );  
        
      }
      else {
        return std::make_pair(  m_xyChips[chipNum].first - ( chipWidth()  - ( m_PixelSize/2. + pixY*m_PixelSize )  ) ,
                                m_xyChips[chipNum].second + ( m_ChipFirstPixelSize[chipNum] + (pixX-1)*m_PixelSize
                                                            + m_ChipLastPixelSize[chipNum]/2.));  
      }
    }

  }

  /// Return the X,Y position of the pixel + fraction  
  inline pixelCoord xyOfPixel(unsigned long pixel, pixelCoord fraction) const {
    int chipNum =( pixel & m_ChipMask) >> m_PixelBit  ;
    int pixX = (pixel & m_PixelLPMask) >> m_PixelHPBit ;
    int pixY =  pixel & m_PixelHPMask  ;
    if ( m_ChipHorizontal[chipNum]){
      if ( pixX == 0 ){
        pixelCoord result =   std::make_pair(  m_xyChips[chipNum].first -( m_ChipFirstPixelSize[chipNum]/2. ),
                                m_xyChips[chipNum].second + m_PixelSize/2. + pixY*m_PixelSize 
                                               + (fraction.second-0.5)*m_PixelSize  );  
        result.first = result.first - (fraction.first-0.5)*m_ChipFirstPixelSize[chipNum];
        return result;
      }
      else if ( pixX < (m_NpixX-1) && pixX > 0 ){
        pixelCoord result =   std::make_pair(  m_xyChips[chipNum].first -( m_ChipFirstPixelSize[chipNum]+ (double)m_PixelSize/2. 
                                                            + (double)(pixX-1)*m_PixelSize),
                                m_xyChips[chipNum].second + m_PixelSize/2. + pixY*m_PixelSize 
                                               + (fraction.second-0.5)*m_PixelSize  ); 
        result.first = result.first - (fraction.first-0.5)*m_PixelSize;
        return result ;
      }
      else {
        pixelCoord result =   std::make_pair(  m_xyChips[chipNum].first -( m_ChipFirstPixelSize[chipNum] 
                                                                           + (double)(pixX-1)*m_PixelSize
                                                            + m_ChipLastPixelSize[chipNum]/2.),
                                m_xyChips[chipNum].second + m_PixelSize/2. + pixY*m_PixelSize  
                                               + (fraction.second-0.5)*m_PixelSize);  
        result.first = result.first - (fraction.first-0.5)*m_ChipLastPixelSize[chipNum];
        return result;
      }
    }
    else {
      // fraction are given in the XY sensor frame coordinates...  
      if ( pixX == 0 ){
        pixelCoord result =   std::make_pair(  m_xyChips[chipNum].first 
                                               - (chipWidth() - (m_PixelSize/2. + pixY*m_PixelSize  
                                                                 - (fraction.first-0.5)*m_PixelSize ) ),
                                m_xyChips[chipNum].second + m_ChipFirstPixelSize[chipNum]/2.  ); 
        result.second = result.second + (fraction.second-0.5)*m_ChipFirstPixelSize[chipNum];
        return result;
      }
      else if ( pixX < ( m_NpixX - 1 ) && pixX != 0 ){
        pixelCoord result =   std::make_pair(  m_xyChips[chipNum].first 
                                               - ( chipWidth() - (m_PixelSize/2. + pixY*m_PixelSize  
                                                                  - (fraction.first-0.5)*m_PixelSize ))    ,
                                m_xyChips[chipNum].second + ( m_ChipFirstPixelSize[chipNum]+ (double)m_PixelSize/2. 
                                                              + (double)(pixX-1)*m_PixelSize)  );  
        result.second = result.second + (fraction.second-0.5)*m_PixelSize;
        return result;
        
      }
      else {
        pixelCoord result =   std::make_pair(  m_xyChips[chipNum].first 
                                               - ( chipWidth() - (m_PixelSize/2. + pixY*m_PixelSize 
                                                                  - (fraction.first-0.5)*m_PixelSize ) ) ,
                                m_xyChips[chipNum].second + ( m_ChipFirstPixelSize[chipNum] + (pixX-1)*m_PixelSize
                                                            + m_ChipLastPixelSize[chipNum]/2.));  
        result.second = result.second + (fraction.second-0.5)*m_ChipLastPixelSize[chipNum];
        return result;
      }
    }
  }
  


  /// Return the X,Y,Z position of the pixel + fraction in local coordinates
  inline Gaudi::XYZPoint xyzOfPixel(unsigned long pixel, pixelCoord fraction) const {
    pixelCoord xy = xyOfPixel(pixel,fraction);
    int chipNum =( pixel & m_ChipMask) >> m_PixelBit  ;
    return Gaudi::XYZPoint (xy.first,xy.second, m_ladders[m_ChipsInLadder[chipNum]].ReferencePoint().z() );
  }
  
  /// Return the X,Y,Z position of the pixel + fraction in local coordinates          
  inline Gaudi::XYZPoint xyzOfPixel(unsigned long pixel) const {    
    pixelCoord xy = xyOfPixel(pixel);
    int chipNum =( pixel & m_ChipMask) >> m_PixelBit  ;
    return Gaudi::XYZPoint (xy.first,xy.second, m_ladders[m_ChipsInLadder[chipNum]].ReferencePoint().z() );
  }

  /// Return the X,Y,Z position of the pixel plus fraction*size of the pixel in the global frame
  inline Gaudi::XYZPoint globalXYZ(unsigned long pixel , pixelCoord fraction) const {
    return localToGlobal(xyzOfPixel( pixel,fraction));
  }
  inline Gaudi::XYZPoint globalXYZ(unsigned long pixel) const {
    return localToGlobal(xyzOfPixel( pixel));
  }
    
  /// Return the  X,Y,Z position of the pixel plus fraction*size of the pixel in the halfbox frame
  inline Gaudi::XYZPoint halfboxXYZ(unsigned long pixel, pixelCoord fraction) const {
    return localToVPHalfBox(xyzOfPixel( pixel,fraction));
  }
  inline Gaudi::XYZPoint halfboxXYZ(unsigned long pixel) const {
    return localToVPHalfBox(xyzOfPixel( pixel));
  }
  /// Return true if is a long pixel
  inline bool isLong(unsigned long pixel) const
  {
    return ( fabs(PixelSize(pixel).first - interchipPixSize()) < 0.0000001 ) 
      || ( fabs(PixelSize(pixel).second - interchipPixSize()) < 0.0000001 ); 
  }

private:
  /// Store the local x,y position and fraction for each pixel in the sensor
  StatusCode calcPixelsParam();

  /// Calculate the global and half box strip radii when the alignment changes
  StatusCode updatePixelSquareCache();
  
  /// Update geomtry cache when the alignment changes
  StatusCode updateGeometryCache();

    
private:
  // vector of Ladder constituing the sensor
  std::vector<PixelLadder> m_ladders;
  // associated sensor on the other side of the velo
  const DeVPSquareType* m_otherSideSensor;

  unsigned int m_PixelLPBit;
  unsigned int m_PixelHPBit;
  unsigned int m_PixelBit;

  unsigned int m_ChipMask ;
  unsigned int m_PixelLPMask;
  unsigned int m_PixelHPMask;
  unsigned int m_PixelMask;
  

  // These are references to local statics accessed via static functions
  // implemented in DeVeloRType.cpp. I stree this because these are
  // NOT ALLOWED TO BE INLINED!
  // Sematically, these data menber should be statics, but this does not work with
  // Windows(tm) DLLs in the CMT framework because they are accessed
  // by inlined accessors. So we have to live with this detour.
  // The staic bool m_staticDataInvalid is not (and never should be!)
  // accessed by any inline function. It's sole purpose is to speed
  // up the initialize() method when the information common to all sensors
  //  - m_PixelSize | standard pixel size
  //  - m_NpixX     | number of chips along the x axis
  //  - m_ChipWidth | chip width needed to transform x to y

  //  - m_xyChips | bottom left X,Y position of 1st pixel in chip (size NChip*std::pair<double,double>)

  //  - m_ChipFirstPixelSize | x size of first pixel (vector of double) size NChip*double
  //  - m_ChipLastPixelSize | x size of last pixel (vector of double) size NChip*double
  //  - m_ChipsInLadder  | ladder of each chip (since it contains the Z relative position wrt. the sensor z=0) size  NChip*int
  //  - m_ChipHorizontal | chip orientation (for the moment Horizontal or vertial --> vector of bool) size  NChip*bool
  

  double& m_PixelSize;
  int& m_NpixX ;
  double& m_ChipWidth ;
  // 64 size at maximum
  std::vector< std::pair<double,double> >& m_xyChips;
  std::vector< double >& m_ChipFirstPixelSize;
  std::vector< double >& m_ChipLastPixelSize;
  std::vector< int >& m_ChipsInLadder;
  std::vector< bool >& m_ChipHorizontal;

  // used to control initialization NEVER ACCESS THIS IN AN INLINED METHOD!
  static bool m_staticDataInvalid;
  // Set output level for message service
  bool m_debug;
  bool m_verbose;

  /// cached Message Stream object
  mutable MsgStream * m_msgStream;

  /// On demand access to MsgStream object
  inline MsgStream & msg() const
  {
    if ( !m_msgStream ) m_msgStream = new MsgStream( msgSvc(), "DeVPSquareType" );
    return *m_msgStream; 
  }
};

/// fast cast to Square sensor, returns 0 for wrong type
inline const DeVPSquareType* DeVPSensor::squareType() const { 
  return (m_isSquare ? static_cast<const DeVPSquareType*>(this) : 0); 
}
  

#endif // VPDET_DEVPSQUARETYPE_H
