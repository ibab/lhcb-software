// $Id: TrackNNGhostId.cpp,v 1.2 2009-10-21 07:14:19 albrecht Exp $
// Include files 
#include "Event/Track.h"
//#include "Kernel/HitPattern.h"


// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "Event/OTTime.h"
#include "Event/STCluster.h"
#include "Event/VeloCluster.h"

#include "TMVA/TMVAClassification_MLP_longTracks.C"

// local
#include "TrackNNGhostId.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackNNGhostId
//
// 2009-10-06 : Johannes Albrecht
//
//  Tool to calculate a track quality variable using a trained (TMVA) netork
//
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TrackNNGhostId );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackNNGhostId::TrackNNGhostId( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackManipulator>(this);
  
}
//=============================================================================
// Destructor
//=============================================================================
TrackNNGhostId::~TrackNNGhostId() {} 

//=============================================================================

StatusCode TrackNNGhostId::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  //input variable names as used for training
  m_inputVars.push_back( "track_probChi2" );
  m_inputVars.push_back( "track_fitMatchChi2" );
  m_inputVars.push_back( "track_ttHits" );
  m_inputVars.push_back( "track_nCandCommonHits" );
  m_inputVars.push_back( "nVeloHits+nTTHits+nITHits+nOTHits" );
  m_inputVars.push_back( "track_veloHits" );
  m_inputVars.push_back( "track_itHits" );
  m_inputVars.push_back( "track_otHits" );
  
  m_classReader = new ReadMLP_long( m_inputVars );
  
  m_inputVec = new std::vector<double>( 8 );
  
  m_otdecoder = tool<IOTRawBankDecoder>("OTRawBankDecoder");

  return StatusCode::SUCCESS;
}


StatusCode TrackNNGhostId::execute(LHCb::Track& aTrack) const{

  bool isDebug   = msgLevel(MSG::DEBUG);

  if ( isDebug ) debug() << "==> Execute" << endmsg;

  if( aTrack.history() != LHCb::Track::PatForward 
      && aTrack.history() != LHCb::Track::TrackMatching )  {

    if ( isDebug ) debug() << "track type is "<<aTrack.history() 
                           <<", currently no ghost classification for this type possible!"<< endmsg;

    return StatusCode::SUCCESS;
  }
  

  
  int veloHits = 0, ttHits=0,itHits=0,otHits=0;  
  const std::vector<LHCb::LHCbID> ids = aTrack.lhcbIDs();
  for(unsigned int i = 0; i < ids.size(); ++i){
    if(ids[i].isVelo()) ++veloHits;
    if(ids[i].isTT()) ++ttHits;
    if(ids[i].isIT()) ++itHits;
    if(ids[i].isOT()) ++otHits;
  }
  
  LHCb::VeloClusters* veloCont = get<LHCb::VeloClusters>("Raw/Velo/Clusters");
  LHCb::STClusters *ttCont = get<LHCb::STClusters>("Raw/TT/Clusters");
  LHCb::STClusters *itCont = get<LHCb::STClusters>("Raw/IT/Clusters");
  int nHitMult = veloCont->size()+ttCont->size()+itCont->size()+m_otdecoder->totalNumberOfHits();
  
  //const LHCb::HitPattern hitPat(aTrack.lhcbIDs());
    
  (*m_inputVec)[0] = aTrack.probChi2();
  (*m_inputVec)[1] = aTrack.info(LHCb::Track::FitMatchChi2,-60);
  (*m_inputVec)[2] = ttHits;
  (*m_inputVec)[3] = aTrack.info(LHCb::Track::NCandCommonHits,-60);
  (*m_inputVec)[4] = nHitMult;
  (*m_inputVec)[5] = veloHits;
  (*m_inputVec)[6] = itHits;
  (*m_inputVec)[7] = otHits;

  if ( isDebug ) debug()<<"m_input variables:"
                        <<"\n ProbChi2:         "<< (*m_inputVec)[0]
                        <<"\n FitMatchChi:      "<< (*m_inputVec)[1]
                        <<"\n ttHits:           "<< (*m_inputVec)[2]
                        <<"\n NCandCommonHits:  "<< (*m_inputVec)[3]
                        <<"\n nTotalHits:       "<< (*m_inputVec)[4]
                        <<"\n velotHits         "<< (*m_inputVec)[5]
                        <<"\n itHits:           "<< (*m_inputVec)[6]
                        <<"\n otHits:           "<< (*m_inputVec)[7]
                        <<endmsg;
  
  
  if(-60 == (*m_inputVec)[1]
     || -60 == (*m_inputVec)[3] ){
    if ( isDebug ) debug()<<"warning: extra info of track not propetly filled ....  exit"<<endmsg;
    return StatusCode::SUCCESS;
  }
  
  
  // retrive the classifier responses            
  double retval = m_classReader->GetMvaValue( *m_inputVec );
    
  //normalize the output to the interval [0,1]
  if(retval<-1) retval=-1;
  retval = double(retval-1.)/double(-2.);

  if ( isDebug ) debug() << "normlized NN output is: "<<retval<<endmsg;

  aTrack.setGhostProbability(retval);


  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode TrackNNGhostId::finalize()
{
  debug() << "==> Finalize" << endmsg;

  delete m_classReader;
  delete m_inputVec;
  
  return GaudiTool::finalize();  // must be called after all other actions
}
