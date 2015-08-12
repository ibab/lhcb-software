// ============================================================================
// Include files
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
  // finalize the base class 
  return CaloCorrectionBase::finalize () ;
}
// ============================================================================

StatusCode CaloSCorrection::initialize (){
  // first initialize the base class 
  StatusCode sc = CaloCorrectionBase::initialize();
  if( sc.isFailure() ) 
    { return Error ( "Unable initialize the base class CaloCorrectionBase!" , sc ) ; }  
  if (UNLIKELY( msgLevel( MSG::DEBUG)) )
    debug() << "Condition name : " << m_conditionName << endmsg;

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
  if( m_hypos.end() == h )return Error ( "Invalid hypothesis!",StatusCode::SUCCESS ) ;  

  // No correction for negative energy :
  if( hypo->e() < 0.){
    counter("Skip negative energy correction") += 1;
    return StatusCode::SUCCESS;
  }

  // get cluster  (special case for SplitPhotons)
  // const LHCb::CaloCluster* GlobalCluster = LHCb::CaloAlgUtils::ClusterFromHypo(hypo, false); // not used
  const LHCb::CaloCluster* MainCluster = LHCb::CaloAlgUtils::ClusterFromHypo(hypo,true) ;


  /*
    Cluster information (e/x/y  and Prs/Spd digit)
  */
  if( 0 == MainCluster )return Warning ( "CaloCLuster* points to NULL -> no correction applied" , StatusCode::SUCCESS) ;

  // ePrs, eSpd currently not used here
  //
  // // get Prs/Spd
  // double ePrs = 0 ;
  // double eSpd = 0 ;
  // getPrsSpd(hypo,ePrs,eSpd);
  // // For Split Photon - share the Prs energy
  // if(  LHCb::CaloHypo::PhotonFromMergedPi0 == hypo->hypothesis() ){
  //   ePrs *= MainCluster->position().e()/GlobalCluster->position().e() ;
  // }

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

  // int area = cellID.area(); // currently not used
 
 

  // Somewhat inelegant way of data sharing between this const method process() and calcSCorrection(). Use of private fields
  // of this CaloSCorrection class for the same purpose would break the constness of the process() interface.
  struct SCorrInputParams  params;           // input parameters: currently only cellID, seedPos and MainCluster->position.z
  struct SCorrOutputParams results = {0, 0}; // output parameters: just d(Xhypo)/d(Xcluster) and d(Yhypo)/d(Ycluster)

  // Cell ID for seed digit 
  params.cellID  = seed->cellID() ;
  params.seedPos = m_det->cellCenter( params.cellID  );
  params.z       = position. z () ;
  
  /** here all information is available 
   *     
   *  (1) Ecal energy in 3x3     :    eEcal       ( not used )
   *  (2) Prs and Spd energies   :    ePrs, eSpd  ( not used )
   *  (3) weighted barycenter    :    xBar, yBar 
   *  (4) Zone/Area in Ecal      :    area        ( not used )  
   *  (5) SEED digit             :    seed   (NOT FOR SPLITPHOTONS !!)
   *  (6) CellID of seed digit   :    cellID
   *  (7) Position of seed cell  :    seedPos 
   */


  double xCor, yCor; // corrected hypo position
  double xCor_x, xCor_y, yCor_x, yCor_y;


  calcSCorrection(xBar, yBar, xCor, yCor, params, &results);

  const double &dXhy_dXcl = results.dXhy_dXcl;
  const double &dYhy_dYcl = results.dYhy_dYcl;

  // protection against unphysical d(Xhypo)/d(Xcluster) == 0 or d(Yhypo)/d(Ycluster) == 0
  if ( fabs( dXhy_dXcl ) < 1e-10 ){
    warning() << "unphysical d(Xhypo)/d(Xcluster) = " << dXhy_dXcl << " reset to 1 as if Xhypo = Xcluster" << endmsg;
    const_cast<double &>( dXhy_dXcl ) = 1.;
  }
  if ( fabs( dYhy_dYcl ) < 1e-10 ){
    warning() << "unphysical d(Yhypo)/d(Ycluster) = " << dYhy_dYcl << " reset to 1 as if Yhypo = Ycluster" << endmsg;
    const_cast<double &>( dYhy_dYcl ) = 1.;
  }


  // numeric partial derivatives w.r.t. X and Y, necessary to check after any change to the S-corrections
  if ( UNLIKELY(msgLevel(MSG::DEBUG)) && m_correctCovariance ){  
    const double dx_rel(1.e-5); // dx ~ 0.1 mm for numeric derivative calculation

    debug() << " ---------- calculation of numeric derivative dXhypo/dXcluster follows -----------" << endmsg;
    calcSCorrection(xBar*(1+dx_rel), yBar,            xCor_x, yCor_x, params, NULL);

    debug() << " ---------- calculation of numeric derivative dYhypo/dYcluster follows -----------" << endmsg;
    calcSCorrection(xBar,            yBar*(1+dx_rel), xCor_y, yCor_y, params, NULL);

    const double dn_xCor_dx = (xCor_x-xCor)/xBar/dx_rel;
    const double dn_yCor_dx = (yCor_x-yCor)/xBar/dx_rel; // sanity test, should be 0
    const double dn_xCor_dy = (xCor_y-xCor)/yBar/dx_rel; // sanity test, should be 0
    const double dn_yCor_dy = (yCor_y-yCor)/yBar/dx_rel;


    if ( fabs((dXhy_dXcl-dn_xCor_dx)/dXhy_dXcl) > 0.02 || fabs((dYhy_dYcl-dn_yCor_dy)/dYhy_dYcl) > 0.02 
         || fabs( dn_yCor_dx ) > 1e-8 || fabs( dn_xCor_dy ) > 1e-7 )
      debug() << " SCorrection numerically-caluclated Jacobian differs (by > 2%) from analytically-calculated one" << endmsg;

    debug() << "================== Jacobian elements ============= " << endmsg;
    debug() << "  semi-analytic dXhy_dXcl = " << dXhy_dXcl << " numeric dn_xCor_dx = " << dn_xCor_dx << " dn_xCor_dy = " << dn_xCor_dy << endmsg;
    debug() << "  semi-analytic dYhy_dYcl = " << dYhy_dYcl << " numeric dn_yCor_dy = " << dn_yCor_dy << " dn_yCor_dx = " << dn_yCor_dx << endmsg;
  }

  const LHCb::CaloPosition* pos = hypo->position() ;

  if (UNLIKELY( msgLevel( MSG::DEBUG)) ){  
    debug() << "Calo Hypothesis :" << hypo->hypothesis() << endmsg;
    debug() << "cellID          : " << params.cellID << endmsg;
    debug() << "Hypo E :  " << hypo->position ()->e()   <<  " "  << params.cellID << endmsg;
    debug() << "xBar/yBar " << xBar  <<  "/" << yBar   <<  endmsg;
    debug() << "xg/yg  "      << pos->x() << "/" << pos->y() <<  endmsg;
    debug() << "xNew/yNew "   << xCor <<  "/" << yCor    <<  endmsg;
    debug() << "xcel/ycel "   << params.seedPos.x() <<  "/" << params.seedPos.y() << endmsg ;
  }
  
  // update position
  LHCb::CaloPosition::Parameters& parameters = hypo ->position() ->parameters () ;
  parameters ( LHCb::CaloPosition::X ) = xCor ;
  parameters ( LHCb::CaloPosition::Y ) = yCor ;


  // update cov.m.: error propagation due to the S-correction
  if ( m_correctCovariance ){
    LHCb::CaloPosition::Covariance& covariance = hypo ->position() ->covariance () ;

    if ( UNLIKELY(msgLevel( MSG::DEBUG)) ){
      debug() << "before s-cor cov.m. = \n" << covariance << endmsg;
    }

    // cov.m packing in double array[5] following ROOT::Math::SMatrix<double,3,3>::Array()
    // for row/column indices (X:0, Y:1, E:2), see comments in CaloECorrection::process()
    double c1[6];

    c1[0] = covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::X); // arr[0] not relying on LHCb::CaloPosition::X == 0
    c1[2] = covariance(LHCb::CaloPosition::Y, LHCb::CaloPosition::Y); // arr[2] not relying on LHCb::CaloPosition::Y == 1
    c1[5] = covariance(LHCb::CaloPosition::E, LHCb::CaloPosition::E); // arr[5] not relying on LHCb::CaloPosition::E == 2
    c1[1] = covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::Y); // arr[1]
    c1[3] = covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::E); // arr[3]
    c1[4] = covariance(LHCb::CaloPosition::Y, LHCb::CaloPosition::E); // arr[4]

    // cov1 = (J * cov0 * J^T) for the special case of diagonal Jacobian for (X,Y,E) -> (X1=X1(X), Y1=Y1(Y), E1=E)
    c1[0] *= dXhy_dXcl * dXhy_dXcl;
    c1[1] *= dXhy_dXcl * dYhy_dYcl;
    c1[2] *= dYhy_dYcl * dYhy_dYcl;
    c1[3] *= dXhy_dXcl;
    c1[4] *= dYhy_dYcl;
    // c1[5] remains unchanged (energy is not chaged by S-correction)


    // alternatively, a code fragment for a general-form Jacobian (cf. a similar comment in CaloECorrection::process())
    // TMatrixD jac(3, 3); // just a diagonal Jacobian in case of (X,Y,E) -> (X1(X), Y1(Y), E) transformation
    // jac(0,0) = dXhy_dXcl;
    // jac(1,1) = dYhy_dYcl;
    // jac(2,2) = 1.;
    // if ( msgLevel( MSG::DEBUG) ){ debug() << "s-cor jacobian = " << endmsg; jac.Print(); }
    // TMarixDSym cov0(3) = ...          // to be initilized from hypo->position()->covariance()
    // TMarixDSym cov1(3);               // resulting extrapolated cov.m.
    // recalculate_cov(jac, cov0, cov1); // calculate:  cov1 = (J * cov0 * J^T)


    // finally update CaloHypo::position()->covariance()
    covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::X) = c1[0]; // cov1(0,0);
    covariance(LHCb::CaloPosition::Y, LHCb::CaloPosition::Y) = c1[2]; // cov1(1,1);
    covariance(LHCb::CaloPosition::E, LHCb::CaloPosition::E) = c1[5]; // cov1(2,2);
    covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::Y) = c1[1]; // cov1(0,1);
    covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::E) = c1[3]; // cov1(0,2);
    covariance(LHCb::CaloPosition::Y, LHCb::CaloPosition::E) = c1[4]; // cov1(1,2);

    // // DG: my little paranoia, should be always ok since Covariance is SMatrix<3,3,double> internally represented as double array[5]
    // assert( covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::Y) == covariance(LHCb::CaloPosition::Y, LHCb::CaloPosition::X));
    // assert( covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::E) == covariance(LHCb::CaloPosition::E, LHCb::CaloPosition::X));
    // assert( covariance(LHCb::CaloPosition::Y, LHCb::CaloPosition::E) == covariance(LHCb::CaloPosition::E, LHCb::CaloPosition::Y));

    if ( UNLIKELY(msgLevel( MSG::DEBUG)) ){
      debug() << "after s-cor cov.m. = \n" << covariance << endmsg;
    }
  }


  return StatusCode::SUCCESS ;

}
// ============================================================================

