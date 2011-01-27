// $Id: HltIsPhotonTool.cpp,v 1.13 2009-12-01 21:33:10 witekma Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltIsPhotonTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class :HltIsPhotonTool
//
// 2008-07-05 : Mariusz Witek
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltIsPhotonTool );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltIsPhotonTool::HltIsPhotonTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
    , m_detector(0)
    , m_tool(0)
    , m_tool1(0)
{
  // just a safeguard against crazy clusters
  declareProperty("MinEtCluster", m_minEtCluster = 2000.);
  declareInterface<ITrackFunctionTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
HltIsPhotonTool::~HltIsPhotonTool() {} 

//=============================================================================
//=============================================================================
// Initialization
//=============================================================================
StatusCode HltIsPhotonTool::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  /// Retrieve geometry of detector
  m_detector = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  if( 0 == m_detector ) { return StatusCode::FAILURE; }

  // Calo clusterization interface
  m_tool = tool<IL0Calo2Calo>("L0Calo2CaloTool");
  if(!m_tool)debug()<<"error retrieving the clasterization tool "<<endreq;

  // Temporary fix
  m_tool1 = tool<ICaloClusterization>("CaloClusterizationTool", this);
  if(!m_tool1)debug()<<"error retrieving the clasterization tool 1 "<<endreq;

  // TMVA discriminant
  std::vector<std::string> inputVars;
  inputVars.push_back("showershape");
  inputVars.push_back("showerasym");
  inputVars.push_back("showerkappa");
  inputVars.push_back("showertail");
  inputVars.push_back("showerenergy");

  m_reader0.reset( new ReadFisherArea0(inputVars) );
  m_reader1.reset( new ReadFisherArea1(inputVars) );
  m_reader2.reset( new ReadFisherArea2(inputVars) );

  return StatusCode::SUCCESS;
}

StatusCode HltIsPhotonTool::finalize() {
  StatusCode sc = GaudiTool::finalize(); // must be executed first
  m_reader0.reset(0);
  m_reader1.reset(0);
  m_reader2.reset(0);
  return sc;
}
//=============================================================================

double HltIsPhotonTool::function(const Track& ctrack)
{

  // get LHCbID of coresponding L0CaloCandidate
  const std::vector< LHCb::LHCbID >&  lista=   ctrack.lhcbIDs ();
  LHCb::CaloCellID idL0;
  bool found_idL0 = false;
  int row_min    = 100000000;
  int col_min    = 100000000;
  for (std::vector< LHCb::LHCbID >::const_iterator itid = lista.begin(); itid != lista.end(); itid++) {
    if( ! itid->isCalo() ) continue;
    LHCb::CaloCellID cid = itid->caloID();
    if ( cid.calo() != 2 ) continue;
    int row = cid.row();
    int col = cid.col();
    if ( row <= row_min && col <= col_min ) {
      row_min = row; col_min = col;
      idL0 = cid; 
      found_idL0 = true;
    } 
  }  

  if ( ! found_idL0 ) return -9999999.;

  const Gaudi::XYZPoint& pos = m_detector->cellCenter( idL0 );
  double xc = pos.x() ;
  double yc = pos.y() ;

  std::vector<CaloCluster*> clusters;
  unsigned int level = 3; // level 1:3x3, 2:5x5, 3:7x7
  // do not delete clusters (owned by tool, registered in TES and deleted after event processing)
  m_tool->clusterize(clusters, idL0, level);
  if (clusters.empty()) return -9999999.;


  // get cluster closest to Track ( = L0Photon )
  double dist2min = 100000000.;
  const LHCb::CaloCluster *clustermin = 0;  

  for( std::vector<CaloCluster*>::iterator cluster = clusters.begin();
        clusters.end() != cluster; ++cluster ) { 
  
    if ( 0 == *cluster ) continue;
  
    const double x = (*cluster)->position().x();
    const double y = (*cluster)->position().y();
  
    double dist2 =  (x-xc)*(x-xc) + (y-yc)*(y-yc);
    if ( dist2 < dist2min) {
      dist2min = dist2;
      clustermin = (*cluster);
    }
  }

  double showershape = 0.;
  double showerasym  = 0.;
  double showerkappa = 0.;
  double showertail  = 0.;

  caloClusterShapeVariables(clustermin, showershape, showerasym, showerkappa, showertail);

  double showerenergy = clustermin->e();

  normalizeShowerShapeVariables(showershape, showerasym, showerkappa, showertail, showerenergy);

  int iarea =  idL0.area();
  double phovar = photonDiscriminant(iarea, showershape, showerasym, showerkappa, showertail, showerenergy);

  // print ==========================================
  /*  
  std::cout << format("isisP area, phovar: %3d %8.5f %8.5f %8.5f %8.5f %8.5f  result = %8.5f  xc yc = %8.5f %8.5f ", 
		      iarea, showershape, showerasym, showerkappa, showertail, showerenergy, phovar, xc, yc) << std::endl;

  std::cout << "clusters size: " << clusters.size() 
            << " area: " << ireg 
            << " xc,yc: " << xc << " " << yc 
            << " closest e: " << clustermin->e() 
            << " track e: " << ctrack.p()
            << " phovar: " << phovar
            << std::endl;
  for( std::vector<CaloCluster*>::iterator cluster = clusters.begin();
        clusters.end() != cluster; ++cluster ) { 
    CaloCluster* cl = *cluster;
    std::cout << " x,y,z,e " 
              << cl->position().x() << " " 
              << cl->position().y() << " "  
              << cl->position().z() << " clu e "  
              << cl->e() << " " 
              << std::endl;

  }
  */
  // end print ======================================

  // updated et
  double x = clustermin->position().x();
  double y = clustermin->position().y();
  double z = clustermin->position().z();
  double sin_alpha = sqrt( (x*x+y*y) / (x*x+y*y+z*z));
  double etnew = clustermin->e()*sin_alpha;


  if ( etnew > m_minEtCluster ) {
    return phovar;
  }

  return -9999999.;

} 


