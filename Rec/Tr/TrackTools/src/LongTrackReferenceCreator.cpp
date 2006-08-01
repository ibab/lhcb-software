// $Id: LongTrackReferenceCreator.cpp,v 1.7 2006-08-01 12:43:07 erodrigu Exp $

// from GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

// Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/Measurement.h"
#include "Event/OTMeasurement.h"
#include "Event/StateTraj.h"

// CLHEP
#include "Kernel/PhysicalConstants.h"

// track tools
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Kernel/ITrajPoca.h"

// local
#include "LongTrackReferenceCreator.h"

using namespace LHCb;
using namespace Gaudi;

static const ToolFactory<LongTrackReferenceCreator>  s_factory;
const IToolFactory& LongTrackReferenceCreatorFactory = s_factory;

LongTrackReferenceCreator::LongTrackReferenceCreator(const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent):
  GaudiTool(type, name, parent)
{ 
  // constructer
  declareInterface<ITrackManipulator>(this);
  declareProperty( "SetLRAmbiguities", m_setLRAmbiguities = false  );
};

LongTrackReferenceCreator::~LongTrackReferenceCreator(){
  // destructer
}

StatusCode LongTrackReferenceCreator::initialize()
{

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  // extrapolator
  m_extrapolator = tool<ITrackExtrapolator>("TrackFastParabolicExtrapolator");

  // Retrieve the magnetic field and the poca tool
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );
  m_poca = tool<ITrajPoca>( "TrajPoca" );

 
  return StatusCode::SUCCESS;
};


StatusCode LongTrackReferenceCreator::execute(const LHCb::Track& aTrack) const{

  // get the starting states in velo and T
  if (aTrack.hasStateAt(LHCb::State::EndVelo) == false){
    return Warning("No Velo State",StatusCode::FAILURE);
  }
  LHCb::State vState = aTrack.stateAt(LHCb::State::EndVelo);

  // state at T 
  if (aTrack.hasStateAt(LHCb::State::AtT) == false){
    return Warning("No T State",StatusCode::FAILURE);
  }
  LHCb::State tState = aTrack.stateAt(LHCb::State::AtT);

  // reset velo Q/p to T one
  vState.setQOverP(tState.qOverP());

  typedef std::vector<LHCb::Measurement*> MeasContainer;
  const MeasContainer& aCont = aTrack.measurements();

  if (aCont.size() == 0){
    return Warning("Tool called for track without measurements",
                   StatusCode::FAILURE);
  }

  MeasContainer::const_iterator iterM = aCont.begin();
  for ( ; iterM != aCont.end(); ++iterM) {
    
    if ( (*iterM)->type() == Measurement::IT  ||
         (*iterM)->type() == Measurement::OT ) {
      addReference(*iterM,tState);
    }
    else {
      addReference(*iterM,vState);
    }
    
  } //iterM

  return StatusCode::SUCCESS;
}

void LongTrackReferenceCreator::addReference( LHCb::Measurement* meas, 
                                              LHCb::State& aState ) const
{
  // Get the measurement trajectory representing the centre of gravity
  m_extrapolator->propagate(aState,meas->z());
  meas->setRefVector(aState.stateVector());

  // Add the L/R ambiguity
  if ( m_setLRAmbiguities && meas->type() == Measurement::OT ) {
    XYZVector distance;
    XYZVector bfield;
    m_pIMF -> fieldVector( aState.position(), bfield );
    StateTraj stateTraj = StateTraj( aState, bfield );
    double s1 = 0.0;
    double s2 = (meas->trajectory()).arclength( stateTraj.position(s1) );
    m_poca->minimize(stateTraj, s1, meas->trajectory(), s2, distance, 
                     20*Gaudi::Units::mm);
    int ambiguity = ( distance.x() > 0.0 ) ? 1 : -1 ;

    OTMeasurement* otmeas = dynamic_cast<OTMeasurement*>(meas);
    otmeas->setAmbiguity( ambiguity );
  }

}