void CaloSCorrection::calcSCorrection( double  xBar, double  yBar, double &xCor, double &yCor,
                                       const struct SCorrInputParams                  &params,
                                       struct SCorrOutputParams                      *results ) const
{
  // declare local short aliases for the input variables passed from process() to calcSCorrection()
  const LHCb::CaloCellID &cellID  = params.cellID;
  const Gaudi::XYZPoint  &seedPos = params.seedPos;
  const double           &z       = params.z;


  double CellSize =  m_det->cellSize( cellID  );
  double Asx   = - ( xBar - seedPos.x() ) / CellSize ;
  double Asy   = - ( yBar - seedPos.y() ) / CellSize ;

  // cash intermediate values
  const double Asx0 = Asx;
  const double Asy0 = Asy;


  // Sshape correction :
  Asx = getCorrection(CaloCorrection::shapeX , cellID , Asx , Asx ); // Asx1 
  Asy = getCorrection(CaloCorrection::shapeY , cellID , Asy , Asy ); // Asy1

  // // cash intermediate values: necessary for debugging by calculating numeric derivatives dn_shapeX, dn_shapeY below
  // const double Asx1 = Asx;
  // const double Asy1 = Asy;


  // Angular correction (if any) [ NEW  - inserted between Sshape and residual correction ]
  double xs = seedPos.x() - Asx * CellSize; // xscor
  double ys = seedPos.y() - Asy * CellSize; // yscor
  double thx = atan( xs / z);
  double thy = atan( ys / z);
  double daX = getCorrection(CaloCorrection::angularX   , cellID , thx , 0.);
  double daY = getCorrection(CaloCorrection::angularY   , cellID , thy , 0.);
  Asx -= daX;
  Asy -= daY;

