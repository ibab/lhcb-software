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

      // write meta information
      // flags
      rawBank.push_back(Tr->flags());

      //
      unsigned int nhits= Tr->nLHCbIDs();
      rawBank.push_back(nhits);

      // write LHCbIDs
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
       std::vector<double> err;
       err.push_back( std::sqrt( state->errX2() ) );
       err.push_back( std::sqrt( state->errY2() ) );
       err.push_back( std::sqrt( state->errTx2() ) );
       err.push_back( std::sqrt( state->errTy2() ) );
       err.push_back( std::sqrt( state->errQOverP2() ) );
       // first store the diagonal then row wise the rest
       
       unsigned int cov_00 = pac.position( err[0] );rawBank.push_back(cov_00);
       unsigned int cov_11 = pac.position( err[1] );rawBank.push_back(cov_11);
       unsigned int cov_22 = pac.slope   ( err[2] );rawBank.push_back(cov_22);
       unsigned int cov_33 = pac.slope   ( err[3] );rawBank.push_back(cov_33);
       unsigned int cov_44 = pac.energy  ( 1.e5 * fabs(p) * err[4] ); rawBank.push_back(cov_44); //== 1.e5 * dp/p (*1.e2)
       unsigned int cov_10 = pac.fraction( state->covariance()(1,0)/err[1]/err[0] ); rawBank.push_back(cov_10);
       unsigned int cov_20 = pac.fraction( state->covariance()(2,0)/err[2]/err[0] ); rawBank.push_back(cov_20);
       unsigned int cov_21 = pac.fraction( state->covariance()(2,1)/err[2]/err[1] ); rawBank.push_back(cov_21);
       unsigned int cov_30 = pac.fraction( state->covariance()(3,0)/err[3]/err[0] ); rawBank.push_back(cov_30);
       unsigned int cov_31 = pac.fraction( state->covariance()(3,1)/err[3]/err[1] ); rawBank.push_back(cov_31);
       unsigned int cov_32 = pac.fraction( state->covariance()(3,2)/err[3]/err[2] ); rawBank.push_back(cov_32);
       unsigned int cov_40 = pac.fraction( state->covariance()(4,0)/err[4]/err[0] ); rawBank.push_back(cov_40);
       unsigned int cov_41 = pac.fraction( state->covariance()(4,1)/err[4]/err[1] ); rawBank.push_back(cov_41);
       unsigned int cov_42 = pac.fraction( state->covariance()(4,2)/err[4]/err[2] ); rawBank.push_back(cov_42);
       unsigned int cov_43 = pac.fraction( state->covariance()(4,3)/err[4]/err[3] ); rawBank.push_back(cov_43);
       
      } //  end loop over states

  }
  //rawBank.push_back(0);
  //std::copy(rawBank.begin(), rawBank.end(), std::ostream_iterator<unsigned int>(std::cout, " "));
  
  
  
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
   
    // read flags this also contains the type
    unsigned int flags = rawit[k++];

    // read number of IDs in track
    unsigned int nid= rawit[k++];
    // std::cout << "Nids in track: " <<  nid << std::endl;
    // Start a new track
    Track* track=new Track();
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
      const double err0 = pac.position( (int) rawit[k++] );
      const double err1 = pac.position( (int) rawit[k++] );
      const double err2 = pac.slope   ( (int) rawit[k++] );
      const double err3 = pac.slope   ( (int) rawit[k++] );
      const double err4 = pac.energy  ( (int) rawit[k++] ) * fabs(par[4]) * 1.e-5; // par[4]=1/p
      
      stateCov(0,0) = err0 * err0;
      stateCov(1,1) = err1 * err1;
      stateCov(2,2) = err2 * err2;
      stateCov(3,3) = err3 * err3;
      stateCov(4,4) = err4 * err4;
      stateCov(1,0) = err1 * err0 * pac.fraction( (short int) rawit[k++] );
      stateCov(2,0) = err2 * err0 * pac.fraction( (short int) rawit[k++] );
      stateCov(2,1) = err2 * err1 * pac.fraction( (short int) rawit[k++] );
      stateCov(3,0) = err3 * err0 * pac.fraction( (short int) rawit[k++] );
      stateCov(3,1) = err3 * err1 * pac.fraction( (short int) rawit[k++] );
      stateCov(3,2) = err3 * err2 * pac.fraction( (short int) rawit[k++] );
      stateCov(4,0) = err4 * err0 * pac.fraction( (short int) rawit[k++] );
      stateCov(4,1) = err4 * err1 * pac.fraction( (short int) rawit[k++] );
      stateCov(4,2) = err4 * err2 * pac.fraction( (short int) rawit[k++] );
      stateCov(4,3) = err4 * err3 * pac.fraction( (short int) rawit[k++] );
      
      track->addToStates(LHCb::State(par,stateCov,z,loc));
    } // end loop over states

    track->setFlags(flags);
    tracks->add(track);
    //std::cout << "RawBank entry counter k= " << k << std::endl;
    //std::cout << "Decoded track: \n" << *track << std::endl;
  }

  return tracks->size();
}


