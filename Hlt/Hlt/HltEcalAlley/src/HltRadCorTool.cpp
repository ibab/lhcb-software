// $Id: HltRadCorTool.cpp,v 1.6 2010-03-13 11:05:32 witekma Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "Event/L0CaloCandidate.h"
#include "Event/Track.h"
#include "Event/CaloCluster.h"
#include "Event/L0DUBase.h"
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
{
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

   return StatusCode::SUCCESS;
}


void HltRadCorTool::getRadiationFromClusters(const LHCb::Track& tra, double & enerad) {
  
  enerad = 0.;

  LHCb::L0CaloCandidates* candidates = get<LHCb::L0CaloCandidates>( LHCb::L0CaloCandidateLocation::Full );

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

  Gaudi::XYZPoint   point_ecal_from_velo(x_ecal, y_ecal, z_ecal);

  double L0_enerad = 0.;
  
  for (std::vector<L0CaloCandidate*>::iterator cand = candidates->begin(); candidates->end() !=cand; ++cand){
    if (0 == *cand) continue;
    // check if it is ECAL photon or electron L0Candidate
    int type = (*cand)->type();
    if ( type == L0DUBase::CaloType::Electron  || 
         type == L0DUBase::CaloType::Photon    || 
         type == L0DUBase::CaloType::Pi0Global || 
         type == L0DUBase::CaloType::Pi0Local) {

      const double x_cand = (*cand)->position().x();
      const double y_cand = (*cand)->position().y();
      const double z_cand = (*cand)->position().z();

      // posTol is 2.5 of cell size. Take 1.5 cel size as a distance limit 
      double tol = 5. * (*cand)->posTol();
      double tol2 = tol * tol;

      double dist2 =  (x_cand-x_ecal)*(x_cand-x_ecal) + (y_cand-y_ecal)*(y_cand-y_ecal);
      if ( dist2 < tol2 ) {
        double e_clu =  (*cand)->et()*sqrt((x_cand*x_cand+y_cand*y_cand+z_cand*z_cand)/(x_cand*x_cand+y_cand*y_cand));
        L0_enerad += e_clu;
      }      
    }
  }
  // if no EM energy derived from L0CaloCandidates then no sense to go for fast cluster reconstruction
  if (L0_enerad < 0.1*Gaudi::Units::GeV) return;   // RETURN
  
  // correction based on fast calo clusterization here  
  LHCb::CaloCellID idcell_from_velo = m_detector->Cell(point_ecal_from_velo);
  if( ! m_detector->valid(idcell_from_velo) ) return;    // RETURN
          
  // get input data (sequential and simultaneously direct access!)  
  std::vector<CaloCluster*> clusters;

  unsigned int level = 1; // level 1:3x3, 2:5x5, 3:7x7, strong timing dependence on level

  // do not delete clusters (owned by tool, registered in TES and deleted after event processing)
  m_tool->clusterize(clusters, idcell_from_velo, level);
  if (clusters.empty()) return;

  // look for Ecal cluster compatible with T extrapolation to avoid double counting
  // namely not to correct for ecal deposit which belongs to the deflected track
  const LHCb::State &tstate =  *(tra.stateAt(LHCb::State::AtT));
    
  double x_T  =  tstate.x();
  double y_T  =  tstate.y();
  double z_T  =  tstate.z();
  double tx_T =  tstate.tx();
  double ty_T =  tstate.ty();

  double x_ecal_T = x_T+tx_T*(z_ecal-z_T);
  double y_ecal_T = y_T+ty_T*(z_ecal-z_T);

  Gaudi::XYZPoint   point_ecal_from_T(x_ecal_T, y_ecal_T, z_ecal);

  LHCb::CaloCellID idcell_from_T = m_detector->Cell(point_ecal_from_T);

  const LHCb::CaloCluster *clustermin_from_T = 0;  

  if( m_detector->valid(idcell_from_T) ) {
    double cellsize_from_T = m_detector->cellSize( idcell_from_T );
    // limit on distance for claster to be compatible with T extrapolation
    double T_range = 1.5 * cellsize_from_T;
    double T_range2 = T_range * T_range;
    double dist2min = T_range2;
    
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
  }

  //
  // here comes real radiation correction
  //

  // distance limit to sum radiated photons
  double cellsize_from_velo = m_detector->cellSize( idcell_from_velo );
  double distacc = 1.5*cellsize_from_velo;
  double dist2acc = distacc * distacc;
  enerad = 0.;
    
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


}
