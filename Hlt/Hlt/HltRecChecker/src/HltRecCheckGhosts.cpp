// $Id: HltRecCheckGhosts.cpp,v 1.12 2009-12-24 14:13:20 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "boost/foreach.hpp"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "HltBase/HltSelection.h"

// local
#include "HltRecCheckGhosts.h"

using namespace LHCb;
//---------------------------------------------------------------------------
// Implementation file for class : HltRecCheckGhosts
//
// 2009-01-26 : V Gligorov
//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltRecCheckGhosts );
//=========================================================================================================
HltRecCheckGhosts::HltRecCheckGhosts( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ) 
  , m_hltSvc(0)
  , m_regSvc(0)
  , m_ghostToolLong(0)
  , m_ghostToolVelo(0)
  , m_ghostToolVeloR(0)
  , m_ghostToolTTrack(0)
  , m_ghostToolUpstream(0)
  , m_ghostToolDownstream(0)
  , m_veloExpectation(0)
  
{
  declareProperty("inputTrackSelection", m_inputTrackSelection ) ;
  declareProperty("purityCut", m_purityCut = 0.7) ;
}
//=========================================================================================================
HltRecCheckGhosts::~HltRecCheckGhosts() {
} 
//=========================================================================================================
StatusCode HltRecCheckGhosts::initialize() {

  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first

  m_regSvc = svc<Hlt::IRegister> ( "Hlt::Service" , true ) ;
  sc = m_regSvc->registerInput(m_inputTrackSelection,this);
  m_hltSvc = svc<Hlt::IData> ( "Hlt::Service" , true ) ;

  m_ghostToolLong       = tool<ITrackGhostClassification>("LongGhostClassification",this);
  m_ghostToolVelo       = tool<ITrackGhostClassification>("VeloGhostClassification",this);
  m_ghostToolVeloR      = tool<ITrackGhostClassification>("VeloRGhostClassification",this);
  m_ghostToolTTrack     = tool<ITrackGhostClassification>("TTrackGhostClassification",this);
  m_ghostToolUpstream   = tool<ITrackGhostClassification>("UpstreamGhostClassification",this);
  m_ghostToolDownstream = tool<ITrackGhostClassification>("DownstreamGhostClassification",this);
  m_veloExpectation     = tool<IVeloExpectation>("VeloExpectation",this);

  //Get the tool for associating track parts
  m_linkTool = tool<ILHCbIDsToMCParticles>("LHCbIDsToMCParticles");  

  return sc;
}
//=========================================================================================================
StatusCode HltRecCheckGhosts::execute() {

  //We need to clear the vertors each time we run the execute, otherwise
  //funny things can happen.  
  m_ghostResultsLong.clear();
  m_ghostClassesLong.clear();
  m_isFromBorDforLong.clear();
  m_nHitsLong.clear();
  m_nLinkedLong.clear();
  m_slopeLong_X.clear();
  m_slopeLong_Y.clear();
  m_pLong.clear();
  m_ptLong.clear();
  m_etaLong.clear();
  m_phiLong.clear();
  m_firstStateLong_X.clear();
  m_firstStateLong_Y.clear();
  m_firstStateLong_Z.clear();
  m_Chi2Long.clear();
  m_nDoFLong.clear();
  m_nStatesLong.clear();

  m_ghostResultsVelo.clear();
  m_ghostClassesVelo.clear();
  m_nHitsVelo.clear();
  m_nLinkedVelo.clear();
  m_nMissedBeforeBeamline.clear();
  m_nMissedInVelo.clear();

  m_ghostResultsVeloR.clear();
  m_ghostClassesVeloR.clear();
  m_nHitsVeloR.clear();
  m_nLinkedVeloR.clear();

  m_ghostResultsTTrack.clear();
  m_ghostClassesTTrack.clear();
  m_nHitsTTrack.clear();
  m_nLinkedTTrack.clear();

  m_nEvent.clear();
  m_nRun.clear();

  //ODIN for the event info
  LHCb::ODIN* odin(0);
  int eventNumber = -1;
  int runNumber = -1;

  //Grab the linker, this is for long tracks only
  //for their parts will need to do something else...
  LinkedTo<MCParticle> link(evtSvc(), msgSvc(), "Hlt/Track/Forward");
  odin = getIfExists<ODIN>( LHCb::ODINLocation::Default ); 
  if( NULL!=odin )
  {
    eventNumber = odin->eventNumber();
    runNumber = odin->runNumber();
    info() << "Recording ntuple for event and run number " << eventNumber << " and " << runNumber << endmsg;
  } 
  else 
  {
    Error("Can't get LHCb::ODINLocation::Default (" +
          LHCb::ODINLocation::Default + ")" );
  }

  if (msgLevel(MSG::DEBUG)) debug() << "About to retrieve the selection" << endmsg;

  const Hlt::Selection* s = m_hltSvc->selection(m_inputTrackSelection,this);

  if (s == NULL) return StatusCode::SUCCESS;

  if (msgLevel(MSG::DEBUG)) debug() << "About to retrieve the tracks" << endmsg;

  //We need to down cast what the data service does in order to get
  //a vector of tracks for us to look at
  const Hlt::TSelection<LHCb::Track>* tracks =  s->down_cast<LHCb::Track >();

  if (msgLevel(MSG::DEBUG)) debug() << "About to print out a  mountain of crap" << endmsg;

  if (tracks == 0) return StatusCode::SUCCESS;

  //Make the ntuple
  Tuple m_ghostTuple = nTuple("GhostTuple");

  if (msgLevel(MSG::DEBUG)) debug() << "Printing out the tracks in input container" << endmsg;
  BOOST_FOREACH( const LHCb::Track* tT, *tracks) {
    verbose() << tT << endmsg;
  }

  if (msgLevel(MSG::VERBOSE)) verbose() << "About to loop over the selected tracks" << endreq;

  BOOST_FOREACH( const LHCb::Track* ghostTrack, *tracks) {

    LHCb::GhostTrackInfo ghostInfoLong;    

    //Get the long track decision
    if (ghostTrack == 0) {
      if (msgLevel(MSG::VERBOSE)) verbose() << "Null pointer retrieved for a track in input container!" << endreq;
      continue;
    }
    if (msgLevel(MSG::VERBOSE)) verbose() << "Track type = " << ghostTrack->type() << endreq;
 
    //Save basic track information
    m_nHitsLong.push_back((*ghostTrack).nLHCbIDs());
    m_slopeLong_X.push_back((*ghostTrack).slopes().X());
    m_slopeLong_Y.push_back((*ghostTrack).slopes().Y());
    m_pLong.push_back((*ghostTrack).p());
    m_ptLong.push_back((*ghostTrack).pt());
    m_etaLong.push_back((*ghostTrack).pseudoRapidity());
    m_phiLong.push_back((*ghostTrack).phi());
    m_firstStateLong_X.push_back((*ghostTrack).position().X());
    m_firstStateLong_Y.push_back((*ghostTrack).position().Y());
    m_firstStateLong_Z.push_back((*ghostTrack).position().Z());
    m_Chi2Long.push_back((*ghostTrack).chi2());
    m_nDoFLong.push_back((*ghostTrack).nDoF());
    m_nStatesLong.push_back((*ghostTrack).nStates());
 
    //Save the event information 
    m_nEvent.push_back(eventNumber);
    m_nRun.push_back(runNumber);

    //Is it a ghost at aill? Use the track associators to check
    MCParticle* mcpar = link.first( (*ghostTrack).key() );
    bool isThisAGhost = true;
    if (mcpar) isThisAGhost = false;

    if (isThisAGhost) {
      m_ghostResultsLong.push_back(1);
      m_isFromBorDforLong.push_back(0);
      if (msgLevel(MSG::VERBOSE)) verbose() << "This long track is a ghost!" << endreq;

      m_ghostToolLong->info(*ghostTrack,ghostInfoLong);
      if (msgLevel(MSG::VERBOSE)) verbose() << "This long ghost is classified as " << ghostInfoLong.classification() << endreq;
      m_ghostClassesLong.push_back(ghostInfoLong.classification());
      m_nLinkedLong.push_back(ghostInfoLong.nLinked());
    } 
    else 
    {
      //If not a ghost get the best linked MCParticle and check where it comes from
      m_ghostToolLong->info(*ghostTrack,ghostInfoLong);
      MCParticle* bestMatch = (ghostInfoLong.bestLink()).first;

      //Find the last non-zero mother of the bestMatch track
      //We want to know if the track was "signal" meaning from a B or D.
      if (bestMatch) {
        //Check if it is from the PV
        if (bestMatch->mother() == NULL) m_isFromBorDforLong.push_back(0);
        else {
          const LHCb::MCParticle* finalmother;
          const LHCb::MCParticle* tmpmother = bestMatch;
          bool foundBorD = false; 

          do { //go through all mothers until you find the final one or find a B or D mother
            finalmother = tmpmother;
            tmpmother = finalmother->mother();
            if (tmpmother!=NULL) {
              if (tmpmother->particleID().abspid() > 400 && tmpmother->particleID().abspid() < 600) foundBorD = true;
            }
          } while (tmpmother != NULL && !foundBorD);
          //Either we found an interesting mother or we didn't 
          if (foundBorD) m_isFromBorDforLong.push_back(1);
          else m_isFromBorDforLong.push_back(0);
        }
      } else m_isFromBorDforLong.push_back(0); //For things that the ghostClassifier calls a ghost
                                               //but the track associators don't       
 
      m_ghostResultsLong.push_back(0); //Force the track associators result for this variable
      m_ghostClassesLong.push_back(ghostInfoLong.classification());
      m_nLinkedLong.push_back(ghostInfoLong.nLinked());
    }
    
    //Classify the Velo and T station parts of the track 
    classifyParts(ghostTrack);

  }
  //Write the Ntuple for offline analysis 
  writeNtuple(m_ghostTuple);

  return StatusCode::SUCCESS;  
}
//=========================================================================================================
StatusCode HltRecCheckGhosts::writeNtuple(Tuple& m_ghostTuple){
  //Write out the nutple
  StatusCode test;

  test = m_ghostTuple->farray( "nEvent",
                               m_nEvent.begin(), m_nEvent.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "nRun",
                               m_nRun.begin(), m_nRun.end(),
                               "nLong", 10000 );

  //First the long tracks
  test = m_ghostTuple->farray( "isGhostLong",
                               m_ghostResultsLong.begin(), m_ghostResultsLong.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "ghostClassesLong",
                               m_ghostClassesLong.begin(), m_ghostClassesLong.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "isFromBorDforLong",
                               m_isFromBorDforLong.begin(), m_isFromBorDforLong.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "nHitsLong",
                               m_nHitsLong.begin(), m_nHitsLong.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "nLinkedLong",
                               m_nLinkedLong.begin(), m_nLinkedLong.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "slopeLong_X",
                               m_slopeLong_X.begin(), m_slopeLong_X.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "slopeLong_Y",
                               m_slopeLong_Y.begin(), m_slopeLong_Y.end(),
                               "nLong", 10000 ); 
  test = m_ghostTuple->farray( "pLong",
                               m_pLong.begin(), m_pLong.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "ptLong",
                               m_ptLong.begin(), m_ptLong.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "etaLong",
                               m_etaLong.begin(), m_etaLong.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "phiLong",
                               m_phiLong.begin(), m_phiLong.end(),
                               "nLong", 10000 ); 
  test = m_ghostTuple->farray( "firstStateLong_X",
                               m_firstStateLong_X.begin(), m_firstStateLong_X.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "firstStateLong_Y",
                               m_firstStateLong_Y.begin(), m_firstStateLong_Y.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "firstStateLong_Z",
                               m_firstStateLong_Z.begin(), m_firstStateLong_Z.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "Chi2Long",
                               m_Chi2Long.begin(), m_Chi2Long.end(),
                               "nLong", 10000 ); 
  test = m_ghostTuple->farray( "nDoFLong",
                               m_nDoFLong.begin(), m_nDoFLong.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "nStatesLong",
                               m_nStatesLong.begin(), m_nStatesLong.end(),
                               "nLong", 10000 );
 
  if( !test.isSuccess() ) return StatusCode::FAILURE;

  //The the velo components (number is equal by definition) 
  test = m_ghostTuple->farray( "isGhostVelo",
                               m_ghostResultsVelo.begin(), m_ghostResultsVelo.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "ghostClassesVelo",
                               m_ghostClassesVelo.begin(), m_ghostClassesVelo.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "nHitsVelo",
                               m_nHitsVelo.begin(), m_nHitsVelo.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "nLinkedVelo",
                               m_nLinkedVelo.begin(), m_nLinkedVelo.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "nMissedBeforeBeamline",
                               m_nMissedBeforeBeamline.begin(), m_nMissedBeforeBeamline.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "nMissedInVelo",
                               m_nMissedInVelo.begin(), m_nMissedInVelo.end(),
                               "nLong", 10000 );
  if( !test.isSuccess() ) return StatusCode::FAILURE;

  //Then the VeloR components
  test = m_ghostTuple->farray( "isGhostVeloR",
                               m_ghostResultsVeloR.begin(), m_ghostResultsVeloR.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "ghostClassesVeloR",
                               m_ghostClassesVeloR.begin(), m_ghostClassesVeloR.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "nHitsVeloR",
                               m_nHitsVeloR.begin(), m_nHitsVeloR.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "nLinkedVeloR",
                               m_nLinkedVeloR.begin(), m_nLinkedVeloR.end(),
                               "nLong", 10000 );
  if( !test.isSuccess() ) return StatusCode::FAILURE;

  //Finally the TTRack componenets
  test = m_ghostTuple->farray( "isGhostTTrack",
                               m_ghostResultsTTrack.begin(), m_ghostResultsTTrack.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "ghostClassesTTrack",
                               m_ghostClassesTTrack.begin(), m_ghostClassesTTrack.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "nHitsTTrack",
                               m_nHitsTTrack.begin(), m_nHitsTTrack.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "nLinkedTTrack",
                               m_nLinkedTTrack.begin(), m_nLinkedTTrack.end(),
                               "nLong", 10000 );

  if( !test.isSuccess() ) return StatusCode::FAILURE;

  m_ghostTuple->write();

  return StatusCode::SUCCESS;

}
//=========================================================================================================
StatusCode HltRecCheckGhosts::classifyParts(const LHCb::Track* ghostTrack) {
  //Classifies the VELO and T parts of the track by making two dummy tracks
  //And adding the appropriate measurements to them.

  if (msgLevel(MSG::VERBOSE)) verbose() << "Getting ready to classify the parts of the ghost track in question" << endreq;

  LHCb::Track* dummyVeloTrack = new LHCb::Track();
  bool dummyVeloTrackIsAGhost = false;
  LHCb::Track* dummyVeloRTrack = new LHCb::Track();
  bool dummyVeloRTrackIsAGhost = false;
  LHCb::Track* dummyTTrack = new LHCb::Track();
  bool dummyTTrackIsAGhost = false;

  //The number of velo measurements matters for the number of 
  //missed hits calculation
  int numDummyVeloMeas = 0;

  LHCb::GhostTrackInfo ghostInfoVelo;
  LHCb::GhostTrackInfo ghostInfoVeloR;
  LHCb::GhostTrackInfo ghostInfoTTrack;

  if (msgLevel(MSG::VERBOSE)) verbose() << "About to reset the dummy tracks" << endreq;

  //Some paranoia initializing
  dummyVeloTrack->reset();
  dummyVeloRTrack->reset();
  dummyTTrack->reset();

  if (msgLevel(MSG::VERBOSE)) verbose() << "About to classify the parts of the ghost track" << endreq;

  //We should never be passed a null track here, but we are paranoid...
  if (ghostTrack == 0) {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Something has gone very wrong indeed!" << endreq;
    return StatusCode::SUCCESS;
  }

  //These states matter for the number of missed hits calculation
  //on the Velo track
  if (msgLevel(MSG::VERBOSE)) verbose() << "About to add states to the VELO track" << endreq;
  dummyVeloTrack->addToStates(*ghostTrack->stateAt(LHCb::State::ClosestToBeam)); //closest to beam
  dummyVeloTrack->addToStates(*ghostTrack->stateAt(LHCb::State::EndVelo)); //End of the Velo

  if (msgLevel(MSG::VERBOSE)) verbose() << "About to get the measurements" << endreq;

  //Now we add the LHCbIDs to our dummy tracks, this is what
  //will allow us to associate them in a moment
  if (msgLevel(MSG::VERBOSE)) verbose() << "About to get the LHCbIDs" << endreq;
  for (std::vector<LHCb::LHCbID>::const_iterator iID = (ghostTrack->lhcbIDs()).begin();
                                                 iID != (ghostTrack->lhcbIDs()).end(); ++iID){

    if ((*iID).isVelo()) { //VELOR, VELOPHI
      dummyVeloTrack->addToLhcbIDs(*iID);
      ++numDummyVeloMeas;
    }
    if ((*iID).isVeloR()) { //VELOR only
      dummyVeloRTrack->addToLhcbIDs(*iID);
    }
    if ((*iID).isIT() || (*iID).isOT()) { //IT+OT
      dummyTTrack->addToLhcbIDs(*iID);
    }

  }

  if (msgLevel(MSG::VERBOSE)) verbose() << "About to run the classification tool on the dummy tracks" << endreq;

  //Now we classify the two dummy tracks above
  if (checkTrackPart(dummyVeloTrack)) dummyVeloTrackIsAGhost = false; else dummyVeloTrackIsAGhost = true;

  m_nHitsVelo.push_back((*dummyVeloTrack).nLHCbIDs());
  //The number of missed hits is only meaningful for the velo track, not the 
  //VeloR or T. We want both the number missed before the beamline, and the 
  //number missed in the Velo as a whole.
  m_nMissedBeforeBeamline.push_back(m_veloExpectation->nMissed(*dummyVeloTrack));
  m_nMissedInVelo.push_back(m_veloExpectation->nExpected(*dummyVeloTrack,zBeamLine(*dummyVeloTrack),850.) - numDummyVeloMeas);
  if (dummyVeloTrackIsAGhost) {
    m_ghostResultsVelo.push_back(1);
    if (msgLevel(MSG::VERBOSE)) verbose() << "This Velo component is a ghost" << endreq; 

    m_ghostToolVelo->info(*dummyVeloTrack,ghostInfoVelo);
    if (msgLevel(MSG::VERBOSE)) verbose() << "This Velo component is classified as " << ghostInfoVelo.classification() << endreq;
    m_ghostClassesVelo.push_back(ghostInfoVelo.classification());
    m_nLinkedVelo.push_back(ghostInfoVelo.nLinked());
  } else {
      m_ghostToolVelo->info(*dummyVeloTrack,ghostInfoVelo);
      m_ghostResultsVelo.push_back(0);
      m_ghostClassesVelo.push_back(ghostInfoVelo.classification());
      m_nLinkedVelo.push_back(ghostInfoVelo.nLinked());
  }

  if (checkTrackPart(dummyVeloRTrack)) dummyVeloRTrackIsAGhost = false; else dummyVeloRTrackIsAGhost = true;

  m_nHitsVeloR.push_back((*dummyVeloRTrack).nLHCbIDs());
  if (dummyVeloRTrackIsAGhost) {
    m_ghostResultsVeloR.push_back(1);
    if (msgLevel(MSG::VERBOSE)) verbose() << "This VeloR component is a ghost" << endreq; 
    
    m_ghostToolVeloR->info(*dummyVeloRTrack,ghostInfoVeloR);
    if (msgLevel(MSG::VERBOSE)) verbose() << "This VeloR component is classified as " << ghostInfoVeloR.classification() << endreq;
    m_ghostClassesVeloR.push_back(ghostInfoVeloR.classification());
    m_nLinkedVeloR.push_back(ghostInfoVeloR.nLinked());
  } else {
      m_ghostToolVeloR->info(*dummyVeloRTrack,ghostInfoVeloR);
      m_ghostResultsVeloR.push_back(0);
      m_ghostClassesVeloR.push_back(ghostInfoVeloR.classification());
      m_nLinkedVeloR.push_back(ghostInfoVeloR.nLinked());
  }

  if (checkTrackPart(dummyTTrack)) dummyTTrackIsAGhost = false; else dummyTTrackIsAGhost = true;
  
  m_nHitsTTrack.push_back((*dummyTTrack).nLHCbIDs());
  if (dummyTTrackIsAGhost) {
    m_ghostResultsTTrack.push_back(1);
    if (msgLevel(MSG::VERBOSE)) verbose() << "This TTrack component is a ghost" << endreq;

    m_ghostToolTTrack->info(*dummyTTrack,ghostInfoTTrack);
    if (msgLevel(MSG::VERBOSE)) verbose() << "This TTrack component is classified as " << ghostInfoTTrack.classification() << endreq;
    m_ghostClassesTTrack.push_back(ghostInfoTTrack.classification());
    m_nLinkedTTrack.push_back(ghostInfoTTrack.nLinked());
  } else { 
      m_ghostToolTTrack->info(*dummyTTrack,ghostInfoTTrack);
      m_ghostResultsTTrack.push_back(0);
      m_ghostClassesTTrack.push_back(ghostInfoTTrack.classification());
      m_nLinkedTTrack.push_back(ghostInfoTTrack.nLinked());
  }

  //Lets not create a memory leak... 
  delete dummyVeloTrack;
  delete dummyVeloRTrack;
  delete dummyTTrack; 

  return StatusCode::SUCCESS;
}
//=========================================================================================================
bool HltRecCheckGhosts::checkTrackPart(LHCb::Track* trackPart) {
//This function tests whether a part of a track is a ghost using the 
//simple 70% criterion as per the Track Associators, ignoring for
//now the >2 Velo hits, >2 T hits criterion since these are met
//by default for tracks in the trigger right now 
//The point is that we want to compare the TrackAssociators and
//ghost classification results, to keep an eye on tracks which
//meet the 70% criterion of the TA but suffer pollution from shared
//hits between many MCParticles and are hence called ghosts by the
//GC tool. This involves some code duplication but such is life.

  //Get the first and last LHCbIDs on the track
  LHCbIDs::const_iterator start = trackPart->lhcbIDs().begin();
  LHCbIDs::const_iterator stop = trackPart->lhcbIDs().end(); 
  
  // get the information on the track
  ILHCbIDsToMCParticles::LinkMap testMap;
  m_linkTool->link(start,stop,testMap);

  //Now we need to find the best link and see if it meets the 70% criterion
  double bestMatch = 0.0;
  for (LHCb::GhostTrackInfo::LinkMap::const_iterator iterMap = testMap.begin();
       iterMap != testMap.end(); 
       ++iterMap
      ){
    if ( iterMap->second > bestMatch ){
      bestMatch = iterMap->second;
    }
  } 

  const double purity = bestMatch/(double (stop - start));

  if (purity > m_purityCut) return true; else return false;

}	
//=========================================================================================================
double HltRecCheckGhosts::zBeamLine(const LHCb::Track& aTrack) const {
//This code is duplicated in many places in LHCb software,
//someone ought to make it a public method eventually but
//I am not volunteering.
  double z =0;
  const LHCb::State& state = aTrack.closestState(0);
  if (state.checkLocation(LHCb::State::ClosestToBeam) == true) {
    z = state.z();
  } else { 
    const Gaudi::TrackVector& vec = state.stateVector();
    double z = state.z();
    if (fabs(vec[2]) > TrackParameters::lowTolerance 
        || fabs(vec[3]) > TrackParameters::lowTolerance ) {
      z -= (vec[0]*vec[2] + vec[1]*vec[3])/(vec[2]*vec[2]+vec[3]*vec[3]);
    }
  }
  return z;
}
//=========================================================================================================
StatusCode HltRecCheckGhosts::finalize() {
  return GaudiTupleAlg::finalize();
}
