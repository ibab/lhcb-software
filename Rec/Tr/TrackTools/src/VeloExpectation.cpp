
//-----------------------------------------------------------------------------
/** @file VeloExpectation.cpp
 *
 *  Implementation file for reconstruction tool : VeloExpectation
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *  @date   11/03/2007
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/SystemOfUnits.h"

// local
#include "VeloExpectation.h"

// velo det
#include "VeloDet/DeVelo.h"

// Tsa
#include "TsaKernel/Line.h"

// Event
#include "Event/State.h"

#include "Event/TrackParameters.h"

using namespace LHCb;
using namespace Gaudi;

DECLARE_TOOL_FACTORY( VeloExpectation )

//-----------------------------------------------------------------------------

VeloExpectation::VeloExpectation( const std::string& type,
                              const std::string& name,
                              const IInterface* parent ):
  GaudiTool ( type, name, parent )
{

  // interface
  declareInterface<IVeloExpectation>(this);
}

VeloExpectation::~VeloExpectation() { }

StatusCode VeloExpectation::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  m_veloDet = getDet<DeVelo>( DeVeloLocation::Default );

  return sc;
}


int VeloExpectation::nExpected ( const Track& aTrack ) const
{
  // velo expectation from zFirst to endVelo
  IVeloExpectation::Info expectedHits = expectedInfo(aTrack);
  return expectedHits.nR + expectedHits.nPhi;
}

IVeloExpectation::Info VeloExpectation::expectedInfo ( const Track& aTrack, VeloPattern velo[4] ) const{

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


IVeloExpectation::Info VeloExpectation::expectedInfo ( const Track& aTrack ) const{

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


int VeloExpectation::nExpected(const LHCb::Track& aTrack, 
                               const double zStart, const double zStop) const{

  IVeloExpectation::Info expectedHits = expectedInfo(aTrack,zStart,zStop);
  return expectedHits.nR + expectedHits.nPhi;
}


IVeloExpectation::Info VeloExpectation::expectedInfo(const LHCb::Track& aTrack, 
                               const double zStart, const double zStop, VeloPattern velo[4]) const{
  return scan(aTrack,zStart, zStop, velo);
}

IVeloExpectation::Info VeloExpectation::expectedInfo(const LHCb::Track& aTrack, 
                               const double zStart, const double zStop) const{

  VeloPattern velo[4];
  return scan(aTrack,zStart, zStop, velo);
}

bool VeloExpectation::isInside(const LHCb::Track& aTrack, 
                               const unsigned int sensorNum) const{

  // make a line representing the track
  const State& state = aTrack.firstState();
  Tf::Tsa::Line xLine(state.tx(), state.x(),state.z());
  Tf::Tsa::Line yLine(state.ty(), state.y(),state.z());
  const DeVeloSensor* sensor = m_veloDet->sensor(sensorNum);
  const double z = sensor->z();  

  return isInside(sensor, xLine, yLine, z);
}


int VeloExpectation::nMissed ( const Track& aTrack ) const
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
  VeloPattern velo[4];
  IVeloExpectation::Info expectedHits = scan(aTrack,zStart,zStop,velo);

  return expectedHits.nR + expectedHits.nPhi - nFound(aTrack,zStart,zStop);
}

int VeloExpectation::nMissed( const Track& aTrack, const double z ) const{

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
  VeloPattern velo[4];
  IVeloExpectation::Info expectedHits = scan(aTrack, zStart, zStop, velo);

  return expectedHits.nPhi + expectedHits.nR - nFound(aTrack,zStart,zStop);
}

IVeloExpectation::Info VeloExpectation::scan(const LHCb::Track& aTrack, 
                          const double zStart, const double zStop, VeloPattern velo[4]) const {

  IVeloExpectation::Info nHits;
  nHits.nR = 0; nHits.nPhi = 0;

  Tf::Tsa::Line xLine(0.,0.); Tf::Tsa::Line yLine(0.,0.);
  std::vector<DeVeloSensor*>::const_iterator iterV = m_veloDet->rPhiSensorsBegin();
  for (; iterV != m_veloDet->rPhiSensorsEnd(); ++iterV){
    // only sensors the track could see
    const double z = (*iterV)->z();
    if (z >= zStart && z <= zStop ) {
      param(aTrack,z,xLine,yLine);        
      if (isInside(*iterV,xLine,yLine,z) == true){
        (*iterV)->isR() ? ++nHits.nR : ++nHits.nPhi; 

	unsigned int station = ((*iterV)->sensorNumber()%64)/2 ; 
	unsigned int side    = ((*iterV)->sensorNumber()%2) ; 
	unsigned int type    = ((*iterV)->sensorNumber()/64) ;
    
	station += 2*(1-type/2) ;
	type     = type%2 ;
	velo[side+2*type].set(station) ;
      
      }
    } // if 
  } // iterV
 
  return nHits;
}

bool VeloExpectation::isInside(const DeVeloSensor* sensor, 
			       const Tf::Tsa::Line& xLine, 
			       const Tf::Tsa::Line& yLine, 
			       const double z) const{
  // check whole sensor readout
  if( ! sensor->isReadOut() ) return false;
  // get the strip corresponding to the point 
  // also checks if local point is in active area
  Gaudi::XYZPoint trackPos(xLine.value(z),yLine.value(z),z);  
  LHCb::VeloChannelID channel;
  double stripFraction;
  double stripPitch;
  StatusCode sc = sensor->pointToChannel(trackPos,channel,stripFraction,
					 stripPitch);
  if(!sc) return false; // was not in a channel
  // can finally test state of nearest strip
  return sensor->OKStrip(channel.strip()); 
}


double VeloExpectation::zMin(const Track& aTrack) const{

  // get the hit at least z
  double z = 99999.0; 
  const std::vector<LHCbID>& vids = aTrack.lhcbIDs();
  for (std::vector<LHCbID>::const_iterator iter = vids.begin() ; iter != vids.end() ; ++iter){
     if (iter->isVelo()){
      VeloChannelID chan = iter->veloID(); 
      const DeVeloSensor* sensor = m_veloDet->sensor(chan);
      if (sensor->z() <z){
        z = sensor->z();
      }
    }
  }  // loop ids  
  return z;
}

double VeloExpectation::zMax(const Track& aTrack) const{

 double z = -99999.0;
 const std::vector<LHCbID>& vids = aTrack.lhcbIDs();
 for (std::vector<LHCbID>::const_iterator iter = vids.begin() ; iter != vids.end() ; ++iter){
   if (iter->isVelo()){
     VeloChannelID chan = iter->veloID(); 
     const DeVeloSensor* sensor = m_veloDet->sensor(chan);
     if (sensor->z() > z){
       z = sensor->z();
     }
   }
 } // loop ids
 return z;
}

int VeloExpectation::nFound(const Track& aTrack,
                            const double zStart, const double zStop) const{

 int nFound = 0;
 const std::vector<LHCbID>& vids = aTrack.lhcbIDs();
 for (std::vector<LHCbID>::const_iterator iter = vids.begin() ; iter != vids.end() ; ++iter){
   if (iter->isVelo()){
     VeloChannelID chan = iter->veloID(); 
     const DeVeloSensor* sensor = m_veloDet->sensor(chan);
     if (sensor->z() >= zStart && sensor->z() <= zStop ){
       ++nFound;
     }
   }
 } // ids

 return nFound;
}

void VeloExpectation::param(const LHCb::Track& aTrack, const double z, 
                            Tf::Tsa::Line& xLine, Tf::Tsa::Line& yLine) const{

  const LHCb::State& state = aTrack.closestState(z);
  xLine = Tf::Tsa::Line(state.tx(), state.x(), state.z());
  yLine = Tf::Tsa::Line(state.ty(), state.y(), state.z());
}

double VeloExpectation::zBeamLine(const Track& aTrack) const{

  double z = 0;
  const State& state = aTrack.closestState(0);
  if (state.checkLocation(State::ClosestToBeam) == true){
    z = state.z();
  }
  else {
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

                                                                              
