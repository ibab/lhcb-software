#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE utestTrackingCoder
#include <boost/test/unit_test.hpp>

#include "core/HltTrackingCoder.h"
#include <vector>
#include "Event/Track.h"
#include "Kernel/LHCbID.h"
using namespace LHCb; 
using namespace std;

// Helper function to compare two track containers element wise
// Check for equal LHCbIDs
bool
equalTracks(Tracks* tracks, Tracks* reftracks){
  LHCb::Tracks::const_iterator  pRefItr = reftracks->begin();
  bool result=true;
  for(LHCb::Tracks::const_iterator  pItr = tracks->begin(); tracks->end() != pItr; ++pItr){
      LHCb::Track* Tr = (*pItr);
      LHCb::Track* refTr = (*pRefItr);
      result &= ((Tr->lhcbIDs()) == (refTr->lhcbIDs()));
      pRefItr++; // don't forget to increment reference iterator 
  }// end loop over tracks
  return result;
}
 
// The test fixture: Create a container of tracks to encode
// This can be used as a common starting point for all tests
struct ExampleTracks {
  
  Tracks m_tracks;                  // collection of tracks
  vector<unsigned int> m_rawbank;   // corresponding RawBank
 
  ExampleTracks() {
    BOOST_TEST_MESSAGE("setup example tracks");
    Track* tr=new Track();
    tr->addToLhcbIDs(LHCbID(1));
    tr->addToLhcbIDs(LHCbID(3));
    tr->addToLhcbIDs(LHCbID(5));
    m_tracks.add(tr);

    tr=new Track();
    tr->addToLhcbIDs(LHCbID(2));
    tr->addToLhcbIDs(LHCbID(4));
    tr->addToLhcbIDs(LHCbID(6));
    m_tracks.add(tr);
    tr=new Track();
    tr->addToLhcbIDs(LHCbID(7));
    tr->addToLhcbIDs(LHCbID(11));
    tr->addToLhcbIDs(LHCbID(13));
    tr->addToLhcbIDs(LHCbID(17));
    m_tracks.add(tr);

    vector<unsigned int> bank = {3,1,3,5,
				 3,2,4,6,
				 4,7,11,13,17};
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
  BOOST_CHECK(equalTracks(&m_tracks,&m_tracks));
}

BOOST_AUTO_TEST_CASE(encode)
{
  vector<unsigned int> rawBank;
  encodeTracks(&m_tracks,rawBank);
  BOOST_CHECK(rawBank.size() == m_rawbank.size() );
  BOOST_CHECK(rawBank == m_rawbank );
}

BOOST_AUTO_TEST_CASE(decode)
{
  Tracks tracks;
  BOOST_CHECK(decodeTracks(m_rawbank,&tracks) == m_tracks.size());
  BOOST_CHECK(tracks.size() == m_tracks.size() );
  BOOST_CHECK(equalTracks(&tracks,&m_tracks));
}


BOOST_AUTO_TEST_CASE(en_de_code)
{
  vector<unsigned int> rawBank;
  encodeTracks(&m_tracks,rawBank);
  Tracks tracks;
  decodeTracks(m_rawbank,&tracks);
  BOOST_CHECK(equalTracks(&tracks,&m_tracks));
}


BOOST_AUTO_TEST_SUITE_END()

