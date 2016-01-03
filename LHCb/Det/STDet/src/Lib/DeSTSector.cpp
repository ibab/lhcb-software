#include "STDet/DeSTSector.h"
#include "STDet/StatusMap.h"

#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

#include <algorithm>

// Kernel
#include "Kernel/LineTraj.h"
#include "Kernel/LHCbID.h"
#include "Kernel/PiecewiseTrajectory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/GaudiException.h"

#include "GaudiKernel/IUpdateManagerSvc.h"
#include "STDet/DeSTSensor.h"

#include "Kernel/LHCbConstants.h"

/** @file DeSTSector.cpp
 *
 *  Implementation of class :  DeSTSector
 *
 *    @author Matthew Needham
 */

using namespace LHCb;

DeSTSector::DeSTSector( const std::string& name ) :
  DeSTBaseElement( name ),
  m_thickness(0.0),
  m_firstStrip(1),
  m_firstBeetle(1),
  m_id(0u),
  m_pitch(0.0),
  m_nStrip(0u),
  m_capacitance(0.0),
  m_stripLength(0.0),
  m_deadWidth(0.0),
  m_dxdy(0.0),
  m_dzdy(0.0),
  m_dy(0.0),
  m_angle(0.0),
  m_cosAngle(0.0),
  m_sinAngle(0.0),
  m_measEff(0.0),
  m_status(OK),
  m_statusString("Status"),
  m_versionString("DC06"),
  m_noiseString("Noise"),
  m_isStereo(false)
{
  // constructer (first strip means we number from 1)
}

DeSTSector::~DeSTSector() {
  // destructer
}

std::ostream& DeSTSector::printOut( std::ostream& os ) const{

  // stream to cout
  os << " Sector :  "  << name() << std::endl;
  os << " Nickname: " << m_nickname
     << "\n ID " << id()
     << "\n type  " << type()
     << "\n pitch " << m_pitch
     << "\n strip " << m_nStrip
     << "\n capacitance " << m_capacitance/Gaudi::Units::picofarad
     << "\n dead width " << m_deadWidth
     << "\n center " << globalCentre()
     << "\n Sector status " << sectorStatus()
     << "\n fraction active " << fractionActive()
     << "\n version " << m_versionString
     << std::endl;
  return os;
}

MsgStream& DeSTSector::printOut( MsgStream& os ) const{

  // stream to Msg service
  os << " Sector : \n "  << name() << std::endl;
  os   << " Nickname: " << m_nickname
       << "\n ID " << id()
       << "type \n " << type()
       << " pitch \n " << m_pitch
       << "n strip \n " << m_nStrip
       << " capacitance \n " << m_capacitance/Gaudi::Units::picofarad
       << "dead width \n " << m_deadWidth
       << "\n center " << globalCentre()
       << "\n fraction active " << fractionActive()
       << "\n version " << m_versionString
       << std::endl;

  return os;
}

StatusCode DeSTSector::initialize() {

  // initialize method

  StatusCode sc = DeSTBaseElement::initialize();
  if (sc.isFailure() ){
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "Failed to initialize detector element" << endmsg;
    return sc;
  }
  else {

    m_pitch = param<double>("pitch");
    m_nStrip =  param<int>("numStrips");
    m_capacitance = param<double>("capacitance");
    m_type = param<std::string>("type");


    // version
    if (exists("version") == true) m_versionString = param<std::string>("version");

    // guard ring
    m_deadWidth = param<double>("verticalGuardRing");

    m_noiseValues.assign(m_nStrip, 0);
    m_electronsPerADC.assign(m_nStrip, 1.);

    if (m_versionString != "DC06")
    {
      sc = registerCondition(this,m_statusString,
                             &DeSTSector::updateStatusCondition,true);
      if (sc.isFailure() ){
        MsgStream msg(msgSvc(), name() );
        msg << MSG::ERROR << "Failed to register status conditions" << endmsg;
        return StatusCode::FAILURE;
      }

      // Try to add the noise from the DB as well..
      // Can't test the version string, it's unfortunalety not trustable
      // it exists a DC07 version (why?) that doesn't contain Noise
      if (m_versionString != "DC07")
      {
        sc = registerCondition(this, m_noiseString,
                               &DeSTSector::updateNoiseCondition, true);

        if (sc.isFailure())
        {
          MsgStream msg(msgSvc(), name() );
          msg << MSG::ERROR << "Failed to register noise conditions" << endmsg;
          return StatusCode::FAILURE;
        }
      }

    } // !DC06
  }
  return StatusCode::SUCCESS;
}

