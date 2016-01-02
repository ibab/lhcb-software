#include <cmath>
#include "Kernel/STLexicalCaster.h"

// Event
#include "Event/STCluster.h"
#include "Event/MCHit.h"

// xml geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeSTSensor.h"

// STTools interfaces from LHCbKernel and MCInterfaces
#include "MCInterfaces/IMCParticleSelector.h"
#include "TrackInterfaces/ISTClusterPosition.h"
#include "Kernel/LineTraj.h"
#include "Math/VectorUtil.h"

// from TrackInterfaces
#include "Kernel/ITrajPoca.h"

// local
#include "STClusterResolution.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STClusterResolution )

//--------------------------------------------------------------------
//
//  STClusterResolution : Resolution plots
//
//--------------------------------------------------------------------

STClusterResolution::STClusterResolution(const std::string& name,
                                         ISvcLocator* pSvcLocator) :
 ST::HistoAlgBase(name, pSvcLocator)
{
  // constructer
  this->declareProperty("PositionToolName", m_positionToolName =
                        "STOfflinePosition");
  this->declareProperty("SelectorName", m_selectorName = "MCParticleSelector" );
  this->declareProperty("MergeSplitClusters", m_mergeSplitClusters = false );
  this->declareProperty("SkipSplitClusters", m_skipSplitClusters = false);
  this->declareSTConfigProperty("InputData", m_clusterLocation , STClusterLocation::TTClusters);

}

STClusterResolution::~STClusterResolution()
{
  // destructer
}

StatusCode STClusterResolution::initialize()
{
  // Set the top directory to IT or TT.
  if ( "" == histoTopDir() ) setHistoTopDir(detType()+"/");

  // Initialize GaudiHistoAlg
  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);


  // selector
  m_selector = tool<IMCParticleSelector>(m_selectorName,m_selectorName, this);

  // position tool
  m_positionTool = tool<ISTClusterPosition>(m_positionToolName, "positionTool",
                                            this);

  m_poca = tool<ITrajPoca>( "TrajPoca" );

  // location of the cluster tool
  m_asctLocation = m_clusterLocation+"2MCHits";

  return sc;
}

StatusCode STClusterResolution::execute()
{
  // retrieve clusters
  const STClusters* clusterCont = get<STClusters>(m_clusterLocation);

  // linker
  AsctTool associator(evtSvc(), m_clusterLocation+"2MCHits");
  const Table* aTable = associator.direct();
  if (!aTable) return Error("Failed to find table", StatusCode::FAILURE);

  // histos per digit
  STClusters::const_iterator iterClus = clusterCont->begin();
  while( iterClus != clusterCont->end() ) {

    // Get the next cluster
    STClusters::const_iterator jterClus = iterClus;
    ++jterClus;

    // Find first cluster that is not a direct neighbour
    if( m_mergeSplitClusters || m_skipSplitClusters ) {
     int lastChannel = (*iterClus)->lastChannel();
      while( jterClus != clusterCont->end() &&
             (*jterClus)->firstChannel() - lastChannel == 1) {
        lastChannel = (*jterClus)->lastChannel();
        ++jterClus;
      }
    }
    // Skip all the merged clusters
    if( m_skipSplitClusters && jterClus - iterClus != 1 )
      iterClus = jterClus;

    // get MC truth for this cluster
    if( iterClus != clusterCont->end() ) {
      Range range = aTable->relations(*iterClus);
      if ( !range.empty() ){
        this->fillHistograms(*iterClus,(*range.begin()).to());
      }
    }

    // Move to next cluster
    iterClus = jterClus;

  } // loop iterClus

  return StatusCode::SUCCESS;
}


