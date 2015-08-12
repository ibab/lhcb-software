// Include files 

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
  m_data(),
  m_def(-1.e+06){
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

  // clear all data
  m_data.clear();
  m_prsdata.clear();

  if( NULL==m_ecal)return m_def;
  if ( LHCb::CaloMomentum(hypo).pt() < m_minPt)return m_def;

  double fr2 = 0;
  double fasym = 0;
  double fkappa = 0;
  double fr2r4 = 0;
  double Eseed = 0;
  double E2 = 0;
  double Ecl = 0;
  int area =0;
  
  double r2PS = 0.;
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

  // evaluate the NN inputs 
  bool ecalV = ClusterVariables(hypo, fr2, fasym, fkappa, fr2r4, Ecl, Eseed, E2, area);
  bool prsV  = PrsVariables(hypo, r2PS, asymPS, kappaPS, r2r4PS, eSumPS, ePrs, eMaxPS, e2ndPS, ecornerPS, 
                            multiPS, multiPS15, multiPS30, multiPS45);
  if( !ecalV || !prsV )return m_def;      
  // return NN output
  return photonDiscriminant(area, fr2, fr2r4, fasym, fkappa, Eseed, E2, 
                            r2PS, asymPS, eMaxPS, e2ndPS, multiPS, multiPS15, multiPS30, multiPS45);
}


bool GammaPi0SeparationTool::ClusterVariables(const LHCb::CaloHypo* hypo,
                                              double& fr2, double& fasym, double& fkappa, double& fr2r4, double& etot, 
                                              double& Eseed, double& E2, int& area) {
  m_data.clear();

  if( NULL == hypo)return false;
  const LHCb::CaloCluster* cluster = LHCb::CaloAlgUtils::ClusterFromHypo( hypo );   // OD 2014/05 - change to Split Or Main  cluster
  if( NULL == cluster)return false;

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )debug()<<"Inside ClusterVariables ------"<<endmsg;

  double cxx = cluster->position().spread()(0,0);
  double cyy = cluster->position().spread()(1,1);
  double cxy = cluster->position().spread()(0,1);
  fr2 = cxx + cyy ;
  fasym = (cxx > 0 && cyy > 0) ? cxy /std::sqrt( cxx*cyy ) : 0;
  fkappa = (fr2 > 0. ) ? std::sqrt( 1. - 4.* ( cxx*cyy - cxy*cxy ) / fr2 / fr2  ) : 0;

  double xmean = cluster->position().x();
  double ymean = cluster->position().y();

  // OD : WARNING cluster->e() is cluster-shape dependent (3x3 / 2x2 ...) RE-EVALUATE E3x3 instead for back. compatibility
  // etot = cluster->e(); //same as position.e  
  etot = 0.;

  const Gaudi::XYZPoint position (xmean, ymean,cluster->position().z());
  
  double r4 = 0.;
  area = -1;
  int ncells = 0;
  double secondE = 0.;

  const LHCb::CaloCluster::Entries& entries = cluster->entries() ;
  for ( LHCb::CaloCluster::Entries::const_iterator entry = entries.begin() ; entries.end() != entry ; ++entry ){
    const LHCb::CaloDigit* digit = entry->digit()  ;
    if ( 0 == digit ) { continue ; }
    const double fraction = entry->fraction();
    const double energy   = digit->e() * fraction ;

    if( abs( digit->cellID().col() - cluster->seed().col() ) <= 1 && 
        abs( digit->cellID().row() - cluster->seed().row() ) <= 1 &&
        digit->cellID().area() == cluster->seed().area() )etot += energy;

    
    const Gaudi::XYZPoint& pos =  m_ecal->cellCenter( digit->cellID() );
    const double x =  pos.x() ;
    const double y =  pos.y() ;
    
    if ( entry->status() & LHCb::CaloDigitStatus::SeedCell ){ 
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
    if( entries.size() <= 1 || rr < 1.e-10 )rr=0; // to avoid huge unphysical value due to machine precision
    r4 += weight * rr*rr;
    
    ncells++;
  }//loop cluster cells
  
  if( etot > 0. ){
    r4 /= etot;
    fr2r4 = (r4 !=0) ? (r4 - fr2*fr2)/r4 : 0.;
    E2 = (secondE+Eseed)/etot;
    Eseed = Eseed/etot;    
  }else{
    // should never happen
    r4 = 0;
    fr2r4 = 0.;
    E2 = 0.;
    Eseed = 0.;
  }
  
  m_data["Ecl"]   = etot;
  m_data["Fr2"]   = fr2;
  m_data["Fr2r4"] = fr2r4;
  m_data["Asym"] = fasym;
  m_data["Kappa"] = fkappa;
  m_data["Eseed"] = Eseed;
  m_data["E2"]    = E2;
  return true;
}