void  HltIsPhotonTool::normalizeShowerShapeVariables(double& showershape, double& showerasym, 
                               double& showerkappa, double& showertail, double& showerenergy) {

  double ALMOST_ONE  = 0.99999;
  double ALMOST_ZERO = 0.00001;
  // normalize shape parameters
  showershape /= 10000.;
  if (showershape >= 1. ) showershape = ALMOST_ONE;        
  if (showershape <  0. ) showershape = ALMOST_ZERO;        

  if (showerasym   < 0. ) showerasym *= -1.;
  if (showerasym   >= 1.) showerasym  = ALMOST_ONE;

  if (showerkappa >= 1. ) showerkappa = ALMOST_ONE;
  if (showerkappa <  0. ) showerkappa = ALMOST_ZERO;

  if (showertail  >= 1. ) showertail  = ALMOST_ONE;
  if (showertail  <  0. ) showertail  = ALMOST_ZERO;

  showerenergy /= 200000.;
  if(showerenergy >= 1. ) showerenergy = ALMOST_ONE;

}

double HltIsPhotonTool::photonDiscriminant(int iarea,
                                           double showershape, 
                                           double showerasym, 
                                           double showerkappa, 
                                           double showertail,
                                           double showerenergy) {
        std::vector<double> input;
        
        input.push_back(showershape);
        input.push_back(showerasym);
        input.push_back(showerkappa);
        input.push_back(showertail);
        input.push_back(showerenergy);

        double value = -1e10;
        // area and region are different  0 1 2 and 3 2 1. TODO Switch to area 
        if(iarea ==0 ) value = m_reader0->GetMvaValue(input);
        if(iarea ==1 ) value = m_reader1->GetMvaValue(input);
        if(iarea ==2 ) value = m_reader2->GetMvaValue(input);

        return value;
}

