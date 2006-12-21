// $Id: STEffMonitor.cpp,v 1.10 2006-12-21 17:54:48 jvantilb Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

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

// GaudiKernel
#include "Kernel/IMCParticleSelector.h"
#include "Kernel/STDetSwitch.h"

// BOOST
#include "boost/lexical_cast.hpp"

// local
#include "STEffMonitor.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STEffMonitor );

//--------------------------------------------------------------------
//
//  STEffMonitor: Plot the ST efficiency
//
//--------------------------------------------------------------------

STEffMonitor::STEffMonitor( const std::string& name, 
                            ISvcLocator* pSvcLocator ) :
  GaudiHistoAlg(name, pSvcLocator),
  m_tracker(0)
{ 
  this->declareProperty("SelectorName", m_selectorName = "MCParticleSelector" );
  this->declareProperty("DetType", m_detType = "TT");
  this->declareProperty("IncludeGuardRings", m_includeGuardRings = true);
  this->declareProperty("PrintEfficiency", m_pEff = true);
}

STEffMonitor::~STEffMonitor()
{
  // destructer
}

StatusCode STEffMonitor::initialize()
{
  if( "" == histoTopDir() ) setHistoTopDir(m_detType+"/");

  // Initialize GaudiHistoAlg 
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // detector element
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));

  // Make location of STClusters and relation table consistent with m_detType
  m_clusterLocation = STClusterLocation::TTClusters;
  m_hitTableLocation = LHCb::MCParticleLocation::Default + "2MCTTHits";
  STDetSwitch::flip( m_detType, m_clusterLocation );
  STDetSwitch::flip( m_detType, m_hitTableLocation );

  // MCParticle selector
  m_selector = tool<IMCParticleSelector>(m_selectorName,m_selectorName,this);

  // Location of STCluster to MCHit relation table
  m_asctLocation = m_clusterLocation + "2MCHits";

  // init histos
  initHistograms();

  return StatusCode::SUCCESS;
}

