// $Id: TsaConfirmTool.cpp,v 1.12 2007-12-11 16:30:16 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/ChronoEntity.h"

// event
#include "Event/Track.h"
#include "Event/State.h"

// from Tsa
#include "TsaKernel/TsaConstants.h"
#include "TsaKernel/TsaFun.h"

#include "TsaKernel/SeedHit.h"
#include "TsaKernel/SeedStub.h"
#include "TsaKernel/SeedTrack.h"

#include "TsaKernel/ITsaSeedStep.h"
#include "TsaKernel/ITsaStubFind.h"
#include "TsaKernel/ITsaStubLinker.h"
#include "TsaKernel/ITsaStubExtender.h"

// boost
#include "boost/lexical_cast.hpp"

// local
#include "TsaConfirmTool.h"
#include "TrackParabel.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TsaConfirmTool
//
// 2007-01-26 : Johannes Albrecht
//-----------------------------------------------------------------------------
using namespace LHCb;
using namespace Tf::Tsa;

DECLARE_TOOL_FACTORY( TsaConfirmTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TsaConfirmTool::TsaConfirmTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackConfirmTool>(this);

  //my own variables
  declareProperty("nSigma",m_nsigma=5);
  declareProperty("debugMode", m_debugMode = false);

  //from TsaSeed
  declareProperty("calcLikelihood", m_calcLikelihood = true);
  declareProperty("selectorType", m_selectorType = "Tf::Tsa::SeedSelector");

  //from conversion from SeedTracks to LHCb::Tracks
  declareProperty("LikCut", m_likCut = -30.);

  declareProperty("restrictTx", m_restrictTx = true);
  declareProperty("restrictTy", m_restrictTy = true);
}
//=============================================================================
// Destructor
//=============================================================================
TsaConfirmTool::~TsaConfirmTool() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TsaConfirmTool::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return sc;
  }
  info() << " Initialize TsaConfirmTool" << endmsg;
   
  m_hitMan = tool<Tf::Tsa::TStationHitManager>("Tf::Tsa::TStationHitManager","TsaDataManager");

  // init the tools from TsaSeed
  for (unsigned int i = 0; i < 3; ++i){
    Tf::Tsa::ITsaSeedStep* xStepIT = tool<Tf::Tsa::ITsaSeedStep>
      ("Tf::Tsa::ITXSearch", "xSearchS" + boost::lexical_cast<std::string>(i) , this);
    ITsaSeedStep* stereoStepIT = tool<Tf::Tsa::ITsaSeedStep>("Tf::Tsa::ITStereoSearch", "stereoS" 
							     + boost::lexical_cast<std::string>(i) , this);
    m_xSearchStep.push_back(xStepIT);
    m_stereoStep.push_back(stereoStepIT); 
  }  
  for (unsigned int o = 3; o < 5; ++o){
    ITsaSeedStep* xStepOT = tool<Tf::Tsa::ITsaSeedStep>
      ("Tf::Tsa::OTXSearch", "xSearchS" + boost::lexical_cast<std::string>(o) , this);
    ITsaSeedStep* stereoStepOT = tool<Tf::Tsa::ITsaSeedStep>("Tf::Tsa::OTStereoSearch", "stereoS" 
							     + boost::lexical_cast<std::string>(o) , this);
    m_xSearchStep.push_back(xStepOT);
    m_stereoStep.push_back(stereoStepOT); 
  }

  m_xSelection = tool<ITsaSeedStep>("Tf::Tsa::XProjSelector", "xSelection" , this); 
  m_finalSelection = tool<ITsaSeedStep>(m_selectorType, "finalSelection" , this);
  m_likelihood = tool<ITsaSeedStep>("Tf::Tsa::Likelihood", "likelihood" , this);
  m_stubFind = tool<ITsaStubFind>("Tf::Tsa::StubFind","stubFinder" ,this);
  m_stubLinker = tool<ITsaStubLinker>("Tf::Tsa::StubLinker","stubLinker",this);
  m_extendStubs =  tool<ITsaStubExtender>("Tf::Tsa::StubExtender","stubExtender",this);

  //from TsaSeedtrack conversion
  m_Tsa2TrackCnv = tool<ITsaSeedTrackCnvTool>("Tf::Tsa::SeedTrackCnvTool");
  
  if(m_debugMode){
    //tool for debug information
    m_DataStore = tool<L0ConfDataStore>("L0ConfDataStore");
  }
  
  return StatusCode::SUCCESS;

}

