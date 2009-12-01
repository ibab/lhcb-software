// $Id: HltRadCorTool.cpp,v 1.5 2009-12-01 21:33:10 witekma Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "Event/L0CaloCandidate.h"
#include "Event/Track.h"
#include "Event/CaloCluster.h"
// local
#include "HltRadCorTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltRadCorTool
//
// 2009-05-25 : Mariusz Witek
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltRadCorTool );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltRadCorTool::HltRadCorTool( const std::string& type,
                const std::string& name,
                const IInterface* parent )
  : GaudiTool ( type, name , parent )
    , m_detector(0)
    , m_tool(0)
    , m_tool1(0)
{
  declareProperty("temporaryFix", m_temporaryFix = false);
  declareInterface<ITracksFromTrack>(this);
 
}

StatusCode HltRadCorTool::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  /// Retrieve geometry of detector
  m_detector = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  if( 0 == m_detector ) { return StatusCode::FAILURE; }

  // Tool Interface
  m_tool = tool<IL0Calo2Calo>("L0Calo2CaloTool");
  if(!m_tool)debug()<<"error retrieving the clasterization tool "<<endreq;

  // Temporary fix
  m_tool1 = tool<ICaloClusterization>("CaloClusterizationTool", this);
  if(!m_tool1)debug()<<"error retrieving the clasterization tool 1 "<<endreq;

  if (m_temporaryFix) {
    info() << " Fix in HltRadCorTool is active !!!!!!!!!!!!!!!!!!" << endreq;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Destructor
//=============================================================================
HltRadCorTool::~HltRadCorTool() {} 

//=============================================================================
StatusCode HltRadCorTool::tracksFromTrack(
                const LHCb::Track& seed,
                std::vector<LHCb::Track*>& tracks ) {


   LHCb::Track* ptrack_rad = seed.clone();

   if ( ! ptrack_rad->hasInfo(401) ) {
     double enerad = 0.;
     getRadiationFromClusters(seed, enerad);
     // put 401 info track.h 
     ptrack_rad->addInfo(401, enerad);
   }
   tracks.push_back(ptrack_rad); 

   // debug only
//   if (ptrack_rad->hasInfo(401) ) {
//     double edef =0;
//     double enerad = ptrack_rad->info(401, edef);
//     info() << " has info: " << enerad << endreq;
//   } else {
//     info() << " does not have info: " << endreq;
//   }
   // end debug

   return StatusCode::SUCCESS;

}


void HltRadCorTool::getRadiationFromClusters(const LHCb::Track& tra, double & enerad) {

  enerad = 0.;

  if (!tra.hasStateAt(LHCb::State::AtT)) return;   // RETURN

  const LHCb::State &astate =  tra.firstState();

  double x_tra  =  astate.x();
  double y_tra  =  astate.y();
  double z_tra  =  astate.z();
  double tx_tra =  astate.tx();
  double ty_tra =  astate.ty();

  double z_ecal = 12700.0;

  double x_ecal = x_tra+tx_tra*(z_ecal-z_tra);
  double y_ecal = y_tra+ty_tra*(z_ecal-z_tra);
  

  Gaudi::XYZPoint   point_ecal(x_ecal, y_ecal, z_ecal);

  LHCb::CaloCellID idL0 = m_detector->Cell(point_ecal);
  if( ! m_detector->valid(idL0) ) return;


  // get input data (sequential and simultaneously direct access!)  
  std::vector<CaloCluster*> clusters;
  unsigned int level = 3; // level 1:3x3, 2:5x5, 3:7x7
  if (m_temporaryFix) {
    if ( !exist<LHCb::CaloDigits>(LHCb::CaloDigitLocation::Ecal) ) {
      warning() << "ECAL not decoded. CaloDigits do not exists." << endreq;
      return;
    }
    // get input data (sequential and simultaneously direct access!)  
    LHCb::CaloDigits* digits = get<LHCb::CaloDigits>( LHCb::CaloDigitLocation::Ecal );
    // remember to delete cluster
    m_tool1->clusterize(clusters, digits, m_detector, idL0, level);
    if (clusters.empty()) return;
  } else {
  // do not delete clusters (owned by tool, registered in TES and deleted after event processing)
    m_tool->clusterize(clusters, idL0, level);
    if (clusters.empty()) return;
  }

  const LHCb::State &tstate =  *(tra.stateAt(LHCb::State::AtT));
  
  double x_T  =  tstate.x();
  double y_T  =  tstate.y();
  double z_T  =  tstate.z();
  double tx_T =  tstate.tx();
  double ty_T =  tstate.ty();

  double x_ecal_T = x_T+tx_T*(z_ecal-z_T);
  double y_ecal_T = y_T+ty_T*(z_ecal-z_T);
 
  double dist2min = 100000000.;
  const LHCb::CaloCluster *clustermin_from_T = 0;  

  for( std::vector<CaloCluster*>::iterator cluster = clusters.begin();
        clusters.end() != cluster; ++cluster ) { 
  
    if ( 0 == *cluster ) continue;
  
    const double x = (*cluster)->position().x();
    const double y = (*cluster)->position().y();
  
    double dist2 =  (x-x_ecal_T)*(x-x_ecal_T) + (y-y_ecal_T)*(y-y_ecal_T);
    if ( dist2 < dist2min) {
      dist2min = dist2;
      clustermin_from_T = (*cluster);
    }
  }

  // put as property
  double dist2acc = 70.*70.;

  for( std::vector<CaloCluster*>::iterator cluster = clusters.begin();
        clusters.end() != cluster; ++cluster ) { 
  
    if ( 0 == *cluster ) continue;
  
    const double x = (*cluster)->position().x();
    const double y = (*cluster)->position().y();
  
    double dist2 =  (x-x_ecal)*(x-x_ecal) + (y-y_ecal)*(y-y_ecal);
    if ( dist2 < dist2acc) {
      if ( (*cluster) != clustermin_from_T) {
        double e_clu =  (*cluster)->e();
        enerad += e_clu;
      }
    }
  }
  if (m_temporaryFix) {
    // delete reconstructed clusters
    for( std::vector<CaloCluster*>::iterator cluster = clusters.begin();
          clusters.end() != cluster; ++cluster ) { 
      CaloCluster* cl = *cluster;
      if ( cl ) delete cl;
    }
  }

}
