// Include files
#include <functional>
#include <algorithm>
#include <iostream>

// local
#include "HltTrackingCoder.h" 

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
    tracks->add(track);
    //std::cout << "RawBank entry counter k= " << k << std::endl;
    // std::cout << "Decoded track: \n" << *track << std::endl;
  }

  return tracks->size();
}


