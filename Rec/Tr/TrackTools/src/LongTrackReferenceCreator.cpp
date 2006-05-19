#include "GaudiKernel/ToolFactory.h"

#include "LongTrackReferenceCreator.h"

// Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/Measurement.h"

// CLHEP
#include "Kernel/PhysicalConstants.h"


// track tools
#include "TrackInterfaces/ITrackExtrapolator.h"


using namespace LHCb;

static const ToolFactory<LongTrackReferenceCreator>  s_factory;
const IToolFactory& LongTrackReferenceCreatorFactory = s_factory;

LongTrackReferenceCreator::LongTrackReferenceCreator(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
GaudiTool(type, name, parent)
{ 
  // constructer
  declareInterface<ITrackManipulator>(this);
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
 
  return StatusCode::SUCCESS;
};


StatusCode LongTrackReferenceCreator::execute(const LHCb::Track& aTrack) const{

  // get the starting states in velo and T
  LHCb::State* vState = aTrack.stateAt(LHCb::State::EndVelo).clone();

  // state at T 
  LHCb::State* tState = aTrack.stateAt(LHCb::State::AtT).clone();
  

  // reset velo Q/p to T one
  vState->setQOverP(tState->qOverP());

  typedef std::vector<LHCb::Measurement*> MeasContainer;
  const MeasContainer& aCont = aTrack.measurements();

  if (aCont.size() != 0){
    return Warning("Tool called for track without measurements",StatusCode::FAILURE);
  }

  for (MeasContainer::const_iterator iterM = aCont.begin(); iterM != aCont.end(); ++iterM){
    
    if (((*iterM)->type() == Measurement::IT ) ||(*iterM)->type() == Measurement::OT){
      addReference(*iterM,*tState);
    }
    else {
      addReference(*iterM,*vState);
    }
    
  } //iterM

  // cleanup
  delete tState; 
  delete vState;

  return StatusCode::SUCCESS;
}

void LongTrackReferenceCreator::addReference(LHCb::Measurement* meas, LHCb::State& aState) const{

  // Get the measurement trajectory representing the centre of gravity
  m_extrapolator->propagate(aState,meas->z());
  meas->setRefVector(aState.stateVector());

}
