// ============================================================================
// Include files
#include "GaudiKernel/SystemOfUnits.h"
#include "Event/CaloHypo.h"
#include "CaloDet/DeCalorimeter.h"
#include "CaloLCorrection.h"

/** @file 
 *  Implementation file for class : CaloLCorrection
 *  
 *  @date 2003-03-10 
 *  @author Xxxx XXXXX xxx@xxx.com 
 */

DECLARE_TOOL_FACTORY( CaloLCorrection )

// ============================================================================
/** Standard constructor
 *  @see GaudiTool 
 *  @see  AlgTool 
 *  @param type tool type (?)
 *  @param name tool name 
 *  @param parent  tool parent 
 */
// ============================================================================
CaloLCorrection::CaloLCorrection 
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent ) 
  : CaloCorrectionBase( type , name , parent ){

  // define conditionName
  const std::string uName ( LHCb::CaloAlgUtils::toUpper( name ) ) ;
  if( uName.find( "ELECTRON" ) != std::string::npos  ){
    m_conditionName = "Conditions/Reco/Calo/ElectronLCorrection";
  }else if ( uName.find( "MERGED" )  != std::string::npos   ||  uName.find( "SPLITPHOTON" )  != std::string::npos ){
    m_conditionName = "Conditions/Reco/Calo/SplitPhotonLCorrection";
  }  
  else if (  uName.find( "PHOTON" ) ){
    m_conditionName = "Conditions/Reco/Calo/PhotonLCorrection"; 
  }
  
  /// interafces 
  declareInterface<ICaloHypoTool> ( this ) ;  
}

// ============================================================================
/// destructor
// ============================================================================
CaloLCorrection::~CaloLCorrection () {}

// ============================================================================
StatusCode CaloLCorrection::finalize   () 
{
  m_hypos.clear();
  return CaloCorrectionBase::finalize () ;
}
// ============================================================================
StatusCode CaloLCorrection::initialize () 
{
  /// first initialize the base class 
  StatusCode sc = CaloCorrectionBase::initialize();
  if( sc.isFailure() ){ return Error ( "Unable initialize the base class CaloCorrectionBase !" , sc ) ; }  

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "Condition name : " << m_conditionName << endmsg;

  return StatusCode::SUCCESS ;
}

// ============================================================================
StatusCode CaloLCorrection::operator() ( LHCb::CaloHypo* hypo  ) const{ 
  return process( hypo ); 
}

