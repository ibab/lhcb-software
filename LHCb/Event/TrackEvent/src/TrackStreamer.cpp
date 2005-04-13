#include "Event/TrackStreamer.h"
#include "Event/TrackKeys.h"

std::ostream& str::track(const Track& track, std::ostream& os) {

  os << str::trackHeader(track,os) << std::endl;
  os << str::trackFields(track,os) << std::endl;
  os << str::states(track.states(),os) << std::endl;
  os << str::lhcbIDs(track.lhcbIDs(),os);
  return os;
}

std::ostream& str::trackHeader(const Track& track, std::ostream& os) {

  os << " chi2  \t" << track.chi2() << std::endl;
  os << " ndof  \t" << track.nDoF() << std::endl;
  os << " physics state \t"; str::state(track.physicsState(),os);
  os << std::endl;
  os << " flags \t " << track.flags() << std::endl;
  os << " nstates \t " << track.states().size() << std::endl;
  os << " nIDs \t " << track.lhcbIDs().size();
  return os;
}


std::ostream& str::states(const std::vector<State*>& states,  
			  std::ostream& os) {
  for (std::vector<State*>::const_iterator it = states.begin(); 
       it != states.end(); it++) {
    const State& state = *(*it);
    str::state(state,os); os << std::endl;
  }
  return os;
}


std::ostream& str::state(const State& state, std::ostream& os) {
  os << " flags \t" << state.flags() << std::endl;
  str::stateFields(state,os); os << std::endl;
  os << " z \t" << state.z() << std::endl;
  os << " state \t" << state.state();
  //  os << " cov " << state.covariance() << std::endl;
  return os;
}

std::ostream& str::lhcbIDs(const std::vector<LHCbID>& ids, std::ostream& os) {
  for (std::vector<LHCbID>::const_iterator it = ids.begin();
       it != ids.end(); it++) 
    str::lhcbID(*it,os); os << std::endl;
  return os;
}

std::ostream& str::lhcbID(const LHCbID& id, std::ostream& os) {
  os << " ID detector " << id.detectorType() 
     << " channel " << id.channelID()
     << " size " << (int) id.spareBits();
  os << std::endl;
  return os;
}


std::ostream& str::trackFields(const Track& track, std::ostream& os) {
  
  os << " type : \t" << track.type() << std::endl;
  os << " status: \t" <<track.status() << std::endl;
  os << " history: \t" << track.history() << std::endl;
  os << " history Fit: \t" << track.historyFit() << std::endl;
  os << " flags: \t";
  for (std::vector<std::string>::const_iterator 
         it = Keys::Track::Flags.begin(); 
       it != Keys::Track::Flags.end(); it++) {
    if (track.checkFlag(*it))
      os << *it  << " ";  
  }
  os << std::endl;
  return os;
}

std::ostream& str::stateFields(const State& state, std::ostream& os) {
  
  if (state.checkType(State::HasMomentum)) os <<"T:HasMomentum, ";
  if (state.checkType(State::StraightLine)) os <<"T:StraightLine, ";
  
  if (state.checkLocation(State::Unknown)) os <<"L:Unknown, ";
  if (state.checkLocation(State::ClosestToBeam)) 
    os <<"L:ClosestToBeam, ";
  if (state.checkLocation(State::FirstMeasurement)) 
    os <<"L:FirstMeasurement, ";
  if (state.checkLocation(State::EndVelo)) os <<"L:EndVelo, ";
  if (state.checkLocation(State::AtTT)) os <<"L:AtTT, ";
  if (state.checkLocation(State::AtT)) os <<"L:AtT, ";
  if (state.checkLocation(State::BegRich1)) os <<"L:BegRich1, ";
  if (state.checkLocation(State::EndRich1)) os <<"L:EndRich1, ";
  if (state.checkLocation(State::BegRich2)) os <<"L:BegRich2, ";
  if (state.checkLocation(State::EndRich2)) os <<"L:EndRich2, ";
  if (state.checkLocation(State::Calo)) os <<"L:Calo, ";
  if (state.checkLocation(State::Muon)) os <<"L:Muon, ";

  return os;  
}

