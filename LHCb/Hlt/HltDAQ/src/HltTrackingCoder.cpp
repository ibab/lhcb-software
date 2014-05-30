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
const StandardPacker pac;

void
encodeTracks(const LHCb::Tracks* tracks,
	     std::vector<unsigned int>& rawBank){

  //std::cout << "Encoding "<<tracks->size()<<" tracks."<<std::endl;
  for(const LHCb::Track *Tr : *tracks ) {

      // write meta information
      // flags
      rawBank.push_back(Tr->flags());

      //
      unsigned int nhits= Tr->nLHCbIDs();
      rawBank.push_back(nhits);

      // write LHCbIDs
      // behold the awesomness of C++11 functional programming
      // here use the C++ "map" functional transform to fill the LHCbIDs into the bank
      assert(nhits == Tr->lhcbIDs().size());
      std::transform( std::begin(Tr->lhcbIDs()),std::end(Tr->lhcbIDs()),
                      std::back_inserter(rawBank),
                      std::mem_fn( &LHCb::LHCbID::lhcbID ) );
     
      // write states
      // check number of states on track
      const std::vector<LHCb::State*>& states = Tr->states();
      unsigned int nstates= states.size();
      assert(nstates<1000);
      rawBank.push_back(nstates);
      // loop over states and encode locations, parameters and covs
      for(unsigned int is=0;is<nstates;++is){
	LHCb::State* state=states[is];
	// store the state location
	rawBank.push_back(state->location());
	// store z
	rawBank.push_back((unsigned int)pac.position(state->z()));
	// store the parameters
	Gaudi::TrackVector& par=state->stateVector();
	rawBank.push_back((unsigned int)pac.position(par[0]));
	rawBank.push_back((unsigned int)pac.position(par[1]));
	rawBank.push_back((unsigned int)pac.slope(par[2]));
	rawBank.push_back((unsigned int)pac.slope(par[3]));

	double p=0;
	if(state->qOverP() !=0 ) p= 1./state->qOverP();
	rawBank.push_back((unsigned int)pac.energy(p));
	//== Get the coded value in case of saturation, to code properly the error later
	p = pac.energy( (int)rawBank.back() );
	
      
      // store covariance matrix
      // the method is analogous to the one descibed
      // in http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/davinci/latest_doxygen/d2/dcf/class_l_h_cb_1_1_track_packer.html#ad854eb5fff1c364a97efe32990e41a5f
      // note that the off-diagonals are short ints
      // --> in principle we can put 2 covs in one uint


      // get errors for scaling
       std::array<double,5> err = { std::sqrt( state->errX2() ) ,
                                    std::sqrt( state->errY2() ),
                                    std::sqrt( state->errTx2() ),
                                    std::sqrt( state->errTy2() ),
                                    std::sqrt( state->errQOverP2() ) };
       // first store the diagonal then row wise the rest
       
       unsigned int cov_00 = pac.position( err[0] );rawBank.push_back(cov_00);
       unsigned int cov_11 = pac.position( err[1] );rawBank.push_back(cov_11);
       unsigned int cov_22 = pac.slope   ( err[2] );rawBank.push_back(cov_22);
       unsigned int cov_33 = pac.slope   ( err[3] );rawBank.push_back(cov_33);
       unsigned int cov_44 = pac.energy  ( 1.e5 * fabs(p) * err[4] ); rawBank.push_back(cov_44); //== 1.e5 * dp/p (*1.e2)
       for (unsigned i=1;i<5;++i) for (unsigned j=0;j<i;++j) {
           rawBank.push_back( pac.fraction( state->covariance()(i,j)/err[i]/err[j] ) );
      } //  end loop over states

  }
  //rawBank.push_back(0);
  //std::copy(rawBank.begin(), rawBank.end(), std::ostream_iterator<unsigned int>(std::cout, " "));
  
    }  
  
}


// returns number of decoded tracks
unsigned int
decodeTracks(unsigned int* rawBankData,
	     unsigned int nentries,
	     LHCb::Tracks* tracks) {
  // due to the way the RawBank presents ist data we have 
  // to loop over the data in the old fashioned way
  // so we use a pointer"iterator"
  unsigned int* rawit=rawBankData;
  //std::cout << "RawBank size = " << nentries << std::endl; 
  unsigned int k=0;
  while(k<nentries){
   
    // read flags this also contains the type
    unsigned int flags = rawit[k++];

    // read number of IDs in track
    unsigned int nid= rawit[k++];
    //std::cout << "Nids in track: " <<  nid << std::endl;
    // Start a new track
    Track* track=new Track();
    track->setFlags(flags);

    for(unsigned int i=0;i<nid;i++){
      track->addToLhcbIDs(LHCbID(rawit[k++]));
       //rawit+=sizeof(unsigned int);
    }

    // read number of states
    unsigned int nstates = rawit[k];
    //std::cout << "Nstates in track: " <<  nstates << std::endl;
    if(k+nstates*22>nentries){
      std::cout << "TOO MANY STATES IN TRACK. ABORTING DECODING. " << std::endl;
      std::cout << "This happens at track " << tracks->size() << " in the event" << std::endl;
      std::cout << "Track so far:\n "<< *track <<std::endl;
      unsigned int nhits= track->nLHCbIDs();
      for(unsigned int i=0;i<nhits;++i){
	std::cout << track->lhcbIDs()[i] << ",\n";
      }
      std::cout << "State of the the bank with " <<nentries << " entries" <<std::endl;
      for(unsigned int i=0; i<60;++i){
	if(i==30) std::cout << "****" << std::endl;
	std::cout << rawit[i-30+k] << std::endl;
      }
      return tracks->size();
    }
    ++k;
    for(unsigned int istate=0;istate<nstates;++istate){
      
      // add location
      LHCb::State::Location loc = LHCb::State::Location(rawit[k++]);
      // z coordinate
      double z = pac.position((int)rawit[k++]);
      // add parameters
      Gaudi::TrackVector par;
      par[0]=pac.position((int)(rawit[k++]));
      par[1]=pac.position((int)(rawit[k++]));
      par[2]=pac.slope((int)(rawit[k++]));
      par[3]=pac.slope((int)(rawit[k++]));
      int p= (int) rawit[k++];
      par[4]=( 0 != p ? 1.0/pac.energy(p) : 0.0 );
      Gaudi::TrackSymMatrix stateCov;

      //== Fill covariance matrix
      std::array<double,5> err = { pac.position( (int) rawit[k++] ),
                                   pac.position( (int) rawit[k++] ),
                                   pac.slope   ( (int) rawit[k++] ),
                                   pac.slope   ( (int) rawit[k++] ),
                                   pac.energy  ( (int) rawit[k++] ) * fabs(par[4]) * 1.e-5  // par[4]=1/p
                                 } ; 
                         
      for (unsigned i=0;i<5;++i) stateCov(i,i) = err[i]*err[i];
      for (unsigned i=1;i<5;++i) for (unsigned j=0;j<i;++j) 
                                 stateCov(i,j) = err[i]*err[j]*pac.fraction( (short int) rawit[k++] );
      
      track->addToStates(LHCb::State(par,stateCov,z,loc));
    } // end loop over states
   
    tracks->add(track);
    //std::cout << "RawBank entry counter k= " << k << std::endl;
    //std::cout << "Decoded track: \n" << *track << std::endl;
  }

  return tracks->size();
}