  // cash intermediate values
  const double Asx2 = Asx;
  const double Asy2 = Asy;


  // residual correction (if any):
  bool residualX_flag = false;
  double dcX = getCorrection(CaloCorrection::residual  , cellID , Asx , 0.);
  if( dcX == 0.){
    dcX = getCorrection(CaloCorrection::residualX  , cellID , Asx , 0.); // check X-specific correction
    residualX_flag = true;
  }
  bool residualY_flag = false;
  double dcY = getCorrection(CaloCorrection::residual  , cellID , Asy , 0.);
  if( dcY == 0.){
    dcY = getCorrection(CaloCorrection::residualY  , cellID , Asy , 0.); // check Y-specific correction
    residualY_flag = true;
  }
  Asx -= dcX;
  Asy -= dcY;

  // cash intermediate values
  const double Asx3 = Asx;
  const double Asy3 = Asy;


  // left/right - up/down asymmetries correction (if any) :
  double ddcX = (xBar < 0 ) ? 
    getCorrection(CaloCorrection::asymM , cellID , Asx , 0.): 
    getCorrection(CaloCorrection::asymP , cellID , Asx , 0.);
  double ddcY = (yBar < 0 ) ?
    getCorrection(CaloCorrection::asymM , cellID , Asy , 0.):
    getCorrection(CaloCorrection::asymP , cellID , Asy , 0.);
  Asx += ddcX;  // Asx4
  Asy += ddcY;  // Asy4


