// $Id: DeOTDetector.cpp,v 1.28 2007-02-04 11:34:36 janos Exp $
/// Kernel
#include "Kernel/LHCbID.h"
#include "Kernel/OTChannelID.h"
#include "Kernel/Point3DTypes.h"
#include "Kernel/Trajectory.h"

// GaudiKernel
#include "GaudiKernel/GaudiException.h"

/// OTDet
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTStation.h"
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTQuarter.h"
#include "OTDet/DeOTModule.h"

/// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

// Kernel/LHCbKernel


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
  m_mapIDStation(),
  m_mapIDLayer(),
  m_mapIDQuarter(),
  m_mapIDModule(),
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
  m_mapIDStation.reserve(3);
  m_mapIDLayer.reserve(12);
  m_mapIDQuarter.reserve(48);
  m_mapIDModule.reserve(432);
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
  StatusCode sc = DetectorElement::initialize();

  if( sc.isFailure() ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "Failed to initialize DetectorElement" << endreq;
    return sc ;
  }
   
  // loop over stations
  typedef IDetectorElement::IDEContainer::const_iterator Iter;
  for (Iter iS = this->childBegin(); iS != this->childEnd(); ++iS) {
    DeOTStation* station = dynamic_cast<DeOTStation*>(*iS);
    if (station) {
      /// fill sation vector
      m_stations.push_back(station);
      /// map station id to station
      m_mapIDStation.insert((station->elementID()).station(), station);
    }
    //loop over layers
    for (Iter iL = (*iS)->childBegin(); iL!= (*iS)->childEnd(); ++iL) {
      DeOTLayer* layer = dynamic_cast<DeOTLayer*>(*iL);
      if (layer) {
        /// fill layer vector
        m_layers.push_back(layer);
        /// map layer id to layer
        m_mapIDLayer.insert((layer->elementID()).uniqueLayer(), layer);
      }
      // loop over quarters
      for (Iter iQ = (*iL)->childBegin(); iQ != (*iL)->childEnd(); ++iQ) {
        DeOTQuarter* quarter = dynamic_cast<DeOTQuarter*>(*iQ);
        if (quarter) {
          /// fill quarter vector
          m_quarters.push_back(quarter);
          /// map quarter id to quarter
          m_mapIDQuarter.insert((quarter->elementID()).uniqueQuarter(), quarter);
        }
        // loop over modules
        for (Iter iM = (*iQ)->childBegin(); iM != (*iQ)->childEnd(); ++iM) {
          DeOTModule* module = dynamic_cast<DeOTModule*>(*iM);
          if (module) {
            // get # of channels
            unsigned int channels = module->nChannels();
            m_nChannels += channels;
            if (channels > m_nMaxChanInModule) m_nMaxChanInModule = channels;
            // fill mod vector
            m_modules.push_back(module);
            /// map module id to module
            m_mapIDModule.insert((module->elementID()).uniqueModule(), module);
          }
        } // modules
      } // quarters
    } // layers
  } // stations
  
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

/// Find the station for a given channelID
DeOTStation* DeOTDetector::findStation(const OTChannelID aChannel) const {
  /// Find the station and return a pointer to the station from channel
  MapIDStation::iterator iS = m_mapIDStation.find(aChannel.station());
  return (iS != m_mapIDStation.end() ? iS->second: 0);
}

/// Find the station for a given XYZ point
DeOTStation* DeOTDetector::findStation(const Gaudi::XYZPoint& aPoint) const {
  /// Find the station and return a pointer to the station from XYZ point
  Stations::const_iterator iS = std::find_if(m_stations.begin(), m_stations.end(),
                                             bind(&DetectorElement::isInside, _1, aPoint));
  return (iS != m_stations.end() ? (*iS) : 0);
}

/// Find the layer for a given channelID
DeOTLayer* DeOTDetector::findLayer(const OTChannelID aChannel)  const {
  MapIDLayer::iterator iL = m_mapIDLayer.find(aChannel.uniqueLayer());
  return (iL != m_mapIDLayer.end() ? iL->second: 0);
}

/// Find the layer for a given XYZ point
DeOTLayer* DeOTDetector::findLayer(const Gaudi::XYZPoint& aPoint) const {
  DeOTStation* s = findStation(aPoint);
  return (s == 0 ? 0 : s->findLayer(aPoint));
}

/// Find the quarter for a given channelID
DeOTQuarter* DeOTDetector::findQuarter(const OTChannelID aChannel) const {
  MapIDQuarter::iterator iQ = m_mapIDQuarter.find(aChannel.uniqueQuarter());
  return (iQ != m_mapIDQuarter.end() ? iQ->second: 0);
}

