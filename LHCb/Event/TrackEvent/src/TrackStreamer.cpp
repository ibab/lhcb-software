#include "Event/TrackStreamer.h"

using namespace LHCb;

std::ostream& str::track(const Track& track, std::ostream& os) {

  str::trackHeader(track,os) << std::endl;
  str::trackFields(track,os) << std::endl;
  str::states(track.states(),os) << std::endl;
  str::lhcbIDs(track.lhcbIDs(),os);
  return os;
}

std::ostream& str::trackHeader(const Track& track, std::ostream& os) {

  os << "-- track header -- " << std::endl;
  os << " chi2  \t" << track.chi2() << std::endl;
  os << " ndof  \t" << track.nDoF() << std::endl;
  os << std::endl;
  os << " flags \t " << track.flags() << std::endl;
  os << " nstates \t " << track.states().size() << std::endl;
  os << " nIDs \t " << track.lhcbIDs().size();
  return os;
}


std::ostream& str::states(const std::vector<State*>& states,  
			  std::ostream& os) {
  os << "-- track states -- " << std::endl;
  for (std::vector<State*>::const_iterator it = states.begin(); 
       it != states.end(); ++it) {
    const State& state = *(*it);
    str::state(state,os); os << std::endl;
  }
  return os;
}


std::ostream& str::state(const State& state, std::ostream& os) {
  os << "-- state -- " << std::endl;
  os << " flags \t" << state.flags() << std::endl;
  str::stateFields(state,os); os << std::endl;
  os << " z \t" << state.z() << std::endl;
  os << " state vector \t" << state.stateVector();
  //  os << " cov " << state.covariance() << std::endl;
  return os;
}

std::ostream& str::lhcbIDs(const std::vector<LHCbID>& ids, std::ostream& os) {
  os << "-- LHCbIDs -- " << std::endl;
  for (std::vector<LHCbID>::const_iterator it = ids.begin();
       it != ids.end(); ++it) 
    str::lhcbID(*it,os); os << std::endl;
  return os;
}

std::ostream& str::lhcbID(const LHCbID& id, std::ostream& os) {
  os << " ID detector " << id.detectorType() 
     << " ID " << id.channelID();
  os << std::endl;
  return os;
}


std::ostream& str::trackFields(const Track& track, std::ostream& os) {
  
  os << " type : \t" << track.type() << std::endl;
  os << " pat. rec. status: \t" <<track.patRecStatus() << std::endl;
  os << " fit status: \t" <<track.fitStatus() << std::endl;
  os << " history: \t" << track.history() << std::endl;
  os << " fit history: \t" << track.fitHistory() << std::endl;
  // os << " flags: \t";
  os << std::endl;
  return os;
}

std::ostream& str::stateFields(const State& state, std::ostream& os) {
  
  if (state.checkLocation(State::LocationUnknown)) os <<"L:Unknown, ";
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
  if (state.checkLocation(State::Spd)) os <<"L:Spd, ";
  if (state.checkLocation(State::Prs)) os <<"L:Prs, ";
  if (state.checkLocation(State::BegECal)) os <<"L:BegECal, ";
  if (state.checkLocation(State::ECalShowerMax)) os <<"L:ECalShowerMax, ";
  if (state.checkLocation(State::EndECal)) os <<"L:EndECal, ";
  if (state.checkLocation(State::BegHCal)) os <<"L:BegHCal, ";
  if (state.checkLocation(State::MidHCal)) os <<"L:MidHCal, ";
  if (state.checkLocation(State::EndHCal)) os <<"L:EndHCal, ";
  if (state.checkLocation(State::Muon)) os <<"L:Muon, ";

  return os;  
}

