// $Id: DeOTDetector.cpp,v 1.19 2006-04-04 14:23:15 ebos Exp $
/// Kernel
#include "Kernel/LHCbID.h"
#include "Kernel/OTChannelID.h"
#include "Kernel/Point3DTypes.h"
#include "Kernel/SystemOfUnits.h"

/// OTDet
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTStation.h"
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTQuarter.h"
#include "OTDet/DeOTModule.h"

/// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

// GaudiKernel
#include "GaudiKernel/GaudiException.h"

// Kernel/LHCbKernel
#include "Kernel/Trajectory.h"

/** @file DeOTDetector.cpp
 *
 *  Implementation of class :  DeOTDetector
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl
 */

using namespace boost::lambda;
using namespace LHCb;

DeOTDetector::DeOTDetector(const std::string& name) :
  DetectorElement( name ),
  m_stations(),
  m_layers(),
  m_quarters(),
  m_modules(),
  m_firstStation(0u),
  m_nChannels(0u),
  m_nMaxChanInModule(0u),
  m_cellRadius(0.0),
  m_resolution(0.0),
  m_propagationDelay(0.0),
  m_maxDriftTime(0.0),
  m_maxDriftTimeCor(0.0),
  m_deadTime(0.0)
{
  /// Constructor
  m_modules.reserve(432);
}

DeOTDetector::~DeOTDetector() {
  /// Destructor
}

const CLID& DeOTDetector::clID () const {
  return DeOTDetector::classID() ;
}

/// Initialise
StatusCode DeOTDetector::initialize()
{
  MsgStream msg( msgSvc(), name() );

  StatusCode sc = DetectorElement::initialize();

  if( sc.isFailure() ) {
    msg << MSG::ERROR << "Failed to initialize DetectorElement" << endreq;
    return sc ;
  } else {
    // loop over stations
    IDetectorElement::IDEContainer::const_iterator iStation = this->childBegin();
    for ( ; this->childEnd() != iStation; ++iStation ) {
      DeOTStation* station = dynamic_cast<DeOTStation*>(*iStation);
      if (station) m_stations.push_back(station);
      //loop over layers
      IDetectorElement::IDEContainer::const_iterator iLayer = (*iStation)->childBegin();
      for ( ; iLayer != (*iStation)->childEnd(); ++iLayer) {
        DeOTLayer* layer = dynamic_cast<DeOTLayer*>(*iLayer);
        if (layer) m_layers.push_back(layer);
        // loop over quarters
        IDetectorElement::IDEContainer::const_iterator iQuarter = (*iLayer)->childBegin();
        for ( ; iQuarter != (*iLayer)->childEnd(); ++iQuarter) {
          DeOTQuarter* quarter = dynamic_cast<DeOTQuarter*>(*iQuarter);
          if (quarter) m_quarters.push_back(quarter);
          // loop over modules
          IDetectorElement::IDEContainer::const_iterator iModule = (*iQuarter)->childBegin();
          for ( ; iModule != (*iQuarter)->childEnd(); ++iModule) {
            DeOTModule* module = dynamic_cast<DeOTModule*>(*iModule);
            if (module) {
              unsigned int channels = module->nChannels();
              m_nChannels += channels;
              m_modules.push_back(module);
              if (channels > m_nMaxChanInModule) m_nMaxChanInModule = channels;
            }
          } // modules
        } // quarters
      } // layers
    } // stations

    msg << MSG::DEBUG << "Successfully initialized DetectorElement !!!" << endreq;
  }
  
  /// Set the first station
  setFirstStation(m_stations.front()->stationID());
  
  /// Get OT Detector parameters
  m_resolution = param<double>("resolution");
  m_propagationDelay = param<double>("propagationDelay");
  m_maxDriftTime = param<double>("maxDriftTime");
  m_maxDriftTimeCor = param<double>("maxDriftTimeCor");
  m_deadTime = param<double>("deadTime");
  m_cellRadius = param<double>("cellRadius");
  
  return sc;
}