double DeSTSector::noise(const LHCb::STChannelID& aChannel) const
{
  // check strip is valid
  if (!isStrip(aChannel.strip())) return 999;

  const Status theStatus = stripStatus(aChannel);

  // take ok strips
  if (theStatus == DeSTSector::OK) return m_noiseValues[aChannel.strip()-1u];

  // and pinholes...
  if (theStatus == DeSTSector::Pinhole) return m_noiseValues[aChannel.strip()-1u];

  return 999;
}

double DeSTSector::rawNoise(const LHCb::STChannelID& aChannel) const{
  return sqrt(noise(aChannel)*noise(aChannel) + cmNoise(aChannel)*cmNoise(aChannel));
}

double DeSTSector::rawSectorNoise() const{
  return sqrt(sectorNoise()*sectorNoise() + cmSectorNoise()*cmSectorNoise());
}

double DeSTSector::rawBeetleNoise(const unsigned int& beetle) const{
  return sqrt(beetleNoise(beetle)*beetleNoise(beetle) + cmBeetleNoise(beetle)*cmBeetleNoise(beetle));
}

double DeSTSector::rawPortNoise(const unsigned int& beetle, const unsigned int& port) const{
  return sqrt(portNoise(beetle,port)*portNoise(beetle,port) + cmPortNoise(beetle,port)*cmPortNoise(beetle,port));
}

double DeSTSector::sectorNoise() const
{
  const std::vector<DeSTSector::Status> statusVector = stripStatus();

  double sum(0.0), number(0.0);

  for (unsigned int chan(0); chan < m_nStrip; ++chan)
  {
    if ( statusVector[chan] == DeSTSector::OK ||
         statusVector[chan] == DeSTSector::Pinhole )
    {
      sum    += m_noiseValues[chan];
      number += 1.;
    }
  }

  if (number < 1.)
    return 999.99;
  else
  {
    MsgStream msg(msgSvc(), name() );
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << number << " strips out of " << nStrip()
          << " are not taken into account" << endmsg;
    return sum / number;
  }
}

double DeSTSector::beetleNoise(const unsigned int& beetle) const
{
  if (beetle > nBeetle())
   {
     MsgStream msg(msgSvc(), name() );
     msg << MSG::WARNING << "You asked for beetle " << beetle
         << " but there are " << nBeetle() << " of them" << endmsg;
     return 999.99;
   }
  else if (beetle == 0)
  {
    MsgStream msg(msgSvc(), name() );
    msg << MSG::WARNING << "You asked for beetle 0 but is starts at 1"
        << endmsg;
     return 999.99;
  }

  const std::vector<DeSTSector::Status> statusVector = stripStatus();

  double sum(0.), number(0.);

  for (unsigned int chan((beetle - 1) * LHCbConstants::nStripsInBeetle);
       chan < beetle * LHCbConstants::nStripsInBeetle; chan++)
  {
    if ( statusVector[chan] == DeSTSector::OK ||
         statusVector[chan] == DeSTSector::Pinhole )
    {
      sum    += m_noiseValues[chan];
      number += 1.;
    }
  }

  if (number < 1.)
    return 999.99;
  else
  {
    MsgStream msg(msgSvc(), name() );
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << number << " strips out of "
          << LHCbConstants::nStripsInBeetle
          << " are not taken into account" << endmsg;
    return sum / number;
  }
}

