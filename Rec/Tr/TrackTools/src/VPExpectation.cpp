
//-----------------------------------------------------------------------------
/** @file VPExpectation.cpp
 *
 *  Implementation file for reconstruction tool : VPExpectation
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *  @date   11/03/2007
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "VPExpectation.h"

// velo det
#include "VPDet/DeVP.h"

// Tsa
#include "TsaKernel/Line.h"

// Event
#include "Event/State.h"

#include "Event/TrackParameters.h"
#include "DetDesc/IGeometryInfo.h"

using namespace LHCb;
using namespace Gaudi;

DECLARE_TOOL_FACTORY( VPExpectation )

//-----------------------------------------------------------------------------

VPExpectation::VPExpectation( const std::string& type,
                              const std::string& name,
                              const IInterface* parent ):
  GaudiTool ( type, name, parent )
{

  // interface
  declareInterface<IVPExpectation>(this);
}

VPExpectation::~VPExpectation() { }

StatusCode VPExpectation::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  m_veloDet = getDet<DeVP>( DeVPLocation::Default );

  return sc;
}


int VPExpectation::nExpected ( const Track& aTrack ) const
{
  IVPExpectation::Info expectedHits = expectedInfo(aTrack);
  return expectedHits.n;
}

IVPExpectation::Info VPExpectation::expectedInfo ( const Track& aTrack, std::bitset<30> velo[4] ) const{

  // work out the first and last z on the track
  double zStart; double zStop;
  if (aTrack.checkFlag( Track::Backward) == false){
    // forward track
    zStart = zMin(aTrack)  - 1e-3;
    zStop = 9999.0;
  } else {
    //backward track
    zStart = -9999.;
    zStop = zMax(aTrack)+ 1e-3;
  }

  return expectedInfo(aTrack, zStart, zStop, velo);
}


IVPExpectation::Info VPExpectation::expectedInfo ( const Track& aTrack ) const {

  // work out the first and last z on the track
  double zStart; double zStop;
  if (aTrack.checkFlag( Track::Backward) == false){
    // forward track
    zStart = zMin(aTrack)  - 1e-3;
    zStop = 9999.0;
  } else {
    //backward track
    zStart = -9999.;
    zStop = zMax(aTrack)+ 1e-3;
  }

  return expectedInfo(aTrack, zStart, zStop);
}

int VPExpectation::nExpected(const LHCb::Track& aTrack,
                               const double zStart, const double zStop) const {

  IVPExpectation::Info expectedHits = expectedInfo(aTrack,zStart,zStop);
  return expectedHits.n;
}


IVPExpectation::Info VPExpectation::expectedInfo(const LHCb::Track& aTrack,
                               const double zStart, const double zStop,std::bitset<30> velo[4]) const{
  return scan(aTrack,zStart, zStop, velo);
}

IVPExpectation::Info VPExpectation::expectedInfo(const LHCb::Track& aTrack,
                               const double zStart, const double zStop) const {

  std::bitset<30> velo[4];
  return scan(aTrack,zStart, zStop, velo);
}

bool VPExpectation::isInside(const LHCb::Track& aTrack,
                             const unsigned int sensorNum) const {

  // make a line representing the track
  const State& state = aTrack.firstState();
  Tf::Tsa::Line xLine(state.tx(), state.x(),state.z());
  Tf::Tsa::Line yLine(state.ty(), state.y(),state.z());
  const DeVPSensor* sensor = m_veloDet->sensorOfChannel(sensorNum);
  const double z = sensor->z();

  return isInside(sensor, xLine, yLine, z);
}


int VPExpectation::nMissed ( const Track& aTrack ) const
{

  // number of hits missed from zBeam to zFirst

  // forward or backward track ?
  double zStart; double zStop;
  if ( aTrack.checkFlag( Track::Backward) == false){
    zStart = zBeamLine(aTrack) - 1e-3;
    zStop = zMin(aTrack);
  }
  else {
    zStart = zMax(aTrack) - 1e-3;
    zStop = zBeamLine(aTrack);
  }


  // number expected...
  std::bitset<30> velo[4];
  IVPExpectation::Info expectedHits = scan(aTrack,zStart,zStop,velo);

  return expectedHits.n - nFound(aTrack,zStart,zStop);
}

int VPExpectation::nMissed( const Track& aTrack, const double z ) const{

  // line representing track
  double zStart; double zStop;
  if ( aTrack.checkFlag( Track::Backward) == false){
    zStart = z;
    zStop = zMin(aTrack) + 1e-3;
  }
  else {
    zStart = zMax(aTrack) - 1e-3;
    zStop = z;
  }

  // number expected...
  std::bitset<30> velo[4];
  IVPExpectation::Info expectedHits = scan(aTrack, zStart, zStop, velo);

  return expectedHits.n - nFound(aTrack,zStart,zStop);
}

IVPExpectation::Info VPExpectation::scan(const LHCb::Track& aTrack,
                          const double zStart, const double zStop, std::bitset<30> velo[4]) const {

  IVPExpectation::Info nHits;
  nHits.n = 0;
  nHits.firstR = 99999.;


  Tf::Tsa::Line xLine(0.,0.); Tf::Tsa::Line yLine(0.,0.);
  std::vector<DeVPSensor*>::const_iterator iterV = m_veloDet->sensorsBegin();
  //std::cout << "scan(track,"<<zStart<<","<<zStop<<"): Looping over the sensors..."<<std::endl;
  for (; iterV != m_veloDet->sensorsEnd(); ++iterV){
    // only sensors the track could see
    const double z = (*iterV)->z();
//     if (true ) {
    if (z >= zStart && z <= zStop ) {
      //std::cout << "scan() sensor in range " << z << std::endl;
      param(aTrack,z,xLine,yLine);
      //std::cout << "scan(): state (" << aTrack.closestState(z).x() << "," << aTrack.closestState(z).y() << "," << aTrack.closestState(z).z() << ")" << std::endl;
      if (isInside(*iterV,xLine,yLine,z) == true){
        //std::cout << "scan() state is inside sensor" << std::endl;
        ++nHits.n ;

        double x = xLine.value(z);
        double y = yLine.value(z);
        double r = sqrt(x*x+y*y);

        if (nHits.firstR > r) {
          nHits.firstX = x;
          nHits.firstY = y;
          nHits.firstZ = z;
          nHits.firstR = r;
        }

        nHits.expectedZ.push_back(z);

        //TODO: this is probably not applicable to VP...
//      unsigned int station = ((*iterV)->sensorNumber()%64)/2 ;
//      unsigned int side    = ((*iterV)->sensorNumber()%2) ;
//      unsigned int type    = ((*iterV)->sensorNumber()/64) ;
//
//      station += 2*(1-type/2) ;
//      type     = type%2 ;
//      velo[side+2*type].set(station) ;

      } else {
        //std::cout << "scan() state is not inside sensor" << std::endl;
      }
    } else {// if
      //std::cout << "scan() sensor out of range " << z << std::endl;
    }
  } // iterV

  return nHits;
}

bool VPExpectation::isInside(const DeVPSensor* sensor,
			       const Tf::Tsa::Line& xLine,
			       const Tf::Tsa::Line& yLine,
			       const double z) const{

  if ( abs(sensor->z() - z) > 1e-2 ) {
    warning() << "The sensor z isnt the sensor z! "  << sensor->z() << " " << z << " " << abs(sensor->z() - z) << endmsg;
    return false;
  }

  Gaudi::XYZPoint trackPos(xLine.value(z),yLine.value(z),z);
  StatusCode sc = sensor->isInActiveArea(sensor->globalToLocal(trackPos));
  if(sc) return true;

  return false;

//   Gaudi::XYZPoint trackPos(xLine.value(z),yLine.value(z),z);
//   StatusCode sc = sensor->isInActiveArea(sensor->globalToLocal(trackPos));
//   if(sc) return true;

//   trackPos = Gaudi::XYZPoint(xLine.value(z+0.504),yLine.value(z+0.504),z+0.504);
//   sc = sensor->isInActiveArea(sensor->globalToLocal(trackPos));
//   if(sc) return true;
//
//   trackPos = Gaudi::XYZPoint(xLine.value(z-0.504),yLine.value(z-0.504),z-0.504);
//   sc = sensor->isInActiveArea(sensor->globalToLocal(trackPos));
//   if(sc) return true;
//
//   trackPos = Gaudi::XYZPoint(xLine.value(z+2.804),yLine.value(z+2.804),z+2.804);
//   sc = sensor->isInActiveArea(sensor->globalToLocal(trackPos));
//   if(sc) return true;
//
//   trackPos = Gaudi::XYZPoint(xLine.value(z-2.804),yLine.value(z-2.804),z-2.804);
//   sc = sensor->isInActiveArea(sensor->globalToLocal(trackPos));
//   if(sc) return true;

//   return false;#






//   StatusCode sc = sensor->isInActiveArea(sensor->globalToLocal(trackPos));
//   if(!sc) return false;
//   return true;

//   VP1
// 175.004
// 173.996
// 0.504
//   VP2
// 177.304
// 171.696
// 2.804
/*

  if ( fabs( z-sensor->z() )>1. ) {
    return false;
  }

  double sensWidth = 14.5;
  double innerR = 5.1;

  // left sensor (L-shape not gamma shape)
  if ( sensor->isLeft() ) {
    // verticle ladders first
    if ( trackPos.x() < (innerR+2.*sensWidth) and trackPos.x() > innerR ) {
      if ( trackPos.y() < (3.*sensWidth-innerR) and trackPos.y() > (-innerR) ) {
        return true;
      }
    }
    // horizontal ladders
    if ( trackPos.x() < (3.*sensWidth-innerR) and trackPos.x() > (-innerR) ) {
      if ( trackPos.y() < (-innerR) and trackPos.y() > (-innerR-2.*sensWidth) ) {
        return true;
      }
    }
    return false;
  // right sensor (backwards gamma shape)
  } else if (sensor->isRight() ) {
    // verticle ladders first
    if ( trackPos.x() < (-innerR) and trackPos.x() > (-innerR - 2.*sensWidth) ) {
      if ( trackPos.y() < (innerR) and trackPos.y() > (innerR-3.*sensWidth) ) {
        return true;
      }
    }
    // horizontal ladders
    if ( trackPos.x() < (innerR) and trackPos.x() > (innerR-3.*sensWidth) ) {
      if ( trackPos.y() < (innerR+2.*sensWidth) and trackPos.y() > (innerR) ) {
        return true;
      }
    }
    return false;
  }
  return false;*/


