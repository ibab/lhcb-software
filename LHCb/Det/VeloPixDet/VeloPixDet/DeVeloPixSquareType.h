// $Id: DeVeloPixSquareType.h,v 1.7 2009-11-27 08:54:09 cocov Exp $
#ifndef VELOPIXDET_DEVELOPIXSQUARETYPE_H 
#define VELOPIXDET_DEVELOPIXSQUARETYPE_H 1

// Include files

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Gaudi Math definitions
#include "GaudiKernel/Point3DTypes.h"

// from VeloPixDet
#include "VeloPixDet/DeVeloPixSensor.h"
#include "VeloPixDet/PixelLadder.h"

// get trajectory
#include "Kernel/Trajectory.h"


// Unique class identifier
static const CLID CLID_DeVeloPixSquareType = 1008204 ;

namespace LHCb {
  class VeloPixChannelID;
}

/** @class DeVeloPixSquareType DeVeloPixSquareType.h VeloPixDet/DeVeloPixSquareType.h
 *  
 *  Detector element for square (symetric) pixel type
 *  contains all the function specific to this type of pixel sensor
 *
 *  @author Victor Coco
 *  @date   2009-05-14
 */

class DeVeloPixSquareType : public DeVeloPixSensor {
public:
  
  typedef std::pair<double,double> pixelCoord;
  
  /// Standard constructor
  DeVeloPixSquareType( const std::string& name = "" ); 

  virtual ~DeVeloPixSquareType( ); ///< Destructor

  /// object identifier (static method)
  static  const CLID& classID() { return CLID_DeVeloPixSquareType; }
  /// object identification
  virtual const CLID& clID()     const;

  /// Initialise the DeVeloPixSensor from the XML
  virtual StatusCode initialize();


  /// Return a trajectory (for track fit) from pixel + fraction
  virtual std::auto_ptr<LHCb::Trajectory> trajectory(const LHCb::VeloPixChannelID& id
                                                     , const std::pair<double,double> offset) const;

  //::::::::::::: Some classes to deal with channelID to real world

  /// Calculate the nearest channel to a 3-d point.
  /// Also returns the fractional x-y position IN the pixel
  virtual StatusCode pointToChannel(const Gaudi::XYZPoint& point,
                                    LHCb::VeloPixChannelID& channel,
                                    std::pair <double, double>& fraction) const;

  /// Calculate the XYZ center of a pixel
  virtual StatusCode channelToPoint( const LHCb::VeloPixChannelID& channel,
                                                Gaudi::XYZPoint& point) const;

  virtual StatusCode channelToPointWithFraction( const LHCb::VeloPixChannelID& channel,
                                                        const std::pair<double,double> offset,
                                                 Gaudi::XYZPoint& point) const;
  
  

  /// Get the list of VeloPixChannelID forming the 3x3 cluster of pixel centered on point
  virtual StatusCode pointTo3x3Channels(const Gaudi::XYZPoint& point,
                                       std::vector <LHCb::VeloPixChannelID>& channels) const;

  /// Get the 8 channel (if they exist) arround a given seed channel
  virtual StatusCode  channelToNeighbours( const LHCb::VeloPixChannelID& seedChannel, 
                                       std::vector <LHCb::VeloPixChannelID>& channels) const;
  
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

  virtual std::pair<double,double> PixelSize( LHCb::VeloPixChannelID channel) const;

  //::::::::::::: Some classes to deal with the sensors
    
  /// Access to the sensor on the other side of the VeloPix
  inline const DeVeloPixSquareType* otherSideSensor() const { return m_otherSideSensor; }
  
  /// Set the sensor on the other side of the VeloPix.  This should only be called by DeVeloPix::initialize()
  inline void setOtherSideSensor(const DeVeloPixSquareType* s) { m_otherSideSensor = s; }


  //::::::::::::: Some classes to deal with coordinates in the different frame