void HltIsPhotonTool::caloClusterShapeVariables(const LHCb::CaloCluster *cluster,
		      double& fr2, double& fasym, double& fkappa, double& fr2r4) {


  fr2    = 0.;
  fr2r4  = 0.;
  fasym  = 0.;
  fkappa = 0.;

  // ignore trivial cases 
  if ( 0 == cluster               ) { return ; }
  if ( cluster->entries().empty() ) { return ; }
  
  /// avoid long names 
  typedef LHCb::CaloCluster::Entries::const_iterator const_iterator;
  
  ///
  double covxx         = 0 ;
  double covyx         = 0 ;
  double covyy         = 0 ;
  double xmean         = 0 ;
  double ymean         = 0 ;
  double etot          = 0 ;
  
  unsigned int ncells  = 0   ;
  
  double cellsize      = -10 ;
 
  
  const LHCb::CaloCluster::Entries& entries = cluster->entries() ;
  for ( LHCb::CaloCluster::Entries::const_iterator entry = entries.begin() ;
        entries.end() != entry ; ++entry ) 
  {
    const LHCb::CaloDigit* digit = entry->digit()  ;
    if ( 0 == digit ) { continue ; }
    ///    
    const double fraction = entry->fraction();
    ///  
    const double energy   = digit->e() * fraction ;
    ///
    const Gaudi::XYZPoint& pos =
      m_detector->cellCenter( digit->cellID() );
    ///
    if ( entry->status() & LHCb::CaloDigitStatus::SeedCell )
    { cellsize = m_detector->cellSize( digit->cellID() ) ; }
    ///
    const double x =  pos.x() ;
    const double y =  pos.y() ;
    ///
    if ( energy <= 0 ) { continue ; }
    const double weight = energy > 0.0 ? energy : 0.0 ;
    ///
    ++ncells ;
    ///
    etot  +=         weight ;
    xmean +=     x * weight ;
    ymean +=     y * weight ;
    //
    covxx += x * x * weight ;
    covyx += y * x * weight ;
    covyy += y * y * weight ;
    ///
  }
  ///
  /// strange combinations 
  if ( ncells    <= 0 ) {return ;  }
  /// energy is not positive  
  if ( etot      <= 0 ) { return ; }
  /// seed cell was not found
  if ( cellsize  <= 0 ) { return ; }
  
  const double uniform  = cellsize * cellsize / 12.  ;
  const double cut      = 0.5 * uniform              ;
  
  xmean /= etot ;
  ymean /= etot ;
  
  /*
  const LHCb::CaloPosition::Center& center_orig = cluster->position().center() ;
  std::cout << "center_orig: " 
  << center_orig ( LHCb::CaloPosition::X ) << " " << xmean << " | | "
  << center_orig ( LHCb::CaloPosition::Y ) << " " << ymean << std::endl;
  */
  covxx /= etot ;
  covyx /= etot ;
  covyy /= etot ;
  
  covxx -= xmean * xmean ;
  covyx -= ymean * xmean ;
  covyy -= ymean * ymean ;
  
  /// could do nothing for 1 cell "clusters" 
  if ( 1 == ncells  ) 
  {
    covxx = uniform ;
    covyy = uniform ;
    covyx = 0.0     ;
  }
    
  if ( covxx < cut ) { covxx = uniform ; covyx = 0 ; }
  if ( covyy < cut ) { covyy = uniform ; covyx = 0 ; }
  
  LHCb::CaloPosition::Spread spread;
  /*
  const LHCb::CaloPosition::Spread& spread_orig = cluster->position().spread() ;
  std::cout << "spread_orig: " 
    << spread_orig ( LHCb::CaloPosition::X , LHCb::CaloPosition::X ) << " " << covxx << " | " 
    << spread_orig ( LHCb::CaloPosition::Y , LHCb::CaloPosition::X ) << " " << covyx << " | " 
    << spread_orig ( LHCb::CaloPosition::Y , LHCb::CaloPosition::Y ) << " " << covyy << std::endl;
  */
  spread ( LHCb::CaloPosition::X , LHCb::CaloPosition::X ) = covxx ;
  spread ( LHCb::CaloPosition::Y , LHCb::CaloPosition::X ) = covyx ;
  spread ( LHCb::CaloPosition::Y , LHCb::CaloPosition::Y ) = covyy ;

  // calculate shape parameters
  
  fr2 = spread(0,0) + spread(1,1);
  fasym = spread(0,1)/sqrt((spread(0,0))*(spread(1,1)));
  fkappa = sqrt( 1-4*( (spread(0,0))*(spread(1,1)) - (spread(0,1))*(spread(1,0)) )/
                ( (spread(0,0) + spread(1,1)) * (spread(0,0) + spread(1,1))  ) );



  double r4 = 0.;

  if (2 > ncells) { return;}

  for ( LHCb::CaloCluster::Entries::const_iterator entry = entries.begin() ;
        entries.end() != entry ; ++entry ) 
  {
    const LHCb::CaloDigit* digit = entry->digit()  ;
    if ( 0 == digit ) { continue ; }
    ///    
    const double fraction = entry->fraction();
    ///  
    const double energy   = digit->e() * fraction ;
    ///
    const Gaudi::XYZPoint& pos =
      m_detector->cellCenter( digit->cellID() );
    ///
    if ( entry->status() & LHCb::CaloDigitStatus::SeedCell )
    { cellsize = m_detector->cellSize( digit->cellID() ) ; }
    ///
    const double x =  pos.x() ;
    const double y =  pos.y() ;
    ///
    if ( energy <= 0 ) { continue ; }
    const double weight = energy > 0.0 ? energy : 0.0 ;
    ///
    ++ncells ;
    ///
    double rr = (x-xmean)*(x-xmean) + (y-ymean)*(y-ymean);
    r4 += weight * rr*rr;
    ///
  }
  r4 /= etot;
  fr2r4 = (r4 - fr2*fr2)/r4;
  //  std::cout << "fr2r4 " << fr2r4 << std::endl;    

  ///
}

