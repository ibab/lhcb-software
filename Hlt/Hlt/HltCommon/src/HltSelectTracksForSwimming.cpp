// $Id: HltSelectTracksForSwimming.cpp,v 1.9 2010-08-13 13:25:13 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "boost/foreach.hpp"

// from Event
#include "HltSelectTracksForSwimming.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltSelectTracksForSwimming 
//
// 2008-02-12 : V. Gligorov
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltSelectTracksForSwimming );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSelectTracksForSwimming::HltSelectTracksForSwimming( const std::string& name, ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selections(*this)
{
  declareProperty("OfflineTrackIDs", m_offlineTrackIDs=std::vector<unsigned int>(1,0)); 
  m_selections.declareProperties();
}

//=============================================================================
// Destructor
//=============================================================================
HltSelectTracksForSwimming::~HltSelectTracksForSwimming() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltSelectTracksForSwimming::initialize() {
  
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_selections.retrieveSelections();
  m_selections.registerSelection();

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSelectTracksForSwimming::execute() {

  m_selections.output()->clean();

  //Lets see what we just did, for debug
  if (msgLevel(MSG::DEBUG)) {

        verbose() << "About to print out a  mountain of crap" << endmsg;

        verbose() << "Printing out the trigger tracks" << endmsg;
        BOOST_FOREACH(const LHCb::Track* iT, *m_selections.input<1>()) {
                verbose() << iT << endmsg;
        }

  }
  //Filter the tracks
  StatusCode sc = filter_Tracks();

  if (msgLevel(MSG::DEBUG)) {
        debug() << "About to print out a mountain of crap" << endmsg;
        debug() << "Printing out the output tracks" << endmsg;
        BOOST_FOREACH(const LHCb::Track *iT, *m_selections.output() ) {
                debug() << iT << endmsg;
        }
  }
  
  int ncan = m_selections.output()->size();
  
  debug() << " candidates found " << ncan << endmsg;
  
  return sc;
  
}
//=============================================================================
StatusCode HltSelectTracksForSwimming::filter_Tracks(){
//Leave only the tracks which are associated to the B in the event


  debug() << "Beginning to match tracks for swimming" << endmsg;

  BOOST_FOREACH(Hlt::TSelection<LHCb::Track>::candidate_type* iT, *m_selections.input<1>() ) {
        verbose() << "Outputing the current trigger track" << endmsg;
        verbose() << iT << endmsg;
        bool trackmatchfound = false;
	//Make a dummy vector
	std::vector<unsigned int> oneTrackIDs;
	oneTrackIDs.clear();
        for( std::vector<unsigned int>::const_iterator iTT=m_offlineTrackIDs.begin();
                                                       iTT!=m_offlineTrackIDs.end();
                                                       ++iTT)
        {
                if (*iTT == 0) {
                	if (tracksMatchInVelo(iT,oneTrackIDs)) {
                        	trackmatchfound = true;
                        	verbose() << "The tracks are a match!" << endmsg;
                        	break;
                	} else verbose() << "No match found... better luck next time!" << endmsg;
			oneTrackIDs.clear();
		} else oneTrackIDs.push_back(*iTT);
        }
        if (trackmatchfound) m_selections.output()->push_back(iT);
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
bool HltSelectTracksForSwimming::tracksMatchInVelo(const LHCb::Track* t1, std::vector<unsigned int> ids2){
//Match the trigger and offline tracks on VELO hits only
//The offline track is passed as a vector of LHCbIDs

  verbose() << "Comparing hits in VELO..." << endmsg;

  int totalnumberofhits = 0; //Set up the counters
  int numberofmatchedhits = 0;

  const std::vector<LHCbID>& ids1 = t1->lhcbIDs(); //Get the LHCbIDs of the track

  for( std::vector<LHCbID>::const_iterator id=ids1.begin();id!=ids1.end();++id){

        //verbose() << "The current hit on Track 1 is " << *id << endmsg;

        if (!(*id).isVelo()) continue; //If it is not a VELO hit, ignore it
        ++totalnumberofhits;
        for( std::vector<unsigned int>::const_iterator id2=ids2.begin();id2!=ids2.end();++id2){

                //verbose() << "The current hit on Track 2 is " << *id2 << endmsg;

		//It is critical that we only pass Velo hits to the tool
                if ((*id).lhcbID() == *id2) {
                        //verbose() << "And they match! Praise the Lord!" << endmsg;
                        ++numberofmatchedhits;
                        break;
                }
        }
  }

  verbose() << "The total number of VELO hits on Track 1 is " << totalnumberofhits << endmsg;
  verbose() << "The total number of matched VELO hits on Track 2 is " << numberofmatchedhits << endmsg;
  verbose() << "For cretins who cannot divide, that makes the match level " << (double) numberofmatchedhits/totalnumberofhits << endmsg;

  double matchlevel = (double) numberofmatchedhits/totalnumberofhits;
  if (matchlevel > 0.7) return true; //The usual 70% requirement
  else return false;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltSelectTracksForSwimming::finalize() {

  StatusCode sc = HltAlgorithm::finalize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  debug() << "==> Finalize" << endmsg;

  return  StatusCode::SUCCESS;
}