bool GammaPi0SeparationTool::PrsVariables(const LHCb::CaloHypo* hypo,
                                          double& r2PS, double& asymPS, double& kappaPS, double& r2r4PS, 
                                          double& eSumPS, double& ePrs, double& eMaxPS, double& e2ndPS, double& ecornerPS, 
                                          int& multiPS, int& multiPS15, int& multiPS30, int& multiPS45) {
  
  // clear prs data
  m_prsdata.clear();  
  e2ndPS = 0.;
  ePrs = 0.;
  eSumPS=0.;
  eMaxPS=0.;
  multiPS=0;
  multiPS15=0;
  multiPS30=0;
  multiPS45=0;  
  asymPS=0.;
  r2PS=0.;
  kappaPS=0.;
  r2r4PS=0.;
  ecornerPS=0.;

  if( NULL == hypo)return false;
  const LHCb::CaloCluster* cluster = LHCb::CaloAlgUtils::ClusterFromHypo( hypo );   
  if( NULL == cluster)return false;


  LHCb::CaloCellID cellPrs = cluster->seed();  // use cluster seed projection (mostly the same as line projection + faster access)
  cellPrs.setCalo(1);

  if( LHCb::CaloCellID() == cellPrs  )return true; // no valid Prs info

  // compute energy sum / max / ...
  const LHCb::CaloHypo::Digits& digits = hypo->digits();
  double xPs = 0.;
  double yPs = 0.;
  for( LHCb::CaloHypo::Digits::const_iterator d = digits.begin() ; digits.end() != d ; ++d ){ 
    const LHCb::CaloDigit* digit = *d;
    if     ( digit == 0     ) continue           ; 
    if ((int) digit->cellID().calo() != 1 )continue;  // select Prs digits
    int dcol = int(digit->cellID().col()) - int(cellPrs.col());
    int drow = int(digit->cellID().row()) - int(cellPrs.row());
    if( abs(dcol) > 1 || abs(drow)> 1 )continue; // keep only neighbors


    double e = digit->e();

    // energy
    eSumPS  += e ; 
    if( cellPrs == digit->cellID() )ePrs = e ;

    // barycenter
    xPs += double(dcol) * digit->e();
    yPs += double(drow) * digit->e();

    // multiplicities
    if( e > eMaxPS ) eMaxPS = e;
    if( e > 0.0    ) multiPS++;
    if( e > 15.0   ) multiPS15++;
    if( e > 30.0   ) multiPS30++;
    if( e > 45.0   ) multiPS45++;
  }


  if(  eSumPS <= 0. )return true;
  xPs = xPs/eSumPS;
  yPs = yPs/eSumPS;

  // spread and related shape variables
  double cxxPS = 0.;
  double cyyPS = 0.;
  double cxyPS = 0.;
  double r4PS    = 0.;
  double c1 = 0.0;
  double c2 = 0.0;
  double c3 = 0.0;
  double c4 = 0.0;
  for( LHCb::CaloHypo::Digits::const_iterator d = digits.begin() ; digits.end() != d ; ++d ){ 
    const LHCb::CaloDigit* digit = *d;
    if     ( digit == 0     ) continue           ; 
    if ((int) digit->cellID().calo() != 1 )continue;  // select Prs digits
    int dcol = ( int(digit->cellID().col()) - int(cellPrs.col()) );
    int drow = ( int(digit->cellID().row()) - int(cellPrs.row()) );
    if( abs(dcol) > 1 || abs(drow)> 1 )continue; // keep only neighbors

    // 2Nd max
    double e = digit->e();
    if( e > e2ndPS && e < eMaxPS )e2ndPS = e;

    // spread
    double dxPS = (xPs - double(dcol));
    double dyPS = (yPs - double(drow));
    cxxPS += dxPS * dxPS * e;
    cyyPS += dyPS * dyPS * e;
    cxyPS += dxPS * dyPS * e;    

    // shape variables
    r4PS += e * ( dxPS * dxPS + dyPS * dyPS) * ( dxPS * dxPS + dyPS * dyPS);

    // corner energy
    if(dcol==-1 && drow== 1 ){ c1 += e;}
    if(dcol==-1 && drow== 0 ){ c1 += e; c3 += e;}
    if(dcol==-1 && drow==-1 ){ c3 += e;}
    if(dcol== 0 && drow== 1 ){ c1 += e; c2 += e;}
    if(dcol== 0 && drow==-1 ){ c3 += e; c4 += e;}
    if(dcol== 1 && drow== 1 ){ c2 += e;}
    if(dcol== 1 && drow== 0 ){ c2 += e; c4 += e;}
    if(dcol== 1 && drow==-1 ){ c4 += e;}        
  }   
  cxxPS = cxxPS/eSumPS;
  cxyPS = cxyPS/eSumPS;
  cyyPS = cyyPS/eSumPS;
  r2PS    = cxxPS + cyyPS;
  asymPS  = ( cxxPS > 0.0 && cyyPS > 0.0) ? cxyPS/std::sqrt( cxxPS * cyyPS) : 0.;
  kappaPS = (r2PS > 0.0) ? std::sqrt( 1.0 - 4.0 *  (cxxPS * cyyPS - cxyPS * cxyPS) / r2PS / r2PS ) : 0;
  r4PS = r4PS/eSumPS;
  if( r4PS!=0.0) r2r4PS = ( r4PS - r2PS * r2PS) /r4PS;

  ecornerPS = MAX(c1, MAX( c2, MAX(c3, c4) ) );
  eMaxPS = eMaxPS/eSumPS;
  e2ndPS = e2ndPS/eSumPS;


  // === store the data
  m_prsdata["PrsFr2"]     = r2PS;
  m_prsdata["PrsAsym"]    = asymPS;
  m_prsdata["PrsM"]       = double(multiPS);
  m_prsdata["PrsM15"]     = double(multiPS15);
  m_prsdata["PrsM30"]     = double(multiPS30);
  m_prsdata["PrsM45"]     = double(multiPS45);
  m_prsdata["PrsEmax"]    = eMaxPS;
  m_prsdata["PrsE2"]      = e2ndPS;
  return true;
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
        else if(area ==1 ) value = m_reader1->GetMvaValue(input);
        else if(area ==2 ) value = m_reader2->GetMvaValue(input);
        //info() << " INPUT TO GAMMA/PI0 : NN[" << input << "]= " << value << " (area="<<area<<")"<< endmsg;
        return value;
}