double DeSTSector::portNoise(const unsigned int& beetle,
                             const unsigned int& port) const
{
  if (beetle > nBeetle())
  {
    MsgStream msg(msgSvc(), name() );
    msg << MSG::WARNING << "You asked for beetle " << beetle
        << " but there are " << nBeetle() << " of them" << endmsg;
    return 999.99;
  }
  else if (beetle == 0)
  {
    MsgStream msg(msgSvc(), name() );
    msg << MSG::WARNING << "You asked for beetle 0 but is starts at 1"
        << endmsg;
    return 999.99;
  }

  if (port > 4)
  {
    MsgStream msg(msgSvc(), name() );
    msg << MSG::WARNING << "You asked for port " << port
        << " but there are 4 of them" << endmsg;
    return 999.99;
  }
  else if (port == 0)
  {
    MsgStream msg(msgSvc(), name() );
    msg << MSG::WARNING << "You asked for port 0 but is starts at 1"
        << endmsg;
    return 0.;
  }

  const std::vector<DeSTSector::Status> statusVector = stripStatus();

  double sum(0.), number(0.);

  for (unsigned int chan((beetle - 1) * LHCbConstants::nStripsInBeetle +
                         (port - 1) * LHCbConstants::nStripsInPort);
       chan < (beetle - 1) * LHCbConstants::nStripsInBeetle +
         port * LHCbConstants::nStripsInPort; chan++)
  {
    if ( statusVector[chan] == DeSTSector::OK ||
         statusVector[chan] == DeSTSector::Pinhole )
    {
      sum    += m_noiseValues[chan];
      number += 1.;
    }
  }

  if (number < 1.)
    return 999.99;
  else
  {
    MsgStream msg(msgSvc(), name() );
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << number << " strips out of "
          << LHCbConstants::nStripsInPort
          << " are not taken into account" << endmsg;
    return sum / number;
  }
}

void DeSTSector::setNoise(const unsigned int& strip, const double& value)
{
  Condition* aCon = condition(m_noiseString);
  if (aCon == 0){
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Failed to find status condition" << endmsg;
  }
  else
  {
    std::vector<double>& reference =
      aCon -> param< std::vector< double > >( "SectorNoise" );
    reference[strip-1u] = value;
    m_noiseValues[strip-1u] = value;
  }
}

void DeSTSector::setNoise(const std::vector<double>& values)
{
  Condition* aCon( condition(m_noiseString) );
  if (aCon == 0)
  {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Failed to find status condition" << endmsg;
  }
  else
  {
    std::vector<double>& reference =
      aCon -> param< std::vector< double > >( "SectorNoise" );
    reference.assign( values.begin(), values.end() );
    m_noiseValues = values;
  }
}

void DeSTSector::setCMNoise(const unsigned int& strip, const double& value)
{
  Condition* aCon = condition(m_noiseString);
  if (aCon == 0){
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Failed to find status condition" << endmsg;
  }
  else
  {
    std::vector<double>& reference =
      aCon -> param< std::vector< double > >( "cmNoise" );
    reference[strip-1u] = value;
    m_cmModeValues[strip-1u] = value;
  }
}

void DeSTSector::setCMNoise(const std::vector<double>& values)
{
  Condition* aCon( condition(m_noiseString) );
  if (aCon == 0)
  {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Failed to find status condition" << endmsg;
  }
  else
  {
    std::vector<double>& reference =
      aCon -> param< std::vector< double > >( "cmNoise" );
    reference.assign( values.begin(), values.end() );
    m_cmModeValues = values;
  }
}

void DeSTSector::setADCConversion(const std::vector<double>& values)
{
  Condition* aCon( condition(m_noiseString) );
  if (aCon == 0)
  {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Failed to find status condition" << endmsg;
  }
  else
  {
    std::vector<double>& reference =
      aCon -> param< std::vector< double > >( "electronsPerADC" );
    reference.assign( values.begin(), values.end() );
    m_electronsPerADC = values;
  }
}

double DeSTSector::cmNoise(const LHCb::STChannelID& aChannel) const
{
  // check strip is valid
  if (!isStrip(aChannel.strip())) return 999;

  const Status theStatus = stripStatus(aChannel);

  // take ok strips
  if (theStatus == DeSTSector::OK) return m_cmModeValues[aChannel.strip()-1u];

  // and pinholes...
  if (theStatus == DeSTSector::Pinhole) return m_cmModeValues[aChannel.strip()-1u];

  return 999;
}


double DeSTSector::cmSectorNoise() const
{
  const std::vector<DeSTSector::Status> statusVector = stripStatus();

  double sum(0.0), number(0.0);

  for (unsigned int chan(0); chan < m_nStrip; ++chan)
  {
    if ( statusVector[chan] == DeSTSector::OK ||
         statusVector[chan] == DeSTSector::Pinhole )
    {
      sum    += m_cmModeValues[chan];
      number += 1.;
    }
  }

  if (number < 1.)
    return 999.99;
  else
  {
    MsgStream msg(msgSvc(), name() );
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << number << " strips out of " << nStrip()
          << " are not taken into account" << endmsg;
    return sum / number;
  }
}