//   // check whole sensor readout
// //   Doesnt seem VP needs this
// //  if( ! sensor->isReadOut() ) return false;
//   // get the strip corresponding to the point
//   // also checks if local point is in active area
//   Gaudi::XYZPoint trackPos(xLine.value(z),yLine.value(z),z);
//   Gaudi::XYZPoint localPoint = sensor->globalToLocal(trackPos);
//   //std::cout << "isInside(" << xLine.value(z) << "," << yLine.value(z) << "," << (z) << "): type " << (sensor->isSquare() ? "square" : (sensor->isX() ? "X" : (sensor->isY() ? "Y" : "unknown"))) << " lv " << ((IGeometryInfo *)(sensor->geometry()))->belongsToPath(trackPos) << std::endl;
//   //std::cout << "isInside(): local (" << localPoint.x() << "," << localPoint.y() << "," << localPoint.z() << ") ladder " << sensor->squareType()->WhichLadder(localPoint) << " lv " << sensor->geometry()->lvolumeName() << std::endl;
//   LHCb::VPChannelID channel;
//   double stripFraction;
//   double stripPitch;
//   std::pair <double,double> temp(stripFraction, stripPitch);
//   bool sc = isInsideChildren((IGeometryInfo *)(sensor->geometry()),trackPos);
// //   bool sc = sensor->squareType()->isInside(trackPos);
// //   StatusCode sc = sensor->squareType()->isInActiveArea(localPoint);
// //   StatusCode sc = sensor->pointToChannel(trackPos,channel,temp);
// //   StatusCode sc = sensor->pointToChannel(trackPos,channel,stripFraction,
// // 					 stripPitch);
//   if(!sc) {
//     //std::cout << "isInside(): pointToChannel failed" << std::endl;
//     return false; // was not in a channel
//   }
//   // can finally test state of nearest strip
// //   return sensor->OKStrip(channel.strip());
//   //std::cout << "isInside(): pointToChannel succeeded" << std::endl;
//   return true;
}

