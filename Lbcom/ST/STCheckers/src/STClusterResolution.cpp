// $Id: STClusterResolution.cpp,v 1.19 2008-10-16 13:10:34 mneedham Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// BOOST
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
#include "Kernel/ISTClusterPosition.h"
#include "Kernel/LineTraj.h"
#include "Math/VectorUtil.h"

// from TrackInterfaces
#include "Kernel/ITrajPoca.h"
#include "gsl/gsl_math.h"

// local
#include "STClusterResolution.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STClusterResolution );

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
  for( ; iterClus != clusterCont->end(); ++iterClus){
    // get MC truth for this cluster
    Range range = aTable->relations(*iterClus);
    if ( !range.empty() ){
      this->fillHistograms(*iterClus,(*range.begin()).to());
    }
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
    const int id = this->histoId((int)aCluster->size());

    // fill double error = measVal.second;
    const double error = measVal.fractionalError * aSensor->pitch();

    // Fill offline resolution and pull
    std::string histTitle = detType() + " " + 
      ST::toString(id) + "-strip clusters";
    plot( uRec-uTrue, 10+id,"Resolution "+histTitle, -0.5, 0.5, 100 );
    plot( (uRec-uTrue)/error, 20+id, "Pull "+histTitle, -10., 10., 100 );

    // now the online version
    double uOnline = aSensor->localU( aChan.strip(),
                                      aCluster->interStripFraction());
    plot( uOnline-uTrue, 30+id,"Online resolution "+histTitle, -0.5, 0.5, 100 );

    // Do the same with trajectories
    std::auto_ptr<Trajectory> tmpTraj = 
      aSensor->trajectory( measVal.strip.strip(), measVal.fractionalPosition );
    const Trajectory& clusTraj = *(tmpTraj.get());

    // Get the true trajectory
    const LineTraj mcTraj(aHit->entry(), aHit->exit());

    // poca !
    Gaudi::XYZVector distance;
    double s1 = 0.0;
    double s2 = clusTraj.arclength( mcTraj.position(s1) );
    m_poca -> minimize( mcTraj, s1, clusTraj, s2, distance,  0.005);
    double residual = distance.R() * GSL_SIGN( distance.x() );

    histTitle += " (using Trajectories)";
    plot( residual, 110+id,"Resolution "+histTitle, -0.5, 0.5, 100 );
    plot( residual/error, 120+id, "Pull "+histTitle, -10., 10., 100 );

    // Plot the residual versus the angle
    double angle = ROOT::Math::VectorUtil::Angle( distance, 
                                                  aSensor->plane().Normal() );
    angle -= 0.5*M_PI;
    plot2D(angle, residual, 100, "residual vs angle",-0.35,0.35,
           -0.5,0.5,50,50);
    plot2D(angle, residual, 100+id, "residual vs angle "+histTitle,-0.35,0.35,
           -0.5,0.5,50,50);

  } // aHit
  
  // end
  return;
}

int STClusterResolution::histoId( const int clusterSize ) const
{
  // determine which histogram to fill
  return (clusterSize <= 4 ?  clusterSize : 4);
}

double STClusterResolution::calculateUTrue( const MCHit* aHit,
                                            const DeSTSensor* aSensor) const
{
  Gaudi::XYZPoint localPoint = aSensor->toLocal(aHit->midPoint());
  return localPoint.x();
}