StatusCode STEffMonitor::initHistograms()
{
  // Intialize histograms

  std::string tDirPath = this->histoPath()+"/";
  
  int numInVector = 0;
  int histID;  

  // Intialize histograms 
  std::vector<DeSTLayer*>::const_iterator iterLayer=m_tracker->layers().begin();
  for ( ; iterLayer != m_tracker->layers().end(); ++iterLayer) {
   
    // uniquely id using station and layer
    // add to map
    STChannelID aChan = (*iterLayer)->elementID();
    unsigned int uniqueID = uniqueHistoID(aChan);
    m_mapping[uniqueID] = numInVector;
    
    // Create layer name
    std::string layerName = ", layer " + 
      boost::lexical_cast<std::string>((int)uniqueID);

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
  
  return StatusCode::SUCCESS;
}

StatusCode STEffMonitor::execute()
{
  // Get the MCParticles
  MCParticles* particles = get<MCParticles>(MCParticleLocation::Default);

  // Get the STCluster to MCHit associator
  AsctTool associator(evtSvc(), m_asctLocation);
  m_table = associator.inverse();
  if (!m_table) return Error("Failed to find table", StatusCode::FAILURE);

  // Get the MCParticle to MCHit associator
  HitTable hitAsct( evtSvc(), m_hitTableLocation );
  m_hitTable = hitAsct.inverse();  
  if (!m_hitTable) return Error("Failed to find hit table at " 
                                + m_hitTableLocation, StatusCode::FAILURE);

  MCParticles::const_iterator iterPart = particles->begin(); 
  for ( ; iterPart != particles->end(); ++iterPart){
    if ( m_selector->accept(*iterPart) ){
      this->layerEff(*iterPart);
    }
  } // loop iterPart

  return StatusCode::SUCCESS;
}

StatusCode STEffMonitor::finalize()
{
  // init the message service
  if (m_pEff == true){
    info() << " -------Efficiency %--------- " << endmsg;
  }
  else {
    info() << " -------InEfficiency %--------- " << endmsg;
  }

  // print out efficiencys
  std::vector<DeSTLayer*>::const_iterator iterLayer=m_tracker->layers().begin();
  for ( ; iterLayer != m_tracker->layers().end(); ++iterLayer){
    
    STChannelID aChan = (*iterLayer)->elementID();
    int iHistoId = findHistoId(uniqueHistoID(aChan));
     
    // eff for this layer
    int nAcc =  m_xLayerHistos[iHistoId]->allEntries();
    int nFound = m_effXLayerHistos[iHistoId]->allEntries();
    double eff = 9999.;
    double err = 9999.;
    if (nAcc > 0){
      eff = 100.0*(double)nFound/(double)nAcc;
      err = sqrt((eff*(100.0-eff))/(double)nAcc);
      if (m_pEff = false) eff = 1-eff;
    }

    if ( m_detType == "TT" ) {
      info() << "Station "  << aChan.station()
             << " Layer" <<  aChan.layer()
             << " eff " << eff <<" +/- " << err << endmsg;
    }
    else {
      info() << "Station "  << aChan.station()
             << " Box " << aChan.detRegion() 
             << " Layer" <<  aChan.layer()
             << " eff " << eff << " +/- " << err << endmsg;
    }
  } // iterLayer

  info() << " -----------------------" << endreq;

  return  GaudiHistoAlg::finalize();
}


StatusCode STEffMonitor::layerEff(const MCParticle* aParticle)
{
  // find all MC hits for this particle
  HitTable::InverseType::Range hits = m_hitTable->relations( aParticle ) ;
  if (hits.empty()) return StatusCode::FAILURE;
  
  std::vector<DeSTLayer*>::const_iterator iterLayer=m_tracker->layers().begin();
  for ( ; iterLayer != m_tracker->layers().end(); ++iterLayer){
    
    // look for MCHit in this layer.....
    HitTable::InverseType::Range::iterator iterHit = hits.begin();
    std::vector<MCHit*> layerHits;  
    while (iterHit != hits.end()){
      MCHit* aHit = const_cast<MCHit*>( iterHit->to() );
      STChannelID hitChan = aHit->sensDetID();
      if( uniqueHistoID( hitChan ) == uniqueHistoID((*iterLayer)->elementID()) )
        layerHits.push_back(aHit);
      ++iterHit;
    } // iterHit
    
    if (layerHits.size() != 0){
      bool found = false;
      std::vector<MCHit*>::iterator tIter = layerHits.begin();  
      while ((tIter != layerHits.end())&&(found == false)){
        Range range = m_table->relations(*tIter);
        if (range.empty() == true) {
          ++tIter;  
        }
        else {
          found = true; 
        }
      } // tIter
      
      int iHistoId = findHistoId(uniqueHistoID((*iterLayer)->elementID()));
      MCHit* aHit = layerHits.front(); 
      const Gaudi::XYZPoint midPoint = aHit->midPoint();
      
      // histo vs x
      m_xLayerHistos[iHistoId]->fill( midPoint.x() );
      
      // histo vs y
      m_yLayerHistos[iHistoId]->fill( midPoint.y() );

      //  xy 
      m_xyLayerHistos[iHistoId]->fill( midPoint.x(), midPoint.y());

      if (found == true){
        if (m_pEff == true){
          m_effXYLayerHistos[iHistoId]->fill( midPoint.x(), midPoint.y() );
          m_effXLayerHistos[iHistoId]->fill( midPoint.x() );  
          m_effYLayerHistos[iHistoId]->fill( midPoint.y() );  
        }
      } 
      else {
        if (m_pEff == false){
          m_effXYLayerHistos[iHistoId]->fill( midPoint.x(), midPoint.y() );
          m_effXLayerHistos[iHistoId]->fill( midPoint.x() );
          m_effYLayerHistos[iHistoId]->fill( midPoint.y() );
        }
      }       
    } //if
  } // iterLayer
  
  return StatusCode::SUCCESS;
}


int STEffMonitor::findHistoId(unsigned int aLayerId)
{
  return m_mapping[aLayerId];
} 

int STEffMonitor::uniqueHistoID(const STChannelID aChan) const
{
  return m_detType == "TT" ? aChan.station()*100 + aChan.layer()  :
    aChan.station()*100 + aChan.detRegion()*10 + aChan.layer();
} 

bool STEffMonitor::isInside(const DeSTLayer* aLayer, const MCHit* aHit) const
{
  // check if expect hit to make cluster
  bool isFound = false;
  if (aLayer->isInside(aHit->midPoint()) == true){
    if (m_includeGuardRings == false){
      DeSTSector* aSector = m_tracker->findSector(aHit->midPoint());
      isFound = aSector->globalInActive(aHit->midPoint());
    }
    else {
      isFound = true;
    }
  }
  return isFound;
}