  /// Return the x,y size of a pixel
  inline std::pair<double,double> PixelSize(unsigned long pixel)const
  {   
    int chipNum =( pixel & m_ChipMask) >> m_PixelBit  ;
    int pix =  pixel & m_PixelMask ;
    if (m_ChipSameOrientation[chipNum])  return m_PixelsSize[pix] ;
    else {
      int pixlp = pix & m_PixelLPMask ;
      if (pixlp!= 0 && pixlp!= (nPixCol()-1)) return m_PixelsSize[pix] ;
      else if ( pixlp == 0 ) return m_PixelsSize[(unsigned int)(nPixCol()-1)] ;
      else  return m_PixelsSize[0] ; 
    }
  }

  /// Return the x,y position of the pixel
  inline pixelCoord xyOfPixel(const unsigned long pixel) const {
    int chipNum =( pixel & m_ChipMask) >> m_PixelBit  ;
    int pix =  pixel & m_PixelMask ;
    if (m_ChipSameOrientation[chipNum]){  
      return std::make_pair(
                            m_xyPixels[pix].first + m_xyChips[pix].first ,
                            m_xyPixels[pix].second + m_xyChips[pix].second
                            );      
    }
    else{
      return std::make_pair(
                            m_xyChips[pix].first + chipLength() - m_xyChips[pix].first ,
                            m_xyChips[pix].second + chipWidth() - m_xyChips[pix].second
                            );
    }
  }

  /// Return the x,y position of the pixel + fraction
  inline pixelCoord xyOfPixel(unsigned long pixel, pixelCoord fraction) const {
    int chipNum =( pixel & m_ChipMask) >> m_PixelBit  ;
    int pix =  pixel & m_PixelMask ;
    if (m_ChipSameOrientation[chipNum]){  
      return std::make_pair(
                            m_xyPixels[pix].first + m_xyChips[pix].first + (fraction.first - 0.5)*m_PixelsSize[pix].first ,
                            m_xyPixels[pix].second + m_xyChips[pix].second + (fraction.second - 0.5)*m_PixelsSize[pix].second
                            );      
    }
    else{
      return std::make_pair(
                            m_xyChips[pix].first + chipLength() - 
                            (m_xyChips[pix].first + (fraction.first - 0.5)*m_PixelsSize[pix].first) ,
                            m_xyChips[pix].second + chipWidth() - 
                            (m_xyChips[pix].second + (fraction.second - 0.5)*m_PixelsSize[pix].second)
                            );
    }
  }


  /// Return the x,y,z position of the pixel + fraction in local coordinates
  inline Gaudi::XYZPoint xyzOfPixel(unsigned long pixel, pixelCoord fraction) const {
    int chipNum =( pixel & m_ChipMask) >> m_PixelBit  ;
    int pix =  pixel & m_PixelMask ;
    if (m_ChipSameOrientation[chipNum]){  
      return Gaudi::XYZPoint (
                            m_xyPixels[pix].first + m_xyChips[pix].first + (fraction.first - 0.5)*m_PixelsSize[pix].first ,
                            m_xyPixels[pix].second + m_xyChips[pix].second + (fraction.second - 0.5)*m_PixelsSize[pix].second,
                            m_ladders[m_ChipsInLadder[chipNum]].ReferencePoint().z()
                            );      
    }
    else{
      return Gaudi::XYZPoint(
                            m_xyChips[pix].first + chipLength() - 
                            (m_xyChips[pix].first + (fraction.first - 0.5)*m_PixelsSize[pix].first) ,
                            m_xyChips[pix].second + chipWidth() - 
                            (m_xyChips[pix].second + (fraction.second - 0.5)*m_PixelsSize[pix].second),
                            m_ladders[m_ChipsInLadder[chipNum]].ReferencePoint().z()
                            );
    }
  }
  
