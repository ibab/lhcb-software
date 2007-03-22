// $Id: TsaConfirmTool.cpp,v 1.4 2007-03-22 18:56:03 albrecht Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/ChronoEntity.h"

// event
#include "Event/Track.h"
#include "Event/State.h"

//from Tsa for initialization
#include "TsaKernel/IITDataSvc.h"
#include "TsaKernel/IOTDataSvc.h"

// from Tsa
#include "TsaKernel/TsaConstants.h"
#include "TsaKernel/TsaFun.h"
#include "TsaKernel/ITsaCollector.h"
#include "TsaKernel/STCluster.h"
#include "TsaKernel/OTCluster.h"

#include "TsaKernel/SeedHit.h"
#include "TsaKernel/SeedTrack.h"
#include "TsaKernel/SeedStub.h"

#include "Event/STLiteCluster.h"
#include "Event/OTTime.h"

#include "TrackInterfaces/ITrackPtKick.h"

#include "TsaKernel/ITsaSeedStep.h"
#include "TsaKernel/ITsaStubFind.h"
#include "TsaKernel/ITsaStubLinker.h"
#include "TsaKernel/ITsaStubExtender.h"

// boost
#include "boost/lexical_cast.hpp"

// local
#include "TsaConfirmTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TsaConfirmTool
//
// 2007-01-26 : Johannes Albrecht
//-----------------------------------------------------------------------------
using namespace LHCb;

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
  declareProperty("debugInfo", m_debugInfo = false);

  //from initialization
  declareProperty("otDataSvcType", m_otDataSvcType = "OTDataSvc");
  declareProperty("itDataSvcType", m_itDataSvcType = "ITDataSvc");
  declareProperty("otDataSvcName", m_otDataSvcName = "OTDataSvc");
  declareProperty("itDataSvcName", m_itDataSvcName = "ITDataSvc");
  declareProperty("initIT", m_initIT = true);
  declareProperty("initOT", m_initOT = true);

  //from TsaSeed
  declareProperty("maxNumHits", m_maxNumHits = 10000);
  declareProperty("calcLikelihood", m_calcLikelihood = true);

  declareProperty("selectorType", m_selectorType = "TsaSeedSelector");
  
  //from TsaSeedtrack conversion
  declareProperty("eX2",m_EX2 = 31*Gaudi::Units::mm2);
  declareProperty("eY2",m_EY2 = 12*Gaudi::Units::mm2); 
  declareProperty("eTX2",m_ETx2 = 2.0e-3);
  declareProperty("eTY2",m_ETy2 = 2.0e-3); // 2e-7;
  declareProperty("eQdivP2",m_EQdivP2 = 0.01);

  declareProperty("LikCut", m_likCut = -30.);
  declareProperty("curvatureFactor", m_curvFactor = 42.0/Gaudi::Units::MeV);
  declareProperty("pFromCurvature", m_pFromCurvature = false);
  declareProperty("increaseErrors", m_largeErrors = true);
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
  always() << " Initialize TsaConfirmTool" << endmsg;
  
  
  //from TsaInitialization
  m_itDataSvc = tool<IITDataSvc>(m_itDataSvcType ,m_itDataSvcName);
  m_otDataSvc = tool<IOTDataSvc>(m_otDataSvcType,m_otDataSvcName);

  // Retrieve TsaCollector tool
  m_tsacollector = tool<ITsaCollector>("TsaCollector");
  
  // init the tools from TsaSeed
  for (unsigned int i = 0; i < 3; ++i){
    ITsaSeedStep* xStepIT = tool<ITsaSeedStep>("TsaITXSearch", "xSearchS" + boost::lexical_cast<std::string>(i) , this);
    ITsaSeedStep* stereoStepIT = tool<ITsaSeedStep>("TsaITStereoSearch", "stereoS" 
                                                    + boost::lexical_cast<std::string>(i) , this);
    m_xSearchStep.push_back(xStepIT);
    m_stereoStep.push_back(stereoStepIT); 
  }  
  for (unsigned int o = 3; o < 5; ++o){
    ITsaSeedStep* xStepOT = tool<ITsaSeedStep>("TsaOTXSearch", "xSearchS" + boost::lexical_cast<std::string>(o) , this);
    ITsaSeedStep* stereoStepOT = tool<ITsaSeedStep>("TsaOTStereoSearch", "stereoS" 
                                                    + boost::lexical_cast<std::string>(o) , this);
    m_xSearchStep.push_back(xStepOT);
    m_stereoStep.push_back(stereoStepOT); 
  }

  m_xSelection = tool<ITsaSeedStep>("TsaXProjSelector", "xSelection" , this); 
  m_finalSelection = tool<ITsaSeedStep>(m_selectorType, "finalSelection" , this);
  m_likelihood = tool<ITsaSeedStep>("TsaLikelihood", "likelihood" , this);
  m_stubFind = tool<ITsaStubFind>("TsaStubFind","stubFinder" ,this);
  m_stubLinker = tool<ITsaStubLinker>("TsaStubLinker","stubLinker",this);
  m_extendStubs =  tool<ITsaStubExtender>("TsaStubExtender","stubExtender",this);

  //from TsaSeedtrack conversion
  m_ptKickTool = tool<ITrackPtKick>("TrackPtKick");

  
  return StatusCode::SUCCESS;

}

