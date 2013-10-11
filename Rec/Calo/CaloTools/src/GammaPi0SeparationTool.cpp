// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "CaloUtils/CaloMomentum.h"
#include "CaloUtils/CaloAlgUtils.h"
// from Event
#include "Event/RecHeader.h"
#include "Event/CaloHypo.h"
#include "Event/CaloCluster.h"

// local
#include "GammaPi0SeparationTool.h"

#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

using namespace LHCb;
using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : GammaPi0SeparationTool
//
// 2010-03-24 : Miriam Calvo Gomez
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GammaPi0SeparationTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GammaPi0SeparationTool::GammaPi0SeparationTool( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_ecal(NULL),
    m_prs(NULL),
    m_vertex(),
    m_planePrs(),
    m_data(){
  declareInterface<IGammaPi0SeparationTool>(this);
  declareProperty("MinPt", m_minPt = 2000.);
}
//=============================================================================
// Destructor
//=============================================================================
GammaPi0SeparationTool::~GammaPi0SeparationTool() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode GammaPi0SeparationTool::initialize() {

  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  /// Retrieve geometry of detector
  m_ecal = getDetIfExists<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  m_prs  = getDetIfExists<DeCalorimeter>( DeCalorimeterLocation::Prs );

  // IP and prs middle plane
  m_vertex = Gaudi::XYZPoint(0.,0.,0.);
  if(m_prs)m_planePrs = m_prs->plane(CaloPlane::Middle);

  // TMVA discriminant
  std::vector<std::string> inputVars;
  
  inputVars.push_back("fr2");
  inputVars.push_back("fr2r4");
  inputVars.push_back("abs(asym)");
  inputVars.push_back("kappa");
  inputVars.push_back("Eseed/Ecl");
  inputVars.push_back("(E2+Eseed)/Ecl");
  inputVars.push_back("eSumPS>0?eMaxPS/eSumPS:0");
  inputVars.push_back("eSumPS>0?e2ndPS/eSumPS:0");
  inputVars.push_back("r2PS");
  inputVars.push_back("abs(asymPS)");
  inputVars.push_back("multiPS");
  inputVars.push_back("multiPS15");
  inputVars.push_back("multiPS30");
  inputVars.push_back("multiPS45");

  m_reader0.reset( new ReadMLPOuter(inputVars) );
  m_reader1.reset( new ReadMLPMiddle(inputVars) );
  m_reader2.reset( new ReadMLPInner(inputVars) );
  

  return StatusCode::SUCCESS;
  
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GammaPi0SeparationTool::finalize() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;
  StatusCode sc = GaudiTool::finalize(); // must be executed first

  m_reader0.reset(0);
  m_reader1.reset(0);
  m_reader2.reset(0);

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================


double GammaPi0SeparationTool::isPhoton(const LHCb::CaloHypo* hypo){
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug()<<"Inside isPhoton ------"<<endmsg;
  m_data.clear();

  double tmva_output = -9999.;
  if( NULL == m_prs || NULL==m_ecal)return tmva_output;

  double pt = LHCb::CaloMomentum(hypo).pt();

  
  if (pt>m_minPt){  

    double fr2 = 0;
    double fasym = 0;
    double fkappa = 0;
    double fr2r4 = 0;
    double Eseed = 0;
    double E2 = 0;
    double Ecl = 0;
    int area =0;

    double r2PS = 0;
    double r2r4PS = 0;
    double asymPS = 0;
    double kappaPS = 0;
    double ePrs = 0;
    double eMaxPS = 0;
    double e2ndPS = 0;
    double ecornerPS = 0;
    double eSumPS = 0.;
    int multiPS = 0;
    int multiPS15 = 0;
    int multiPS30 = 0;
    int multiPS45 = 0;

    const LHCb::CaloCluster* cluster = LHCb::CaloAlgUtils::ClusterFromHypo( hypo , false);        
    if (cluster!=0){
      ClusterVariables(cluster, fr2, fasym, fkappa, fr2r4, Ecl, Eseed, E2, area);
      PrsVariables(cluster, r2PS, asymPS, kappaPS, r2r4PS, eSumPS, ePrs, eMaxPS, e2ndPS, ecornerPS, multiPS, multiPS15, 
                   multiPS30, multiPS45);
      
      tmva_output = photonDiscriminant(area, fr2, fr2r4, fasym, fkappa, Eseed, E2, 
                                       r2PS, asymPS, eMaxPS, e2ndPS, multiPS, multiPS15, multiPS30, multiPS45);
    }//cluster exists
    else{
      tmva_output = -9999; 
    }		
  } 
  else{
    tmva_output = -9999;
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug()<<"Outside range of pt"<<endmsg;
  }   
  
  return tmva_output;
}


void GammaPi0SeparationTool::ClusterVariables(const LHCb::CaloCluster *cluster,
                                                   double& fr2, double& fasym, double& fkappa, double& fr2r4, double& etot, 
                                              double& Eseed, double& E2, int& area) {
  m_data.clear();
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug()<<"Inside ClusterVariables ------"<<endmsg;
  fr2 = cluster->position().spread()(0,0)+cluster->position().spread()(1,1);
  fasym = (cluster->position().spread()(0,1))/
    (sqrt( (cluster->position().spread()(0,0))*(cluster->position().spread()(1,1)) ));
  fkappa = sqrt( 1-4*( (cluster->position().spread()(0,0))*(cluster->position().spread()(1,1)) - 
                       (cluster->position().spread()(0,1))*(cluster->position().spread()(1,0)) )
                 /( (cluster->position().spread()(0,0) + cluster->position().spread()(1,1)) * 
                    (cluster->position().spread()(0,0) + cluster->position().spread()(1,1))  ) );
  
  double xmean = cluster->position().x();
  double ymean = cluster->position().y();
  etot = cluster->e(); //same as position.e
  
  const Gaudi::XYZPoint position (xmean, ymean,cluster->position().z());
  
  double r4 = 0.;
  //  double cellsize = 0.;
  area = -1;
  int ncells = 0;
  double secondE = 0.;

  const LHCb::CaloCluster::Entries& entries = cluster->entries() ;
  for ( LHCb::CaloCluster::Entries::const_iterator entry = entries.begin() ;
        entries.end() != entry ; ++entry ){
    const LHCb::CaloDigit* digit = entry->digit()  ;
    if ( 0 == digit ) { continue ; }
    const double fraction = entry->fraction();
    const double energy   = digit->e() * fraction ;
    
    const Gaudi::XYZPoint& pos =  m_ecal->cellCenter( digit->cellID() );
    const double x =  pos.x() ;
    const double y =  pos.y() ;
    
    if ( entry->status() & LHCb::CaloDigitStatus::SeedCell ){ 
      //      cellsize = m_ecal->cellSize( digit->cellID() );
      area = digit->cellID().area();
      Eseed = energy;
    }else{
      if(energy>secondE) {
        secondE = energy;
      }
    }
    
    if ( energy <= 0 ) { continue ; }
    const double weight = energy > 0.0 ? energy : 0.0 ;
    
    double rr = (x-xmean)*(x-xmean) + (y-ymean)*(y-ymean);
    r4 += weight * rr*rr;
    
    ncells++;
  }//loop cluster cells
  
  
  r4 /= etot;
  fr2r4 = (r4 - fr2*fr2)/r4;
  
  E2 = (secondE+Eseed)/etot;
  Eseed = Eseed/etot;
  
  m_data["Ecl"]   = etot;
  m_data["Fr2"]   = fr2;
  m_data["Fr2r4"] = fr2r4;
  m_data["Asym"] = fasym;
  m_data["Kappa"] = fkappa;
  m_data["Eseed"] = Eseed;
  m_data["E2"]    = E2;
}

void GammaPi0SeparationTool::PrsVariables(const LHCb::CaloCluster *cluster,
                                          double& r2PS, double& asymPS, double& kappaPS, double& r2r4PS, 
                                          double& eSumPS, double& ePrs, double& eMaxPS, double& e2ndPS, double& ecornerPS, 
                                          int& multiPS, int& multiPS15, int& multiPS30, int& multiPS45) {
  
  //PS info
  
  int colPS = 0;
  int rowPS = 0;
  int col_i = 0;
  int row_i = 0;
  double e_i = 0.0;
  double xmeanPS = 0.0;
  double ymeanPS = 0.0;
  double covxxPS = 0.0;
  double covyyPS = 0.0;
  double covxyPS = 0.0;
  double r4PS = 0.0;
  double c1 = 0.0;
  double c2 = 0.0;
  double c3 = 0.0;
  double c4 = 0.0;
  
  // point in the detector
  const Gaudi::XYZPoint position (cluster->position().x(), cluster->position().y(), cluster->position().z());
  Gaudi::XYZPoint prsPoint;
  const Gaudi::XYZVector direction = position - m_vertex;
  
  typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector>  LineType ;
  const LineType line =  LineType(m_vertex , direction );
  double mu=0;
  if( !Gaudi::Math::intersection<LineType,Gaudi::Plane3D, Gaudi::XYZPoint>( line , m_planePrs , prsPoint , mu) )
    warning() << " CAN NOT EXTRAPOLATE TO THE Prs PLANE " << endreq;
  const LHCb::CaloCellID cellPrs = m_prs->Cell( prsPoint );

  if( !exist<CaloDigits>( CaloDigitLocation::Prs ) ){
    debug() << "No Table container found at " << CaloDigitLocation::Prs << endreq;
    //return StatusCode::SUCCESS;
  }
  CaloDigits* digitsPrs = getIfExists<CaloDigits>(CaloDigitLocation::Prs);
  if( NULL == digitsPrs ){
    Warning("Data missing at "+CaloDigitLocation::Prs,StatusCode::SUCCESS).ignore();
    return;
  }
  

  if( !(LHCb::CaloCellID() == cellPrs)  ) { // valid cell!
    // Determine which cells have a hit
    
    const CaloNeighbors& neighPS = m_prs->zsupNeighborCells(cellPrs);
    LHCb::CaloDigit::Vector NeighbordigitsPrs;
    
    
    for(CaloDigits::iterator idig = digitsPrs->begin() ; digitsPrs->end() !=  idig ; idig++){
      if ( NULL == *idig )continue;
      if ((*idig)->cellID()==cellPrs) {
        ePrs=(*idig)->e();
        eSumPS += ePrs;
        colPS = cellPrs.col();
        rowPS = cellPrs.row();
        NeighbordigitsPrs.push_back(*idig);
        
      }
    }
    
    for(CaloDigits::iterator idig = digitsPrs->begin() ; digitsPrs->end() !=  idig ; idig++){  
      for (CaloNeighbors::const_iterator neighIt = neighPS.begin(); neighIt != neighPS.end(); neighIt++){
        if ((*neighIt) == (*idig)->cellID()){
          
          eSumPS += (*idig)->e();
          
          row_i =  (*neighIt).row();
          col_i = (*neighIt).col();
          e_i = (*idig)->e();
          xmeanPS += (col_i - colPS) * e_i;
          ymeanPS += (row_i - rowPS) * e_i;
          NeighbordigitsPrs.push_back(*idig);
        }
      }
    }
    
    if (eSumPS>0.0){
      xmeanPS = xmeanPS/eSumPS;
      ymeanPS = ymeanPS/eSumPS;
      
      for(LHCb::CaloDigit::Vector::iterator ip = NeighbordigitsPrs.begin(); ip != NeighbordigitsPrs.end(); ip++) {
        e_i = (*ip)->e();
        if(e_i>0.0) multiPS++;
        if(e_i>15.0) multiPS15++;
        if(e_i>30.0) multiPS30++;
        if(e_i>45.0) multiPS45++;
        if (e_i > eMaxPS) eMaxPS = e_i;
        col_i = (*ip)->cellID().col();
        row_i = (*ip)->cellID().row();
        double dxPS = (xmeanPS - ( col_i - colPS));
        double dyPS = (ymeanPS - ( row_i - rowPS));
        
        covxxPS += dxPS * dxPS * e_i;
        covyyPS += dyPS * dyPS * e_i;
        covxyPS += dxPS * dyPS * e_i;	
        
        r4PS += e_i * ( dxPS * dxPS + dyPS * dyPS) * ( dxPS * dxPS + dyPS * dyPS);
        
        if((col_i-colPS)==-1 && (row_i-rowPS)==1) c1 += e_i;
        if((col_i-colPS)==-1 && (row_i-rowPS)==0){ c1 += e_i; c3 += e_i;}
        if((col_i-colPS)==-1 && (row_i-rowPS)==-1) c3 += e_i;
        if((col_i-colPS)==0 && (row_i-rowPS)==1){ c1 += e_i; c2 += e_i;}
        if((col_i-colPS)==0 && (row_i-rowPS)==-1){ c3 += e_i; c4 += e_i;}
        if((col_i-colPS)==1 && (row_i-rowPS)==1) c2 += e_i;
        if((col_i-colPS)==1 && (row_i-rowPS)==0){ c2 += e_i; c4 += e_i;}
        if((col_i-colPS)==1 && (row_i-rowPS)==-1) c4 += e_i;
        
      }
      for(LHCb::CaloDigit::Vector::iterator ip = NeighbordigitsPrs.begin(); ip != NeighbordigitsPrs.end(); ip++) { 
        e_i = (*ip)->e();
        if(e_i>e2ndPS && e_i<eMaxPS) e2ndPS = e_i;
      }
      
      covxxPS = covxxPS/eSumPS;
      covxyPS = covxyPS/eSumPS;
      covyyPS = covyyPS/eSumPS;
      
      if( covxxPS != 0.0 && covyyPS != 0.0) {
        asymPS = covxyPS/sqrt( covxxPS * covyyPS);
      }
      if( covxxPS != 0.0 || covyyPS != 0.0) {
        kappaPS = sqrt( 1.0 - 4.0 * ( (covxxPS * covyyPS) - ( covxyPS * covxyPS)) / (( covxxPS + covyyPS) * (covxxPS + covyyPS)));
      }
    
      r2PS = covxxPS + covyyPS;
      
      if( r4PS!=0.0){
        r4PS = r4PS/eSumPS;
        r2r4PS = ( r4PS - r2PS * r2PS) /r4PS;
        
      }
      
      ecornerPS = MAX(c1, MAX( c2, MAX(c3, c4) ) );
      eMaxPS = eMaxPS/eSumPS;
      e2ndPS = e2ndPS/eSumPS;
      
    }//of E_PS!=0
  }//PS cell exists
}


double GammaPi0SeparationTool::photonDiscriminant(int area, 
                                                  double r2, double r2r4, double asym, 
                                                  double kappa, double Eseed, double E2,
                                                  double r2PS, double asymPS, double eMaxPS, double e2ndPS,
                                                  int multiPS, int multiPS15, int multiPS30, int multiPS45) {


        std::vector<double> input;
        
        input.push_back(r2);
        input.push_back(r2r4);
        input.push_back(abs(asym));
        input.push_back(kappa);
        input.push_back(Eseed);//already divided by Ecl
        input.push_back(E2); //means (e2+eseed)/ecl
        input.push_back(eMaxPS);//divided by Esum
        input.push_back(e2ndPS);//divided by Esum
        input.push_back(r2PS);
        input.push_back(abs(asymPS));
        input.push_back(multiPS);
        input.push_back(multiPS15);
        input.push_back(multiPS30);
        input.push_back(multiPS45);
        
        double value = -1e10;
        if(area ==0 ) value = m_reader0->GetMvaValue(input);
        if(area ==1 ) value = m_reader1->GetMvaValue(input);
        if(area ==2 ) value = m_reader2->GetMvaValue(input);

        return value;
}
