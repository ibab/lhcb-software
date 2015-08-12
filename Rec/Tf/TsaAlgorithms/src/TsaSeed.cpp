#include "TsaSeed.h"

#include "Event/Track.h"

// boost
#include "boost/lexical_cast.hpp"
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>

// try
#include "TsaKernel/TsaTStationHitManager.h"
#include "TsaKernel/TsaSeedingHit.h"

#include "Event/STLiteCluster.h"
#include "OTDAQ/IOTRawBankDecoder.h"

#include "Event/ProcStatus.h"

using namespace Tf::Tsa;
using namespace boost::lambda;

typedef Tf::Tsa::TStationHitManager ITHitMan;
typedef Tf::Tsa::TStationHitManager::HitRange Hits;



// factory defs
DECLARE_ALGORITHM_FACTORY( Seed )

Seed::Seed(const std::string& name,
           ISvcLocator* pSvcLocator):
  BaseAlg(name, pSvcLocator),
  m_xSelection(NULL),
  m_finalSelection(NULL),
  m_likelihood(NULL),
  m_addHits(NULL),
  m_stubFind(NULL),
  m_stubLinker(NULL),
  m_extendStubs(NULL),
  m_rawBankDecoder(NULL),
  m_timerTool(NULL)
{

  declareProperty("addHits", m_addHitsInITOverlap = true);
  declareProperty("calcLikelihood", m_calcLikelihood = true);
  declareProperty("selectorType", m_selectorType = "Tf::Tsa::SeedSelector");
  declareProperty("seedTracksLocation", m_seedTrackLocation = SeedTrackLocation::Default);
  declareProperty("seedHitLocation",  m_seedHitLocation = SeedHitLocation::Default);
  declareProperty("seedStubLocation", m_seedStubLocation = SeedStubLocation::Default);
  declareProperty("OnlyGood", m_onlyGood = false);
  declareProperty("DiscardChi2", m_discardChi2 = 1.5);
  declareProperty( "maxITHits" ,  m_maxNumberITHits = 3000);
  declareProperty( "maxOTHits" , m_maxNumberOTHits = 10000 );
  declareProperty("ITLiteClusters", m_clusterLocation = LHCb::STLiteClusterLocation::ITClusters);
  declareProperty( "TimingMeasurement", m_doTiming = false);

}

Seed::~Seed(){}

StatusCode Seed::initialize()
{
  // Initialization

  const StatusCode sc = BaseAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize");
  }

  // tool handle to the ot raw bank decoder
  m_rawBankDecoder = tool<IOTRawBankDecoder>("OTRawBankDecoder");

  // init the tools
  for (unsigned int i = 0; i < 3; ++i){
    ITsaSeedStep* xStepIT = tool<ITsaSeedStep>("Tf::Tsa::ITXSearch", "xSearchS" + boost::lexical_cast<std::string>(i) , this);
    ITsaSeedStep* stereoStepIT = tool<ITsaSeedStep>("Tf::Tsa::ITStereoSearch", "stereoS"
                                                    + boost::lexical_cast<std::string>(i) , this);
    m_xSearchStep.push_back(xStepIT);
    m_stereoStep.push_back(stereoStepIT);
  }
  for (unsigned int o = 3; o < 5; ++o){
    ITsaSeedStep* xStepOT = tool<ITsaSeedStep>("Tf::Tsa::OTXSearch", "xSearchS" + boost::lexical_cast<std::string>(o) , this);
    ITsaSeedStep* stereoStepOT = tool<ITsaSeedStep>("Tf::Tsa::OTStereoSearch", "stereoS"
                                                    + boost::lexical_cast<std::string>(o) , this);
    m_xSearchStep.push_back(xStepOT);
    m_stereoStep.push_back(stereoStepOT);
  }

  m_xSelection = tool<ITsaSeedStep>("Tf::Tsa::XProjSelector", "xSelection" , this);
  m_finalSelection = tool<ITsaSeedStep>(m_selectorType, "finalSelection" , this);
  m_likelihood = tool<ITsaSeedStep>("Tf::Tsa::Likelihood", "likelihood" , this);
  m_addHits = tool<ITsaSeedAddHits>("Tf::Tsa::SeedAddHits","SeedAddHits",this);
  m_stubFind = tool<ITsaStubFind>("Tf::Tsa::StubFind","stubFinder" ,this);
  m_stubLinker = tool<ITsaStubLinker>("Tf::Tsa::StubLinker","stubLinker",this);
  m_extendStubs =  tool<ITsaStubExtender>("Tf::Tsa::StubExtender","stubExtender",this);


  if ( m_doTiming) {
    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" );
    m_timerTool->increaseIndent();
    m_seedTime = m_timerTool->addTimer( "Internal TsaSeeding" );
    m_timerTool->decreaseIndent();
  }

  return sc;
}