bool VPExpectation::isInsideChildren(const IGeometryInfo* igi, const Gaudi::XYZPoint globalPoint) const{
  //std::cout << igi->lvolumeName() << std::endl;
  if (igi->isInside(globalPoint)) {
    return true;
    //std::cout << "return TRUE" << std::endl;
  } else {
    std::vector<IGeometryInfo*> children = igi->childIGeometryInfos();
    for (std::vector<IGeometryInfo*>::const_iterator iChild = children.begin(); iChild != children.end(); ++iChild) {
      if (isInsideChildren((*iChild), globalPoint)) {
        //std::cout << "return TRUE" << std::endl;
        return true;
      }
    }
  }
  //std::cout << "return false" << std::endl;
  return false;
}

double VPExpectation::zMin(const Track& aTrack) const{

  // get the hit at least z
  double z = 99999.0;
  const std::vector<LHCbID>& vids = aTrack.lhcbIDs();
  for (std::vector<LHCbID>::const_iterator iter = vids.begin() ; iter != vids.end() ; ++iter){
      if (iter->isVP() )  {
      VPChannelID chan = iter->vpID();
      const DeVPSensor* sensor = m_veloDet->sensorOfChannel(chan);
      if (sensor != NULL) {
        if (sensor->z() < z){
          z = sensor->z();
        }
      } else warning() << "Sensor is null for " << chan << endmsg;
    }
  }  // loop ids
  return z;
}

