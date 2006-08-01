// $Id: TsaSeedReferenceCreator.cpp,v 1.1 2006-08-01 09:46:18 mneedham Exp $

// from GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"



// Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/Measurement.h"
#include "Event/OTMeasurement.h"
#include "Event/StateTraj.h"

// CLHEP
#include "Kernel/PhysicalConstants.h"
#include "Kernel/LineTraj.h"

// track tools
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Kernel/ITrajPoca.h"

// local
#include "TsaSeedReferenceCreator.h"

using namespace LHCb;
using namespace Gaudi;


DECLARE_TOOL_FACTORY( TsaSeedReferenceCreator );


TsaSeedReferenceCreator::TsaSeedReferenceCreator(const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent):
  GaudiTool(type, name, parent),
  m_configured(false)
{ 
  // constructer
  declareInterface<ITrackManipulator>(this);
  declareProperty( "SetLRAmbiguities", m_setLRAmbiguities = false  );
};

TsaSeedReferenceCreator::~TsaSeedReferenceCreator(){
  // destructer
}

StatusCode TsaSeedReferenceCreator::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  // Retrieve the magnetic field and the poca tool
  m_poca = tool<ITrajPoca>( "TrajPoca" );
 
  incSvc()->addListener( this, IncidentType::BeginEvent );

  return StatusCode::SUCCESS;
};

StatusCode TsaSeedReferenceCreator::execute(const LHCb::Track& aTrack) const{

  // get the input - seeds
  if (!m_configured) initEvent();

  typedef std::vector<LHCb::Measurement*> MeasContainer;
  const MeasContainer& aCont = aTrack.measurements();

  if (aCont.size() == 0){
    return Warning("Tool called for track without measurements",
                   StatusCode::FAILURE);
  }

  const SeedTrack* seedTrack = m_seeds->object(aTrack.key());
  if (seedTrack == 0){
    return Warning("No seed track !", StatusCode::FAILURE);
  } 

  MeasContainer::const_iterator iterM = aCont.begin();
  for ( ; iterM != aCont.end(); ++iterM) {
    addReference(*iterM,seedTrack, aTrack.firstState().qOverP());
  } //iterM
   
  return StatusCode::SUCCESS;
}

void TsaSeedReferenceCreator::addReference( LHCb::Measurement* meas, 
                                        const SeedTrack* aTrack, 
                                        const double qOverP ) const
{

  const double z  = meas->z();   

  Gaudi::TrackVector stateVec = Gaudi::TrackVector();
  stateVec(0) = aTrack->x(z,0.);
  stateVec(1) = aTrack->y(z,0.);
  stateVec(2) = aTrack->xSlope(z,0.);
  stateVec(3) = aTrack->sy();
  stateVec(4) = qOverP;  

  // make a line traj
  Gaudi::XYZVector vec = Gaudi::XYZVector(stateVec(2),stateVec(3),1.);
  Gaudi::XYZPoint point = Gaudi::XYZPoint(stateVec(0),stateVec(1),z);
  LineTraj lTraj(point,vec,std::make_pair(-20., 20.));

  Gaudi::XYZVector distance;

  double s1 = 0.0;
  double s2 = (meas->trajectory()).arclength( lTraj.position(s1) );
  m_poca->minimize(lTraj, s1, meas->trajectory(), s2, distance, 
                   20*Gaudi::Units::mm);

  if ( m_setLRAmbiguities && meas->type() == Measurement::OT ) {
    int ambiguity = ( distance.x() > 0.0 ) ? 1 : -1 ;
    OTMeasurement* otmeas = dynamic_cast<OTMeasurement*>(meas);
    otmeas->setAmbiguity( ambiguity );
  }

  // update the track parameters with a better guess of the poca
  Gaudi::XYZPoint p1 = meas->trajectory().position(s2);
  Gaudi::XYZPoint p2 = lTraj.position(s1);

  Gaudi::XYZPoint p3 = p1 + 0.5*(p2-p1);

  stateVec(0) = p3.x();
  stateVec(1) = p3.y();
  meas->setZ(p3.z());

  meas->setRefVector(stateVec);

}

void TsaSeedReferenceCreator::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { m_configured = false; }
}

void TsaSeedReferenceCreator::initEvent() const{

 m_configured = true;
 m_seeds = get<SeedTracks>(SeedTrackLocation::Default) ;
}