//=============================================================================
StatusCode TsaConfirmTool::tracks(const LHCb::State& seedState, std::vector<Track*>& outputTracks ) 
{

  ChronoEntity tCollect, tInit, tTracking, tConvert;
  tCollect.start();

  Tsa::OTClusters* otClusters = new Tsa::OTClusters();
  Tsa::STClusters* itClusters = new Tsa::STClusters();
  
  int nSigmaTmp;
  //   if( fabs(seedState.pt() ) < 5000 ) {
  //     nSigmaTmp = 10*m_nsigma;
  //   }
  //   else{
  //     nSigmaTmp = m_nsigma;
  //   }
  
   if(m_debugInfo){
     debug()<<" myStatePos.pt() "<<  seedState.pt() <<endmsg;
     debug()<<"m_nsigma "<<nSigmaTmp <<"\n"<<endmsg;
   }
   
   //select hits in n sigma window and write them in TsaCluster vector
   m_tsacollector->collect( seedState , otClusters , m_nsigma );
   m_tsacollector->collect( seedState , itClusters , m_nsigma );
  
   tCollect.stop();

   if(m_debugInfo){
    debug() <<"In Tool:"<<endmsg;
    debug() << " n tsa IT clusters p (" << m_nsigma << " sigma)   = " << itClusters->size() << endmsg;
    debug() << " n tsa OT clusters p (" << m_nsigma << " sigma)   = " << otClusters->size() << endmsg;
    
   }

   m_stClsuters = itClusters->size();
   m_otClusters = otClusters->size();
   
  //***************************************************************************************
  // initialization of Tsa seeding
  tInit.start();
  if (m_initIT == true) m_itDataSvc->initializeEvent(itClusters);
  if (m_initOT == true) m_otDataSvc->initializeEvent(otClusters);
  tInit.stop();
  //***************************************************************************************
  //-------------------------------------------------------------------------
  //  Steering routine for track seeding
  //-------------------------------------------------------------------------
  
  //@ja: it and ot hits only for nHits cut?
  //@ja weg?
  tTracking.start();
  typedef LHCb::STLiteCluster::FastContainer FastContainer;
  LHCb::OTTimes* otCont = get<LHCb::OTTimes>(LHCb::OTTimeLocation::Default);
  FastContainer* liteCont = get<FastContainer>( LHCb::STLiteClusterLocation::ITClusters);
  double nHit = liteCont->size() + otCont->size();
  m_nTHits = int(nHit);

   if(m_debugInfo) debug()<<"number of hits (OT + IT) = "<<nHit <<endmsg;
  
   SeedTracks* seedSel = new SeedTracks();    //  Selected seed candidates
   seedSel->reserve(1000);
   
   std::vector<SeedStub*> stubs[3];            //  IT stubs per station
   for (unsigned iS = 0; iS < 3; ++iS) stubs[iS].reserve(100);
   
   SeedStubs* stubsCont = new SeedStubs();  
   stubsCont->reserve(200);
   
   SeedHits* hitsCont = new SeedHits();
   hitsCont->reserve(11000);
   
   if (nHit > m_maxNumHits) return StatusCode::SUCCESS;
   
   // Loop over sectors of tracker (0-2 are IT, 3-4 are OT)
   for ( int sector = 0; sector < 5; ++sector ) {
     
     std::vector<SeedHit*> hits[6], sHits[6];  //  Hits per layer in X and stereo
     
     std::vector<SeedTrack*> seeds;            //  Seed candidates within the sector
     seeds.reserve(1000);
     
     m_xSearchStep[sector]->execute(seeds,hits);  // x search
     
     if (sector >2 ) m_xSelection->execute(seeds); // x selection
     
     m_stereoStep[sector]->execute(seeds,sHits); // add stereo

     if (m_calcLikelihood == true) m_likelihood->execute(seeds); // likelihood
     
     m_finalSelection->execute(seeds); // final selection
     
     if ( sector <= 2 ) m_stubFind->execute( hits, sHits, stubs );

     //  After the IT stub finding is finished, try to link the stubs to make seed candidates
     if ( sector == 2 ) m_stubLinker->execute( stubs, seeds );
     
     //Delete the temporary objects that have been created
     for ( std::vector<SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {
       seedSel->insert( *it);
     }

     //  For those IT stubs that remain, try to extend them into the OT
     if ( sector > 2 ) {
      std::vector<SeedTrack*> extendedSeeds; 
      extendedSeeds.reserve(50);
      m_extendStubs->execute( sector, stubs, hits, sHits, extendedSeeds );
      for ( std::vector<SeedTrack*>::iterator itEx = extendedSeeds.begin(); extendedSeeds.end() != itEx; ++itEx ) {
        seedSel->insert( *itEx);
      }
     }

     for ( int lay = 0; lay < 6; ++lay ) {
       for ( std::vector<SeedHit*>::iterator it = hits[lay].begin(); hits[lay].end() != it; ++it ) {
         hitsCont->insert(*it);
         //         (*it)->setUse( false );
      }
      
      for ( std::vector<SeedHit*>::iterator it = sHits[lay].begin(); sHits[lay].end() != it; ++it ) {

        hitsCont->insert(*it);
        //        (*it)->setUse( false );
      }
     }
  }//loop sectors
  
  // clean up the stubs...
  for ( int stn = 0; stn < 3; ++stn ) {
    for ( std::vector<SeedStub*>::iterator it = stubs[stn].begin(); stubs[stn].end() != it; ++it ) {
      stubsCont->insert(*it);
    }
  }

  tTracking.stop();
  
  tConvert.start();
  //******************************************************************
  //from TsaSeedTrackConvertor

  for (SeedTracks::const_iterator iterTrack = seedSel->begin(); 
       iterTrack != seedSel->end(); ++iterTrack) {
    
    if ( ((*iterTrack)->select() != 0)
         &&((*iterTrack)->lik() > m_likCut) ) {
      
      LHCb::Track* fitTrack = this->convert(*iterTrack);
      outputTracks.push_back( &(*fitTrack) );
    }
  }

  tConvert.stop();
  
  if( m_debugInfo ) debug()<<"tracks found sofar in TsaSearch Tool: "<<outputTracks.size()<<endmsg;

  m_collectTime = int( tCollect.eTotalTime());
  m_trackingTime = int( tInit.eTotalTime() + tTracking.eTotalTime() + tConvert.eTotalTime() );
  m_nModules = -1;//moduleList.size();

  delete otClusters;
  delete itClusters;
  delete hitsCont;
  delete stubsCont;
  delete seedSel;
  
  return StatusCode::SUCCESS;
  
}

LHCb::Track* TsaConfirmTool::convert(const SeedTrack* aTrack) const{

  // make a fit track...
  LHCb::Track* fitTrack = new LHCb::Track();


  std::vector<SeedPnt> pnts = aTrack->usedPnts();
  //std::cout << " starting a track " << pnts.size() << std::endl; 
  for ( std::vector<SeedPnt>::const_iterator itP = pnts.begin(); pnts.end() != itP; ++itP ) {
    fitTrack->addToLhcbIDs((*itP).hit()->clus()->id());
    //std::cout << (*itP).hit()->ITChan() << std::endl;
  } //it

  addState(aTrack, fitTrack, pnts.back().z());

  // add history
  fitTrack->setHistory(LHCb::Track::TsaTrack);
  fitTrack->setType(LHCb::Track::Ttrack);
  fitTrack ->setPatRecStatus( LHCb::Track::PatRecIDs );
  fitTrack->addInfo(LHCb::Track::Likelihood, aTrack->lik());

  return fitTrack;
}


void TsaConfirmTool::addState(const SeedTrack* aTrack, LHCb::Track* lTrack, const double z) const{

  // first  state info
  Gaudi::TrackVector stateVec = Gaudi::TrackVector();
  Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();

  // initial errors - either large or from seed
  if (m_largeErrors == true){
    stateCov(0,0) = m_EX2;
    stateCov(1,1) = m_EY2;
    stateCov(2,2) = m_ETx2;
    stateCov(3,3) = m_ETy2;
  }
  else {
    // x
    stateCov(0,0) = aTrack->xErr(0);
    stateCov(2,2) = aTrack->xErr(3); 
    stateCov(0,1) = aTrack->xErr(1);
    if (m_pFromCurvature == true){
      stateCov(0,4) = aTrack->xErr(2)*m_curvFactor;
      stateCov(2,4) = aTrack->xErr(4)*m_curvFactor;
    }
    // y  
    stateCov(1,1) = aTrack->yErr(0);
    stateCov(3,3) = aTrack->yErr(2); 
    stateCov(1,3) = aTrack->yErr(1);
  }


  LHCb::State aState = LHCb::State(stateVec,stateCov,z,LHCb::State::AtT); 

  // state all parameters but p
  aState.setX(aTrack->x(z,0.));
  aState.setY(aTrack->y(z,0.));
  aState.setTx(aTrack->xSlope(z,0.));
  aState.setTy(aTrack->sy());
 
  //p estimate can come from the curvature or the pt kick
  if (m_pFromCurvature == true) {
      aState.setQOverP(Tsa::estimateCurvature(aTrack->tx(), m_curvFactor));
      if (m_largeErrors == true) {
        aState.setErrQOverP2( m_EQdivP2*gsl_pow_2(stateVec(4)));
      }
      else {
        aState.setErrQOverP2(aTrack->xErr(5)*gsl_pow_2(m_curvFactor));
      }
    }
    else {
  m_ptKickTool->calculate(&aState);
  }

  // add to states
  lTrack->addToStates(aState);

}


void TsaConfirmTool::getDebugInfo( int& nHits, 
                                   int& stClusters, int& otClusters,
                                   int& collectTime, int& trackingTime, 
                                   int& nModules )
{
  
  nHits =  m_nTHits;
  stClusters = m_stClsuters;
  otClusters = m_otClusters;
  collectTime = m_collectTime;
  trackingTime = m_trackingTime;
  nModules = m_nModules;
  
  
  return;
}