double VPExpectation::zMax(const Track& aTrack) const{

  double z = -99999.0;
  const std::vector<LHCbID>& vids = aTrack.lhcbIDs();
  for (std::vector<LHCbID>::const_iterator iter = vids.begin() ; iter != vids.end() ; ++iter){
    if (iter->isVP()){
      VPChannelID chan = iter->vpID();
      const DeVPSensor* sensor = m_veloDet->sensorOfChannel(chan);
      if (sensor->z() > z){
        z = sensor->z();
      }
      //std::cout << "zMax(): VP hit z " << sensor->z() << " max " << z << std::endl;
    } else {
      //std::cout << "zMax(): Not a VP hit" << std::endl;
    }
  } // loop ids
 return z;
}

int VPExpectation::nFound(const Track& aTrack,
                            const double zStart, const double zStop) const{

 int nFound = 0;
 const std::vector<LHCbID>& vids = aTrack.lhcbIDs();
 for (std::vector<LHCbID>::const_iterator iter = vids.begin() ; iter != vids.end() ; ++iter){
   if (iter->isVP()){
     VPChannelID chan = iter->vpID();
     const DeVPSensor* sensor = m_veloDet->sensorOfChannel(chan);
     if (sensor->z() >= zStart && sensor->z() <= zStop ){
       ++nFound;
     }
   }
 } // ids

 return nFound;
}

void VPExpectation::param(const LHCb::Track& aTrack, const double z,
                            Tf::Tsa::Line& xLine, Tf::Tsa::Line& yLine) const {

  const LHCb::State& state = aTrack.closestState(z);
  xLine = Tf::Tsa::Line(state.tx(), state.x(), state.z());
  yLine = Tf::Tsa::Line(state.ty(), state.y(), state.z());
}

double VPExpectation::zBeamLine(const Track& aTrack) const {
  double z = 0;
  const State& state = aTrack.closestState(0);
  if (state.checkLocation(State::ClosestToBeam) == true) {
    z = state.z();
  } else {
    const TrackVector& vec = state.stateVector();
    double z = state.z();
    // check on division by zero (track parallel to beam line!)
    if ( fabs(vec[2]) > TrackParameters::lowTolerance
        || vec[3]  > TrackParameters::lowTolerance ) {
      z -= ( vec[0]*vec[2] + vec[1]*vec[3] ) / ( vec[2]*vec[2] + vec[3]*vec[3] );
    }
  }
  return z;
}


