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
    m_ecal(0),
    m_prs(0),
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
  m_ecal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  if( 0 == m_ecal ) { return StatusCode::FAILURE; }
  m_prs = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
  if( 0 == m_prs ) { return StatusCode::FAILURE; }

  // IP and prs middle plane
  m_vertex = Gaudi::XYZPoint(0.,0.,0.);
  m_planePrs = m_prs->plane(CaloPlane::Middle);

  // TMVA discriminant
  std::vector<std::string> inputVars;
  
  
  inputVars.push_back("Ecl");
  inputVars.push_back("fr2");
  inputVars.push_back("fr2r4");
  inputVars.push_back("abs(asym)");
  inputVars.push_back("kappa");
  inputVars.push_back("Eseed/Ecl");
  inputVars.push_back("(E2+Eseed)/Ecl");
  //inputVars.push_back("Eprs");
  
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

  double pt = LHCb::CaloMomentum(hypo).pt();

  double tmva_output = -9999.;
  
  if (pt>m_minPt){  
    double fr2 = 0;
    double fasym = 0;
    double fkappa = 0;
    double fr2r4 = 0;
    double Eseed = 0;
    double E2 = 0;
    double Ecl = 0;
    double EPrs = 0;
    int area =0;
    
    const LHCb::CaloCluster* cluster = LHCb::CaloAlgUtils::ClusterFromHypo( hypo , false);        
    if (cluster!=0){
      ClusterVariables(cluster, fr2, fasym, fkappa, fr2r4, Ecl, Eseed, E2, EPrs, area);      
      tmva_output = photonDiscriminant(area, Ecl, fr2, fr2r4, fasym, fkappa, Eseed, E2);    
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
                                              double& Eseed, double& E2, double& ePrs, int& area) {

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
  //  double cellsize = 0.; // set but not used
  area = -1;
  int ncells = 0;
  double secondE = 0.;
  //    double E3x3 = 0.0;
  //  double dist = 0.0; // set but not used
  
  //CaloCellID seedID = cluster->seed();
  //const Gaudi::XYZPoint& posSeed =  m_ecal->cellCenter( seedID );
  //double xs = posSeed.x();
  //double ys = posSeed.y();
  //const CaloNeighbors& neigh = m_ecal->zsupNeighborCells(seedID);
  
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
      // cellsize = m_ecal->cellSize( digit->cellID() ); // set but not used
      area = digit->cellID().area();
      Eseed = energy;
    }else{
      if(energy>secondE) {
        secondE = energy;
        // dist = sqrt((x-xs)*(x-xs)+(y-ys)*(y-ys))/cellsize; // set but not used
      }
    }
    
    if ( energy <= 0 ) { continue ; }
    const double weight = energy > 0.0 ? energy : 0.0 ;
    
    double rr = (x-xmean)*(x-xmean) + (y-ymean)*(y-ymean);
    r4 += weight * rr*rr;
    
    //       CaloCellID cellid = digit->cellID();
//       for (CaloNeighbors::const_iterator neighIt = neigh.begin(); neighIt != neigh.end(); neighIt++){
//         if ((*neighIt) == cellid) E3x3 = E3x3+energy;
//       }
    
    ncells++;
  }//loop cluster cells
  
  
  r4 /= etot;
  fr2r4 = (r4 - fr2*fr2)/r4;
  
  //PS info
  
  // point in the detector
  Gaudi::XYZPoint prsPoint;
  const Gaudi::XYZVector direction = position - m_vertex;
  //double et=etot*direction.Rho()/direction.R();
  
  
  typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector>  LineType ;
  const LineType line =  LineType(m_vertex , direction );
  double mu=0;
  if( !Gaudi::Math::intersection<LineType,Gaudi::Plane3D, Gaudi::XYZPoint>( line , m_planePrs , prsPoint , mu) )
    warning() << " CAN NOT EXTRAPOLATE TO THE Prs PLANE " << endmsg;
  const LHCb::CaloCellID cellPrs = m_prs->Cell( prsPoint );
  
  if( !exist<CaloDigits>( CaloDigitLocation::Prs ) ){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "No Table container found at " << CaloDigitLocation::Prs << endmsg;
    //return StatusCode::SUCCESS;
  }
  CaloDigits* digitsPrs = get<CaloDigits>(CaloDigitLocation::Prs);
  // Determine which cells have a hit
  for(CaloDigits::iterator idig = digitsPrs->begin() ; digitsPrs->end() !=  idig ; idig++){
    if ( NULL == *idig )continue;
    //CaloCellID cell = (*idig)->cellID();
    if ((*idig)->cellID()==cellPrs) {
        ePrs=(*idig)->e();
    }
  }
  
  
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



double GammaPi0SeparationTool::photonDiscriminant(int area, double Ecl, 
                                           double r2, 
                                           double r2r4, 
                                           double asym, 
                                           double kappa,
                                           double Eseed, double E2) {
        std::vector<double> input;
        
        input.push_back(Ecl);
        input.push_back(r2);
        input.push_back(r2r4);
        input.push_back(asym);
        input.push_back(kappa);
        input.push_back(Eseed);
        input.push_back(E2);
        //input.push_back(Eprs);
        

        double value = -1e10;
        if(area ==0 ) value = m_reader0->GetMvaValue(input);
        if(area ==1 ) value = m_reader1->GetMvaValue(input);
        if(area ==2 ) value = m_reader2->GetMvaValue(input);

        return value;
}
