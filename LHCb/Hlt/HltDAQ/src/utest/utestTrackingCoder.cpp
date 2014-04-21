#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utestTrackingCoder
#include <boost/test/unit_test.hpp>

#include "../HltTrackingCoder.h"
#include <vector>
#include <algorithm>
#include "Event/Track.h"
#include "Event/State.h"
#include "Kernel/LHCbID.h"
#include "Math/SMatrix.h"
#include <iostream>

// Helper function to compare two track containers element wise
template <typename Container, typename Cmp>
bool
equal(const Container& c1, const Container& c2, Cmp cmp) {
  return  c1.size() == c2.size() &&
           std::equal( std::begin(c1), std::end(c1),
                       std::begin(c2),
                       cmp );
}



// Check for equal LHCbIDs
bool
equalLHCbIDs(const LHCb::Tracks& tracks, const LHCb::Tracks& reftracks){
  return  equal(tracks, reftracks, [](const LHCb::Track* t1, const LHCb::Track* t2) { 
                           return t1->lhcbIDs() == t2->lhcbIDs() ; 
                       } );
}
 
// Check for equal types
bool
equalMeta(const LHCb::Tracks& tracks, const LHCb::Tracks& reftracks){
  return  equal(tracks, reftracks, [](const LHCb::Track* t1, const LHCb::Track* t2) { 
                           return t1->type() == t2->type() &&
                                  t1->flags() == t2->flags();
                       } );
}


// Check for equal States
bool
equalStates(const LHCb::Tracks& tracks, const LHCb::Tracks& reftracks){
  return equal( tracks, reftracks, [](const LHCb::Track* Tr, const LHCb::Track* refTr) {
      // check the number of states
      unsigned int nstates=Tr->states().size();
      if (nstates != (refTr->states().size())) return false;
      const std::vector<LHCb::State*>& states = Tr->states();
      const std::vector<LHCb::State*>& refstates = refTr->states();
      for(unsigned int i=0;i<nstates;++i) {
        bool result = (states[i]->location() == refstates[i]->location());
        if(!result){
          std::cout << states[i]->location() << std::endl;
          std::cout << refstates[i]->location() << std::endl;
        }
        result &= (states[i]->stateVector() == refstates[i]->stateVector());
        if(!result){
          std::cout << states[i]->stateVector() << std::endl;
          std::cout << refstates[i]->stateVector() << std::endl;
        }
        // check for approximate equivalence of covariance matrix
        double* cov = states[i]->covariance().Array();
        double* refcov = refstates[i]->covariance().Array();
        for(unsigned int ic=0;ic<15;++ic){
          result &= (fabs(cov[ic]-refcov[ic]) < 0.001*fabs(refcov[ic])); 
        }
        if(!result){
          std::cout << states[i]->covariance() << std::endl;
          std::cout << refstates[i]->covariance() << std::endl;
        }
        if (!result) return result;
      } // end loop over states
      return true;

        } ); 
}


// The test fixture: Create a container of tracks to encode
// This can be used as a common starting point for all tests
struct ExampleTracks {
  
  LHCb::Tracks m_tracks;                  // collection of tracks
  std::vector<unsigned int> m_rawbank;   // corresponding RawBank
 
