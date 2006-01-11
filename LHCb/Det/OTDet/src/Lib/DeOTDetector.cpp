// $Id: DeOTDetector.cpp,v 1.16 2006-01-11 09:29:15 janos Exp $

// MathCore
#include "Kernel/Point3DTypes.h"
#include "Kernel/SystemOfUnits.h"

// OTDet
#include "OTDet/DeOTDetector.h"

/** @file DeOTDetector.cpp
 *
 *  Implementation of class :  DeOTDetector
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl
 */

using namespace LHCb;

DeOTDetector::DeOTDetector( const std::string& name ) :
  DetectorElement( name ),
  m_resolution(0.0),
  m_propagationDelay(0.0),
  m_maxDriftTime(0.0),
  m_maxDriftTimeCor(0.0),
  m_deadTime(0.0),
  m_cellRadius(0.0),
  m_numStations(0),
  m_firstOTStation(0),
  m_stations(),
  m_layers(),
  m_modules(),
  m_nChannels(0),
  m_nMaxChanInModule(0)
{ }

DeOTDetector::~DeOTDetector()
{
}

const CLID& DeOTDetector::clID () const 
{ 
  return DeOTDetector::classID() ; 
}

StatusCode DeOTDetector::initialize() 
{
  MsgStream msg( msgSvc(), name() );

  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) { 
    msg << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
    return sc ; 
  }

  // get resolution parameters
  m_resolution = param<double>("resolution");

  // get the rt relation parameters
  m_propagationDelay = param<double>("propagationDelay");
  m_maxDriftTime = param<double>("maxDriftTime");
  m_maxDriftTimeCor = param<double>("maxDriftTimeCor");
  m_cellRadius = param<double>("cellRadius");

  // get the channel dead time
  m_deadTime = param<double>("deadTime");
  
  // get station number parameters
  m_numStations = param<int>("numStations");
  m_firstOTStation = param<int>("firstOTStation");

  m_nMaxChanInModule = 0;
  m_nChannels = 0;
  // loop over stations
  IDetectorElement::IDEContainer::const_iterator iStation;
  for (iStation = this->childBegin(); this->childEnd() != iStation; 
       ++iStation ) {
    DeOTStation* otStation = dynamic_cast<DeOTStation*>(*iStation);
    if ( otStation) {
      m_stations.push_back(otStation);
    }
    //loop over layers
    IDetectorElement::IDEContainer::const_iterator iLayer;
    for (iLayer = (*iStation)->childBegin(); iLayer != (*iStation)->childEnd();
         ++iLayer) {  
      DeOTLayer* otLayer = dynamic_cast<DeOTLayer*>(*iLayer);
      if ( otLayer) {
        m_layers.push_back(otLayer);
      }
      // loop over quarters
      IDetectorElement::IDEContainer::const_iterator iQuarter;
      for (iQuarter = (*iLayer)->childBegin(); 
           iQuarter != (*iLayer)->childEnd(); ++iQuarter) {    
        DeOTQuarter* otQuarter = dynamic_cast<DeOTQuarter*>(*iQuarter);
        if ( otQuarter ) {
          m_quarters.push_back(otQuarter);
        }
        // loop over modules      
        IDetectorElement::IDEContainer::const_iterator iModule;
        for (iModule = (*iQuarter)->childBegin(); 
             iModule != (*iQuarter)->childEnd(); ++iModule) {    
          DeOTModule* otModule = dynamic_cast<DeOTModule*>(*iModule);
          if ( otModule ) {
            unsigned int channels = otModule->nChannels();
            m_nChannels += channels;
            m_modules.push_back(otModule);
            if (channels > m_nMaxChanInModule) m_nMaxChanInModule = channels;
          }        
        } // modules
      } // quarters
    } // layers
  } // stations

  msg << MSG::DEBUG << "initialize DetectorElement succeeded!!!" << endreq;
  return sc;
}

StatusCode DeOTDetector::calculateHits(const Gaudi::XYZPoint& entryPoint,
                                       const Gaudi::XYZPoint& exitPoint,
                                       std::vector<OTChannelID>& channels,
                                       std::vector<double>& driftDistances)

{
  StatusCode sc = StatusCode::SUCCESS; 

  Gaudi::XYZPoint point = entryPoint + 0.5*(exitPoint - entryPoint);  
  DeOTModule* otModule = this->module(point);
  if (otModule) {
    sc = otModule->calculateHits(entryPoint, exitPoint, 
                                 channels, driftDistances);
  } else {
    MsgStream msg(msgSvc(), name());
    msg << "Unable to find module." << endmsg;
    sc = StatusCode::FAILURE;
  }
  
  return sc;
}


DeOTStation* DeOTDetector::station(unsigned int stationID) const
{  
  // Find the station
  DeOTStation* otStation = 0;
  std::vector<DeOTStation*>::const_iterator iterStation = m_stations.begin();
  while ( iterStation != m_stations.end() &&
          !( (*iterStation)->stationID() == stationID ) )
    ++iterStation;

  if ( iterStation != m_stations.end()) otStation = *iterStation;
  return otStation;
}


DeOTLayer* DeOTDetector::layer(OTChannelID aChannel) const
{
  // Find the layer
  DeOTLayer* otLayer = 0;
  DeOTStation* otStation = this->station( aChannel.station() );
  if ( otStation != 0 ) otLayer = otStation->layer( aChannel.layer() );
  return otLayer;
}