  // Recompute position and fill CaloPosition
  xCor = seedPos.x() - Asx * CellSize;
  yCor = seedPos.y() - Asy * CellSize;



/* DG,20140714: derivative calculation for  d(Xhypo)/d(Xcluster)
 *
 * Asx0 =-(xBar - seedPos.x)/CellSize; // xBar = Xcluster
 * Asx1 = shapeX(Asx0)
 * xs   = seedPos.x - Asx1*CellSize
 * thx  = atan(xs/z); // in principle, this brings in an implicit dependence on cluster E, but it's logarithmic so let's neglect it
 * daX  = angular(thx)
 * Asx2 = Asx1 - daX
 * dcX  = residual(Asx2) != 0 ? residual(Asx2) : residualX(Asx2); // add an auxiliary bool residualX_flag
 * Asx3 = Asx2 - dcX
 * ddcX = asym(Asx3)
 * Asx4 = Asx3 + ddcX = Asx
 * Xhypo= xCor(Asx4)  = seedPos.x - Asx4*CellSize
 *
 * d(Xhypo)/d(Xcluster) = d(xCor)/d(Asx4) * product[ d(Asx%i)/d(Asx%{i-1}), for i=1..4 ] * d(Asx0)/d(Xcluster)
 *
 * d(xCor)/d(Asx4)      =-CellSize
 * d(Asx0)/d(Xcluster)  = d(Asx0)/d(xBar)     = -1/CellSize
 * d(Asx1)/d(Asx0)      = DshapeX(Asx0)
 * d(thx)/d(Asx1)       = d(thx)/d(xs) * d(xs)/d(Asx1) =-CellSize/(1+(xc/z)**2)*(1/z)
 * d(xs)/d(Asx1)        =-CellSize
 * d(Asx2)/d(Asx1)      = 1 - d(daX)/d(Asx1)  = 1 - Dangular(thx)*d(thx)/d(Asx1) = 1 + Dangular(thx)*CellSize/z/(1+(xs/z)**2)
 * d(Asx3)/d(Asx2)      = 1 - d(dcX)/d(Asx2)  = 1 - ( residual(Asx2) != 0 ? Dresidual(Asx2) : DresidualX(Asx2) ) 
 * residualX_flag       = residual(Asx2) != 0 ? false : true
 * d(Asx4)/d(Asx3)      = 1 + d(ddcX)/d(Asx3) = 1 + Dasym(Asx3)
 *
 *
 * d(Xhypo)/d(Xcluster) = (1 + Dasym(Asx3)) * (1 - (resudualX_flag ? DresidualX(Asx2) : Dresidual(Asx2)))
 *                       *(1 + Dangular(thx)*CellSize/z/(1+(xs/z)**2)) * DshapeX(Asx0)
 */

