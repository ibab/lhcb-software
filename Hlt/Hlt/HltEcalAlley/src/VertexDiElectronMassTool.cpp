// $Id: VertexDiElectronMassTool.cpp,v 1.1 2009-02-17 16:24:03 witekma Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
//
#include "Event/L0CaloCandidate.h"

// local
#include "VertexDiElectronMassTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class :VertexDiElectronMassTool
//
// 2008-07-05 : Mariusz Witek
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VertexDiElectronMassTool );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VertexDiElectronMassTool::VertexDiElectronMassTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareProperty("MinEtCluster", m_minEtCluster = 2800.);
  declareInterface<IRecVertexFunctionTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
VertexDiElectronMassTool::~VertexDiElectronMassTool() {} 

//=============================================================================
//=============================================================================
// Initialization
//=============================================================================
StatusCode VertexDiElectronMassTool::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  /// Retrieve geometry of detector
  m_detector = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  if( 0 == m_detector ) { return StatusCode::FAILURE; }

  // Tool Interface
  m_tool = tool<ICaloClusterization>("CaloClusterizationTool", this);
  if(!m_tool)debug()<<"error retrieving the clasterization tool "<<endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================

double VertexDiElectronMassTool::function(const RecVertex& vertex)
{
  const LHCb::Track& t1 = *(vertex.tracks()[0]);
  const LHCb::Track& t2 = *(vertex.tracks()[1]);
  // get calo candidates
  //  L0CaloCandidates* calos = get<L0CaloCandidates>(L0CaloCandidateLocation::Full);

  Gaudi::XYZVector phorad1, phorad2;
  //  getRadiation(t1, calos, phorad1);
  //  getRadiation(t2, calos, phorad2);
  getRadiationFromClusters(t1, phorad1);
  getRadiationFromClusters(t2, phorad2);
  Gaudi::XYZVector pcor1 = t1.momentum()+phorad1;
  Gaudi::XYZVector pcor2 = t2.momentum()+phorad2;
  Gaudi::XYZVector p = pcor1+pcor2;
  double em2 = 0.00051*0.00051;
  double e = sqrt(em2+pcor1.Mag2()) + sqrt(em2+pcor2.Mag2());    
  double m2 = e*e-p.Mag2();
  double m = 0;
  if(m2>0.) m = sqrt(m2); 
  /*
  std::cout << "p12 " << t1.momentum() << " " << t2.momentum() << std::endl;
  std::cout << "phorad12 " << phorad1 << " " << phorad2 << std::endl;
  std::cout << "mass " << m << " " << std::endl;
  */
  return m;
} 

void VertexDiElectronMassTool::getRadiation(const LHCb::Track& tra, L0CaloCandidates* calos, Gaudi::XYZVector& phorad) {

  phorad.SetXYZ(0.,0.,0.);
  const LHCb::State &astate =  tra.firstState();

  double x_tra  =  astate.x();
  double y_tra  =  astate.y();
  double z_tra  =  astate.z();
  double tx_tra =  astate.tx();
  double ty_tra =  astate.ty();

  double dist2min = 70.*70.;
  std::vector<const L0CaloCandidate*> l0calorad;


  for (L0CaloCandidates::const_iterator it = calos->begin(); 
     it != calos->end(); ++it) {

    const L0CaloCandidate& calo = **it;

    if (calo.type() > 1 ) continue;

    double xc = calo.position().X();
    double yc = calo.position().Y();
    double zc = calo.position().Z();

    double x = x_tra+tx_tra*(zc-z_tra);
    double y = y_tra+ty_tra*(zc-z_tra);

    double dx = xc - x;
    double dy = yc - y;

    double dist2 = dx*dx+dy*dy;
    if ( dist2 < dist2min) {
      dist2min = dist2;
      l0calorad.push_back(&calo);
    }
  }
  for ( std::vector<const L0CaloCandidate*>::iterator itc = l0calorad.begin(); itc != l0calorad.end(); itc++) {
    double xc = (*itc)->position().X();
    double yc = (*itc)->position().Y();
    double zc = (*itc)->position().Z();
    double e  = (*itc)->et()*sqrt(xc*xc+yc*yc+zc*zc)/sqrt(xc*xc+yc*yc);
    phorad += e*tra.momentum().Unit();
  }
}

void VertexDiElectronMassTool::getRadiationFromClusters(const LHCb::Track& tra, Gaudi::XYZVector& phorad) {

  phorad.SetXYZ(0.,0.,0.);   

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

  // get input data (sequential and simultaneously direct access!)  
  LHCb::CaloDigits* digits = get<LHCb::CaloDigits>( LHCb::CaloDigitLocation::Ecal );
  std::vector<CaloCluster*> clusters;
  unsigned int level = 3; // level 1:3x3, 2:5x5, 3:7x7
  // remember to delete cluster
  m_tool->clusterize(clusters, digits, m_detector, idL0, level);
 
  if (!tra.hasStateAt(LHCb::State::AtT)) return;   // RETURN

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
        phorad += e_clu * tra.momentum().Unit();
      }
    }
  }

}
