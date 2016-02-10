// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// xml geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTLayer.h"

// Event
#include "Event/STCluster.h"
#include "Event/MCHit.h"
#include "Event/MCParticle.h"

// Kernel
#include "MCInterfaces/IMCParticleSelector.h"
#include "Kernel/STDetSwitch.h"

#include "Kernel/STLexicalCaster.h"

// local
#include "HistFun.h"
#include "STEffChecker.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STEffChecker )

//--------------------------------------------------------------------
//
//  STEffChecker: Plot the ST efficiency
//
//--------------------------------------------------------------------

STEffChecker::STEffChecker( const std::string& name,
                            ISvcLocator* pSvcLocator ) :
  ST::HistoAlgBase(name, pSvcLocator)
{
  declareSTConfigProperty("InputData" , m_clusterLocation  , STClusterLocation::TTClusters);
  declareProperty("SelectorName", m_selectorName = "MCParticleSelector" );
  declareProperty("IncludeGuardRings", m_includeGuardRings = false);
  declareProperty("PrintEfficiency", m_pEff = true);
  setProperty( "HistoPrint", false );

  setForcedInit();
  m_hitTableLocation = LHCb::MCParticleLocation::Default + "2MCTTHits";
  m_asctLocation = m_clusterLocation + "2MCHits";
  addToFlipList(&m_hitTableLocation);
  addToFlipList(&m_asctLocation);
}

StatusCode STEffChecker::initialize()
{
  if( histoTopDir().empty() ) setHistoTopDir(detType()+"/");

  // Initialize GaudiHistoAlg
  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);


  // MCParticle selector
  m_selector = tool<IMCParticleSelector>(m_selectorName,m_selectorName,this);

  // init histos
  initHistograms();

  return sc;
}

void STEffChecker::initHistograms()
{
  std::string tDirPath = histoPath()+"/";
  int numInVector = 0;
  int histID;

  // Intialize histograms
  for (const auto& layer : tracker()->layers()) {

    // uniquely id using station and layer
    // add to map
    STChannelID aChan = layer->elementID();
    unsigned int uniqueID = uniqueHistoID(aChan);
    m_mapping[uniqueID] = numInVector;

    // Create layer name
    std::string layerName = ", layer " + ST::toString((int)uniqueID);

    // Book x distribution histogram MCHits
    histID = (int)uniqueID+5000;
    IHistogram1D* xLayerHisto =
      book1D( histID, "x distribution hits"+layerName,-1000.,1000.,400);
    m_xLayerHistos.push_back(xLayerHisto);

    // Book y distribution histogram MCHits
    histID = (int)uniqueID+6000;
    IHistogram1D* yLayerHisto =
      book1D( histID, "y distribution hits"+layerName,-1000.,1000.,400);
    m_yLayerHistos.push_back(yLayerHisto);

    // Book x distribution histogram STClusters
    histID = (int)uniqueID+7000;
    IHistogram1D* effXLayerHisto =
      book1D( histID, "x distribution ass. clusters"+layerName,
              -1000.,1000.,400);
    m_effXLayerHistos.push_back(effXLayerHisto);

    // Book y distribution histogram STClusters
    histID = (int)uniqueID+8000;
    IHistogram1D* effYLayerHisto =
      book1D( histID, "y distribution ass. clusters"+layerName,
              -1000.,1000.,400);
    m_effYLayerHistos.push_back(effYLayerHisto);

    // Book x vs y distribution histogram MCHits
    histID = (int)uniqueID+9000;
    IHistogram2D* xyLayerHisto =
      book2D( histID, "x vs y distribution hits"+layerName,
              -1000.,1000., 200, -1000., 1000.,200);
    m_xyLayerHistos.push_back(xyLayerHisto);

    // Book x vs y distribution histogram STClusters
    histID = (int)uniqueID+10000;
    IHistogram2D* effXYLayerHisto =
      book2D( histID, "x vs y distribution ass. clusters"+layerName,
              -1000., 1000., 200, -1000., 1000., 200);
    m_effXYLayerHistos.push_back(effXYLayerHisto);

    ++numInVector;
  } // iLayer
}

StatusCode STEffChecker::execute()
{
  // Get the MCParticles
  const MCParticles* particles = get<MCParticles>(MCParticleLocation::Default);

  // Get the STCluster to MCHit associator
  AsctTool associator(evtSvc(), m_asctLocation);
  m_table = associator.inverse();
  if (!m_table) return Error("Failed to find table", StatusCode::FAILURE);

  // Get the MCParticle to MCHit associator
  HitTable hitAsct( evtSvc(), m_hitTableLocation );
  m_hitTable = hitAsct.inverse();
  if (!m_hitTable) return Error("Failed to find hit table at "
                                + m_hitTableLocation, StatusCode::FAILURE);

  for (const auto& particle : *particles) {
    if ( m_selector->accept(particle) ) layerEff(particle);
  }

  return StatusCode::SUCCESS;
}

