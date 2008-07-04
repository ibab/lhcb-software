// $Id: HltSelectTracksForSwimming.cpp,v 1.3 2008-07-04 08:07:41 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

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
{

  declareProperty( "ParticlesName" , m_particlesName = "");
}

//=============================================================================
// Destructor
//=============================================================================
HltSelectTracksForSwimming::~HltSelectTracksForSwimming() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltSelectTracksForSwimming::initialize() {

  info() << "Starts" << endmsg;
  
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_inputTracks = &(retrieveTSelection<LHCb::Track>(m_inputSelectionName));

  if (!m_inputSelection2Name.empty()) {
    m_inputTracks2 =
      &(retrieveTSelection<LHCb::Track>(m_inputSelection2Name));
  }

  m_outputTracks = &(registerTSelection<LHCb::Track>());

  saveConfiguration();
  info() << "HltAlgorithm initialized" << endmsg;

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltSelectTracksForSwimming::execute() {

  StatusCode sc = HltAlgorithm::execute(); // must be executed first

  //Check if particles exist
  if (!exist<Particles>(m_particlesName)) return sc;

  Particles* pars = get<Particles>(m_particlesName);
  if (m_debug) {
    if (pars == 0) verbose() << " no particles found! " << endmsg;
    else verbose() << " particles found " << pars->size() << endmsg;
  }  
  if (pars == 0) return sc;
  
  m_outputTracks->clean();

  //Lets see what we just did, for debug
  if (m_debug) {

        verbose() << "About to print out a  mountain of crap" << endmsg;

        verbose() << "Printing out the trigger tracks" << endmsg;
        for (iT = (*m_inputTracks).begin(); iT != (*m_inputTracks).end(); ++iT) {

                verbose() << (*iT) << endmsg;

        }

        debug() << "Printing out the B tracks" << endmsg;
        for (iT = (*m_inputTracks2).begin(); iT != (*m_inputTracks2).end(); ++iT) {

                debug() << (*iT) << endmsg;

        }

  }
  //Filter the tracks
  sc = filter_Tracks();

  if (m_debug) {

        debug() << "About to print out a mountain of crap" << endmsg;

        debug() << "Printing out the output tracks" << endmsg;
        for (iT = (*m_outputTracks).begin(); iT != (*m_outputTracks).end(); ++iT) {

                debug() << (*iT) << endmsg;

        }

  }
  
  int ncan = m_outputTracks->size();
  //candidateFound(ncan);
  
  debug() << " candidates found " << ncan << endmsg;
  if (m_debug)
    printInfo(" tracks from particles ",*m_outputTracks);
  
  return sc;
  
}
//=============================================================================
StatusCode HltSelectTracksForSwimming::filter_Tracks(){
//Leave only the tracks which are associated to the B in the event

  bool trackmatchfound = false;

  debug() << "Beginning to match tracks for swimming" << endmsg;

  for (iT = (*m_inputTracks).begin(); iT != (*m_inputTracks).end(); ++iT) {

        verbose() << "Outputing the current trigger track" << endmsg;
        verbose() << *iT << endmsg;

        for (iTT = (*m_inputTracks2).begin(); iTT != (*m_inputTracks2).end(); ++iTT) {

                verbose() << "Outputing the current B track" << endmsg;
                verbose() << *iTT << endmsg;

                if (tracksMatchInVelo(*iT,*iTT)) {

                        trackmatchfound = true;
                        verbose() << "The tracks are a match!" << endmsg;
                        break;

                } else verbose() << "No match found... better luck next time!" << endmsg;

        }

        if (trackmatchfound) m_outputTracks->push_back( const_cast<Track*>(*iT));
        trackmatchfound = false;

  }

  return StatusCode::SUCCESS;
}
//=============================================================================
bool HltSelectTracksForSwimming::tracksMatchInVelo(LHCb::Track* t1, LHCb::Track* t2){
//Match the trigger and offline tracks on VELO hits only

  verbose() << "Comparing hits in VELO..." << endmsg;

  int totalnumberofhits = 0; //Set up the counters
  int numberofmatchedhits = 0;

  const std::vector<LHCbID>& ids1 = t1->lhcbIDs(); //Get the LHCbIDs of the tracks
  const std::vector<LHCbID>& ids2 = t2->lhcbIDs();

  for( std::vector<LHCbID>::const_iterator id=ids1.begin();id!=ids1.end();++id){

        //verbose() << "The current hit on Track 1 is " << *id << endmsg;

        if (!(*id).isVelo()) continue; //If it is not a VELO hit, ignore it
        ++totalnumberofhits;
        for( std::vector<LHCbID>::const_iterator id2=ids2.begin();id2!=ids2.end();++id2){

                //verbose() << "The current hit on Track 2 is " << *id2 << endmsg;

                if (!(*id2).isVelo()) continue; //If it is not a VELO hit, ignore it
                if (*id == *id2) {
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