/// Find the quarter for a given XYZ point
DeOTQuarter* DeOTDetector::findQuarter(const Gaudi::XYZPoint& aPoint) const {
  DeOTLayer* l = findLayer(aPoint);
  return (l == 0 ? 0 : l->findQuarter(aPoint));
}

/// Find the module for a given channelID
DeOTModule* DeOTDetector::findModule(const OTChannelID aChannel) const {
  MapIDModule::iterator iM = m_mapIDModule.find(aChannel.uniqueModule());
  return (iM != m_mapIDModule.end() ? iM->second: 0);
}

/// Find the module for a given XYZ point
DeOTModule* DeOTDetector::findModule(const Gaudi::XYZPoint& aPoint) const {
  DeOTQuarter* q = findQuarter(aPoint);
  return(q == 0 ? 0 : q->findModule(aPoint));
}

const int DeOTDetector::sensitiveVolumeID( const Gaudi::XYZPoint& aPoint ) const {
  DeOTDetector* nonConstThis = const_cast<DeOTDetector*>(this);
  DeOTModule* m = nonConstThis->findModule(aPoint);
  
  if (m==0) return -1;
  
  return m->elementID();
}

double DeOTDetector::distanceAlongWire(const OTChannelID aChannel,
                                       double xHit, double yHit) const {
  DeOTModule* aModule = findModule(aChannel);
  if (!aModule) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "DistanceAlongWire requested for module that does not exist " << endreq;
    return 0.0;
  }
  
  return (aModule->distanceAlongWire(xHit, yHit));
}

double DeOTDetector::propagationTime(const OTChannelID aChannel,
                                     const double x, const double y ) const {
  /// Calculate the propagation delay along the wire
  return (distanceAlongWire(aChannel, x, y)*propagationDelay());
}

OTChannelID DeOTDetector::nextChannelLeft(const OTChannelID aChannel) const {
  DeOTModule* aModule = findModule(aChannel);
  int nextLeft = aModule->nextLeftStraw(aChannel.straw());
  return (nextLeft == 0u) ? OTChannelID( 0u ) : OTChannelID( aChannel.station(),
                                                             aChannel.layer(),
                                                             aChannel.quarter(),
                                                             aChannel.module(),
                                                             nextLeft );
}

OTChannelID DeOTDetector::nextChannelRight(const OTChannelID aChannel) const {
  DeOTModule* aModule = findModule(aChannel);
  int nextRight = aModule->nextRightStraw(aChannel.straw());
  return (nextRight == 0u) ? OTChannelID( 0u ) : OTChannelID( aChannel.station(),
                                                              aChannel.layer(),
                                                              aChannel.quarter(),
                                                              aChannel.module(),
                                                              nextRight );
}

std::auto_ptr<LHCb::Trajectory> DeOTDetector::trajectoryFirstWire(const LHCb::LHCbID& id, 
								  int monolayer) const {
  if (!id.isOT()) {
    throw GaudiException("The LHCbID is not of OT type!", "DeOTDetector.cpp",
			 StatusCode::FAILURE);
  }
  
  DeOTModule* aModule = findModule(id.otID());
  if (!aModule) {
    throw GaudiException("Failed to find module", "DeOTDetector.cpp",
			 StatusCode::FAILURE);
  } 
  
  return std::auto_ptr<LHCb::Trajectory>(aModule->trajectoryFirstWire(monolayer));
}

std::auto_ptr<LHCb::Trajectory> DeOTDetector::trajectoryLastWire(const LHCb::LHCbID& id, 
								 int monolayer) const {
  if (!id.isOT()) {
    throw GaudiException("The LHCbID is not of OT type!", "DeOTDetector.cpp",
			 StatusCode::FAILURE);
  }
  
  DeOTModule* aModule = findModule(id.otID());
  if (!aModule) {
    throw GaudiException("Failed to find module", "DeOTDetector.cpp",
			 StatusCode::FAILURE);
  }
  
  return std::auto_ptr<LHCb::Trajectory>(aModule->trajectoryLastWire(monolayer));
}

/// Returns a Trajectory representing the wire identified by the LHCbID
/// The offset is zero for all OT Trajectories
std::auto_ptr<LHCb::Trajectory> DeOTDetector::trajectory(const LHCb::LHCbID& id,
							 const double /*offset*/) const {
  if (!id.isOT()) {
    throw GaudiException("The LHCbID is not of OT type!", "DeOTDetector.cpp",
			 StatusCode::FAILURE);
  }
  
  DeOTModule* aModule = findModule(id.otID());
  if(!aModule) {
    throw GaudiException("Failed to find module", "DeOTDetector.cpp",
			 StatusCode::FAILURE);
  }
  
  // Offset hardcoded to 0. to eliminate warning about unused parameter
  return std::auto_ptr<LHCb::Trajectory>(aModule->trajectory(id.otID(),0));
}