StatusCode STEffChecker::finalize()
{
  // init the message service
  if (m_pEff == true){
    info() << " -------Efficiency %--------- " << endmsg;
  }
  else {
    info() << " -------InEfficiency %--------- " << endmsg;
  }

  // print out efficiencys
  for (const auto& layer : tracker()->layers()) {

    STChannelID aChan = layer->elementID();
    int iHistoId = findHistoId(uniqueHistoID(aChan));

    // eff for this layer
    int nAcc =  m_xLayerHistos[iHistoId]->allEntries();
    int nFound = m_effXLayerHistos[iHistoId]->allEntries();
    double eff = 9999.;
    double err = 9999.;
    if (nAcc > 0){
      eff = 100.0*(double)nFound/(double)nAcc;
      err = sqrt((eff*(100.0-eff))/(double)nAcc);
      if (m_pEff == false) eff = 1-eff;
    }

    info() << uniqueLayer(aChan)
             << " eff " << eff <<" +/- " << err << endmsg;

  } // layer

  info() << " -----------------------" << endmsg;


  // hack to prevent crash
  StatusCode sc = ST::HistoAlgBase::finalize();
  if (sc.isFailure()){
    return Warning("Failed to finalize base class", sc);
  }

  // unbook if not full detail mode: histograms are not saved
  if (!fullDetail()){
    unBookHistos();
    eraseHistos();
  }

  return StatusCode::SUCCESS;
}


void STEffChecker::layerEff(const MCParticle* aParticle)
{
  // find all MC hits for this particle
  auto hits = m_hitTable->relations(aParticle ) ;
  if (hits.empty()) return;

  for (const auto& layer : tracker()->layers()) {

    // look for MCHit in this layer.....

    std::vector<const MCHit*> layerHits;
    for ( const auto& link : hits ) {
      auto aHit = link.to();
      STChannelID hitChan = STChannelID(aHit->sensDetID());
      if ( uniqueHistoID( hitChan ) == uniqueHistoID(layer->elementID()) && isInside(layer,aHit)) {
          layerHits.push_back(aHit);
      }
    };

    if (!layerHits.empty()){
      bool found = std::any_of( layerHits.begin(), layerHits.end(),
                                [&](const MCHit* h) {
                return !m_table->relations(h).empty();
      });

      int iHistoId = findHistoId(uniqueHistoID(layer->elementID()));
      const MCHit* aHit = layerHits.front();
      const Gaudi::XYZPoint midPoint = aHit->midPoint();

      // histo vs x
      m_xLayerHistos[iHistoId]->fill( midPoint.x() );
      // histo vs y
      m_yLayerHistos[iHistoId]->fill( midPoint.y() );
      //  xy
      m_xyLayerHistos[iHistoId]->fill( midPoint.x(), midPoint.y());

      if (found){
        if (m_pEff){
          m_effXYLayerHistos[iHistoId]->fill( midPoint.x(), midPoint.y() );
          m_effXLayerHistos[iHistoId]->fill( midPoint.x() );
          m_effYLayerHistos[iHistoId]->fill( midPoint.y() );
        }
      }
      else {
        if (!m_pEff){
          m_effXYLayerHistos[iHistoId]->fill( midPoint.x(), midPoint.y() );
          m_effXLayerHistos[iHistoId]->fill( midPoint.x() );
          m_effYLayerHistos[iHistoId]->fill( midPoint.y() );
        }
      }
    } //if
  } // layer
}


int STEffChecker::findHistoId(unsigned int aLayerId)
{
  return m_mapping[aLayerId];
}

int STEffChecker::uniqueHistoID(const STChannelID aChan) const
{
  return ( detType() == "TT" || detType() == "UT") ?
    aChan.station()*100 + aChan.layer()  :
    aChan.station()*100 + aChan.detRegion()*10 + aChan.layer();
}

bool STEffChecker::isInside(const DeSTLayer* aLayer, const MCHit* aHit) const
{
  // check if expect hit to make cluster
  if (!aLayer->isInside(aHit->midPoint())) return false;
  if (m_includeGuardRings) return true;
  const DeSTSector* aSector = tracker()->findSector(aHit->midPoint());
  return aSector && aSector->globalInActive(aHit->midPoint());
}

void STEffChecker::unBookHistos(){

  // give ownership back to vector - histos no longer in store
  HistFun::unBookVector(m_xLayerHistos,     histoSvc());
  HistFun::unBookVector(m_yLayerHistos,     histoSvc());
  HistFun::unBookVector(m_xyLayerHistos,    histoSvc());
  HistFun::unBookVector(m_effXLayerHistos,  histoSvc());
  HistFun::unBookVector(m_effYLayerHistos,  histoSvc());
  HistFun::unBookVector(m_effXYLayerHistos, histoSvc());
}

void STEffChecker::eraseHistos(){

  // clear everything
  HistFun::eraseVector(m_xLayerHistos);
  HistFun::eraseVector(m_yLayerHistos);
  HistFun::eraseVector(m_xyLayerHistos);
  HistFun::eraseVector(m_effXLayerHistos);
  HistFun::eraseVector(m_effYLayerHistos);
  HistFun::eraseVector(m_effXYLayerHistos);
}

