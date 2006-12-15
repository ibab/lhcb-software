// $:  $
     
// Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "TsaKernel/TimeSummary.h"

#include "TsaKernel/SeedHit.h"
#include "TsaKernel/SeedTrack.h"
#include "TsaKernel/SeedStub.h"

#include "Event/STLiteCluster.h"
#include "Event/OTTime.h"


#include "TsaKernel/ITsaSeedStep.h"
#include "TsaKernel/ITsaStubFind.h"
#include "TsaKernel/ITsaStubLinker.h"
#include "TsaKernel/ITsaStubExtender.h"
#include "ITsaSeedAddHits.h"
#include "TsaSeed.h"

// boost
#include "boost/lexical_cast.hpp"

// factory defs
DECLARE_ALGORITHM_FACTORY( TsaSeed );


TsaSeed::TsaSeed(const std::string& name,
                                     ISvcLocator* pSvcLocator):
  TsaBaseAlg(name, pSvcLocator)
{
  
  declareProperty("maxNumHits", m_maxNumHits = 14000);
  declareProperty("addHits", m_addHitsInITOverlap = true);
  declareProperty("calcLikelihood", m_calcLikelihood = true);

  declareProperty("selectorType", m_selectorType = "TsaSeedSelector");
  declareProperty("seedTracksLocation", m_seedTrackLocation = SeedTrackLocation::Default);
  declareProperty("seedHitLocation",  m_seedHitLocation = SeedHitLocation::Default); 
  declareProperty("seedStubLocation", m_seedStubLocation = SeedStubLocation::Default);
 
}

TsaSeed::~TsaSeed(){}  

StatusCode TsaSeed::initialize()
{
  // Initialization
  
  StatusCode sc = TsaBaseAlg::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize");
  }

  // init the tools 
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
  m_addHits = tool<ITsaSeedAddHits>("TsaSeedAddHits","addHits",this);
  m_stubFind = tool<ITsaStubFind>("TsaStubFind","stubFinder" ,this);
  m_stubLinker = tool<ITsaStubLinker>("TsaStubLinker","stubLinker",this);
  m_extendStubs =  tool<ITsaStubExtender>("TsaStubExtender","stubExtender",this);

  return StatusCode::SUCCESS;
}

StatusCode TsaSeed::execute(){
//-------------------------------------------------------------------------
//  Steering routine for track seeding
//-------------------------------------------------------------------------

  //  startTimer();

  typedef LHCb::STLiteCluster::FastContainer FastContainer;
  LHCb::OTTimes* otCont = get<LHCb::OTTimes>(LHCb::OTTimeLocation::Default);
  FastContainer* liteCont = get<FastContainer>( LHCb::STLiteClusterLocation::ITClusters);
  double nHit = liteCont->size() + otCont->size();

  SeedTracks* seedSel = new SeedTracks();    //  Selected seed candidates
  seedSel->reserve(1000);

  std::vector<SeedStub*> stubs[3];            //  IT stubs per station
  for (unsigned iS = 0; iS < 3; ++iS) stubs[iS].reserve(100);
 
  SeedStubs* stubsCont = new SeedStubs();  
  stubsCont->reserve(200);

  SeedHits* hitsCont = new SeedHits();
  hitsCont->reserve(10000);
  
  // put output in the store
  put(seedSel,m_seedTrackLocation);
  put(hitsCont, m_seedHitLocation); 
  put(stubsCont, m_seedStubLocation);

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
      std::vector<SeedTrack*> extendedSeeds; extendedSeeds.reserve(50);
      m_extendStubs->execute( sector, stubs, hits, sHits, extendedSeeds );
      for ( std::vector<SeedTrack*>::iterator itEx = extendedSeeds.begin(); extendedSeeds.end() != itEx; ++itEx ) {
        seedSel->insert( *itEx);
      }
    }

    for ( int lay = 0; lay < 6; ++lay ) {
      for ( std::vector<SeedHit*>::iterator it = hits[lay].begin(); hits[lay].end() != it; ++it ) hitsCont->insert(*it);
      for ( std::vector<SeedHit*>::iterator it = sHits[lay].begin(); sHits[lay].end() != it; ++it ) hitsCont->insert(*it);
    }
  }
  
  // clean up the stubs...
  for ( int stn = 0; stn < 3; ++stn ) {
    for ( std::vector<SeedStub*>::iterator it = stubs[stn].begin(); stubs[stn].end() != it; ++it ) {
      stubsCont->insert(*it);
    }
  }

  // add hits in IT overlap region
  if (m_addHitsInITOverlap == true) m_addHits->execute(seedSel, hitsCont);

  //stopTimer();

  //plot(timer().lasttime()/1000 , "timer", 0., 10., 200  );
  //plot2D(nHit, timer().lasttime()/1000 , "timer", 0., 20000., 0., 10., 200, 400 );
  //plot(nHit,"hits", 0., 20000., 500);

  return StatusCode::SUCCESS;
}

StatusCode TsaSeed::finalize(){
    
  return TsaBaseAlg::finalize();
}