  if ( m_correctCovariance && results ){
    // // ---- calculation of numeric derivatives of individual correction functions, important for debugging in case of code changes ---
    // debug() << "---------- numeric derivatives of individual S-correction functions ---------------" << endmsg;
    // double tmpd = ( fabs(Asx0) > 1.e-5 ) ? Asx0*2.e-2 : 2.e-7;
    // double dn_shapeX    = ( getCorrection(CaloCorrection::shapeX, cellID, Asx0 + tmpd, Asx0 + tmpd) - Asx1 )/tmpd;
    // tmpd = ( fabs(Asy0) > 1.e-5 ) ? Asy0*2.e-2 : 2.e-7;
    // double dn_shapeY    = ( getCorrection(CaloCorrection::shapeY, cellID, Asy0 + tmpd, Asy0 + tmpd) - Asy1 )/tmpd;
    // 
    // double dn_angularX  = ( getCorrection(CaloCorrection::angularX, cellID, thx*1.002, 0.) - daX )/thx/2e-3;
    // double dn_angularY  = ( getCorrection(CaloCorrection::angularY, cellID, thy*1.002, 0.) - daY )/thy/2e-3;
    // 
    // tmpd = ( fabs(Asx2) > 1.e-5 ) ? Asx2*2.e-3 : 2.e-8;
    // double dn_residualX = ( getCorrection((residualX_flag ? CaloCorrection::residualX : CaloCorrection::residual),
    //                                                                                          cellID, Asx2 + tmpd, 0.) - dcX )/tmpd;
    // tmpd = ( fabs(Asy2) > 1.e-5 ) ? Asy2*2.e-3 : 2.e-8;
    // double dn_residualY = ( getCorrection((residualY_flag ? CaloCorrection::residualY : CaloCorrection::residual),
    //                                                                                         cellID, Asy2 + tmpd, 0.) - dcY )/tmpd;
    // tmpd = ( fabs(Asx3) > 1.e-5 ) ? Asx3*2.e-3 : 2.e-8;
    // double dn_asymX     = (xBar < 0 ) ? 
    //   ( getCorrection(CaloCorrection::asymM , cellID , Asx2 + tmpd , 0.) - ddcX )/tmpd : 
    //   ( getCorrection(CaloCorrection::asymP , cellID , Asx2 + tmpd , 0.) - ddcX )/tmpd  ;
    // 
    // tmpd = ( fabs(Asy3) > 1.e-5 ) ? Asy3*2.e-3 : 2.e-8;
    // double dn_asymY     = (yBar < 0 ) ?
    //   ( getCorrection(CaloCorrection::asymM , cellID , Asy2 + tmpd , 0.) - ddcY )/tmpd :
    //   ( getCorrection(CaloCorrection::asymP , cellID , Asy2 + tmpd , 0.) - ddcY )/tmpd  ;
    // // -------------------------------------------------------------------------------------------------------------------------------


    // calculation of the analytic derivatives:
    // NB: printouts comparing analytic calculations with numeric derivatives which are commented-out below
    // are useful for debugging in case of changes in the correction function code
    if ( UNLIKELY(msgLevel( MSG::DEBUG)) )
      debug() << "---------- analytic derivatives of individual S-correction functions ---------------" << endmsg;

    double DshapeX = getCorrectionDerivative(CaloCorrection::shapeX , cellID , Asx0 , 1. );
    double DshapeY = getCorrectionDerivative(CaloCorrection::shapeY , cellID , Asy0 , 1. );

    // std::cout << "shapeX: Asx0 = " << Asx0 << " DshapeX = " << DshapeX << " dn_shapeX = " << dn_shapeX << std::endl;
    // std::cout << "shapeY: Asy0 = " << Asy0 << " DshapeY = " << DshapeY << " dn_shapeY = " << dn_shapeY << std::endl;

    double DangularX  = getCorrectionDerivative(CaloCorrection::angularX   , cellID , thx , 0.);
    double DangularY  = getCorrectionDerivative(CaloCorrection::angularY   , cellID , thy , 0.);

    // std::cout << "angularX: thx = " << thx << " DangularX = " << DangularX << " dn_angularX = " << dn_angularX << std::endl;
    // std::cout << "angularY: thy = " << thy << " DangularY = " << DangularY << " dn_angularY = " << dn_angularY << std::endl;

    double DresidualX = getCorrectionDerivative((residualX_flag ? CaloCorrection::residualX : CaloCorrection::residual), cellID , Asx2 , 0.);
    double DresidualY = getCorrectionDerivative((residualY_flag ? CaloCorrection::residualY : CaloCorrection::residual), cellID , Asy2 , 0.);

    // std::cout << " residual X: Asx2 = " << Asx2 << " residualX_flag = " << residualX_flag << " DresidualX = " << DresidualX
    //           << " dn_residualX = " << dn_residualX << std::endl;
    // std::cout << " residual Y: Asy2 = " << Asy2 << " residualY_flag = " << residualY_flag << " DresidualY = " << DresidualY
    //           << " dn_residualY = " << dn_residualY << std::endl;

    double DasymX = (xBar < 0 ) ? 
      getCorrectionDerivative(CaloCorrection::asymM , cellID , Asx3 , 0.): 
      getCorrectionDerivative(CaloCorrection::asymP , cellID , Asx3 , 0.);
    double DasymY = (yBar < 0 ) ?
      getCorrectionDerivative(CaloCorrection::asymM , cellID , Asy3 , 0.):
      getCorrectionDerivative(CaloCorrection::asymP , cellID , Asy3 , 0.);

    // std::cout << " asymX: xBar = " << xBar << " Asx3 = " << Asx3 << " DasymX = " << DasymX << " dn_asymX = " << dn_asymX << std::endl;
    // std::cout << " asymY: yBar = " << yBar << " Asy3 = " << Asy3 << " DasymY = " << DasymY << " dn_asymY = " << dn_asymY << std::endl;

    double tx = xs / z;
    double ty = ys / z;

    results->dXhy_dXcl = (1. + DasymX) * (1. - DresidualX) *(1. + DangularX*CellSize/z/(1.+tx*tx)) * DshapeX;
    results->dYhy_dYcl = (1. + DasymY) * (1. - DresidualY) *(1. + DangularY*CellSize/z/(1.+ty*ty)) * DshapeY;
  }
}
