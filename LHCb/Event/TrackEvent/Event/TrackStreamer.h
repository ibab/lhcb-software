#ifndef TrackStreamer_H
#define TrackStreamer_H 1

#include "Event/Track.h"

//! @author J.A. Hernando
//! @date 08/02/2005
namespace str {

  //! write track data into the ostream 
  std::ostream& track(const Track& track, std::ostream& os = std::cout);
  
  //! write track header (chi2 and physics state)
  std::ostream& trackHeader(const Track& track,std::ostream& os = std::cout);

  //! write track fiels (type and flags) into the ostream 
  std::ostream& trackFields(const Track& track, std::ostream& os = std::cout);

  //! write track states into the ostream
  std::ostream& states(const std::vector<State*>& states,
		       std::ostream& os = std::cout);

  //! write state data into the ostream 
  std::ostream& state(const State& state, std::ostream& os = std::cout);
    
  //! write state fiels (type and locations) into the ostream 
  std::ostream& stateFields(const State& state, std::ostream& os = std::cout);

  //! write the list of LHCbIDs
  std::ostream& lhcbIDs(const std::vector<LHCbID>& ids, 
			std::ostream& os = std::cout);
  
  //! write a LHCbID
  std::ostream& lhcbID(const LHCbID& id, std::ostream& os = std::cout);

};

inline std::ostream& operator << (std::ostream& os, const Track& track) {
  return str::track(track,os);
}

inline std::ostream& operator << (std::ostream& os, const State& state) {
  return str::state(state,os);
}


#endif