DeOTModule* DeOTDetector::module(OTChannelID aChannel) const
{
  // Find the module
  DeOTModule* otModule = 0;
  std::vector<DeOTStation*>::const_iterator iterStation = m_stations.begin();
  
  while ( iterStation != m_stations.end() &&
          !( (*iterStation)->stationID() == aChannel.station() ) )
    ++iterStation;

  if ( iterStation != m_stations.end() ) {

     DeOTLayer* otLayer = (*iterStation)->layer( aChannel.layer() ); 
    if (otLayer != 0) {

      DeOTQuarter* otQuarter = otLayer->quarter( aChannel.quarter() );
      if (otQuarter != 0) otModule = otQuarter->module( aChannel.module() );
    }
  }
  return otModule;
}


DeOTModule* DeOTDetector::module(const Gaudi::XYZPoint& point) const
{
  DeOTModule* otModule = 0;
  std::vector<DeOTStation*>::const_iterator iterStation = m_stations.begin();
  while ( iterStation != m_stations.end() &&
          !( (*iterStation)->isInside(point) ) ) ++iterStation;

  if ( iterStation != m_stations.end() ) {
    DeOTLayer* otLayer = (*iterStation)->layer( point );
    if ( otLayer != 0 ) otModule = otLayer->module( point );
  }
  return otModule;
}

const int DeOTDetector::sensitiveVolumeID( const Gaudi::XYZPoint& globalPos ) const
{
  int tmpSenVolIDStation = 0;
  int tmpSenVolIDLayer = 0;
  int tmpSenVolIDQuarter = 0;
  int tmpSenVolIDModule = 0;
  int otSenVolID = 0;
  
  
  std::vector<DeOTStation*>::const_iterator iterStation = m_stations.begin();
  while ( iterStation != m_stations.end() &&
          !( (*iterStation)->isInside( globalPos ) ) ) ++iterStation;

  if ( iterStation != m_stations.end() ) {
    tmpSenVolIDStation = ( ( (*iterStation)->stationID() ) << 30 ) & stationMask;
    DeOTLayer* otLayer = (*iterStation)->layer( globalPos );
    
    if ( otLayer != 0 ) {
      tmpSenVolIDLayer = ( ( otLayer -> layerID() ) << 28 ) & layerMask;
      DeOTQuarter* otQuarter = otLayer->quarter( globalPos );
      
      if ( otQuarter != 0 ) {
	tmpSenVolIDQuarter = ( ( otQuarter->quarterID() ) << 26 ) & quarterMask;
	DeOTModule* otModule = otQuarter->module( globalPos );
	
	if ( otModule != 0 ) tmpSenVolIDModule = 
			       ( otModule->moduleID() << 22 ) & moduleMask;
      }
    }
    otSenVolID = tmpSenVolIDStation | tmpSenVolIDLayer | tmpSenVolIDQuarter | tmpSenVolIDModule;
    return ( otSenVolID );
  }
  else { return -1 ; }
}

double DeOTDetector::distanceAlongWire(OTChannelID channelID,
                                       double xHit, double yHit) const 
{
  // distance along wire
  double dist = 0.0;
  DeOTModule* otModule = this->module(channelID);
  if (otModule == 0) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING
        << "DistanceAlongWire requested for module that does not exist "
        << endreq;
  } else {
    dist = otModule->distanceAlongWire(xHit, yHit);
  }
  return dist;
}


double DeOTDetector::propagationTime(const OTChannelID aChannel, 
                                     const double x, const double y ) const
{
  // Calculate the propagation delay along the wire
  double distAlongWire = distanceAlongWire(aChannel, x, y);

  return distAlongWire * propagationDelay();
}


OTChannelID DeOTDetector::nextChannelRight(OTChannelID aChannel) const
{
  DeOTModule* otModule = this->module(aChannel);
  int nextRight = otModule->nextRightStraw( aChannel.straw() );
  return (nextRight == 0) ? 
    OTChannelID( 0 ) : OTChannelID( aChannel.station(), 
                                    aChannel.layer(),
                                    aChannel.quarter(),
                                    aChannel.module(), 
                                    nextRight );  
} 


OTChannelID DeOTDetector::nextChannelLeft(OTChannelID aChannel) const
{
  DeOTModule* otModule = this->module(aChannel);
  int nextLeft = otModule->nextLeftStraw( aChannel.straw() );
  return (nextLeft == 0) ? 
    OTChannelID( 0 ) : OTChannelID( aChannel.station(), 
                                    aChannel.layer(),
                                    aChannel.quarter(),
                                    aChannel.module(),
                                    nextLeft );  
}


double DeOTDetector::driftTime(const double driftDist,const double by) const
{
  // r-t relation with correction for the magnetic field
  // get max drift time 
  double maxDriftTime = maxDriftTimeFunc( by );

  // convert r to t - hack as drift dist can > rCell
  double driftTime;
  if ( fabs(driftDist) < m_cellRadius ) {
    driftTime = (fabs(driftDist) / m_cellRadius) * maxDriftTime;
  } else {
    driftTime = maxDriftTime;
  }
  return driftTime;
}


double DeOTDetector::driftDistance( const double driftTime, 
                                    const double by ) const
{
  // inverse r-t relation with correction for the magnetic field
  // get max drift time with correction for magnetic field
  double maxDriftTime = maxDriftTimeFunc(by);

  // inverse r-t relation
  return driftTime * m_cellRadius / maxDriftTime;  
}
