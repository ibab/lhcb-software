#include "Event/TrackStreamer.h"

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
  os << " physics state \t" 
     << str::state(track.physicsState(),os)
     << std::endl;
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
    os << str::state(state,os) << std::endl;
  }
  return os;
}


std::ostream& str::state(const State& state, std::ostream& os) {
  os << " flags " << state.flags() << std::endl;
  os << str::stateFields(state,os) << std::endl;
  os << " z " << state.z() << std::endl;
  os << " state " << state.state();
  //  os << " cov " << state.covariance() << std::endl;
  return os;
}

std::ostream& str::lhcbIDs(const std::vector<LHCbID>& ids, std::ostream& os) {
  for (std::vector<LHCbID>::const_iterator it = ids.begin();
       it != ids.end(); it++) 
    os << str::lhcbID(*it,os) << std::endl;
  return os;
}

std::ostream& str::lhcbID(const LHCbID& id, std::ostream& os) {
  os << " ID detector " << id.detectorType() 
     << " channel " << id.channelID()
     << " size " << id.spareBits();
  return os;
}


std::ostream& str::trackFields(const Track& track, std::ostream& os) {
  
  if (track.checkType(Track::Velo)) os << "T:Velo, ";
  if (track.checkType(Track::VeloR)) os  << "T:VeloR, ";
  if (track.checkType(Track::Backward)) os << "T:Backward, ";
  if (track.checkType(Track::Long)) os << "T:Long, ";
  if (track.checkType(Track::Upstream)) os << "T:Upstream, ";
  if (track.checkType(Track::Downstream)) os <<"T:Downstream, ";
  if (track.checkType(Track::Ttrack)) os << "T:Ttrack, ";
  
  if (track.producedByAlgo(Track::LongTrack)) os << "H:LongTrack, ";
  if (track.producedByAlgo(Track::Seeding)) os <<"H:Seeding , ";
  if (track.producedByAlgo(Track::TrKshort)) os <<"H::TrKshort, ";
  if (track.producedByAlgo(Track::TrMatching)) os <<"H::TrMatching, ";
  if (track.producedByAlgo(Track::VeloTrack))  os <<"H:VeloTrack, ";
  if (track.producedByAlgo(Track::VeloTT)) os <<"H:VeloTT, ";
  if (track.producedByAlgo(Track::TrgForward)) os <<"H:TrgForward, ";
  if (track.producedByAlgo(Track::TrgVelo)) os <<"H:TrgVelo, ";
  if (track.producedByAlgo(Track::TrgVeloTT)) os <<"H:TrgVeloTT, ";
  
  if (track.checkFlag(Track::Valid)) os <<"F:Valid, ";
  if (track.checkFlag(Track::Unique)) os <<"F:Unique, ";
  
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

