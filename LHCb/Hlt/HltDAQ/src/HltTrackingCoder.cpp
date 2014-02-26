// Include files
#include <functional>
#include <algorithm>
#include <iostream>


// local
#include "HltTrackingCoder.h" 
#include "Event/State.h"
#include "Event/StandardPacker.h"

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


// It is stupid to have to instantiate this
StandardPacker pac;

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
	rawBank.push_back((unsigned int)pac.position(par[0]));
	rawBank.push_back((unsigned int)pac.position(par[1]));
	rawBank.push_back((unsigned int)pac.slope(par[2]));
	rawBank.push_back((unsigned int)pac.slope(par[3]));

	double p=0;
	if(state->qOverP() !=0 ) p= 1./state->qOverP();
	rawBank.push_back((unsigned int)pac.energy(p));
	
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
      par[0]=pac.position((int)(rawit[k++]));
      par[1]=pac.position((int)(rawit[k++]));
      par[2]=pac.slope((int)(rawit[k++]));
      par[3]=pac.slope((int)(rawit[k++]));
      int p= (int) rawit[k++];
      par[4]=( 0 != p ? 1.0/pac.energy(p) : 0.0 );
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