double DeSTSector::cmBeetleNoise(const unsigned int& beetle) const
{
  if (beetle > nBeetle())
   {
     MsgStream msg(msgSvc(), name() );
     msg << MSG::WARNING << "You asked for beetle " << beetle
         << " but there are " << nBeetle() << " of them" << endmsg;
     return 999.99;
   }
  else if (beetle == 0)
  {
    MsgStream msg(msgSvc(), name() );
    msg << MSG::WARNING << "You asked for beetle 0 but is starts at 1"
        << endmsg;
     return 999.99;
  }

  const std::vector<DeSTSector::Status> statusVector = stripStatus();

  double sum(0.), number(0.);

  for (unsigned int chan((beetle - 1) * LHCbConstants::nStripsInBeetle);
       chan < beetle * LHCbConstants::nStripsInBeetle; chan++)
  {
    if ( statusVector[chan] == DeSTSector::OK ||
         statusVector[chan] == DeSTSector::Pinhole )
    {
      sum    += m_cmModeValues[chan];
      number += 1.;
    }
  }

  if (number < 1.)
    return 999.99;
  else
  {
    MsgStream msg(msgSvc(), name() );
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << number << " strips out of "
          << LHCbConstants::nStripsInBeetle
          << " are not taken into account" << endmsg;
    return sum / number;
  }
}

double DeSTSector::cmPortNoise(const unsigned int& beetle,
                               const unsigned int& port) const
{
  if (beetle > nBeetle())
  {
    MsgStream msg(msgSvc(), name() );
    msg << MSG::WARNING << "You asked for beetle " << beetle
        << " but there are " << nBeetle() << " of them" << endmsg;
    return 999.99;
  }
  else if (beetle == 0)
  {
    MsgStream msg(msgSvc(), name() );
    msg << MSG::WARNING << "You asked for beetle 0 but is starts at 1"
        << endmsg;
    return 999.99;
  }

  if (port > 4)
  {
    MsgStream msg(msgSvc(), name() );
    msg << MSG::WARNING << "You asked for port " << port
        << " but there are 4 of them" << endmsg;
    return 999.99;
  }
  else if (port == 0)
  {
    MsgStream msg(msgSvc(), name() );
    msg << MSG::WARNING << "You asked for port 0 but is starts at 1"
        << endmsg;
    return 0.;
  }

  const std::vector<DeSTSector::Status> statusVector = stripStatus();

  double sum(0.), number(0.);

  for (unsigned int chan((beetle - 1) * LHCbConstants::nStripsInBeetle +
                         (port - 1) * LHCbConstants::nStripsInPort);
       chan < (beetle - 1) * LHCbConstants::nStripsInBeetle +
         port * LHCbConstants::nStripsInPort; chan++)
  {
    if ( statusVector[chan] == DeSTSector::OK ||
         statusVector[chan] == DeSTSector::Pinhole )
    {
      sum    += m_cmModeValues[chan];
      number += 1.;
    }
  }

  if (number < 1.)
    return 999.99;
  else
  {
    MsgStream msg(msgSvc(), name() );
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << number << " strips out of "
          << LHCbConstants::nStripsInPort
          << " are not taken into account" << endmsg;
    return sum / number;
  }
}


double DeSTSector::toADC(const double& e,
                         const LHCb::STChannelID& aChannel) const
{
  return toADC(e, aChannel.strip());
}

double DeSTSector::toADC(const double& e, const unsigned int& aStrip) const
{
  return e / m_electronsPerADC[aStrip-1];
}

double DeSTSector::toElectron(const double& val,
                              const LHCb::STChannelID& aChannel) const
{
  return toElectron(val, aChannel.strip());
}

double DeSTSector::toElectron(const double& val,
                              const unsigned int& aStrip) const
{
  return val * m_electronsPerADC[aStrip-1];
}

std::unique_ptr<LHCb::Trajectory>
DeSTSector::trajectory(const STChannelID& aChan, const double offset) const
{

  if (!contains(aChan)){
    MsgStream msg(msgSvc(), name() );
    msg << MSG::ERROR << "Failed to link to sector " << nickname()
        << " test strip  number "  << aChan.strip() << " strip "  << endmsg;
    throw GaudiException( "Failed to make trajectory",
                          "DeSTSector.cpp", StatusCode::FAILURE );
  }

  return createTraj(aChan.strip(), offset);
}

std::unique_ptr<LHCb::Trajectory> DeSTSector::trajectoryFirstStrip() const
{
  return createTraj(m_firstStrip,0.);
}

