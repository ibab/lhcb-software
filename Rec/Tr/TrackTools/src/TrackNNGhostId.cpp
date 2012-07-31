// Include files 
#include "Event/Track.h"
//#include "Kernel/HitPattern.h"


// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "Event/OTTime.h"
#include "Event/STCluster.h"
#include "Event/VeloCluster.h"

#include "TMVA/TMVAClassification_MLP_fittedVelo.C"
#include "TMVA/TMVAClassification_MLP_fittedUpstream.C"
#include "TMVA/TMVAClassification_MLP_fittedDownstream.C"
#include "TMVA/TMVAClassification_MLP_fittedTtrack.C"
#include "TMVA/TMVAClassification_MLP_fittedLong.C"

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
DECLARE_TOOL_FACTORY( TrackNNGhostId )


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

  /**
   *   initialize input variable names as used for training
   *   and class readers for all track types
   *
   */
  //velo tracks
  m_inNames.clear();
  m_inNames.push_back( "track_probChi2" );
  m_inNames.push_back( "nVeloHits+nTTHits+nITHits+nOTHits" );
  m_inNames.push_back( "track_veloHits" );
  m_readerVelo = new ReadMLP_fittedVelo( m_inNames );

  //upstream tracks
  m_inNames.clear();
  m_inNames.push_back( "track_probChi2" );
  m_inNames.push_back( "track_ttHits" );
  m_inNames.push_back( "nVeloHits+nTTHits+nITHits+nOTHits" );
  m_inNames.push_back( "track_veloHits" );
  m_readerUpstream = new ReadMLP_fittedUpstream( m_inNames );

  //downstream tracks
  m_inNames.clear();
  m_inNames.push_back( "track_probChi2" );
  m_inNames.push_back( "track_ttHits" );
  m_inNames.push_back( "nVeloHits+nTTHits+nITHits+nOTHits" );
  m_inNames.push_back( "track_itHits" );
  m_inNames.push_back( "track_otHits" );
  m_readerDownstream = new ReadMLP_fittedDownstream( m_inNames );

  //Ttracks
  m_inNames.clear();
  m_inNames.push_back( "track_probChi2" );
  m_inNames.push_back( "nVeloHits+nTTHits+nITHits+nOTHits" );
  m_inNames.push_back( "track_itHits" );
  m_inNames.push_back( "track_otHits" );
  m_readerTtrack = new ReadMLP_fittedTtrack( m_inNames );

  //long tracks
  m_inNames.clear();
  m_inNames.push_back( "track_probChi2" );
  m_inNames.push_back( "track_fitMatchChi2" );
  m_inNames.push_back( "track_ttHits" );
  m_inNames.push_back( "track_nCandCommonHits" );
  m_inNames.push_back( "nVeloHits+nTTHits+nITHits+nOTHits" );
  m_inNames.push_back( "track_veloHits" );
  m_inNames.push_back( "track_itHits" );
  m_inNames.push_back( "track_otHits" );
  m_readerLong = new ReadMLP_fittedLong( m_inNames );
  
  m_inputVec = new std::vector<double>;
  
  m_otdecoder = tool<IOTRawBankDecoder>("OTRawBankDecoder");

  return StatusCode::SUCCESS;
}


