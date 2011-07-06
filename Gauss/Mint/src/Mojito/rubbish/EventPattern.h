#ifndef EVENTPATTERN_HH
#define EVENTPATTERN_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT

#include <vector>
#include "DecayTree.h"

class EventPattern : public std::vector<DecayTreeItem>{
 public:
  bool equal(const EventPattern& other) const;
  bool operator==(const EventPattern& rhs) const;
};


// EventPattern contains a list of all final state
// particles in the order in which they appear in 
// the event record, identified by pdg-id.
#endif
//