std::unique_ptr<LHCb::Trajectory> DeSTSector::trajectoryLastStrip() const
{
  return createTraj(nStrip(), 0.);
}

std::unique_ptr<LHCb::Trajectory> DeSTSector::createTraj(const unsigned int strip,
                                                       const double offset) const{

  // collect the individual traj
  const Sensors& theSensors = sensors();
  Sensors::const_iterator iterS = theSensors.begin();

  if (theSensors.size() == 1){
    // can just return a line traj
    return theSensors.front()->trajectory(strip,offset);
  }
  // return a piecewise traj
  std::unique_ptr<STTraj> traj{ new STTraj() };
  for (; iterS != theSensors.end(); ++iterS) {
    auto sensTraj = (*iterS)->trajectory(strip,offset);
    if (traj->numberOfPieces() == 0) {
      traj->append(sensTraj.release());
    }
    else {

      const double d1 = (sensTraj->beginPoint()-traj->endPoint()).mag2();
      const double d2 = (sensTraj->endPoint()-traj->beginPoint()).mag2();
      if (d1 < d2) {
        double mu = sensTraj->muEstimate(traj->endPoint());
        sensTraj->setRange(mu,sensTraj->endRange());
        traj->append(sensTraj.release());
      }
      else {
        const double mu = sensTraj->muEstimate(traj->beginPoint());
        sensTraj->setRange(sensTraj->beginRange(),mu);
        traj->prepend(sensTraj.release());
      }
    }
  } // loop
  return std::move(traj);
}

StatusCode DeSTSector::cacheInfo()
{
  auto firstTraj = createTraj(m_firstStrip,0);

  // get the start and end point. for piecewise trajectories, we
  // effectively make an approximation by a straight line.
  const Gaudi::XYZPoint g1 = firstTraj->beginPoint();
  const Gaudi::XYZPoint g2 = firstTraj->endPoint();

  const double activeWidth = m_sensors.front()->activeWidth();

  // direction
  Gaudi::XYZVector direction = g2 - g1;
  m_stripLength = sqrt(direction.Mag2());
  direction = direction.Unit();

  // cross with normal along z
  Gaudi::XYZVector zVec(0,0,1);
  Gaudi::XYZVector norm = direction.Cross(zVec);

  // trajectory of middle
  const Gaudi::XYZPoint g3 = g1 + 0.5*(g2 - g1);
  const Gaudi::XYZPoint g4 = g3 + activeWidth*norm ;

  // creating the 'fast' trajectories
  const Gaudi::XYZVector vectorlayer = (g4-g3).unit() * m_pitch ;
  const Gaudi::XYZPoint p0 = g3-0.5*m_stripLength*direction ;
  m_dxdy = direction.x()/direction.y() ;
  m_dzdy = direction.z()/direction.y() ;
  m_dy   = m_stripLength * direction.y() ;
  m_dp0di.SetX( vectorlayer.x() - vectorlayer.y() * m_dxdy ) ;
  m_dp0di.SetY( vectorlayer.y()  ) ;
  m_dp0di.SetZ( vectorlayer.z() - vectorlayer.y() * m_dzdy ) ;
  m_p0.SetX( p0.x() - p0.y() * m_dxdy ) ;
  m_p0.SetY( p0.y() ) ;
  m_p0.SetZ( p0.z() - p0.y() * m_dzdy ) ;

  // Update the stereo angle. We correct by 'pi' if necessary.
  Gaudi::XYZVector dir = direction ;
  if(dir.y()<0) dir *= -1 ;
  m_angle    = atan2(-dir.x(),dir.y()) ;
  m_cosAngle = cos( m_angle ) ;
  m_sinAngle = sin( m_angle ) ;

  return StatusCode::SUCCESS;

}

STChannelID DeSTSector::nextLeft(const STChannelID testChan) const
{
  if ((contains(testChan))&& (isStrip(testChan.strip()- 1u) == true)){
    return STChannelID(testChan.type(),
                       testChan.station(),
                       testChan.layer(),
                       testChan.detRegion(),
                       testChan.sector(),
                       testChan.strip() - 1u);
  }
  else {
    return LHCb::STChannelID(0u);
  }
}