void STClusterResolution::fillHistograms( const STCluster* aCluster,
                                          const MCHit* aHit ) const
{
  // fill histograms
  if ( (0 != aHit) && (m_selector->accept(aHit->mcParticle())) ){

    // get true u - need stereoangle/z info from channel
    const STChannelID aChan = aCluster->channelID();
    const DeSTSector* aSector = findSector(aChan);

    const DeSTSensor* aSensor = aSector->findSensor(aHit->midPoint());
    if (aSensor == 0){
      Warning("Failed to find sensor", StatusCode::SUCCESS).ignore();
      return;
    }

    const double uTrue = this->calculateUTrue(aHit,aSensor);

    // rec u - offline
    ISTClusterPosition::Info measVal = m_positionTool->estimate(aCluster);
    const double uRec = aSensor->localU( measVal.strip.strip(),
                                         measVal.fractionalPosition);

    // determine which histos to fill based on cluster size
    const int id = this->histoId((int)measVal.clusterSize);

    // fill double error = measVal.second;
    const double error = measVal.fractionalError * aSensor->pitch();

    // Fill offline resolution and pull
    std::string histTitle = detType() + " " +
      ST::toString(id) + "-strip clusters";
    plot( uRec-uTrue, 10+id,"Resolution "+histTitle, -0.25, 0.25, 100 );
    plot( (uRec-uTrue)/error, 20+id, "Pull "+histTitle, -10., 10., 100 );

    // Fill true fraction position versus reconstructed frac position
    double trueFracPos = uTrue - aSensor->localU( measVal.strip.strip(), 0 );
    int sign = (aSensor->localU(2) - aSensor->localU(1) > 0 ) ? 1 : -1;
    trueFracPos = double(sign)*trueFracPos/aSensor->pitch();
    profile1D( measVal.fractionalPosition, trueFracPos, 50+id,
               "True frac position "+histTitle+" vs rec frac pos",
               -0.005, 1.005, 101);

    // Plot the cluster size versus angle in each station (Lorentz deflection)
    profile1D( aHit->dxdz(), aCluster->size(), 60+aChan.station(),
               "Cluster size vs dx/dz in station "
               +ST::toString(aChan.station()), -0.2, 0.2, 50);

    // Plot total number and number of symmetric two-strip clusters versus
    // angle in each station (the ratio shows the Lorentz deflection)
    if( id == 2 && fabs(measVal.fractionalPosition-0.5) < 0.2)
      plot( aHit->dxdz(),70+aChan.station(),
            "Num sym 2-strip clus vs dxdz in station "
            + ST::toString(aChan.station()), -0.2, 0.2, 50);
    plot( aHit->dxdz(),80+aChan.station(), "Num of clus vs dxdz in station "
          + ST::toString(aChan.station()), -0.2, 0.2, 50);

    // now the online version
    double uOnline = aSensor->localU( aChan.strip(),
                                      aCluster->interStripFraction());
    plot( uOnline-uTrue, 30+id,"Online resolution "+histTitle, -0.25, 0.25,100);

    // Do the same with trajectories
    auto clusTraj =
      aSensor->trajectory( measVal.strip.strip(), measVal.fractionalPosition );

    // Get the true trajectory
    const LineTraj mcTraj(aHit->entry(), aHit->exit());

    // poca !
    Gaudi::XYZVector distance;
    double s1 = 0.0;
    double s2 = clusTraj->arclength( mcTraj.position(s1) );
    m_poca -> minimize( mcTraj, s1, *clusTraj, s2, distance,  0.005);
    double residual = std::copysign( distance.R(), distance.x() );

    // Resolution and pull plots
    plot( residual, 110,"Resolution "+detType()+" (using Trajectories)",
          -0.25, 0.25, 100 );
    plot( residual/error, 120, "Pull "+detType()+" (using Trajectories)",
          -10., 10., 100 );
    histTitle += " (using Trajectories)";
    plot( residual, 110+id,"Resolution "+histTitle, -0.25, 0.25, 100 );
    plot( residual/error, 120+id, "Pull "+histTitle, -10., 10., 100 );

  } // aHit
  // end
}

int STClusterResolution::histoId( const int clusterSize ) const
{
  // determine which histogram to fill
  return (clusterSize <= 9 ?  clusterSize : 9);
}

double STClusterResolution::calculateUTrue( const MCHit* aHit,
                                            const DeSTSensor* aSensor) const
{
  Gaudi::XYZPoint localPoint = aSensor->toLocal(aHit->midPoint());
  return localPoint.x();
}
