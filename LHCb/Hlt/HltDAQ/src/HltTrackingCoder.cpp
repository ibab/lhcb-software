// Include files 
#include <functional>
#include <algorithm>

// local
#include "core/HltTrackingCoder.h" 

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
//
// 20014-01-14 : Sebastian Neubert
//-----------------------------------------------------------------------------


void
encodeTracks(const LHCb::Tracks* tracks,
	     std::vector<unsigned int>& rawBank){
  
  for(LHCb::Tracks::const_iterator  pItr = tracks->begin(); tracks->end() != pItr; ++pItr){
      LHCb::Track* Tr = (*pItr);
      unsigned int nhits= Tr->nLHCbIDs();
      rawBank.push_back(nhits);

      // behold the awesomness of C++11 functional programming
      // here use the C++ "map" functional transform together with a C++ Lambda construct to fill the LHCbIDs into the bank
      transform(Tr->lhcbIDs().begin(),Tr->lhcbIDs().end(),std::back_inserter(rawBank),[](const LHCb::LHCbID& id){ return id.lhcbID(); });    

  }
}


unsigned int
// returns number of decoded tracks
decodeTracks(const std::vector<unsigned int>& rawBank,
	     LHCb::Tracks* tracks){

  std::vector<unsigned int>::const_iterator rawit=rawBank.begin();
  while(rawit!=rawBank.end()){
    // Start a new track
    Track* track=new Track();
    // read number of IDs in track
    unsigned int nid= *rawit;
    // advance iterator to point at first LHCbID
    rawit++;
    for(unsigned int i=0;i<nid;i++){
      track->addToLhcbIDs(LHCbID(*rawit));
      rawit++;
    }
    tracks->add(track);

  }
    
  return tracks->size();
} 