STChannelID DeSTSector::nextRight(const LHCb::STChannelID testChan) const
{
  if ((contains(testChan) == true)&& (isStrip(testChan.strip()+ 1u) == true)){
    return STChannelID(testChan.type(),
                       testChan.station(),
                       testChan.layer(),
                       testChan.detRegion(),
                       testChan.sector(),
                       testChan.strip() + 1u);
  }
  else {
    return LHCb::STChannelID(0u);
  }
}


StatusCode DeSTSector::registerConditionsCallbacks(){

  // cache trajectories
  // initialize method
  MsgStream msg(msgSvc(), name() );

  if (sensors().empty()){
    msg << MSG::ERROR << "Sterile detector element ! No conditions registered" << endmsg;
    return StatusCode::FAILURE;
  }

  StatusCode sc = registerCondition(this,sensors().front(),&DeSTSector::cacheInfo,true);
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to register geometry condition for first child" << endmsg;
    return StatusCode::FAILURE;
  }

  sc = registerCondition(this,sensors().back(),&DeSTSector::cacheInfo,true);
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to register geometry condition for first child" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode DeSTSector::updateStatusCondition(){

  Condition* aCon = const_cast<Condition*>(statusCondition());
  if (aCon == 0){
    MsgStream msg(msgSvc(), name());
    msg << "failed to find status condition" << endmsg;
    return StatusCode::FAILURE;
  }

  const int tStatus = aCon->param<int>("SectorStatus");
  m_status = Status(tStatus);

  std::map<int,int> beetleMap = aCon->param<std::map<int,int> >("BeetleStatus");
  toEnumMap(beetleMap,m_beetleStatus);

  std::map<int,int> stripMap = aCon->param<std::map<int,int> >("StripStatus");
  toEnumMap(stripMap,m_stripStatus);

  if (aCon->exists("measuredEff") == true){
    m_measEff = aCon-> param<double >("measuredEff");
  }
  else {
    m_measEff = 1.0;
    aCon->addParam("measuredEff", m_measEff, "Measured sector Eff");
  }

  return StatusCode::SUCCESS;
}

StatusCode DeSTSector::updateNoiseCondition()
{
  Condition* aCon = condition(m_noiseString);
  if (aCon == 0){
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "failed to find noise condition" << endmsg;
    return StatusCode::FAILURE;
  }

  std::vector<double > tmpNoise = aCon -> param<std::vector< double > >("SectorNoise");

  if (tmpNoise.size() == m_nStrip)
    m_noiseValues.assign(tmpNoise.begin(), tmpNoise.end());
  else
  {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Size mismatch for SectorNoise" << endmsg;
    return StatusCode::FAILURE;
  }

  std::vector<double > tmpElectrons = aCon -> param<std::vector< double > >("electronsPerADC");

  if (tmpElectrons.size() == m_nStrip)
    m_electronsPerADC.assign(tmpElectrons.begin(), tmpElectrons.end());
  else
  {
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Size mismatch for electronsPerADC" << endmsg;
    return StatusCode::FAILURE;
  }

  if (aCon->exists("cmNoise") == true){
    std::vector <double > tmpCM =  aCon -> param<std::vector< double > >("cmNoise");
    if (tmpCM.size() == m_nStrip)
      m_cmModeValues.assign(tmpCM.begin(), tmpCM.end());
    else {
     MsgStream msg(msgSvc(), name());
     msg << MSG::ERROR << "Size mismatch for cmNoise" << endmsg;
     return StatusCode::FAILURE;
    }
  }
  else {
    // doesn't exists (MC early databases...)
    m_cmModeValues.assign(m_nStrip, 0.);
    //for (unsigned int i = 0; i < m_nStrip; ++i) m_cmModeValues[i] = 0.0;
    aCon->addParam("cmNoise", m_cmModeValues, "Common mode per sector");
  }

  return StatusCode::SUCCESS;
}

void DeSTSector::toEnumMap(const std::map<int,int>& input, DeSTSector::StatusMap& output) {
  output.clear();
  std::map<int,int>::const_iterator iterM = input.begin();
  for (; iterM != input.end(); ++iterM){
    output[iterM->first] = Status(iterM->second);
  } // iterM

}

DeSTSensor* DeSTSector::findSensor(const Gaudi::XYZPoint& point) const{
  // return pointer to the layer from point
  auto iter = std::find_if( m_sensors.begin(), m_sensors.end(),
                            [&](const DeSTSensor* s) {
                      return s->isInside(point);
                  } );
  return iter != m_sensors.end() ? *iter: nullptr;
}