StatusCode TrackNNGhostId::execute(LHCb::Track& aTrack) const{

  bool isDebug   = msgLevel(MSG::DEBUG);
  double retval = 0;
  if ( UNLIKELY( isDebug ) ) debug() << "==> Execute" << endmsg;

  int veloHits = 0, ttHits=0,itHits=0,otHits=0;  
  const std::vector<LHCb::LHCbID> ids = aTrack.lhcbIDs();
  for(unsigned int i = 0; i < ids.size(); ++i){
    if(ids[i].isVelo()) ++veloHits;
    if(ids[i].isTT()) ++ttHits;
    if(ids[i].isIT()) ++itHits;
    if(ids[i].isOT()) ++otHits;
  }

  
  int nVeloCont = 0;
  int nTTCont = 0;
  int nITCont = 0;


  if (exist<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default)){
    LHCb::VeloClusters* veloCont = get<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default);
    nVeloCont = veloCont->size();
  }
  if (exist<LHCb::STClusters>(LHCb::STClusterLocation::TTClusters)){
    LHCb::STClusters *ttCont = get<LHCb::STClusters>(LHCb::STClusterLocation::TTClusters);
    nTTCont = ttCont->size();
  }
  if (exist<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters)){
    LHCb::STClusters *itCont = get<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters);
    nITCont = itCont->size();
  }

  int nHitMult = nVeloCont + nTTCont + nITCont + m_otdecoder->totalNumberOfHits();


  
  /**
   *   now evaluate the MVA value for different track types
   *
   */
  if( aTrack.checkType(LHCb::Track::Velo) ){
    m_inputVec->clear();
    m_inputVec->reserve(3);
    m_inputVec->push_back( aTrack.probChi2() );
    m_inputVec->push_back( nHitMult );
    m_inputVec->push_back( veloHits );
    // retrive the classifier responses            
    retval = m_readerVelo->GetMvaValue( *m_inputVec );
  }//end evaluate velo track

  if( aTrack.checkType(LHCb::Track::Upstream) ){
    m_inputVec->clear();
    m_inputVec->reserve(4);
    m_inputVec->push_back( aTrack.probChi2() );
    m_inputVec->push_back( ttHits );
    m_inputVec->push_back( nHitMult );
    m_inputVec->push_back( veloHits );
    // retrive the classifier responses            
    retval = m_readerUpstream->GetMvaValue( *m_inputVec );
  }//end evaluate upstream track

  if( aTrack.checkType(LHCb::Track::Downstream) ){
    m_inputVec->clear();
    m_inputVec->reserve(5);
    m_inputVec->push_back( aTrack.probChi2() );
    m_inputVec->push_back( ttHits );
    m_inputVec->push_back( nHitMult );
    m_inputVec->push_back( itHits );
    m_inputVec->push_back( otHits );
    // retrive the classifier responses            
    retval = m_readerDownstream->GetMvaValue( *m_inputVec );
  }//end evaluate downstream track
  
  if( aTrack.checkType(LHCb::Track::Ttrack) ){ 
    m_inputVec->clear();
    m_inputVec->reserve(4);
    m_inputVec->push_back( aTrack.probChi2() );
    m_inputVec->push_back( nHitMult );
    m_inputVec->push_back( itHits );
    m_inputVec->push_back( otHits );
    // retrive the classifier responses            
    retval = m_readerTtrack->GetMvaValue( *m_inputVec );
  }//end evaluate Ttrack track
  
  if( aTrack.checkType(LHCb::Track::Long) ){
    m_inputVec->clear();
    m_inputVec->reserve(8);
    m_inputVec->push_back( aTrack.probChi2() );
    m_inputVec->push_back( aTrack.info(LHCb::Track::FitMatchChi2,-60) );
    m_inputVec->push_back( ttHits);
    m_inputVec->push_back( aTrack.info(LHCb::Track::NCandCommonHits,-60));
    m_inputVec->push_back( nHitMult);
    m_inputVec->push_back( veloHits);
    m_inputVec->push_back( itHits);
    m_inputVec->push_back( otHits);
    if(-60 != (*m_inputVec)[1]
       && -60 != (*m_inputVec)[3] ){
      // retrive the classifier responses            
      retval = m_readerLong->GetMvaValue( *m_inputVec );
    } else {
      if( UNLIKELY( isDebug ) ) debug()<<"warning: extra info of track not propetly filled ....  "<<endmsg;
    }
  }//end evaluate long track
  

  //normalize the output to the interval [0,1]
  if(retval<-1) retval=-1;
  if(retval>1) retval=1;
  retval = -0.5*double(retval-1.);
  
  if( UNLIKELY ( isDebug ) ) {
    for(unsigned int i=0;i<m_inputVec->size();i++)  debug()<<"input variable [ "<<i<<" ] :"<<(*m_inputVec)[i]<<endmsg;
    debug() << "direct NN output is: "<<retval<<endmsg;
    debug() << "normlized NN output is: "<<retval<<endmsg;
  } 

  aTrack.setGhostProbability(retval);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode TrackNNGhostId::finalize()
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;

  delete m_readerVelo;
  delete m_readerUpstream;
  delete m_readerDownstream;
  delete m_readerTtrack;
  delete m_readerLong;
  delete m_inputVec;
  
  return GaudiTool::finalize();  // must be called after all other actions
}
