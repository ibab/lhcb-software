// ============================================================================
// Include files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "Event/CaloHypo.h"
#include "CaloSCorrection.h"

/** @file 
 *  Implementation file for class : CaloSCorrection
 *  
 *  @date 2003-03-10 
 *  @author Xxxx XXXXX xxx@xxx.com 
 */

DECLARE_TOOL_FACTORY( CaloSCorrection )

// ============================================================================
/** Standard constructor
 *  @see GaudiTool 
 *  @see  AlgTool 
 *  @param type tool type (?)
 *  @param name tool name 
 *  @param parent  tool parent 
 */
// ============================================================================
CaloSCorrection::CaloSCorrection( const std::string& type   , 
                                  const std::string& name   ,
                                  const IInterface*  parent ) 
  : CaloCorrectionBase( type , name , parent ){


  // define conditionName
  const std::string uName ( LHCb::CaloAlgUtils::toUpper( name ) ) ;
  if( uName.find( "ELECTRON" ) != std::string::npos  ){
    m_conditionName = "Conditions/Reco/Calo/ElectronSCorrection";
  }else if ( uName.find( "MERGED" )  != std::string::npos   ||  uName.find( "SPLITPHOTON" )  != std::string::npos ){
    m_conditionName = "Conditions/Reco/Calo/SplitPhotonSCorrection";
  }  
  else if (  uName.find( "PHOTON" ) ){
    m_conditionName = "Conditions/Reco/Calo/PhotonSCorrection"; 
  }

  declareInterface<ICaloHypoTool> ( this ) ;
}
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
CaloSCorrection::~CaloSCorrection () {}
// ============================================================================

StatusCode CaloSCorrection::finalize   () 
{
  m_hypos.clear();
  /// finalize the base class 
  return CaloCorrectionBase::finalize () ;
}
// ============================================================================

StatusCode CaloSCorrection::initialize (){
  /// first initialize the base class 
  StatusCode sc = CaloCorrectionBase::initialize();
  if( sc.isFailure() ) 
    { return Error ( "Unable initialize the base class CaloCorrectionBase!" , sc ) ; }  
  return StatusCode::SUCCESS ;
}
// ============================================================================

StatusCode CaloSCorrection::operator() ( LHCb::CaloHypo* hypo  ) const{ 
return process( hypo ); 
}
// ============================================================================