bool DeSTSector::globalInActive(const Gaudi::XYZPoint& point,
                                Gaudi::XYZPoint tol) const{
  return std::any_of( m_sensors.begin(), m_sensors.end(),
                      [&](const DeSTSensor* s) {
                          return s->globalInActive(point,tol);
                      });
}

bool DeSTSector::globalInBondGap(const Gaudi::XYZPoint& point,
                                 double tol) const
{
  const DeSTSensor* aSensor =  findSensor(point);
  return (aSensor ?  aSensor->globalInBondGap(point, tol) : false );
}

double DeSTSector::fractionActive() const {

  // fraction of the sector that works
  unsigned int nActive = 0u;
  const std::vector<DeSTSector::Status> statusVector = stripStatus();
  std::vector<DeSTSector::Status>::const_iterator iter = statusVector.begin();
  for (; iter != statusVector.end(); ++iter){
    if ( *iter == DeSTSector::OK) ++nActive;
  }

  return nActive/double(nStrip());
}

void DeSTSector::setMeasEff(const double value){
  m_measEff = value;
  Condition* aCon = condition(m_statusString);
  if (aCon == 0){
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Failed to find status condition" << endmsg;
  } else {
    double& tvalue = aCon->param<double>("measuredEff");
    tvalue = double(value);
  }
}

void DeSTSector::setSectorStatus(const DeSTSector::Status& newStatus)
{
  m_status = newStatus;

  // Set the condition
  Condition* aCon = condition(m_statusString);
  if (aCon == 0){
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Failed to find status condition" << endmsg;
  } else {
    int& value = aCon->param<int>("SectorStatus");
    value = int(newStatus);
  }
}

void DeSTSector::setBeetleStatus(const unsigned int beetle,
                                 const DeSTSector::Status& newStatus){

  // update the beetle status properly...
  MsgStream msg(msgSvc(), name());

  if (sectorStatus() != DeSTSector::OK){
    // if the sector is not ok nothing to be done
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << "Sector is off anyway: set request ignored " << endmsg;
  }
  else {
    if (newStatus == DeSTSector::OK){
      // Lazarus walks...if we have an entry in the map delete it
      m_beetleStatus.erase(beetle);
      setStatusCondition("BeetleStatus", beetle, newStatus);
    }
    else {
      // death comes to this beetle, update the map
      if (std::find(::Status::validBeetleStates().begin(),
                    ::Status::validBeetleStates().end(),
                    newStatus) != ::Status::validBeetleStates().end() ){
        m_beetleStatus[beetle] = newStatus;
        setStatusCondition("BeetleStatus", beetle, newStatus);
      } // check is valid state
      else {
        msg << "Not a valid Beetle state: set request ignored " << endmsg;
      }
    }
  }
}

void DeSTSector::setStripStatus( const unsigned int strip,
                                 const DeSTSector::Status& newStatus )
{
  // update the strip status properly...
  MsgStream msg(msgSvc(), name());

  if(sectorStatus() != DeSTSector::OK || beetleStatus(strip) != DeSTSector::OK){
    // if the sector is not ok nothing to be done
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << "Sector/Beetle is off anyway: set request ignored "
          << endmsg;
  }
  else {
    if (newStatus == DeSTSector::OK){
      // Lazarus walks...if we have an entry in the map delete it
      m_stripStatus.erase(strip);
      setStatusCondition("StripStatus", strip, newStatus);
    }
    else {
      // death comes to this strip, update the map
      Status oldStatus = m_stripStatus.find(strip)->second;
      if (std::find(::Status::protectedStates().begin(),
                    ::Status::protectedStates().end(),
                    oldStatus) == ::Status::protectedStates().end() ){
        m_stripStatus[strip] = newStatus;
        setStatusCondition("StripStatus", strip, newStatus);
      }
      else {
        msg << "Strip in protected state: set request ignored " << endmsg;
      }
    }
  }
}

void DeSTSector::setStatusCondition( const std::string& type,
                                     const unsigned int entry,
                                     const DeSTSector::Status& newStatus )
{
  // Set the condition
  Condition* aCon = condition(m_statusString);
  if (aCon == 0){
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR << "Failed to find status condition" << endmsg;
  } else {
    std::map<int,int>& condMap =
      aCon->param<std::map<int,int> >(type);
    condMap[entry] = int(newStatus);
  }
}
