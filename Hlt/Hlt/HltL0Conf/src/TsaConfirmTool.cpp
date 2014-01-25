// $Id: TsaConfirmTool.cpp,v 1.22 2010-08-11 12:16:20 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/ChronoEntity.h"

// event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/StateParameters.h"

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

#include "HltBase/ParabolaHypothesis.h"
#include "TfKernel/TStationHitManager.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TsaConfirmTool
//
// 2007-01-26 : Johannes Albrecht
//-----------------------------------------------------------------------------
using namespace LHCb;
using namespace Tf::Tsa;

DECLARE_TOOL_FACTORY( TsaConfirmTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TsaConfirmTool::TsaConfirmTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_l0ConfExtrapolator(0)
  , m_hitMan(0)
  , m_xSelection(0)
  , m_finalSelection(0)
  , m_likelihood(0)
  , m_stubFind(0)
  , m_stubLinker(0)
  , m_extendStubs(0)
  , m_Tsa2TrackCnv(0)
  , m_DataStore(0)
{
  declareInterface<ITrackConfirmTool>(this);

  //my own variables
  declareProperty("nSigma",m_nsigma = 5.);
  declareProperty("debugMode", m_debugMode = false);

  //from TsaSeed
  declareProperty("calcLikelihood", m_calcLikelihood = true);
  declareProperty("selectorType", m_selectorType = "Tf::Tsa::SeedSelector");

  //from conversion from SeedTracks to LHCb::Tracks
  declareProperty("LikCut", m_likCut = -30.);

  declareProperty("restrictTx", m_restrictTx = true);
  declareProperty("restrictTy", m_restrictTy = true);

  declareProperty("minHitsInOT", m_minHitsInOT = 14);
}
//=============================================================================
// Destructor
//=============================================================================
TsaConfirmTool::~TsaConfirmTool() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TsaConfirmTool::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }
  info() << " Initialize TsaConfirmTool" << endmsg;
   
  m_hitMan = tool<Tf::Tsa::TStationHitManager>("Tf::Tsa::TStationHitManager","TsaDataManager");

  // init the tools from TsaSeed
  for (unsigned int i = 0; i < 3; ++i) {
    Tf::Tsa::ITsaSeedStep* xStepIT = tool<Tf::Tsa::ITsaSeedStep>
      ("Tf::Tsa::ITXSearch", "xSearchS" + boost::lexical_cast<std::string>(i) , this);
    ITsaSeedStep* stereoStepIT = tool<Tf::Tsa::ITsaSeedStep>("Tf::Tsa::ITStereoSearch", "stereoS" 
                                                             + boost::lexical_cast<std::string>(i) , this);
    m_xSearchStep.push_back(xStepIT);
    m_stereoStep.push_back(stereoStepIT); 
  }  
  for (unsigned int o = 3; o < 5; ++o) {
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
  
  if (m_debugMode) {
    //tool for debug information
    m_DataStore = tool<L0ConfDataStore>("L0ConfDataStore");
  }
  
  m_l0ConfExtrapolator = tool<IL0ConfExtrapolator>("L0ConfExtrapolator");
  

  return sc;
}