// ============================================================================
StatusCode CaloSCorrection::process    ( LHCb::CaloHypo* hypo  ) const{

  // check arguments 
  if( 0 == hypo )return Warning( " CaloHypo* points to NULL!",StatusCode::SUCCESS ) ; 


  // check the Hypo
  Hypotheses::const_iterator h = 
    std::find( m_hypos.begin() , m_hypos.end() , hypo->hypothesis() ) ;
  if( m_hypos.end() == h ) { 
    return Error ( "Invalid hypothesis!" ) ; 
  }

 // get cluster  (special case for SplitPhotons)
  const LHCb::CaloCluster* GlobalCluster = LHCb::CaloAlgUtils::ClusterFromHypo(hypo, false);
  const LHCb::CaloCluster* MainCluster = LHCb::CaloAlgUtils::ClusterFromHypo(hypo,true) ;


  /*
    Cluster information (e/x/y  and Prs/Spd digit)
  */
  if( 0 == MainCluster )return Warning ( "CaloCLuster* points to NULL -> no correction applied" , StatusCode::SUCCESS) ;

  // get Prs/Spd
  double ePrs = 0 ;
  double eSpd = 0 ;
  getPrsSpd(hypo,ePrs,eSpd);
  // For Split Photon - share the Prs energy
  if(  LHCb::CaloHypo::PhotonFromMergedPi0 == hypo->hypothesis() ){
    ePrs *= MainCluster->position().e()/GlobalCluster->position().e() ;
  }



  // Get position
  const LHCb::CaloPosition& position = MainCluster->position();
  const double xBar  = position. x () ;
  const double yBar  = position. y () ;


  //  Informations from seed Digit Seed ID & position
  const LHCb::CaloCluster::Entries& entries = MainCluster->entries();
  LHCb::CaloCluster::Entries::const_iterator iseed =
    LHCb::ClusterFunctors::locateDigit ( entries.begin () ,entries.end   () ,LHCb::CaloDigitStatus::SeedCell );
  if( entries.end() == iseed )return Warning( "The seed cell is not found -> no correction applied",StatusCode::SUCCESS ) ; 

  // get the "area" of the cluster (where seed is) 
  const LHCb::CaloDigit*  seed    = iseed->digit();
  if( 0 == seed )return Warning ( "Seed digit points to NULL -> no correction applied",StatusCode::SUCCESS ) ;

  // Cell ID for seed digit 
  LHCb::CaloCellID cellID = seed->cellID() ;
  Gaudi::XYZPoint seedPos = m_det->cellCenter( cellID  );

  
  /** here all information is available 
   *     
   *  (1) Ecal energy in 3x3     :    eEcal 
   *  (2) Prs and Spd energies   :    ePrs, eSpd 
   *  (3) weighted barycenter    :    xBar, yBar 
   *  (4) Zone/Area in Ecal      :    area   
   *  (5) SEED digit             :    seed   (NOT FOR SPLITPHOTONS !!)
   *  (6) CellID of seed digit   :    cellID
   *  (7) Position of seed cell  :    seedPos 
   */

  double CellSize =  m_det->cellSize( cellID  );
  double Asx   = - ( xBar - seedPos.x() ) / CellSize ;
  double Asy   = - ( yBar - seedPos.y() ) / CellSize ;


  Asx = getCorrection(CaloCorrection::shapeX , cellID , Asx , Asx );
  Asy = getCorrection(CaloCorrection::shapeY , cellID , Asy , Asy );
  double dcX = getCorrection(CaloCorrection::residual  , cellID , Asx , 0.);
  double dcY = getCorrection(CaloCorrection::residual  , cellID , Asy , 0.);
  Asx -= dcX;
  Asy -= dcY;
  double ddcX = (xBar < 0 ) ? 
    getCorrection(CaloCorrection::asymM , cellID , Asx , 0.): 
    getCorrection(CaloCorrection::asymP , cellID , Asx , 0.);
  double ddcY = (yBar < 0 ) ?
    getCorrection(CaloCorrection::asymM , cellID , Asy , 0.):
    getCorrection(CaloCorrection::asymP , cellID , Asy , 0.);
  Asx += ddcX;
  Asy += ddcY;  

  // Recompute position and fill CaloPosition
  double xCor = seedPos.x() - Asx * CellSize;
  double yCor = seedPos.y() - Asy * CellSize;

  const LHCb::CaloPosition* pos = hypo->position() ;


  if ( msgLevel( MSG::DEBUG) ){  
    debug() << "Calo Hypothesis :" << hypo->hypothesis() << endmsg;
    debug() << "cellID          : " << cellID << endmsg;
    debug() << "Hypo E :  " << hypo->position ()->e()   <<  " "  <<cellID << endmsg;
    debug() << "xBar/yBar " << xBar  <<  "/" << yBar   <<  endmsg;
    debug() << "xg/yg  "      << pos->x() << "/" << pos->y() <<  endmsg;
    debug() << "xNew/yNew "   << xCor <<  "/" << yCor    <<  endmsg;
    debug() << "xcel/ycel "   << seedPos.x() <<  "/" << seedPos.y() << endmsg ;
  }
  
  // update position
  LHCb::CaloPosition::Parameters& parameters = hypo ->position() ->parameters () ;
  parameters ( LHCb::CaloPosition::X ) = xCor ;
  parameters ( LHCb::CaloPosition::Y ) = yCor ;
  //  CaloPosition::Covariance& covariance = hypo ->position() ->covariance () ;

  return StatusCode::SUCCESS ;

}
// ============================================================================