  ExampleTracks() {
    BOOST_TEST_MESSAGE("setup example tracks");
    auto tr=new LHCb::Track();

    
    tr->setFlags(1);
    tr->setType(LHCb::Track::Velo);

    tr->addToLhcbIDs(LHCb::LHCbID(1));
    tr->addToLhcbIDs(LHCb::LHCbID(3));
    tr->addToLhcbIDs(LHCb::LHCbID(5));
    m_tracks.add(tr);

    tr=new LHCb::Track();
    tr->setFlags(1);
    tr->setType(LHCb::Track::TT);

    tr->addToLhcbIDs(LHCb::LHCbID(2));
    tr->addToLhcbIDs(LHCb::LHCbID(4));
    tr->addToLhcbIDs(LHCb::LHCbID(6));
    m_tracks.add(tr);

    tr=new LHCb::Track();
    tr->setFlags(8);
    tr->setType(LHCb::Track::Long);
     
    tr->addToLhcbIDs(LHCb::LHCbID(7));
    tr->addToLhcbIDs(LHCb::LHCbID(11));
    tr->addToLhcbIDs(LHCb::LHCbID(13));
    tr->addToLhcbIDs(LHCb::LHCbID(17));
    
    // add a State to this track
    Gaudi::TrackVector v(1,-1,1,-1,1);

    // build an example covariance matrix 
    // put the upper triangle intp vector
    double val[15] = {1.1,0.21,0.31,0.41,0.51,    // first row
		      1.2,0.22,0.32,0.42,
		      1.3,0.23,0.33,
		      1.4,0.24,
		      1.5};
    ROOT::Math::SVector<double,15> c(val,15);
    Gaudi::TrackSymMatrix cov(c,false);  // the false flag ensure we se the upper triangle
    //std::cout << cov << std::endl;

    //double* ar=cov.Array();
    //for(unsigned int i=0;i<15;++i){
    //  std::cout << ar[i] << " , ";
    //}
    //std::cout << std::endl;


    LHCb::State newYork(v,cov,200.,LHCb::State::EndVelo);
    tr->addToStates(newYork);

    LHCb::State Michigan(v,cov,220.,LHCb::State::ClosestToBeam);
    tr->addToStates(Michigan);

    m_tracks.add(tr);
    
    // format:                   flags nIDs  IDs    nStates States(Loc, z, par)
    std::vector<unsigned int> bank = { 1,    3,  1,3,5,    0,      
			          9,    3,  2,4,6,    0,
			          3,    4,  7,11,13,17, 
				                          2,  // 2 states in this track
				 3 , 2000000, 10000, 4294957296, 100000000, 4194967296, 100, // state at location 3
				 10488, 10954, 114017543, 118321596, 12247449, 5483, 7777, 5284, // its cov
				 9912, 7407, 5115, 11911, 9391, 7090, 4968, 
				 1 , 2200000, 10000, 4294957296, 100000000, 4194967296, 100, // // state at location 1
				 10488, 10954, 114017543, 118321596, 12247449, 5483, 7777, 5284, // its cov
				 9912, 7407, 5115, 11911, 9391, 7090, 4968 }; 
    m_rawbank = bank;
  }

  ~ExampleTracks() {
    BOOST_TEST_MESSAGE("tear down example tracks");
    m_tracks.clear();
  }

};

// Each test in this suite will use the ExampleTracks fixture
// fixtures are setup and teared down for each test individually
BOOST_FIXTURE_TEST_SUITE(TrackingCoder, ExampleTracks)
 
BOOST_AUTO_TEST_CASE(comparetracks)
{
  BOOST_CHECK(equalLHCbIDs(m_tracks,m_tracks));
  BOOST_CHECK(equalStates(m_tracks,m_tracks));
  BOOST_CHECK(equalMeta(m_tracks,m_tracks));
}

BOOST_AUTO_TEST_CASE(encode)
{
  std::vector<unsigned int> rawBank;
  encodeTracks(&m_tracks,rawBank);
  BOOST_CHECK(rawBank.size() == m_rawbank.size()  );
  BOOST_CHECK(rawBank == m_rawbank );
}

BOOST_AUTO_TEST_CASE(decode)
{
  LHCb::Tracks tracks;
  BOOST_CHECK(decodeTracks(m_rawbank.data(),m_rawbank.size(),&tracks) == m_tracks.size());
  BOOST_CHECK(tracks.size() == m_tracks.size() );
  BOOST_CHECK(equalLHCbIDs(tracks,m_tracks));
  BOOST_CHECK(equalStates(tracks,m_tracks));
  BOOST_CHECK(equalMeta(tracks,m_tracks));

}



BOOST_AUTO_TEST_CASE(en_de_code)
{
  std::vector<unsigned int> rawBank;
  encodeTracks(&m_tracks,rawBank);
  LHCb::Tracks tracks;
  decodeTracks(m_rawbank.data(),m_rawbank.size(),&tracks);
  BOOST_CHECK(equalLHCbIDs(tracks,m_tracks));
  BOOST_CHECK(equalStates(tracks,m_tracks));
  BOOST_CHECK(equalMeta(tracks,m_tracks));
}


BOOST_AUTO_TEST_SUITE_END()