// ============================================================================
StatusCode CaloLCorrection::process    ( LHCb::CaloHypo* hypo  ) const{
  
  // check arguments 
  if( 0 == hypo ) { return Warning ( " CaloHypo* points to NULL!" , StatusCode::SUCCESS) ; }

  // check the Hypo
  Hypotheses::const_iterator h =std::find( m_hypos.begin() , m_hypos.end() , hypo->hypothesis() ) ;
  if( m_hypos.end() == h )return Error ( "Invalid hypothesis -> no correction applied", StatusCode::SUCCESS ) ; 
  
  // No correction for negative energy :
  if( hypo->e() < 0.){
    counter("Skip negative energy correction") += 1;
    return StatusCode::SUCCESS;
  }


  // get Prs/Spd
  double ePrs = 0 ;
  double eSpd = 0 ;
  getPrsSpd(hypo,ePrs,eSpd);


  // get cluster energy (special case for SplitPhotons)
  const LHCb::CaloCluster* GlobalCluster = LHCb::CaloAlgUtils::ClusterFromHypo(hypo, false);
  const LHCb::CaloCluster* MainCluster = LHCb::CaloAlgUtils::ClusterFromHypo(hypo,true) ;

  /*
    Cluster information (e/x/y  and Prs/Spd digit)
  */
  if( 0 == MainCluster ) { return Error ( "CaloCLuster* points to NULL!" ) ; }

  // For Split Photon - share the Prs energy
  if(  LHCb::CaloHypo::PhotonFromMergedPi0 == hypo->hypothesis() ){
    ePrs *= MainCluster->position().e()/GlobalCluster->position().e() ;
  }


  const double energy = hypo->e () ;

  const LHCb::CaloCluster::Entries& entries = MainCluster->entries();
  LHCb::CaloCluster::Entries::const_iterator iseed = 
    LHCb::ClusterFunctors::locateDigit ( entries.begin () ,entries.end   () ,LHCb::CaloDigitStatus::SeedCell );
  if( entries.end() == iseed ){ return Error ( "The seed cell is not found !" ) ; }

  // get the "area" of the cluster (where seed is) 
  const LHCb::CaloDigit*  seed    = iseed->digit();
  if( 0 == seed ) { return Error ( "Seed digit points to NULL!" ) ; }

  // Cell ID for seed digit 
  LHCb::CaloCellID cellID = seed->cellID() ;

  
  /** here all information is available 
   *     
   *  (1) Ecal energy in 3x3     :    energy
   *  (2) Prs and Spd energies   :    ePrs, eSpd 
   *  (3) weighted barycenter    :    xBar, yBar 
   *  (4) Zone/Area in Ecal      :    area   
   *  (5) SEED digit             :    seed   
   *  (6) CellID of seed digit   :    cellID
   *  (7) Position of seed cell  :    seedPos 
   */


  // Account for the tilt
  ROOT::Math::Plane3D plane = m_det->plane(CaloPlane::Front); // Ecal Front-Plane
  const LHCb::CaloPosition* pos = hypo->position() ;

  double  xg = pos->x() - m_origin.X();
  double  yg = pos->y() - m_origin.Y();
  Gaudi::XYZVector normal=plane.Normal();
  double Hesse = plane.HesseDistance();
  double z0 = (-Hesse-normal.X()*pos->x()-normal.Y()*pos->y())/normal.Z();
  double  zg = z0 - m_origin.Z();
    
  // hardcoded inner offset (+7.5 mm)
  if(cellID.area() == 2)zg+=7.5;
  

  // Uncorrected angle
  double tth   = sqrt ( pow(xg,2) + pow(yg,2) ) / zg ;
  double cth   = cos  ( atan( tth ) ) ;

  // Corrected angle
  
  double gamma0 = getCorrection(CaloCorrection::gamma0, cellID);
  double delta0 = getCorrection(CaloCorrection::delta0, cellID);
  double gammaP = getCorrection(CaloCorrection::gammaP, cellID, ePrs);
  double deltaP = getCorrection(CaloCorrection::deltaP, cellID, ePrs);
  double g = gamma0 - gammaP;
  double d = delta0 + deltaP;

  //double alpha = Par_Al1[zon] - exp( Par_Al2[zon] - Par_Al3[zon] * ePrs/Gaudi::Units::MeV );
  //double beta  = Par_Be1[zon] + exp( Par_Be2[zon] - Par_Be3[zon] * ePrs/Gaudi::Units::MeV  );


  counter("<gamma> " + cellID.areaName() ) += g ;  
  counter("<delta> " + cellID.areaName() ) += d ;

  double tgfps = (energy > 0.) ? g * log(energy/Gaudi::Units::GeV) + d : 0.;

  tth = tth / ( 1. + tgfps * cth / zg ) ;
  cth= cos( atan( tth ) ) ;
  double dzfps = cth * tgfps ;
  counter("Delta(Z) "+cellID.areaName()) += dzfps;

  /*
  info() << " ======= Z0  FRONT-PLANE = " << z0 << " " << zg << endmsg;
  ROOT::Math::Plane3D planeSM = m_det->plane(CaloPlane::ShowerMax); // Ecal Front-Plane
  info() << " ======= Z0  SHOWERMAX = " << -planeSM.HesseDistance() <<endmsg;
  ROOT::Math::Plane3D planeM = m_det->plane(CaloPlane::Middle); // Ecal Middle
  info() << " ======= Z0  MIDDLE = " << -planeM.HesseDistance() <<endmsg;
  */

  




// Recompute Z position and fill CaloPosition
  double zCor = z0 + dzfps;

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    debug() << "Hypothesis :" << hypo->hypothesis() << endmsg;
    debug() << " ENE  " << hypo->position ()->e() <<  " "
            << "xg "   << xg <<  " "<< "yg "   << yg <<  endmsg;
    debug() << "zg "   << pos->z() << " " 
            << "z0 "   << z0 <<  " "
            << "DeltaZ "   << dzfps <<  " "
            << "zCor "   << zCor 
            << endmsg ;
  }
  
  hypo -> position() -> setZ( zCor ) ;

  
  return StatusCode::SUCCESS ;

}
// ============================================================================