void DeOTDetector::setFirstStation(const unsigned int iStation) {
  m_firstStation = iStation;
}

StatusCode DeOTDetector::calculateHits(const Gaudi::XYZPoint& entryPoint,
                                       const Gaudi::XYZPoint& exitPoint,
                                       std::vector<OTChannelID>& channels,
                                       std::vector<double>& driftDistances) {
  StatusCode sc = StatusCode::SUCCESS;

  Gaudi::XYZPoint point = entryPoint + 0.5*(exitPoint - entryPoint);
  DeOTModule* module = this->findModule(point);
  if (module) {
    sc = module->calculateHits(entryPoint, exitPoint, channels, driftDistances);
  } else {
    MsgStream msg(msgSvc(), name());
    msg << "Module doesn't contain hit" << endmsg;
    sc = StatusCode::FAILURE;
  }

  return sc;
}

/// Find the station for a given channelID
DeOTStation* DeOTDetector::findStation(const OTChannelID aChannel) const {
  /// Find the station and return a pointer to the station from channel
  Stations::const_iterator iter = std::find_if(m_stations.begin(), m_stations.end(),
                                               bind(&DeOTStation::contains, _1, aChannel));
  return (iter != m_stations.end() ? (*iter) : 0);
}

/// Find the station for a given XYZ point
DeOTStation* DeOTDetector::findStation(const Gaudi::XYZPoint& aPoint) const {
  /// Find the station and return a pointer to the station from XYZ point
  Stations::const_iterator iter = std::find_if(m_stations.begin(), m_stations.end(),
                                               bind(&DetectorElement::isInside, _1, aPoint));
  return (iter != m_stations.end() ? (*iter) : 0);
}

/// Find the layer for a given t
// template <typename T>
// DeOTLayer* DeOTDetector::layer(const T& t)  const {
//   DeOTStation* s = station(t);
//   return (s == 0 ? 0 : s->layer(t));
// }

/// Find the layer for a given channelID
DeOTLayer* DeOTDetector::findLayer(const OTChannelID aChannel)  const {
  DeOTStation* s = findStation(aChannel);
  return (s == 0 ? 0 : s->findLayer(aChannel));
}

/// Find the layer for a given XYZ point
DeOTLayer* DeOTDetector::findLayer(const Gaudi::XYZPoint& aPoint) const {
  DeOTStation* s = findStation(aPoint);
  return (s == 0 ? 0 : s->findLayer(aPoint));
}

/// Find the quarter for a given channelID
DeOTQuarter* DeOTDetector::findQuarter(const OTChannelID aChannel) const {
  DeOTLayer* l = findLayer(aChannel);
  return (l == 0 ? 0 : l->findQuarter(aChannel));
}

/// Find the quarter for a given XYZ point
DeOTQuarter* DeOTDetector::findQuarter(const Gaudi::XYZPoint& aPoint) const {
  DeOTLayer* l = findLayer(aPoint);
  return (l == 0 ? 0 : l->findQuarter(aPoint));
}

/// Find the module for a given channelID
DeOTModule* DeOTDetector::findModule(const OTChannelID aChannel) const {
  DeOTQuarter* q = findQuarter(aChannel);
  return(q == 0 ? 0 : q->findModule(aChannel));
}

/// Find the module for a given XYZ point
DeOTModule* DeOTDetector::findModule(const Gaudi::XYZPoint& aPoint) const {
  DeOTQuarter* q = findQuarter(aPoint);
  return(q == 0 ? 0 : q->findModule(aPoint));
}