//=============================================================================
StatusCode TsaConfirmTool::tracks(const LHCb::State& seedState, std::vector<Track*>& outputTracks ) 
{
  ChronoEntity tDecode, tTracking;
  if (m_debugMode) tDecode.start();
 
  ParabolaHypothesis tp = prepareT( seedState );
    
  if (m_debugMode) {
    tDecode.stop();
    m_DataStore->nTHits.push_back( m_hitMan->hits().size() ) ;
    m_DataStore->trackHypo.push_back(new ParabolaHypothesis(tp));
    m_DataStore->decodingTime.push_back( tDecode.eTotalTime() );
    if (msgLevel(MSG::INFO)) 
      debug()<<" number of decoded hits: "<<m_hitMan->hits().size()<<endmsg;
    tTracking.start();
  }
  unsigned minHits = m_minHitsInOT;
  // work out if we're likely to hit the IT - if so, we're satisfied with
  // half the number of hits
  double x = seedState.x() + seedState.tx() *
    (StateParameters::ZBegT - seedState.z());
  double y = seedState.y() + seedState.ty() *
    (StateParameters::ZBegT - seedState.z());
  if (std::abs(x) < 700. && std::abs(y) < 250.) minHits /= 2;
  else {
    x += seedState.tx() * (StateParameters::ZMidT - StateParameters::ZBegT);
    y += seedState.ty() * (StateParameters::ZMidT - StateParameters::ZBegT);
    if (std::abs(x) < 700. && std::abs(y) < 250.) minHits /= 2;
    else {
      x += seedState.tx() * (StateParameters::ZEndT - StateParameters::ZMidT);
      y += seedState.ty() * (StateParameters::ZEndT - StateParameters::ZMidT);
      if (std::abs(x) < 700. && std::abs(y) < 250.) minHits /= 2;
    }
  }

  StatusCode sc;
  // if there are too few hits in the window, we won't even start
  if (m_hitMan->hits().size() >= minHits) {
    /*
     *  Steering routine for Tsa track seeding
     */
    std::vector<Tf::Tsa::SeedStub*> stubs[3];            //  IT stubs per station
    for (unsigned iS = 0; iS < 3; ++iS) stubs[iS].reserve(100);

    // Loop over sectors of tracker (0-2 are IT, 3-4 are OT)
    for ( int sector = 0; sector < 5; ++sector ) {

      if ( msgLevel(MSG::DEBUG) ) debug()<<"process sector: "<<sector <<endmsg;

      std::vector<Tf::Tsa::SeedHit*> hits[6], sHits[6];  //  Hits per layer in X and stereo

      std::vector<Tf::Tsa::SeedTrack*> seeds;            //  Seed candidates within the sector
      seeds.reserve(64);

      // x search: give state as input to restrict slope
      LHCb::State aState(seedState);
      if ( m_restrictTx ) {
        sc = m_xSearchStep[sector]->execute(aState , seeds , hits ); 
        if (sc.isFailure()) {
          return Error("x search failed", StatusCode::FAILURE,1);
        }
        
      } else {
        sc = m_xSearchStep[sector]->execute(seeds,hits);  
        if (sc.isFailure()) {
          return Error("x search failed", StatusCode::FAILURE,1);
        }
      }

      if (sector >2 ) {
        sc = m_xSelection->execute(seeds); // x selection
        if (sc.isFailure()) {
          return Error("x selection failed", StatusCode::FAILURE,1);
        }
      }
      
      // add stereo
      if ( m_restrictTy ) {
        sc = m_stereoStep[sector]->execute(aState , seeds , sHits );
        if (sc.isFailure()) {
          return Error("stereo search failed", StatusCode::FAILURE,1);
        }
      } else {
        sc = m_stereoStep[sector]->execute(seeds,sHits); 
        if (sc.isFailure()) {
          return Error("stereo search failed", StatusCode::FAILURE,1);
        }
      }

      if (m_calcLikelihood == true) {
        sc = m_likelihood->execute(seeds); // likelihood
        if (sc.isFailure()) {
          return Error("likelihood failed", StatusCode::FAILURE,1);
        }
      }
      
        
      sc = m_finalSelection->execute(seeds); // final selection
      if (sc.isFailure()) {
        return Error("selection failed", StatusCode::FAILURE,1);
      }

      if ( sector <= 2 ) {
        sc = m_stubFind->execute( hits, sHits, stubs );
        if (sc.isFailure()) {
          return Error("stub finding failed", StatusCode::FAILURE,1);
        }
      }
      
        

      //  After the IT stub finding is finished, try to link the stubs to make seed candidates
      if ( sector == 2 ) {
        sc = m_stubLinker->execute( stubs, seeds );
        if (sc.isFailure()) {
          return Error("stub linking failed", StatusCode::FAILURE,1);
        }
        sc = m_finalSelection->execute(seeds); 
        if (sc.isFailure()) {
          return Error("stub selection failed", StatusCode::FAILURE,1);
        }
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
        sc = m_extendStubs->execute( sector, stubs, hits, sHits, extendedSeeds );
        if (sc.isFailure()) {
          return Error("stub extension", StatusCode::FAILURE,1);
        }

        sc = m_finalSelection->execute(extendedSeeds); 
        if (sc.isFailure()) {
          return Error("extended stub selection failed", StatusCode::FAILURE,1);
        }

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
    } //loop sectors

      // clean up the stubs...
    for ( int stn = 0; stn < 3; ++stn ) {
      for ( std::vector<Tf::Tsa::SeedStub*>::iterator it = stubs[stn].begin(); stubs[stn].end() != it; ++it ) {
        delete *it;
      }
    }
  }
  if (m_debugMode) {
    tTracking.stop();
    m_DataStore->trackingTime.push_back( tTracking.eTotalTime() );    
  }

  if ( msgLevel(MSG::INFO) ) 
    debug()<<"tracks found sofar in TsaSearch Tool: "<<outputTracks.size()<<endmsg;

  m_hitMan->clearHits();
  
  return StatusCode::SUCCESS;
}

ParabolaHypothesis TsaConfirmTool::prepareT( const LHCb::State& seedState )
{
  ParabolaHypothesis tp = m_l0ConfExtrapolator->getParabolaHypothesis( seedState, m_nsigma );
  m_hitMan->prepareHitsInWindow(tp);
  return tp;
}


ParabolaHypothesis TsaConfirmTool::prepareT( const LHCb::State& seedState ,std::vector<LHCb::LHCbID>& ids)
{

  //prepare hits in T-stations
  ParabolaHypothesis tp = prepareT( seedState );
  Tf::TStationHitManager<Tf::Tsa::SeedingHit>::HitRange hits = m_hitMan->hits();
  
  //get lhcbids from hits and fill output
  Tf::TStationHitManager<Tf::Tsa::SeedingHit>::HitRange::const_iterator it;
  for ( it = hits.begin();it != hits.end();++it){
    //  Tf::Tsa::SeedingHit* myHit = (*it);
    LHCb::LHCbID id = (*it)->hit()->lhcbID();
    ids.push_back(id);
  }
  return tp;
}