  /// Return the x,y,z position of the pixel + fraction in local coordinates
  inline Gaudi::XYZPoint xyzOfPixel(unsigned long pixel) const {    
    int chipNum =( pixel & m_ChipMask) >> m_PixelBit  ;
    int pix =  pixel & m_PixelMask ;
    if (m_ChipSameOrientation[chipNum]){  
      return Gaudi::XYZPoint(
                            m_xyPixels[pix].first + m_xyChips[pix].first ,
                            m_xyPixels[pix].second + m_xyChips[pix].second,
                            m_ladders[m_ChipsInLadder[chipNum]].ReferencePoint().z()
                            );      
    }
    else{
      return Gaudi::XYZPoint(
                            m_xyChips[pix].first + chipLength() - m_xyChips[pix].first ,
                            m_xyChips[pix].second + chipWidth() - m_xyChips[pix].second,
                            m_ladders[m_ChipsInLadder[chipNum]].ReferencePoint().z()
                            );
    }
  }
  

  /// Return the x,y,z position of the pixel plus fraction*size of the pixel in the global frame
  inline Gaudi::XYZPoint globalXYZ(unsigned long pixel, pixelCoord fraction) const {
    return localToGlobal(xyzOfPixel( pixel,fraction));
  }
  inline Gaudi::XYZPoint globalXYZ(unsigned long pixel) const {
    return localToGlobal(xyzOfPixel( pixel));
  }
    
  /// Return the x,y position of the pixel plus fraction*size of the pixel in the halfbox frame
  inline Gaudi::XYZPoint halfboxXYZ(unsigned long pixel, pixelCoord fraction) const {
    return localToVeloPixHalfBox(xyzOfPixel( pixel,fraction));
  }
  inline Gaudi::XYZPoint halfboxXYZ(unsigned long pixel) const {
    return localToVeloPixHalfBox(xyzOfPixel( pixel));
  }


private:
  /// Store the local x,y position and fraction for each pixel in the sensor
  void calcPixelsParam();

  /// Calculate the global and half box strip radii when the alignment changes
  StatusCode updatePixelSquareCache();
  
  /// Update geomtry cache when the alignment changes
  StatusCode updateGeometryCache();

    
private:
  // vector of Ladder constituing the sensor
  std::vector<PixelLadder> m_ladders;
  // associated sensor on the other side of the velo
  const DeVeloPixSquareType* m_otherSideSensor;

  
  unsigned int m_ChipBit;
  unsigned int m_PixelLPBit;
  unsigned int m_PixelHPBit;
  unsigned int m_PixelBit;
  unsigned int m_numberOfPixelsPerChipBits;
  unsigned int m_numberOfChipsBits;
  
  unsigned int m_ChipMask ;
  unsigned int m_PixelLPMask;
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

  // 65536 size at maximum (for TIMEPIX it is always at maximum)
  std::vector< std::pair<double,double> >& m_xyPixels;
  std::vector< std::pair<double,double> >& m_PixelsSize;

  // 64 size at maximum
  std::vector< std::pair<double,double> >& m_xyChips;
  std::vector< bool >& m_ChipSameOrientation;
  std::vector< int >& m_ChipsInLadder;

  /// cache, model to review if we need to use it,
  /// since we should take Z into account as soon as there is more than one plane of chip
  /// would be something like
  //std::vector<pixelCoord> m_globalXY;
  //std::vector<pixelCoord> m_globalXYChips;
  //std::vector<pixelCoord> m_halfboxXY;
  //std::vector<pixelCoord> m_halfboxXYChips;


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
    if ( !m_msgStream ) m_msgStream = new MsgStream( msgSvc(), "DeVeloPixSquareType" );
    return *m_msgStream; 
  }
};

/// fast cast to Square sensor, returns 0 for wrong type
inline const DeVeloPixSquareType* DeVeloPixSensor::squareType() const { 
  return (m_isSquare ? static_cast<const DeVeloPixSquareType*>(this) : 0); 
}
  

#endif // VELOPIXDET_DEVELOPIXSQUARETYPE_H
