// include files
#include <boost/foreach.hpp>

#include "GaudiKernel/AlgFactory.h"

#include "Event/RecSummary.h"
#include "Event/VeloLiteCluster.h"

// local
#include "VeloEventShapeCutsS20p3.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloEventShapeCuts, Stripping20p3 version
//
// 2013-12-29 : Maurizio Martinelli, Wouter Hulsbergen, Pieter David
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloEventShapeCutsS20p3 )

//==============================================================================
// Constructor
//==============================================================================
VeloEventShapeCutsS20p3::VeloEventShapeCutsS20p3(const std::string& name, ISvcLocator* pSvcLocator)
 : GaudiHistoAlg(name, pSvcLocator)
 , m_velo(0)
 , m_hVeloRatio(0)
 , m_hPhiVectorSize(0)
{
  declareProperty("MaxVeloRatio", m_maxVeloRatio = -1.
                 ,"Maximum allowed value for the ratio tracks/clusters");
  declareProperty("MaxPhiVectorSize", m_maxPhiVectorSize = -1.
                 ,"Maximum length of the vector sum of all hits' phi direction");
}
//==============================================================================
// Destructor
//==============================================================================
VeloEventShapeCutsS20p3::~VeloEventShapeCutsS20p3()
{
}

//==============================================================================
// Initialize
//==============================================================================
StatusCode VeloEventShapeCutsS20p3::initialize()
{
  if (msgLevel(MSG::DEBUG)) { debug() << "==> Initialize" << endmsg; }

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) { return sc; }

  m_velo = getDet<DeVelo>(DeVeloLocation::Default);
  if (m_velo == 0) { return Error("Could not find Velo detector", StatusCode::FAILURE); }

  if (produceHistos()) {
    m_hVeloRatio = book1D("VeloRatio", 0., 1., 100);
    m_hPhiVectorSize = book1D("Velo Phi vector size", 0., 1000., 100);
  }

  return StatusCode::SUCCESS;
}

//==============================================================================
// Execute
//==============================================================================
StatusCode VeloEventShapeCutsS20p3::execute()
{
  if (msgLevel(MSG::DEBUG)) { debug() << "==> Execute" << endmsg; }

  setFilterPassed(false);
  if ( ( ( m_maxVeloRatio     < 0. ) || ( calculateVeloRatio()     < m_maxVeloRatio     ) )
    && ( ( m_maxPhiVectorSize < 0. ) || ( calculatePhiVectorSize() < m_maxPhiVectorSize ) ) )
  {
    setFilterPassed(true);
  }
  if (produceHistos()) {
    fill( m_hVeloRatio, calculateVeloRatio(), 1.0 );
    fill( m_hPhiVectorSize, calculatePhiVectorSize(), 1.0 );
  }
  counter("#passed") += ( filterPassed() ? 1 : 0 );

  return StatusCode::SUCCESS;
}

//==============================================================================
// Velo ratio
//==============================================================================
double VeloEventShapeCutsS20p3::calculateVeloRatio() const
{
  const LHCb::RecSummary* recSummary = get<LHCb::RecSummary>(LHCb::RecSummaryLocation::Default);
  if (recSummary == 0) {
    Error("Could not retrieve LHCb::RecSummary from "+LHCb::RecSummaryLocation::Default);
    return 0.;
  }
  return float(recSummary->info(LHCb::RecSummary::nVeloTracks, 0))/recSummary->info(LHCb::RecSummary::nVeloClusters, 1);
}

namespace {
  double phiVectorSize( const LHCb::VeloLiteCluster::VeloLiteClusters* clusters, const DeVelo* velo )
  {
    double sumx(0.), sumy(0.);
    LHCb::VeloChannelID chan; double phi;
    BOOST_FOREACH( const LHCb::VeloLiteCluster& clus, *clusters ) {
      if (clus.isPhiType()) {
        chan = clus.channelID();
        phi = velo->phiSensor(chan.sensor())->globalPhiOfStrip(chan.strip());
        sumx += std::sin(phi);
        sumy += std::cos(phi);
      }
    }
    return std::sqrt(sumx*sumx + sumy*sumy);
  }
}

//==============================================================================
// Phi vector size
//==============================================================================
double VeloEventShapeCutsS20p3::calculatePhiVectorSize() const
{
  const LHCb::VeloLiteCluster::VeloLiteClusters* clusters = get<LHCb::VeloLiteCluster::VeloLiteClusters>(LHCb::VeloLiteClusterLocation::Default);
  if (clusters == 0) {
    Error("Could not find velo clusters at "+LHCb::VeloLiteClusterLocation::Default);
    return 0.;
  }
  return phiVectorSize(clusters, m_velo);
}

//==============================================================================
// Finalize
//==============================================================================
StatusCode VeloEventShapeCutsS20p3::finalize()
{
  if (msgLevel(MSG::DEBUG)) { debug() << "==> Finalize" << endmsg; }

  return GaudiAlgorithm::finalize();
}

// ExtraInfo tool

#include "Kernel/RelatedInfoNamed.h"

DECLARE_TOOL_FACTORY( AddVeloEventShapeS21 )

AddVeloEventShapeS21::AddVeloEventShapeS21( const std::string& type, const std::string& name, const IInterface* parent )
  : GaudiTool( type, name, parent )
{
  declareInterface<IRelatedInfoTool>(this);
}

AddVeloEventShapeS21::~AddVeloEventShapeS21()
{}

StatusCode AddVeloEventShapeS21::initialize()
{
  if (msgLevel(MSG::DEBUG)) { debug() << "==> Initialize" << endmsg; }

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) { return sc; }

  m_velo = getDet<DeVelo>(DeVeloLocation::Default);
  if (m_velo == 0) { return Error("Could not find Velo detector", StatusCode::FAILURE); }

  return StatusCode::SUCCESS;
}

StatusCode AddVeloEventShapeS21::calculateRelatedInfo( const LHCb::Particle* /* top */, const LHCb::Particle* /* part */ )
{
  m_map.clear();

  const LHCb::VeloLiteCluster::VeloLiteClusters* clusters = get<LHCb::VeloLiteCluster::VeloLiteClusters>(LHCb::VeloLiteClusterLocation::Default);
  if (clusters == 0) {
    Error("Could not find velo clusters at "+LHCb::VeloLiteClusterLocation::Default);
    return StatusCode::FAILURE;
  }

  m_map.insert(std::make_pair(RelatedInfoNamed::VELOPHIHITSVECTORSIZE , phiVectorSize(clusters, m_velo)));

  return StatusCode::SUCCESS;
}

LHCb::RelatedInfoMap* AddVeloEventShapeS21::getInfo(void)
{
    return &m_map;
}
