// $Id: HltRecCheckGhosts.cpp,v 1.1 2009-01-30 18:55:59 gligorov Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "boost/foreach.hpp"

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
  //, m_selections(*this)
  
{
  declareProperty("inputTrackSelection", m_inputTrackSelection = "") ;
  //m_selections.declareProperties();
}
//=========================================================================================================
HltRecCheckGhosts::~HltRecCheckGhosts() {
} 
//=========================================================================================================
StatusCode HltRecCheckGhosts::initialize() {

  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first

  m_dataSvc = svc<IHltDataSvc>("HltDataSvc",true);

  //m_selections.retrieveSelections();
  //m_selections.registerSelection();

  m_ghostToolLong       = tool<ITrackGhostClassification>("LongGhostClassification");
  m_ghostToolVelo       = tool<ITrackGhostClassification>("VeloGhostClassification");
  m_ghostToolVeloR      = tool<ITrackGhostClassification>("VeloRGhostClassification");
  m_ghostToolTTrack     = tool<ITrackGhostClassification>("TTrackGhostClassification");
  m_ghostToolUpstream   = tool<ITrackGhostClassification>("UpstreamGhostClassification");
  m_ghostToolDownstream = tool<ITrackGhostClassification>("DownstreamGhostClassification");

  //saveConfiguration();

  return sc;
}
//=========================================================================================================
StatusCode HltRecCheckGhosts::execute() {

  m_ghostResultsLong.clear();
  m_ghostClassesLong.clear();
  m_ghostResultsVelo.clear();
  m_ghostClassesVelo.clear();
  m_ghostResultsVeloR.clear();
  m_ghostClassesVeloR.clear();
  m_ghostResultsTTrack.clear();
  m_ghostClassesTTrack.clear();

  //Make the ntuple
  Tuple m_ghostTuple = nTuple("GhostTuple");

  //m_selections.output()->clean();

  Hlt::Selection& s = m_dataSvc->selection(stringKey(m_inputTrackSelection),this);

  Hlt::TSelection<LHCb::Track>* tracks =  s.down_cast<LHCb::Track >(); 
  //Hlt::TSelection<LHCb::Track>* tracks =  s.down_cast();
  //Lets see what we just did, for debug
  //if (m_debug) {

  debug() << "About to print out a  mountain of crap" << endmsg;

  debug() << "Printing out the tracks firing the trigger" << endmsg;
  BOOST_FOREACH( LHCb::Track* tT, *tracks/**m_selections.input<1>()*/) {
    verbose() << tT << endmsg;
  }

  //}

  verbose() << "About to loop over the selected tracks" << endreq;

  BOOST_FOREACH( LHCb::Track* ghostTrack, *tracks/**m_selections.input<1>()*/) {

    LHCb::GhostTrackInfo ghostInfoLong;    

    //Get the long track decision
    if (ghostTrack == 0) {
      verbose() << "Null pointer retrieved for the track firing the trigger!" << endreq;
      continue;
    }
    verbose() << "Triggering track type = " << ghostTrack->type() << endreq;
 
    //Is it a ghost at all?
    bool isThisAGhost = m_ghostToolLong->isGhost(*ghostTrack);
    if (isThisAGhost) {
      m_ghostResultsLong.push_back(1);
      verbose() << "This long track is a ghost!" << endreq;

      //m_selections.output()->push_back(const_cast<LHCb::Track*> (ghostTrack));

      m_ghostToolLong->info(*ghostTrack,ghostInfoLong);
      verbose() << "This long ghost is classified as " << ghostInfoLong.classification() << endreq;
      m_ghostClassesLong.push_back(ghostInfoLong.classification());
    } else {
      m_ghostResultsLong.push_back(0);
      m_ghostClassesLong.push_back(1);
    }
    
    classifyParts(ghostTrack);

  }
  writeNtuple(m_ghostTuple);

  return StatusCode::SUCCESS;  
}
//=========================================================================================================
StatusCode HltRecCheckGhosts::writeNtuple(Tuple& m_ghostTuple){
  //Write out the nutple
  StatusCode test;

  //First the long tracks
  test = m_ghostTuple->farray( "isGhostLong",
                               m_ghostResultsLong.begin(), m_ghostResultsLong.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "ghostClassesLong",
                               m_ghostClassesLong.begin(), m_ghostClassesLong.end(),
                               "nLong", 10000 );
  if( !test.isSuccess() ) return StatusCode::FAILURE;

  //The the velo components (number is equal by definition) 
  test = m_ghostTuple->farray( "isGhostVelo",
                               m_ghostResultsVelo.begin(), m_ghostResultsVelo.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "ghostClassesVelo",
                               m_ghostClassesVelo.begin(), m_ghostClassesVelo.end(),
                               "nLong", 10000 );
  if( !test.isSuccess() ) return StatusCode::FAILURE;

  //Then the VeloR components
  test = m_ghostTuple->farray( "isGhostVeloR",
                               m_ghostResultsVeloR.begin(), m_ghostResultsVeloR.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "ghostClassesVeloR",
                               m_ghostClassesVeloR.begin(), m_ghostClassesVeloR.end(),
                               "nLong", 10000 );
  if( !test.isSuccess() ) return StatusCode::FAILURE;

  //Finally the TTRack componenets
  test = m_ghostTuple->farray( "isGhostTTrack",
                               m_ghostResultsTTrack.begin(), m_ghostResultsTTrack.end(),
                               "nLong", 10000 );
  test = m_ghostTuple->farray( "ghostClassesTTrack",
                               m_ghostClassesTTrack.begin(), m_ghostClassesTTrack.end(),
                               "nLong", 10000 );
  if( !test.isSuccess() ) return StatusCode::FAILURE;

  m_ghostTuple->write();

}
//=========================================================================================================
StatusCode HltRecCheckGhosts::classifyParts(const LHCb::Track* ghostTrack) {
  //Classifies the VELO and T parts of the track by making two dummy tracks
  //And adding the appropriate measurements to them.

  verbose() << "Getting ready to classify the parts of the ghost track in question" << endreq;

  LHCb::Track* dummyVeloTrack = new LHCb::Track();
  bool dummyVeloTrackIsAGhost = false;
  LHCb::Track* dummyVeloRTrack = new LHCb::Track();
  bool dummyVeloRTrackIsAGhost = false;
  LHCb::Track* dummyTTrack = new LHCb::Track();
  bool dummyTTrackIsAGhost = false;

  LHCb::GhostTrackInfo ghostInfoVelo;
  LHCb::GhostTrackInfo ghostInfoVeloR;
  LHCb::GhostTrackInfo ghostInfoTTrack;

  verbose() << "About to reset the dummy tracks" << endreq;

  dummyVeloTrack->reset();
  dummyVeloRTrack->reset();
  dummyTTrack->reset();

  verbose() << "About to classify the parts of the ghost track" << endreq;

  if (ghostTrack == 0) {
    verbose() << "Something has gone very wrong indeed!" << endreq;
    return StatusCode::SUCCESS;
  }

  verbose() << "About to get the measurements" << endreq;

  for (std::vector<LHCb::Measurement*>::const_iterator iM = (ghostTrack->measurements()).begin(); 
                                                       iM != (ghostTrack->measurements()).end(); ++iM){
    //Not strictly necessary singe GhostClassification only uses LHCbIDs but we can keep it
    //for possible future use

    if (*iM == 0) {
      verbose() << "Something has gone very wrong indeed!" << endreq;
      continue;
    }
    
    //verbose() << "About to add the appropriate measurements to the dummy tracks" << endreq;

    if ((*iM)->type() == 1 || (*iM)->type() == 2 ) { //VELOR, VELOPHI
      dummyVeloTrack->addToMeasurements(**iM);
    }
    if ((*iM)->type() == 1) { //VELOR only
      dummyVeloRTrack->addToMeasurements(**iM);
    }
    if ((*iM)->type() == 4 || (*iM)->type() == 5 || //IT,OT
        (*iM)->type() == 8 //IT lite
       ) {
      dummyTTrack->addToMeasurements(**iM);
    }

  }

  verbose() << "About to get the LHCbIDs" << endreq;
  for (std::vector<LHCb::LHCbID>::const_iterator iID = (ghostTrack->lhcbIDs()).begin();
                                                 iID != (ghostTrack->lhcbIDs()).end(); ++iID){

    //verbose() << "About to add the appropriate LHCbIDs to the dummy tracks" << endreq;

    if ((*iID).isVelo()) { //VELOR, VELOPHI
      dummyVeloTrack->addToLhcbIDs(*iID);
    }
    if ((*iID).isVeloR()) { //VELOR only
      dummyVeloRTrack->addToLhcbIDs(*iID);
    }
    if ((*iID).isIT() || (*iID).isOT()) { //IT+OT
      dummyTTrack->addToLhcbIDs(*iID);
    }

  }

  verbose() << "About to run the classification tool on the dummy tracks" << endreq;

  //Now we classify the two dummy tracks above
  dummyVeloTrackIsAGhost = m_ghostToolVelo->isGhost(*dummyVeloTrack);
  if (dummyVeloTrackIsAGhost) {
    m_ghostResultsVelo.push_back(1);
    verbose() << "This Velo component is a ghost" << endreq; 

    m_ghostToolVelo->info(*dummyVeloTrack,ghostInfoVelo);
    verbose() << "This Velo component is classified as " << ghostInfoVelo.classification() << endreq;
    m_ghostClassesVelo.push_back(ghostInfoVelo.classification());
  } else {
      m_ghostResultsVelo.push_back(0);
      m_ghostClassesVelo.push_back(1);
  }

  dummyVeloRTrackIsAGhost = m_ghostToolVeloR->isGhost(*dummyVeloRTrack);
  if (dummyVeloRTrackIsAGhost) {
    m_ghostResultsVeloR.push_back(1);
    verbose() << "This VeloR component is a ghost" << endreq; 
    
    m_ghostToolVeloR->info(*dummyVeloRTrack,ghostInfoVeloR);
    verbose() << "This VeloR component is classified as " << ghostInfoVeloR.classification() << endreq;
    m_ghostClassesVeloR.push_back(ghostInfoVeloR.classification());
  } else {
      m_ghostResultsVeloR.push_back(0);
      m_ghostClassesVeloR.push_back(1);
  }

  dummyTTrackIsAGhost = m_ghostToolTTrack->isGhost(*dummyTTrack);
  if (dummyTTrackIsAGhost) {
    m_ghostResultsTTrack.push_back(1);
    verbose() << "This TTrack component is a ghost" << endreq;

    m_ghostToolTTrack->info(*dummyTTrack,ghostInfoTTrack);
    verbose() << "This TTrack component is classified as " << ghostInfoTTrack.classification() << endreq;
    m_ghostClassesTTrack.push_back(ghostInfoTTrack.classification());
  } else {
      m_ghostResultsTTrack.push_back(0);
      m_ghostClassesTTrack.push_back(1);
  }

  delete dummyVeloTrack;
  delete dummyVeloRTrack;
  delete dummyTTrack; 

  return StatusCode::SUCCESS;
}
//=========================================================================================================
StatusCode HltRecCheckGhosts::finalize() {
  return GaudiTupleAlg::finalize();
}
