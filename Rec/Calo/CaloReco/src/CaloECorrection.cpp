// ============================================================================
// Include files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "Event/CaloHypo.h"
#include "CaloECorrection.h"

/** @file 
 *  Implementation file for class : CaloECorrection
 *  
 *  @date 2003-03-10 
 *  @author Deschamps Olivier
 */

DECLARE_TOOL_FACTORY( CaloECorrection )

// ============================================================================
/*  Standard constructor
 *  @see GaudiTool 
 *  @see  AlgTool 
 *  @param type tool type (?)
 *  @param name tool name 
 *  @param parent  tool parent 
 */
// ============================================================================
CaloECorrection::CaloECorrection( const std::string& type   , 
                                  const std::string& name   ,
                                  const IInterface*  parent ) 
  : CaloCorrectionBase( type , name , parent ){

  declareProperty    ( "SpdFilter"    , m_sFilt = 0x3 ); // 1 : noSpd ; 2 : Spd ; 3: both
  declareProperty    ( "PrsFilter"    , m_pFilt = 0x3 ); // 1 : noPrs ; 2 : Prs ; 3: both

  // define conditionName
  const std::string uName ( LHCb::CaloAlgUtils::toUpper( name ) ) ;
  if( uName.find( "ELECTRON" ) != std::string::npos  ){
    m_conditionName = "Conditions/Reco/Calo/ElectronECorrection";
  }else if ( uName.find( "MERGED" )  != std::string::npos   ||  uName.find( "SPLITPHOTON" )  != std::string::npos ){
    m_conditionName = "Conditions/Reco/Calo/SplitPhotonECorrection";
  }  
  else if (  uName.find( "PHOTON" ) ){
    m_conditionName = "Conditions/Reco/Calo/PhotonECorrection"; 
  }
  
  declareInterface<ICaloHypoTool> ( this ) ;  
}
// ============================================================================
// destructor
// ============================================================================
CaloECorrection::~CaloECorrection () {} 
StatusCode CaloECorrection::finalize   (){
  m_hypos.clear();
  return CaloCorrectionBase::finalize () ;
}
// ============================================================================
StatusCode CaloECorrection::initialize (){
  /// first initialize the base class 
  StatusCode sc = CaloCorrectionBase::initialize();
  if( sc.isFailure() )return Error ( "Unable initialize the base class CaloCorrectionBase!" , sc ) ;
  return StatusCode::SUCCESS ;
}

// ============================================================================
StatusCode CaloECorrection::operator() ( LHCb::CaloHypo* hypo  ) const{ 
  return process( hypo ); 
}

