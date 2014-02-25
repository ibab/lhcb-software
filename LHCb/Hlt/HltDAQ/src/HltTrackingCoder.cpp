// Include files
#include <functional>
#include <algorithm>
#include <iostream>


// local
#include "HltTrackingCoder.h" 
#include "Event/State.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for Track en/decoding functions
// to be used by the HltTrackingWriter and HltTrackingDecoder
// Both functions are implemented here in the same file in order to
// keep them in synch easily.
//
// Unit tests for these functions are in utestTrackingCoder.cxx
//
// Track rawbank format:
//
//  For each track:
//  (nLhHCbIDs, ID0, ID1, ... IDn)
//
//  tracks are just concatenated
//  The RawBank is terminated with an entry nLHCbIDS=0 
//
// 20014-01-14 : Sebastian Neubert
//-----------------------------------------------------------------------------


// packing macros to put a double into 2 uints
// from: http://stackoverflow.com/questions/4306577/how-to-get-the-upper-lower-machine-word-of-a-double-according-to-ieee-754-ansi
#define REP(x) ((union { double v; uint64_t r; }){ x }).r
#define HI(x) (uint32_t)(REP(x) >> 32)
#define LO(x) (uint32_t)(REP(x))

#define HILO(u,l) (((uint64_t)u) << 32)|((uint64_t)l)
#define PER(u,l) ((union {uint64_t r; double v;}){HILO(u,l)}).v


void
encodeTracks(const LHCb::Tracks* tracks,
	     std::vector<unsigned int>& rawBank){

  //std::cout << "Encoding "<<tracks->size()<<" tracks."<<std::endl;
  for(LHCb::Tracks::const_iterator  pItr = tracks->begin(); tracks->end() != pItr; ++pItr){
      LHCb::Track* Tr = (*pItr);
      unsigned int nhits= Tr->nLHCbIDs();
      rawBank.push_back(nhits);

      // behold the awesomness of C++11 functional programming
      // here use the C++ "map" functional transform together with a C++ Lambda construct to fill the LHCbIDs into the bank
      transform(Tr->lhcbIDs().begin(),Tr->lhcbIDs().end(),std::back_inserter(rawBank),[](const LHCb::LHCbID& id){ return id.lhcbID(); });

      // write states
      // check number of states on track
      const std::vector<LHCb::State*>& states = Tr->states();
      unsigned int nstates= states.size();
      rawBank.push_back(nstates);
      // loop over states and encode locations, parameters and covs
      for(unsigned int is=0;is<nstates;++is){
	LHCb::State* state=states[is];
	// store the state location
	rawBank.push_back(state->location());
	// store the parameters
	Gaudi::TrackVector& par=state->stateVector();
	for(unsigned int ipar=0;ipar<5;++ipar){
	  rawBank.push_back(HI(par[ipar]));
	  rawBank.push_back(LO(par[ipar]));
	}
      }
      
      //rawBank.push_back(0);
      //std::copy(rawBank.begin(), rawBank.end(), std::ostream_iterator<unsigned int>(std::cout, " "));
  }
  
  
}


unsigned int
// returns number of decoded tracks
decodeTracks(unsigned int* rawBankData,
	     unsigned int nentries,
	     LHCb::Tracks* tracks){
  // due to the way the RawBank presents ist data we have 
  // to loop over the data in the old fashioned way
  // so we use a pointer"iterator"
  unsigned int* rawit=rawBankData;
  //std::cout << "RawBank size = " << nentries << std::endl; 
  unsigned int k=0;
  while(k<nentries){
    // read number of IDs in track
    unsigned int nid= rawit[k];
    // std::cout << "Nids in track: " <<  nid << std::endl;
    // Start a new track
    Track* track=new Track();
    // advance iterator to point at first LHCbID
    //rawit+=sizeof(unsigned int);
    ++k;
    for(unsigned int i=0;i<nid;i++){
      track->addToLhcbIDs(LHCbID(rawit[k]));
      ++k; //rawit+=sizeof(unsigned int);
    }

    // read number of states
    unsigned int nstates = rawit[k];
    ++k;
    for(unsigned int istate=0;istate<nstates;++istate){
      
      // add location
      LHCb::State::Location loc = LHCb::State::Location(rawit[k++]);
      // add parameters
      Gaudi::TrackVector par;
      for(unsigned int ip=0;ip<5;++ip){
	unsigned int hi=rawit[k++];
	unsigned int lo=rawit[k++];
	par[ip]=PER(hi,lo);
      }
      Gaudi::TrackSymMatrix cov;
      track->addToStates(LHCb::State(par,cov,200.,loc));
    }

    track->setType(LHCb::Track::Velo);
    tracks->add(track);
    //std::cout << "RawBank entry counter k= " << k << std::endl;
    // std::cout << "Decoded track: \n" << *track << std::endl;
  }

  return tracks->size();
}


