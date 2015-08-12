// BOOST
#include <boost/assign/list_of.hpp> // for 'map_list_of()
// track interfaces
#include "Event/Track.h"

#include "TrackToDST.h"
using namespace LHCb;
using namespace std;
using namespace boost::assign; // bring 'map_list_of()' into scope

DECLARE_ALGORITHM_FACTORY( TrackToDST )

  TrackToDST::TrackToDST(const std::string& name,
                         ISvcLocator* pSvcLocator):
    GaudiAlgorithm(name, pSvcLocator)
{
  // constructor
  this->declareProperty( "TracksInContainer",
                         m_inputLocation = TrackLocation::Default );
  this->declareProperty( "StoreAllStates", m_storeAllStates = false );

  StateMap tmp = map_list_of("ClosestToBeam", State::ClosestToBeam)
    ("FirstMeasurement", State::FirstMeasurement )
    ("EndVelo",State::EndVelo)
    ("AtTT",State::AtTT)
    ("AtT",State::AtT)
    ("BegRich1",State::BegRich1)
    ("EndRich1",State::EndRich1)
    ("BegRich2",State::BegRich2)
    ("EndRich2",State::EndRich2)
    ("Spd",State::Spd)
    ("Prs",State::Prs)
    ("BegECal", State::BegECal)
    ("ECalShowerMax", State::ECalShowerMax)
    ("EndECal",State::EndECal)
    ("BegHCal", State::BegHCal)
    ("MidHCal",State::MidHCal)
    ("EndHCal",State::EndHCal)
    ("Muon",State::Muon)
    ("V0Vertex",State::V0Vertex);
  m_theMap = tmp;
  
  // CRJ : Orignal list
//   m_veloStrings = list_of("ClosestToBeam");
//   m_longStrings = list_of("ClosestToBeam")("FirstMeasurement")("BegRich1")("BegRich2")("V0Vertex");
//   m_tStrings = list_of("FirstMeasurement")( "BegRich2");
//   m_downstreamStrings = list_of("BegRich1")("FirstMeasurement")("BegRich2")("V0Vertex");
//   m_upstreamStrings = list_of("ClosestToBeam")("FirstMeasurement")("BegRich1");
//   m_muonStrings     = list_of("ClosestToBeam")("BegRich1")("BegRich2")("Muon");

  // (Slightly) reduced list
  std::vector<std::string> t1 = list_of("ClosestToBeam");
  std::vector<std::string> t2 = list_of("ClosestToBeam")("FirstMeasurement")("BegRich2")("V0Vertex");
  std::vector<std::string> t3 = list_of("FirstMeasurement")( "BegRich2");
  std::vector<std::string> t4 = list_of("FirstMeasurement")("BegRich2")("V0Vertex");
  std::vector<std::string> t5 = list_of("ClosestToBeam")("FirstMeasurement");
  std::vector<std::string> t6 = list_of("FirstMeasurement");

  declareProperty("veloStates",       m_veloStrings       = t1);
  declareProperty("longStates",       m_longStrings       = t2);
  declareProperty("TTrackStates",     m_tStrings          = t3);
  declareProperty("downstreamStates", m_downstreamStrings = t4);
  declareProperty("upstreamStates",   m_upstreamStrings   = t5);
  declareProperty("muonStates",       m_muonStrings       = t6);

}

TrackToDST::~TrackToDST()
{
  // destructor
}

StatusCode TrackToDST::initialize()
{
  // Initializes TsaInitialization at the begin of program execution.

  const StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize",sc);
  }

  stringToLoc(m_veloStrings,m_veloStates);
  stringToLoc(m_longStrings,m_longStates);
  stringToLoc(m_tStrings,m_tStates);
  stringToLoc(m_downstreamStrings,m_downstreamStates);
  stringToLoc(m_upstreamStrings,m_upstreamStates);
  stringToLoc(m_muonStrings,m_muonStates);

  return sc;
}

StatusCode TrackToDST::execute()
{
  Tracks* inCont = get<Tracks>(m_inputLocation);

  // loop
  for ( Tracks::iterator iterT = inCont->begin();
        iterT != inCont->end(); ++iterT )
  {
    
    // remove the necessary States on the Track
    if ( !m_storeAllStates )
    {
      // done in an ugly way for now - will be easier with the new
      // jobOptions parser
      const LHCb::Track::Types type = (*iterT)->type();
      switch (type)
      {
      case Track::Velo:
        cleanStates(*iterT, m_veloStates);
        break;
      case Track::VeloR:
        cleanStates(*iterT, m_veloStates);
        break;
      case Track::Long:
        cleanStates(*iterT, m_longStates);
        break;
      case Track::Upstream:
        cleanStates(*iterT, m_upstreamStates);
        break;
      case Track::Downstream:
        cleanStates(*iterT, m_downstreamStates);
        break;
      case Track::Ttrack:
        cleanStates(*iterT, m_tStates);
        break;
      case Track::Muon:
        cleanStates(*iterT, m_muonStates);
        break;
      default:
        Warning( format("Unknown track type %i",type) , StatusCode::SUCCESS, 1 ).ignore();
        break;
      } // switch
    } // if
    // set the appropriate flag!
    (*iterT) -> setPatRecStatus( Track::PatRecIDs );

  } // iterT

  return StatusCode::SUCCESS;
}

void TrackToDST::cleanStates(Track* aTrack, const SLocations& loc) const
{
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Analysing Track key=" << aTrack->key()
              << " type=" << aTrack->type()
              << " : " << aTrack->states().size() << " States at : z =";
    for ( std::vector<LHCb::State*>::const_iterator iS = aTrack->states().begin();
          iS != aTrack->states().end(); ++iS )
    {
      if (*iS) verbose() << " (" << (*iS)->z() << " " << (*iS)->location() << ")";
    }
    verbose() << endmsg;
  }

  std::vector<State*> tempCont;
  for ( SLocations::const_iterator iterL = loc.begin();
        iterL != loc.end(); ++iterL )
  {
    const State* state = aTrack->stateAt(*iterL);
    if ( state )
    {
      tempCont.push_back(state->clone());
    }
    else if ( *iterL != LHCb::State::V0Vertex )
    {
      Warning("Failed to find state - more info in DEBUG",StatusCode::SUCCESS,1).ignore();
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      {
        debug() << "Missing state at " << *iterL << " on track " << aTrack->key()
                << " of type " << aTrack->type() << endmsg;
      }
    }
  } // loca

  aTrack->clearStates();
  for ( std::vector<State*>::iterator iterS = tempCont.begin();
        iterS != tempCont.end(); ++iterS )
  {
    aTrack->addToStates(**iterS);
    delete *iterS;
  }
}

void TrackToDST::stringToLoc(const Strings& sCont, SLocations& loc) const
{
  loc.reserve(sCont.size());

  for ( Strings::const_iterator iterS = sCont.begin(); 
        iterS != sCont.end(); ++iterS )
  {
    StateMap::const_iterator mIter = m_theMap.find(*iterS);
    if ( mIter != m_theMap.end() )
    {
      loc.push_back(mIter->second);
    }
    else
    {
      error() << "attempt to convert unknown state" << endmsg;
    }
  } // iter
}