// const int DeOTDetector::sensitiveVolumeID( const Gaudi::XYZPoint& globalPoint ) const
// {
//   int tmpSenVolIDStation = 0;
//   int tmpSenVolIDLayer = 0;
//   int tmpSenVolIDQuarter = 0;
//   int tmpSenVolIDModule = 0;
//   int otSenVolID = 0;
//   std::vector<DeOTStation*>::const_iterator iterStation = m_stations.begin();
//   //std::vector<DeOtStation*>::const_iterator iterStation = 
//   // std::find_if(m_stations.begin(),m_stations.end(),_1->isInside(globalPos));
//   while ( iterStation != m_stations.end() &&
//           !( (*iterStation)->isInside( globalPos ) ) ) ++iterStation;
//   if ( iterStation != m_stations.end() ) {
//     tmpSenVolIDStation = ( ( (*iterStation)->stationID() ) << 30 ) & stationMask;
//     DeOTLayer* otLayer = (*iterStation)->layer( globalPos );
//     if ( otLayer != 0 ) {
//       tmpSenVolIDLayer = ( ( otLayer -> layerID() ) << 28 ) & layerMask;
//       DeOTQuarter* otQuarter = otLayer->quarter( globalPos );
//       if ( otQuarter != 0 ) {
// 	tmpSenVolIDQuarter = ( ( otQuarter->quarterID() ) << 26 ) & quarterMask;
// 	DeOTModule* otModule = otQuarter->module( globalPos );
// 	if ( otModule != 0 ) tmpSenVolIDModule =
// 			       ( otModule->moduleID() << 22 ) & moduleMask;
//       }
//     }
//     otSenVolID = tmpSenVolIDStation | tmpSenVolIDLayer | tmpSenVolIDQuarter | tmpSenVolIDModule;
//     return ( otSenVolID );
//   }
//   else { return -1 ; }
// }

double DeOTDetector::distanceAlongWire(const OTChannelID aChannel,
                                       double xHit, double yHit) const {
  /// Distance along wire
  double dist = 0.0;
  DeOTModule* module = this->findModule(aChannel);
  if (module == 0) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING
        << "DistanceAlongWire requested for module that does not exist "
        << endreq;
  } else {
    dist = module->distanceAlongWire(xHit, yHit);
  }
  return dist;
}

double DeOTDetector::propagationTime(const OTChannelID aChannel,
                                     const double x, const double y ) const {
  /// Calculate the propagation delay along the wire
  double distAlongWire = distanceAlongWire(aChannel, x, y);
  return distAlongWire * propagationDelay();
}

OTChannelID DeOTDetector::nextChannelLeft(const OTChannelID aChannel) const {
  DeOTModule* module = this->findModule(aChannel);
  int nextLeft = module->nextLeftStraw(aChannel.straw());
  return (nextLeft == 0) ?
    OTChannelID( 0u ) : OTChannelID( aChannel.station(),
                                     aChannel.layer(),
                                     aChannel.quarter(),
                                     aChannel.module(),
                                     nextLeft );
}

OTChannelID DeOTDetector::nextChannelRight(const OTChannelID aChannel) const {
  DeOTModule* module = this->findModule(aChannel);
  int nextRight = module->nextRightStraw(aChannel.straw());
  return (nextRight == 0) ?
    OTChannelID( 0u ) : OTChannelID( aChannel.station(),
                                     aChannel.layer(),
                                     aChannel.quarter(),
                                     aChannel.module(),
                                     nextRight );
}


/// Returns a Trajectory representing the wire identified by the LHCbID
/// The offset is zero for all OT Trajectories
std::auto_ptr<LHCb::Trajectory> DeOTDetector::trajectory( const LHCb::LHCbID& id,
                                                          const double /*offset*/ ) const
{
  std::auto_ptr<LHCb::Trajectory> traj;
  if( !id.isOT() ) {
    throw GaudiException( "The LHCbID is not of OT type!", "DeOTDetector.cpp",
                          StatusCode::FAILURE );
  }
  DeOTModule* aModule = findModule( id.otID() );
  if( aModule != 0 ) {
    // Offset hardcoded to 0. to eliminate warning about unused parameter
    traj = aModule->trajectory( id.otID(), 0. );
  } else {
    throw GaudiException( "Failed to find module", "DeOTDetector.cpp",
                          StatusCode::FAILURE );
  }
  return traj;
}