// ============================================================================
StatusCode CaloECorrection::process    ( LHCb::CaloHypo* hypo  ) const{
  
  // check arguments 
  if( 0 == hypo )return Warning( " CaloHypo* points to NULL!",StatusCode::SUCCESS ) ; 

  // check the Hypo
  Hypotheses::const_iterator h = std::find( m_hypos.begin() , m_hypos.end() , hypo->hypothesis() ) ;
  if( m_hypos.end() == h )
    return Error ( "Invalid hypothesis -> no correction applied" ,StatusCode::FAILURE) ;

  // get Prs/Spd
  double ePrs = 0 ;
  double eSpd = 0 ;
  getPrsSpd(hypo,ePrs,eSpd);
  
  if( eSpd == 0 && (m_sFilt & 0x1) == 0)return StatusCode::SUCCESS;
  if( eSpd >  0 && (m_sFilt & 0x2) == 0)return StatusCode::SUCCESS;
  if( ePrs == 0 && (m_pFilt & 0x1) == 0)return StatusCode::SUCCESS;
  if( ePrs >  0 && (m_pFilt & 0x2) == 0)return StatusCode::SUCCESS;
  if ( msgLevel( MSG::DEBUG) )
    debug() << " Accepted  spd/prs : " << (int) (eSpd > 0 )<< " / " << (int) (ePrs > 0) << endmsg;
  
  // get cluster  (special case for SplitPhotons)
  const LHCb::CaloCluster* GlobalCluster = LHCb::CaloAlgUtils::ClusterFromHypo(hypo, false);
  const LHCb::CaloCluster* MainCluster = LHCb::CaloAlgUtils::ClusterFromHypo(hypo,true) ;
  

  /*
    Position information (e/x/y )
  */
  if( 0 == MainCluster )return Warning ( "CaloCLuster* points to NULL -> no correction applied" , StatusCode::SUCCESS) ;
  
  // For Split Photon - share the Prs energy
  if(  LHCb::CaloHypo::PhotonFromMergedPi0 == hypo->hypothesis() ){
    ePrs *= MainCluster->position().e()/GlobalCluster->position().e() ;
  }

  // Get position
  const LHCb::CaloPosition& position = MainCluster->position();
  double eEcal = position. e () ;
  const double xBar  = position. x () ;
  const double yBar  = position. y () ;

  // seed ID & position
  const LHCb::CaloCluster::Entries& entries = MainCluster->entries();
  LHCb::CaloCluster::Entries::const_iterator iseed =
    LHCb::ClusterFunctors::locateDigit ( entries.begin () ,entries.end() ,LHCb::CaloDigitStatus::SeedCell );
  if( entries.end() == iseed )return Warning( "The seed cell is not found -> no correction applied",StatusCode::SUCCESS ) ; 

  // get the "area" of the cluster (where seed is) 
  const unsigned int area = m_area( MainCluster );
  const LHCb::CaloDigit*  seed    = iseed->digit();
  if( 0 == seed )return Warning ( "Seed digit points to NULL -> no correction applied",StatusCode::SUCCESS ) ;
  // Cell ID for seed digit 
  const LHCb::CaloCellID cellID = seed->cellID() ;
  const Gaudi::XYZPoint  seedPos = m_det->cellCenter( cellID  );

  //  incidence angle
  double dtheta = incidence( hypo ) - incidence(hypo, true);
  //  info() << "------> " << incidence( hypo ) << " " <<  incidence(hypo, true) << "  " << dtheta << endmsg;


  /** here all information is available 
   *     
   *  (1) Ecal energy in 3x3     :    eEcal 
   *  (2) Prs and Spd energies   :    ePrs, eSpd 
   *  (3) weighted barycenter    :    xBar, yBar 
   *  (4) Zone/Area in Ecal      :    area   
   *  (5) SEED digit             :    seed    (NO for split!)
   *  (6) CellID of seed digit   :    cellID  (OK for split!)
   *  (7) Position of seed cell  :    seedPos (OK for split!)
   *
   */

  const int ShiftCol[3] = {  0 ,  0 ,  8 };
  const int ShiftRow[3] = {  6 , 12 , 14 };
   

  /////////////////////////////////////////////////////////

  
  double CellSize =  m_det->cellSize( cellID  );
  double Asx   = ( xBar - seedPos.x() ) / CellSize ;
  double Asy   = ( yBar - seedPos.y() ) / CellSize ;
  unsigned int    Col   = cellID.col()  - ShiftCol[area] +1;
  unsigned int    Row   = cellID.row()  - ShiftRow[area] +1;


  double bDist = sqrt( Asx*Asx + Asy*Asy) * sqrt ( 2. ) ;

  // leakage induced by Ecal module frame 
  double signX = 0; 
  if( 1 == Col % (area+1) )  {signX  = +1.;} 
  if( 0 == Col % (area+1) )  {signX  = -1.;} 
  double signY = 0; 
  if( 1 == Row % (area+1) )  {signY  = +1.;} 
  if( 0 == Row % (area+1) )  {signY  = -1.;} 
  Asx *=  signX;
  Asy *=  signY;

  // Pileup subtraction at the cluster level
  if( m_pileup->method("Ecal") >= 10 ){
    bool spd = (eSpd>0) ? true:false;
    double offset = m_pileup->offset( cellID,spd );
    if(offset < eEcal){
      double eee=eEcal;
      eEcal -= offset;
      counter("Pileup offset") += offset;
      counter("Pileup subtracted ratio")+=eEcal/eee;
      counter("Pileup scale") += m_pileup->getScale();
    }
  }
  
  // apply corrections
  double aG  = getCorrection(CaloCorrection::alphaG    , cellID         );  // global Ecal factor
  double aE  = getCorrection(CaloCorrection::alphaE    , cellID , eEcal );  // longitudinal leakage
  double aB  = getCorrection(CaloCorrection::alphaB    , cellID , bDist );  // lateral leakage
  double aX  = getCorrection(CaloCorrection::alphaX    , cellID , Asx   );  // module frame dead material X-direction
  double aY  = getCorrection(CaloCorrection::alphaY    , cellID , Asy   );  // module frame dead material Y-direction
  double beta= getCorrection(CaloCorrection::beta      , cellID , eEcal , 0 );  
  double betaC= getCorrection(CaloCorrection::betaC      , cellID , eEcal , 0. );

  double gT  = getCorrection(CaloCorrection::globalT   , cellID , dtheta );  // incidence angle (delta)
  double dT  = getCorrection(CaloCorrection::offsetT   , cellID , dtheta );  // incidence angle (delta)


  double gC = 1.;
  if( eSpd > 0){
    gC = getCorrection(CaloCorrection::globalC , cellID ); // global correction factor for converted photons
    if( betaC != 0.)beta = betaC;
  }
  

  

  
  // Apply Ecal leakage corrections
  double alpha = aG * aE * aB * aX * aY  ;
  double eCor  = ( alpha * eEcal + beta *ePrs ) * gC * gT + dT;

  // revoir le debug
  if ( msgLevel( MSG::DEBUG) ){
    debug() << "Calo hypothesis : " << hypo->hypothesis() << endmsg;
    debug() << "cellID          : " << cellID << endmsg;
    debug() << "eSpd : "  << eSpd <<  " "    << "ePrs : "  << ePrs  <<  endmsg;
    debug() << "asx : "  << Asx <<  " " << "asy : "  << Asy  << endmsg;
    debug() << "alpha " << alpha << " = " << aG << " x " << aE << " x " << aB << " x " << aX << " x " << aY <<  endmsg;
    debug() << "beta"   << beta << endmsg;
    debug() << "Global Factor " << gC << endmsg;
    debug() << "Global theta correction " << gT << endmsg;
    debug() << "eEcal " << eEcal <<  " --> "    << "eCor "  <<  eCor    << endmsg;
  }
  
  counter("<alpha> " + cellID.areaName() ) += alpha;  
  counter("<beta*Eprs> " + cellID.areaName() ) += beta * ePrs;
  
  // update position
  LHCb::CaloPosition::Parameters& parameters = hypo ->position() ->parameters () ;
  parameters ( LHCb::CaloPosition::E ) = eCor ;
  counter("Corrected energy")+=eCor;
  //  CaloPosition::Covariance& covariance = hypo ->position() ->covariance () ;
  
  return StatusCode::SUCCESS ;
}