StatusCode Seed::execute(){
  //-------------------------------------------------------------------------
  //  Steering routine for track seeding
  //-------------------------------------------------------------------------

  if ( m_doTiming ) m_timerTool->start( m_seedTime );

  //  startTimer();
  SeedTracks* seedSel = new SeedTracks();    //  Selected seed candidates
  seedSel->reserve(1000);
  put(seedSel,m_seedTrackLocation);
  std::vector<SeedTrack*> tempSel; tempSel.reserve(1000);

  // reject hot events
  const LHCb::STLiteCluster::STLiteClusters* clusterCont = get<LHCb::STLiteCluster::STLiteClusters>(m_clusterLocation);
  if (clusterCont->size() > m_maxNumberITHits ){
    LHCb::ProcStatus* procStat =
      getOrCreate<LHCb::ProcStatus,LHCb::ProcStatus>(
                                                     LHCb::ProcStatusLocation::Default);
    // give some indication that we had to skip this event
    // (ProcStatus returns zero status for us in cases where we don't
    // explicitly add a status code)
    procStat->addAlgorithmStatus( name(), "Tracking", "LimitOfITHitsExceeded", -3 , true );
    return Warning("To many IT hits event rejected", StatusCode::SUCCESS, 1);
  }



  const unsigned int nHitsInOT = m_rawBankDecoder->totalNumberOfHits();
  if (nHitsInOT > m_maxNumberOTHits){
    LHCb::ProcStatus* procStat =
      getOrCreate<LHCb::ProcStatus,LHCb::ProcStatus>(
                                                     LHCb::ProcStatusLocation::Default);
    // give some indication that we had to skip this event
    // (ProcStatus returns zero status for us in cases where we don't
    // explicitly add a status code)
    procStat->addAlgorithmStatus( name(), "Tracking", "LimitOfOTHitsExceeded", -3 , true );
    return Warning("To Many OT hits event rejected", StatusCode::SUCCESS,1);
  }



  if (m_onlyGood) {
    // retrieve all TsaSeedingHits
    ITHitMan* m_hitMan = tool<ITHitMan>("Tf::Tsa::TStationHitManager","TsaDataManager");
    Hits allhits = m_hitMan->hits();


    LHCb::Tracks* fwdTracks = get<LHCb::Tracks>( LHCb::TrackLocation::Forward );
    for ( LHCb::Tracks::const_iterator itT = fwdTracks->begin(); fwdTracks->end() != itT; ++itT ) {
      LHCb::Track* tr = *itT;
      if (tr->fitStatus() == LHCb::Track::Fitted) {
        // if fitted - used chi2pdf from fit
        if (tr->chi2PerDoF() < m_discardChi2) continue;
      }
      //else {
      // otherwise use PatQuality from patreco
      // maybe we will have good discriminating property from pat reco only
      //  if (tr->info(LHCb::Track::PatQuality, -1.) < 4.0) continue;
      //}

      int nHits = 0;
      for ( std::vector<LHCb::LHCbID>::const_iterator itId = tr->lhcbIDs().begin();
            tr->lhcbIDs().end() != itId; ++itId ) {
        for ( Hits::const_iterator itIter = allhits.begin(); itIter != allhits.end(); ++itIter) {
          if ( (*itIter)->hit()->lhcbID()==(*itId) ){
            nHits++;
            (*itIter)->hit()->setStatus(Tf::HitBase::UsedByPatForward, false);
            break;
          }
        }
      }

    }
  }



  std::vector<SeedStub*> stubs[3];            //  IT stubs per station
  for (unsigned iS = 0; iS < 3; ++iS) stubs[iS].reserve(100);

  SeedStubs* stubsCont = new SeedStubs();
  stubsCont->reserve(200);

  SeedHits* hitsCont = new SeedHits();
  hitsCont->reserve(10000);

  // put output in the store
  put(hitsCont, m_seedHitLocation);
  put(stubsCont, m_seedStubLocation);

  StatusCode sc(StatusCode::SUCCESS,true);

  // Loop over sectors of tracker (0-2 are IT, 3-4 are OT)
  for ( int sector = 0; sector < 5; ++sector ) {


    std::vector<SeedHit*> hits[6], sHits[6];  //  Hits per layer in X and stereo

    std::vector<SeedTrack*> seeds;            //  Seed candidates within the sector
    seeds.reserve(1000);


    sc = m_xSearchStep[sector]->execute(seeds,hits);  // x search
    if (sc.isFailure()) {
      return Error("x search failed", StatusCode::FAILURE,1);
    }

    if (sector >2 ) {
      sc = m_xSelection->execute(seeds); // x selection
      if (sc.isFailure()) {
        return Error("x selection failed", StatusCode::FAILURE,1);
      }

    }


    sc = m_stereoStep[sector]->execute(seeds,sHits); // add stereo
    if (sc.isFailure()) {
      return Error("stereo search failed", StatusCode::FAILURE,1);
    }



    if (m_calcLikelihood) {
      sc = m_likelihood->execute(seeds); // likelihood
      if (sc.isFailure()) {
        return Error("likelihood failed", StatusCode::FAILURE,1);
      }
    }


    sc = m_finalSelection->execute(seeds); // final selection
    if (sc.isFailure()) {
      return Error("selection failed", StatusCode::FAILURE,1);
    }


    //Delete the temporary objects that have been created
    for ( std::vector<SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {
      seedSel->insert(*it);

    }

    if ( sector <= 2 ) {
      sc = m_stubFind->execute( hits, sHits, stubs );
      if (sc.isFailure()) {
        return Error("stub finding failed", StatusCode::FAILURE,1);
      }
    }

    //  After the IT stub finding is finished, try to link the stubs to make seed candidates
    if ( sector == 2 ) {

      std::vector<SeedTrack*> linkedSeeds; linkedSeeds.reserve(50);
      sc = m_stubLinker->execute( stubs, linkedSeeds );
      if (sc.isFailure()) {
        return Error("stub linking failed", StatusCode::FAILURE,1);
      }
      if (m_calcLikelihood == true) {
        sc = m_likelihood->execute(linkedSeeds);
        if (sc.isFailure()) {
          return Error("stub likelihood failed", StatusCode::FAILURE,1);
        }
      }
      sc = m_finalSelection->execute(linkedSeeds);
      if (sc.isFailure()) {
        return Error("stub selection failed", StatusCode::FAILURE,1);
      }
      for ( std::vector<SeedTrack*>::iterator itLinked = linkedSeeds.begin(); linkedSeeds.end() != itLinked; ++itLinked ) {
        seedSel->insert( *itLinked);

      }  // it
    }

    //  For those IT stubs that remain, try to extend them into the OT
    if ( sector > 2 ) {
      std::vector<SeedTrack*> extendedSeeds; extendedSeeds.reserve(50);
      sc = m_extendStubs->execute( sector, stubs, hits, sHits, extendedSeeds );
      if (sc.isFailure()) {
        return Error("stub extension", StatusCode::FAILURE,1);
      }
      if (m_calcLikelihood == true) {
        sc = m_likelihood->execute(extendedSeeds);
        if (sc.isFailure()) {
          return Error("extended stub likelihood failed", StatusCode::FAILURE,1);
        }
      }
      sc = m_finalSelection->execute(extendedSeeds);
      if (sc.isFailure()) {
        return Error("extended stub selection failed", StatusCode::FAILURE,1);
      }
      for ( std::vector<SeedTrack*>::iterator itEx = extendedSeeds.begin(); extendedSeeds.end() != itEx; ++itEx ) {
        seedSel->insert( *itEx);

      }
    }

    for ( int lay = 0; lay < 6; ++lay ) {
      for ( std::vector<SeedHit*>::iterator it = hits[lay].begin(); hits[lay].end() != it; ++it )
      {
        hitsCont->insert(*it);

      }
      for ( std::vector<SeedHit*>::iterator it = sHits[lay].begin(); sHits[lay].end() != it; ++it )
      {
        hitsCont->insert(*it);

      }
    }
  }

  // clean up the stubs...
  for ( int stn = 0; stn < 3; ++stn ) {
    for ( std::vector<SeedStub*>::iterator it = stubs[stn].begin(); stubs[stn].end() != it; ++it ) {
      stubsCont->insert(*it);

    }
  }

  // add hits in IT overlap region
  if (m_addHitsInITOverlap == true) {
    sc = m_addHits->execute(seedSel, hitsCont);
    if (sc.isFailure()) {
      return Error("failed to add hits", StatusCode::FAILURE,1);
    }

    SeedTracks::iterator iterT = seedSel->begin();
    for (; iterT != seedSel->end(); ++iterT){
      std::vector<SeedPnt> clusVector = (*iterT)->usedPnts();
      std::for_each(clusVector.begin(),clusVector.end(),
                    bind(&SeedPnt::setOnTrack,_1, false));
      if ((*iterT)->select() == false) (*iterT)->setLive(false) ;
      (*iterT)->setSelect(false);
      tempSel.push_back(*iterT);
    } // iterT

    sc = m_finalSelection->execute(tempSel);
    if (sc.isFailure()) {
      return Error("failed to make final selection", StatusCode::FAILURE,1);
    }

  }

  debug() << "Created " << seedSel->size() << " SeedTracks at " << m_seedTrackLocation << endreq;

  if ( m_doTiming ) m_timerTool->stop( m_seedTime );

  return StatusCode::SUCCESS;
}