//=============================================================================
StatusCode TsaConfirmTool::tracks(const LHCb::State& seedState, std::vector<Track*>& outputTracks ) 
{
  ChronoEntity tDecode, tTracking;
  if(m_debugMode) tDecode.start();

  TrackParabel::TrackParabel tp(seedState, m_nsigma); 
  m_hitMan->prepareHitsInWindow(tp);
  
  if(msgLevel(MSG::INFO)) 
    debug()<<" number of decoded hits: "<<m_hitMan->hits().size()<<endmsg;
 
  if(m_debugMode) {
    tDecode.stop();
    m_DataStore->nTHits.push_back( m_hitMan->hits().size() ) ;
    m_DataStore->decodingTime.push_back( tDecode.eTotalTime() );
    tTracking.start();
  }
   
  /*
   *  Steering routine for Tsa track seeding
   */
  std::vector<Tf::Tsa::SeedStub*> stubs[3];            //  IT stubs per station
  for (unsigned iS = 0; iS < 3; ++iS) stubs[iS].reserve(100);
  
  // Loop over sectors of tracker (0-2 are IT, 3-4 are OT)
  for ( int sector = 0; sector < 5; ++sector ) {

    if( msgLevel(MSG::DEBUG) ) debug()<<"process sector: "<<sector <<endmsg;
    
    std::vector<Tf::Tsa::SeedHit*> hits[6], sHits[6];  //  Hits per layer in X and stereo
    
    std::vector<Tf::Tsa::SeedTrack*> seeds;            //  Seed candidates within the sector
    seeds.reserve(64);
     
    // x search: give state as input to restrict slope
    LHCb::State aState(seedState);
    if( m_restrictTx ) {
      m_xSearchStep[sector]->execute(aState , seeds , hits ); 
    }
     else {
       m_xSearchStep[sector]->execute(seeds,hits);  
     }
    
    if (sector >2 ) m_xSelection->execute(seeds); // x selection
    
    // add stereo
    if( m_restrictTy ) {
      m_stereoStep[sector]->execute(aState , seeds , sHits );
    }
    else
    {
      m_stereoStep[sector]->execute(seeds,sHits); 
    }
    
    if (m_calcLikelihood == true) m_likelihood->execute(seeds); // likelihood
     
    m_finalSelection->execute(seeds); // final selection
     
    if ( sector <= 2 ) m_stubFind->execute( hits, sHits, stubs );

    //  After the IT stub finding is finished, try to link the stubs to make seed candidates
    if ( sector == 2 ) {
      m_stubLinker->execute( stubs, seeds );
      m_finalSelection->execute(seeds); 
    }

    // Convert TsaSeedTracks to LHCb tracks
    for ( std::vector<Tf::Tsa::SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {
      if ( ((*it)->select() != 0)
           &&((*it)->lik() > m_likCut) ) {
        LHCb::Track* fitTrack = m_Tsa2TrackCnv->convert(*it);
        outputTracks.push_back( fitTrack );
      }
      delete *it;
    }
    
    //  For those IT stubs that remain, try to extend them into the OT
    if ( sector > 2 ) {
      std::vector<SeedTrack*> extendedSeeds; 
      extendedSeeds.reserve(10);
      m_extendStubs->execute( sector, stubs, hits, sHits, extendedSeeds );
      m_finalSelection->execute(extendedSeeds); 
    
      //Convert TsaSeedTracks to LHCb tracks
      for ( std::vector<Tf::Tsa::SeedTrack*>::iterator itEx = extendedSeeds.begin(); extendedSeeds.end() != itEx; ++itEx ) {
        if ( ((*itEx)->select() != 0)
             &&((*itEx)->lik() > m_likCut) ) {
          LHCb::Track* fitTrack = m_Tsa2TrackCnv->convert(*itEx);
          outputTracks.push_back( fitTrack );
        }
        delete *itEx;
      }
    }

    for ( int lay = 0; lay < 6; ++lay ) {
      for ( std::vector<Tf::Tsa::SeedHit*>::iterator it = hits[lay].begin(); hits[lay].end() != it; ++it ) {
        delete *it;
      }
       
      for ( std::vector<Tf::Tsa::SeedHit*>::iterator it = sHits[lay].begin(); sHits[lay].end() != it; ++it ) {
        delete *it;
      }
    }
  }//loop sectors
   
   // clean up the stubs...
  for ( int stn = 0; stn < 3; ++stn ) {
    for ( std::vector<Tf::Tsa::SeedStub*>::iterator it = stubs[stn].begin(); stubs[stn].end() != it; ++it ) {
      delete *it;
    }
  }
  
  if(m_debugMode) {
    tTracking.stop();
    m_DataStore->trackingTime.push_back( tTracking.eTotalTime() );    
  }
   
  if( msgLevel(MSG::INFO) ) 
    debug()<<"tracks found sofar in TsaSearch Tool: "<<outputTracks.size()<<endmsg;

  return StatusCode::SUCCESS;
  
}

