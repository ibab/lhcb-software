// ============================================================================
// Include files
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
  // first initialize the base class 
  StatusCode sc = CaloCorrectionBase::initialize();
  if( sc.isFailure() )return Error ( "Unable initialize the base class CaloCorrectionBase!" , sc ) ;

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "Condition name : " << m_conditionName << endmsg;

  //info() << "  =========== Condition name : " << m_conditionName << endmsg;




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
  if( m_hypos.end() == h )return Error ( "Invalid hypothesis -> no correction applied" ,StatusCode::SUCCESS) ;

  // No correction for negative energy :
  if( hypo->e() < 0.){
    counter("Skip negative energy correction") += 1;
    return StatusCode::SUCCESS;
  }


  // Somewhat inelegant way of data sharing between this const method process() and calcECorrection(). Use of private fields
  // of this CaloECorrection class for the same purpose would break the constness of the process() interface.
  struct ECorrInputParams  _params; // input parameters calculated once in process() and used in all the calcECorrection() calls
  ECorrOutputParams       _results; // Jacobian elements plus many various intermediate parameters, used e.g. for debug printout

  // input variables passed from process() to calcECorrection()
  LHCb::CaloCellID &cellID  = _params.cellID;
  Gaudi::XYZPoint  &seedPos = _params.seedPos;
  double           &eSpd    = _params.eSpd;
  double           &dtheta  = _params.dtheta;
  unsigned int     &area    = _params.area;


  // get Prs/Spd
  double ePrs = 0 ;
  eSpd = 0 ;
  getPrsSpd(hypo,ePrs,eSpd);
  
  if( eSpd == 0 && (m_sFilt & 0x1) == 0)return StatusCode::SUCCESS;
  if( eSpd >  0 && (m_sFilt & 0x2) == 0)return StatusCode::SUCCESS;
  if( ePrs == 0 && (m_pFilt & 0x1) == 0)return StatusCode::SUCCESS;
  if( ePrs >  0 && (m_pFilt & 0x2) == 0)return StatusCode::SUCCESS;
  if ( UNLIKELY(msgLevel( MSG::DEBUG) ))
    debug() << " Accepted  spd/prs : " << (int) (eSpd > 0 )<< " / " << (int) (ePrs > 0) << endmsg;
  
  // get cluster  (special case for SplitPhotons)
  const LHCb::CaloCluster* GlobalCluster = LHCb::CaloAlgUtils::ClusterFromHypo(hypo, false);
  const LHCb::CaloCluster* MainCluster = LHCb::CaloAlgUtils::ClusterFromHypo(hypo,true) ;
  

  /*
    Position information (e/x/y )
  */
  if( 0 == MainCluster )return Warning ( "CaloCluster* points to NULL -> no correction applied" , StatusCode::SUCCESS) ;
  
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
  area = m_area( MainCluster );
  const LHCb::CaloDigit*  seed    = iseed->digit();
  if( 0 == seed )return Warning ( "Seed digit points to NULL -> no correction applied",StatusCode::SUCCESS ) ;
  // Cell ID for seed digit 
  cellID = seed->cellID() ;
  seedPos = m_det->cellCenter( cellID  );

  //  incidence angle
  dtheta = incidence( hypo ) - incidence(hypo, true);
  //  info() << "------> " << incidence( hypo ) << " " <<  incidence(hypo, true) << "  " << dtheta << endmsg;

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


  /////////////////////////////////////////////////////////


  /* Calculate corrected energy in a separate function call. Necessary for debugging the Jacobian by calculating
   * numeric derivatives w.r.t. (X, Y, E, [and possibly ePrs]) in case of any changes in the correction code.
   *
   * Input positions and energies are passed as parameters for ease of numeric derivative calculation,
   * all the other paramers and results of intermediate calculations are shared between the two methods
   * using local struct ECorrInputParams _params, and [zero-initialized] ECorrOutputParams _results.
   */
  double eCor = calcECorrection(xBar, yBar, eEcal, ePrs, _params, &_results);

  // results of semi-analytic derivative calculation
  const double &dEcor_dXcl = _results.dEcor_dXcl;
  const double &dEcor_dYcl = _results.dEcor_dYcl;
  const double &dEcor_dEcl = _results.dEcor_dEcl;

  // protection against unphysical d(Ehypo)/d(Ecluster) == 0
  if ( fabs( dEcor_dEcl ) < 1e-10 ){
    warning() << "unphysical d(Ehypo)/d(Ecluster) = " << dEcor_dEcl << " reset to 1 as if Ehypo = Ecluster" << endmsg;
    const_cast<double &>(dEcor_dEcl) = 1.;
  }

  // debugging necessary in case if any new corrections are added or their sequence is changed!
  if ( UNLIKELY(msgLevel( MSG::DEBUG)) && m_correctCovariance ){
    const double dx_rel(1e-5), dy_rel(1e-5), de_rel(1e-3); // dx,dy ~ few*0.1*mm, de ~ few MeV

    // calculate numeric derivatives to be compared with analytic ones
    debug() << "\n ------------------------ ECorrection(x+dx, y, e) calculation follows ------------------- " << endmsg;
    double eCor_x    = calcECorrection(xBar*(1+dx_rel), yBar,            eEcal,            ePrs, _params, NULL);

    debug() << "\n ------------------------ ECorrection(x, y+dy, e) calculation follows ------------------- " << endmsg;
    double eCor_y    = calcECorrection(xBar,            yBar*(1+dy_rel), eEcal,            ePrs, _params, NULL);

    debug() << "\n ------------------------ ECorrection(e, y, e+de) calculation follows ------------------- " << endmsg;
    double eCor_e    = calcECorrection(xBar,            yBar,            eEcal*(1+de_rel), ePrs, _params, NULL);


    double dn_eCor_dx = (eCor_x-eCor)/xBar /dx_rel;
    double dn_eCor_dy = (eCor_y-eCor)/yBar /dy_rel;
    double dn_eCor_de = (eCor_e-eCor)/eEcal/de_rel;

    // avoid division in comparison for possible dE/dX == 0 or dE/dY == 0
    if (   fabs(dEcor_dXcl - dn_eCor_dx) > fabs(dEcor_dXcl) * 0.1
        || fabs(dEcor_dYcl - dn_eCor_dy) > fabs(dEcor_dYcl) * 0.1
        || fabs(dEcor_dEcl - dn_eCor_de) > fabs(dEcor_dEcl) * 0.1 )
    {
      debug() << " some CaloECorrection analytically-calculated Jacobian elements differ (by > 10%) from numerically-calculated ones! " << endmsg;
    }

    debug() << "**********" << " betaC_flag = " << _results.betaC_flag << " ******* Jacobian elements J(2,*) =" << endmsg;
    debug() << "   semi-analytic dEcor_dXcl = " << dEcor_dXcl << " numeric dn_eCor_dx = " << dn_eCor_dx << endmsg;
    debug() << "   semi-analytic dEcor_dYcl = " << dEcor_dYcl << " numeric dn_eCor_dy = " << dn_eCor_dy << endmsg;
    debug() << "   semi-analytic dEcor_dEcl = " << dEcor_dEcl << " numeric dn_eCor_de = " << dn_eCor_de << endmsg;
  }



  // revoir le debug
  if ( UNLIKELY(msgLevel( MSG::DEBUG) )){
    debug() << "Calo hypothesis : " << hypo->hypothesis() << endmsg;
    debug() << "cellID          : " << cellID << endmsg;
    debug() << "eSpd : "  << eSpd <<  " "    << "ePrs : "  << ePrs  <<  endmsg;
    debug() << "asx : "  << _results.Asx <<  " " << "asy : "  << _results.Asy  << endmsg;
    debug() << "alpha " << _results.alpha << " = " << _results.aG << " x " << _results.aE << " x " << _results.aB << " x " << _results.aX << " x " << _results.aY <<  endmsg;
    debug() << "beta "  << _results.beta << endmsg;
    debug() << "Global Factor " << _results.gC << endmsg;
    debug() << "Global theta correction " << _results.gT << endmsg;
    debug() << "eEcal " << eEcal <<  " --> "    << "eCor "  <<  eCor    << endmsg;
  }
  
  counter("<alpha> " + cellID.areaName() ) += _results.alpha;  
  counter("<beta*Eprs> " + cellID.areaName() ) += _results.beta * ePrs;
 
  // update position
  LHCb::CaloPosition::Parameters& parameters = hypo ->position() ->parameters () ;
  parameters ( LHCb::CaloPosition::E ) = eCor ;
  counter("Corrected energy")+=eCor;


  // ----------------------------------------- apply semi-analytic cov.m. propagation due to the (X,Y,E) corrections 
  if ( m_correctCovariance ){
    LHCb::CaloPosition::Covariance& covariance = hypo ->position() ->covariance () ;

    if ( UNLIKELY(msgLevel( MSG::DEBUG)) ){
      debug() << "before E-corr. cov.m. = \n" << covariance << endmsg;
    }

    // index numbering just follows ROOT::Math::SMatrix<double,3,3>::Array() for row/column indices (X:0, Y:1, E:2)
    double c0[6], c1[6];
    /* 
     * Indexing following ROOT::Math::SMatrix<double,3,3,ROOT::Math::MatRepSym<double,3> >::Array() :
     *
     * The iterators access the matrix element in the order how they are
     * stored in memory. The C (row-major) convention is used, and in the
     * case of symmetric matrices the iterator spans only the lower diagonal
     * block. For example for a symmetric 3x3 matrices the order of the 6
     * elements \f${a_0,...a_5}\f$ is:
     * \f[
     * M = \left( \begin{array}{ccc}
     *     a_0 & a_1 & a_3  \\
     *     a_1 & a_2  & a_4  \\
     *     a_3 & a_4 & a_5   \end{array} \right)
     * \f]
     */
    c0[0] = covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::X); // arr[0] not relying on LHCb::CaloPosition::X == 0
    c0[2] = covariance(LHCb::CaloPosition::Y, LHCb::CaloPosition::Y); // arr[2] not relying on LHCb::CaloPosition::Y == 1
    c0[5] = covariance(LHCb::CaloPosition::E, LHCb::CaloPosition::E); // arr[5] not relying on LHCb::CaloPosition::E == 2
    c0[1] = covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::Y); // arr[1]
    c0[3] = covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::E); // arr[3]
    c0[4] = covariance(LHCb::CaloPosition::Y, LHCb::CaloPosition::E); // arr[4]

    // cov1 = (J * cov0 * J^T) for the special case of Jacobian for (X,Y,E) -> (X1=X, Y1=Y, E1=E(X,Y,E))
    c1[0] = c0[0];
    c1[1] = c0[1];
    c1[2] = c0[2];
    c1[3] = c0[0]*dEcor_dXcl + c0[1]*dEcor_dYcl + c0[3]*dEcor_dEcl;
    c1[4] = c0[1]*dEcor_dXcl + c0[2]*dEcor_dYcl + c0[4]*dEcor_dEcl;
    double tmp = c0[3]*dEcor_dXcl + c0[4]*dEcor_dYcl + c0[5]*dEcor_dEcl;
    c1[5] = c1[3]*dEcor_dXcl + c1[4]*dEcor_dYcl + tmp*dEcor_dEcl;
                
    // additional protection against cov.m.(E,E) <= 0 due to numerical effects
    if (  c1[5] < 1.e-10 ){
      warning() << "unphysical variance(Ehypo) = " << c1[5] << " reset cov.m.(Ehypo,*) = cov.m.(Ecluster,*) as if Ehypo = Ecluster" << endmsg;
      c1[5] = c0[5]; c1[3] = c0[3]; c1[4] = c0[4];
    }

    // --------------------------------------------------------------------------- alternative calculation for a general-form Jacobian
    // /* typedef ROOT::Math::SMatrix<double, 3, 3, ROOT::Math::MatRepSym<double,3> >	Gaudi::SymMatrix3x3;
    //  * typedef Gaudi::SymMatrix3x3                                              	LHCb::CaloPosition::Covariance;
    //  *
    //  * calculations done with TMatrixDSym (for which TMatrixD.Mult() is defined) for simplicity instead of ROOT::Math::SMatrix<...> 
    //  * cov1 = J * cov0 * J^T
    //  */
    //
    // // Jacobian for (X,Y,E) -> (X1=X, Y1=Y, E1=E(X,Y,E))
    // TMatrixD jac(3, 3);
    // jac(0,0) = 1.;
    // jac(1,1) = 1.;
    // jac(2,0) = dEcor_dXcl;
    // jac(2,1) = dEcor_dYcl;
    // jac(2,2) = dEcor_dEcl;
    // // std::cout << "jacobian = " << std::endl; jac.Print();
    //
    // TMatrixDSym cov0( 3 ); // cov.m. is 3x3
    //
    // // use of SMatrix<>::Array() would have required less to type but assumes a particular storage sequence of the (X,Y,E) elements
    // cov0(0,0) =             covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::X); // c0[0] nomatter if LHCb::CaloPosition::X == 0
    // cov0(1,1) =             covariance(LHCb::CaloPosition::Y, LHCb::CaloPosition::Y); // c0[2] nomatter if LHCb::CaloPosition::Y == 1
    // cov0(2,2) =             covariance(LHCb::CaloPosition::E, LHCb::CaloPosition::E); // c0[5] nomatter if LHCb::CaloPosition::E == 2
    // cov0(1,0) = cov0(0,1) = covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::Y); // c0[1]
    // cov0(2,0) = cov0(0,2) = covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::E); // c0[3]
    // cov0(2,1) = cov0(1,2) = covariance(LHCb::CaloPosition::Y, LHCb::CaloPosition::E); // c0[4]
    //
    // // cov1 = J * cov *J^T  using  TMatrixD.Mult()  and  TMatrixDSym  for a general-form Jacobian J
    // TMatrixDSym cov1( cov0 );
    // recalculate_cov(jac, cov0, cov1);
    // 
    // // std::cout << "cov0 = " << std::endl; cov0.Print();
    // // std::cout << "cov1 = " << std::endl; cov1.Print();
    // -------------------------------------------------------------------------------------------------------------------------------

    // finally update CaloHypo::position()->covariance()
    covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::X) = c1[0]; // cov1(0,0);
    covariance(LHCb::CaloPosition::Y, LHCb::CaloPosition::Y) = c1[2]; // cov1(1,1);
    covariance(LHCb::CaloPosition::E, LHCb::CaloPosition::E) = c1[5]; // cov1(2,2);
    covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::Y) = c1[1]; // cov1(0,1);
    covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::E) = c1[3]; // cov1(0,2);
    covariance(LHCb::CaloPosition::Y, LHCb::CaloPosition::E) = c1[4]; // cov1(1,2);

    // // paranoic test (should be always ok since Covariance is SMatrix<3,3,double> with internal representation as double array[5])
    // assert( covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::Y) == covariance(LHCb::CaloPosition::Y, LHCb::CaloPosition::X));
    // assert( covariance(LHCb::CaloPosition::X, LHCb::CaloPosition::E) == covariance(LHCb::CaloPosition::E, LHCb::CaloPosition::X));
    // assert( covariance(LHCb::CaloPosition::Y, LHCb::CaloPosition::E) == covariance(LHCb::CaloPosition::E, LHCb::CaloPosition::Y));

    if ( UNLIKELY(msgLevel( MSG::DEBUG)) ){
      debug() << "after E-corr. cov.m. = \n" << covariance << endmsg;
    }
  }
  
  return StatusCode::SUCCESS ;
}


double CaloECorrection::calcECorrection( double xBar, double yBar, double eEcal, double ePrs,
                                         const struct CaloECorrection::ECorrInputParams &_params,
                                         CaloECorrection::ECorrOutputParams             *_results ) const
{
  // local aliases for the input variables passed from process() to calcECorrection()
  const LHCb::CaloCellID &cellID  = _params.cellID;
  const Gaudi::XYZPoint  &seedPos = _params.seedPos;
  const double           &eSpd    = _params.eSpd;
  const double           &dtheta  = _params.dtheta;
  const unsigned int     &area    = _params.area;

  const int ShiftCol[3] = {  0 ,  0 ,  8 };
  const int ShiftRow[3] = {  6 , 12 , 14 };
   
  double CellSize =  m_det->cellSize( cellID  );
  double Asx   = ( xBar - seedPos.x() ) / CellSize ; // Asx0
  double Asy   = ( yBar - seedPos.y() ) / CellSize ; // Asy0

  const double Asx0 = Asx;
  const double Asy0 = Asy;

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
  Asx *=  signX; // Asx1
  Asy *=  signY; // Asy1


  // analytic derivatives of the correction functions
  double DaE(0), DaB(0), DaX(0), DaY(0), Dbeta(0), DbetaPR(0), DbetaC(0), DbetaCPR(0);

  //
  // apply corrections
  // NB: numeric derivative calculation calls and printouts which are commented-out below
  // are useful for debugging in case of changes in the correction function code
  //
  //// aG = const(X,Y,E), no need to calculate derivatives
  double aG  = getCorrection(CaloCorrection::alphaG    , cellID         );  // global Ecal factor
 
  //// aE = alphaE(eEcal)
  double aE  = getCorrection(CaloCorrection::alphaE    , cellID , eEcal );  // longitudinal leakage
  if ( m_correctCovariance ){
    if ( _results ) DaE      = getCorrectionDerivative(CaloCorrection::alphaE    , cellID , eEcal ); 
    // double dn_aE  = (getCorrection(CaloCorrection::alphaE    , cellID , eEcal*1.02 )-aE)/eEcal/2.e-2;
    // std::cout << " alphaE: eEcal = " << eEcal << " aE = " << aE << " DaE = " << DaE
    //           << " numeric d(aE)/d(eEcal) = dn_aE = " << dn_aE  << std::endl;
  }

  //// aB = alphaB(bDist)
  double aB  = getCorrection(CaloCorrection::alphaB    , cellID , bDist );  // lateral leakage
  if ( m_correctCovariance ){
    if ( _results ) DaB      = getCorrectionDerivative(CaloCorrection::alphaB    , cellID , bDist );
    // double tmpd = ( bDist > 1e-5 ) ? bDist*2.e-2 : 2.e-7;
    // double dn_aB = (getCorrection(CaloCorrection::alphaB    , cellID , bDist + tmpd )-aB)/tmpd;
    // std::cout << "\t alphaB: bDist = " << bDist << " aB = " << aB << " DaB = " << DaB
    //           << " numeric  d(aB)/d(bDist) = dn_aB = " << dn_aB << std::endl;
  }

  //// aX = alphaX(Asx1)
  double aX  = getCorrection(CaloCorrection::alphaX    , cellID , Asx   );  // module frame dead material X-direction
  if ( m_correctCovariance ){
    if ( _results ) DaX  = getCorrectionDerivative(CaloCorrection::alphaX    , cellID , Asx   );
    // double tmpd = ( fabs(Asx) > 1e-5 ) ? Asx*2.e-2 : 2.e-7;
    // double dn_aX  = (getCorrection(CaloCorrection::alphaX    , cellID , Asx + tmpd  ) -aX)/ tmpd;
    // std::cout << "\t alphaX Asx = " << Asx << " aX = " << aX << " DaX = " << DaX
    //           << " numeric d(aX)/d(Asx1) = dn_aX = " << dn_aX << std::endl;
  }

  //// aY = alphaY(Asy1)
  double aY  = getCorrection(CaloCorrection::alphaY    , cellID , Asy   );  // module frame dead material Y-direction
  if ( m_correctCovariance ){
    if ( _results ) DaY  = getCorrectionDerivative(CaloCorrection::alphaY    , cellID , Asy   ); 
    // double tmpd = ( fabs(Asy) > 1e-5 ) ? Asy*2.e-2 : 2.e-7;
    // double dn_aY  = (getCorrection(CaloCorrection::alphaY    , cellID , Asy + tmpd  ) -aY)/ tmpd;
    // std::cout << "\t alphaY: Asy = " << Asy << " aY = " << aY << " DaY = " << DaY
    //           << " numeric d(aY)/d(Asy1) = dn_aY = " << dn_aY  << std::endl;
  }

  //// alphaP(ePrs) = const(X,Y,E), no need to calculate derivatives
  //// unless a full 4D cov.m.(X,Y,E,ePrs) is used including sigma(ePrs) and ePrs correlations with X,Y,E
  double aP  = getCorrection(CaloCorrection::alphaP    , cellID , ePrs  );  // Prs deposit dependency


  // Prs correction
  double ratio = (eEcal > 0 ) ? ePrs / eEcal : 0. ;
  //// beta0 = beta(eEcal)
  double beta = getCorrection(CaloCorrection::beta , cellID , eEcal  , 0. );    // eEcal dependency if any
  if ( m_correctCovariance ){
    if ( _results ) Dbeta = getCorrectionDerivative(CaloCorrection::beta , cellID , eEcal , 0 );
    // double dn_beta = (getCorrection(CaloCorrection::beta       , cellID , eEcal*1.02 , 0. )-beta)/eEcal/2.e-2;
    // std::cout << "\t beta: eEcal = " << eEcal << " beta = " << beta << " Dbeta = " << Dbeta
    //           << " numeric d(beta)/d(eEcal) = dn_beta = " << dn_beta  << std::endl;
  }

  double betaP = getCorrection(CaloCorrection::betaP      , cellID , ePrs   , 0. );    // add Eprs dependency if any
  beta += betaP; // beta1
  //// betaPR(ratio = ePrs/eEcal)
  double betaPR = getCorrection(CaloCorrection::betaPR     , cellID , ratio  , 0. );    // add ePrs/eEcal dependency if any
  if ( m_correctCovariance ){
    if ( _results ) DbetaPR = getCorrectionDerivative(CaloCorrection::betaPR     , cellID , ratio  , 0. );
  }
  beta += betaPR; // beta2


  // angular correction
  // assume dtheta to be independent of X,Y,E, although it may still be implicitly a bit dependent on X,Y
  double gT  = getCorrection(CaloCorrection::globalT   , cellID , dtheta );  // incidence angle (delta)
  double dT  = getCorrection(CaloCorrection::offsetT   , cellID , dtheta , 0. );  // incidence angle (delta)

 
  // Energy offset
  double sinT  =  m_det->cellSine( cellID );
  double offset = ( eSpd == 0) ?
    getCorrection(CaloCorrection::offset, cellID , sinT , 0.) :
    getCorrection(CaloCorrection::offsetC, cellID , sinT , 0.) ;  

  // - dedicate correction for 'converted photon'
  double gC = 1.;
  bool betaC_flag = false;
  if( eSpd > 0){
    gC = getCorrection(CaloCorrection::globalC , cellID ); // global correction factor for converted photons
    //// betaC0 = betaC(eEcal)
    double betaC= getCorrection(CaloCorrection::betaC      , cellID , eEcal , 0.  );
    if ( m_correctCovariance ){
      if ( _results ) DbetaC = getCorrectionDerivative(CaloCorrection::betaC      , cellID , eEcal , 0.  );
    }

    //// betaCP = betaCP(ePrs) = const(X,Y,E) => no need to calculate derivaties unless 4D cov.m.(X,Y,E,ePrs) is used
    double betaCP = getCorrection(CaloCorrection::betaCP          , cellID , ePrs  , 0.  );
    betaC += betaCP; // betaC1

    //// betaCPR(ratio = ePrs/eEcal)
    double betaCPR = getCorrection(CaloCorrection::betaCPR         , cellID , ratio , 0.  );
    if ( m_correctCovariance ){
      if ( _results ) DbetaCPR = getCorrectionDerivative(CaloCorrection::betaCPR         , cellID , ratio , 0.  );
    }
    betaC += betaCPR; // betaC2

    if( betaC != 0.){ beta = betaC; betaC_flag = true; }
  }
  

  
  // Apply Ecal leakage corrections
  double alpha = aG * aE * aB * aX * aY  * aP;
  double eCor  = ( alpha * eEcal + beta *ePrs ) * gC * gT + dT + offset;



/* DG,20140714: derivative calculation
 *
 * Asx0  = (Xcluster-seedPos.x)/CellSize
 * bDist = sqrt(2)*sqrt(Asx0**2+Asy0**2)
 * signX = signX(cellID); // const(X,Y,Ecluster)
 * Asx1  = signX*Asx0
 * eEcal = Ecluster - pileup_offset(cellID, eSpd); // => d(eEcal)/d(Ecluster) = 1
 * aG    = alphaG(cellID); // const(X,Y, Ecluster)
 * aE    = alphaE(eEcal)
 * aB    = alphaB(bDist)
 * aX    = alphaX(Asx1)
 * aY    = alphaY(Asy1)
 * aP    = alphaP(ePrs); // const(X,Y, Ecluster)
 * ratio = ePrs/eEcal; //  if eEcal > 0, otherwise = 0
 * beta0 = beta(eEcal)
 * beta1 = beta0 + betaP(ePrs)
 * beta2 = beta1 + betaPR(ratio)
 * gT    = globalT(dtheta); // const(X,Y,Ecluster) although dtheta may indirectly depend on (X,Y)
 * dT    = offsetT(dtheta); // const(X,Y,Ecluster)
 * sinT  = cellSince(cellID); // const(X,Y,Ecluster)
 * offset= eSpd > 0 ? offsetC(cellID, sinT) : offset(cellID, sinT)
 * gC    = eSpd > 0 ? globalC(cellID) : 1; // const(X,Y,Ecluster)
 * betaC0= betaC(eEcal)
 * betaC1= betaC0_betaCP(ePrs)
 * betaC2= betaC1+betaCPR(ratio)
 *
 * betaQ2= betaC_flag ? betaC2 : beta2
 *
 * d(Asx0)/dX       = +1/CellSize
 * d(Asx1)/d(Asx0)  = signX
 * d(bDist)/d(Asx0) = sqrt(2)*2*Asx0/2/sqrt(Asx0**2+Asy0**2) = 2*Asx0/bDist; // if bDist != 0, otherwise 0
 *   if bDist=0 <=> (Asx=0,Asy=0), but for any Asy!=0 (if Asx=0 => d(bDist)/d(Asx) = 0)
 *   => for continuity similarly define the same for Asy=0, i.e. if bDist=0 => d(bDist)/d(Asx) = 0
 *
 * d(aB)/dX             = d(aB)/d(bDist)*d(bDist)/d(Asx0)*d(Asx0)/dX = DalphpaB*(2*Asx0/bDist)*(1/CellSize)
 * d(aX)/dX             = d(aX)/d(Asx1)*d(Asx1)/d(Asx0)*d(Asx0)/dX = DalphaX*signX*(1/CellSize)
 * d(eEcal)/d(Ecluster) = 1
 * d(ratio)/d(eEcal)    = - ePrs/eEcal**2 = - ratio / eEcal; // if eEcal > 0, otherwise = 0
 *
 * betaQ        = betaC_flag ? betaC : beta
 * d(betaQ2)/dE = d( betaQ(eEcal) + betaQP(ePrs) + betaQPR(ratio) )/d(eEcal) = DbetaQ + 0 + DbetaQ*d(ratio)/dE =
 *              = DbetaQ - DbetaQPR*ePrs/eEcal**2 = DbetaQ - DbetaQPR*ratio/eEcal
 *
 * alpha = aG * aE(eEcal) * aB(bDist) * aX(Asx1) * aY(Asy1)  * aP(ePrs);
 * Ehypo = eCor = ( alpha(eEcal, bDist, Asx1, Asy1) * eEcal + beta2(eEcal, ratio, ePrs) * ePrs ) * gC * gT + dT + offset;
 *
 * d(alpha)/d(eEcal) = (aG*aB*aX*aY*aP) * d(aE)/d(eEcal) = (alpha/aE) * DalphaE; // if aE!=0, otherwise aG*aB*aX*aY*aP*DalphaE
 *
 *
 * d(Ehypo)/d(Ecluster) = gC*gT*(eEcal*d(alpha)/d(eEcal) + alpha + ePrs*d(beta2)/d(eEcal))
 *                      = gC * gT * ( ePrs *  DbetaQ2 + alpha * (1. + DaE / aE * eEcal) )
 * d(Ehypo)/d(Xcluster) = gC*gT*eEcal*d(alpha)/dX = gC*gT*eEcal*aG*aE*aY*aP*(d(aB)/dX*aX+d(aX)/dX*aB)
 *                      = gC * gT * eEcal * aG * aE * aY * aP * (DaB*2.*Asx0/bDist*aX + signX*aB*DaX)/CellSize
 * d(Ehypo)/d(Ycluster) = [ same as for d(Ehypo)/d(Xcluster) with ( X <-> Y ) ]
 * 			= gC * gT * eEcal * aG * aE * aX * aP * (DaB*2.*Asy0/bDist*aY + signY*aB*DaY)/CellSize
 */

  if ( _results != NULL ){
    if ( m_correctCovariance ){
      double DbetaQ2 = betaC_flag ? ( DbetaC - (eEcal > 0 ? DbetaCPR*ratio/eEcal : 0) )
                                  : ( Dbeta  - (eEcal > 0 ? DbetaPR *ratio/eEcal : 0) );
      double d_alpha_dE = (aE != 0) ? DaE*alpha/aE : DaE*aG*aB*aX*aY*aP; // though in principle, aE should never be 0

      _results->dEcor_dEcl  = gC * gT * ( ePrs *  DbetaQ2 + alpha + d_alpha_dE * eEcal );
      _results->dEcor_dXcl  = gC * gT * eEcal * aG * aE * aY * aP * ((bDist == 0 ? 0. : DaB*2.*Asx0/bDist*aX) + signX*aB*DaX)/CellSize;
      _results->dEcor_dYcl  = gC * gT * eEcal * aG * aE * aX * aP * ((bDist == 0 ? 0. : DaB*2.*Asy0/bDist*aY) + signY*aB*DaY)/CellSize;

      _results->betaC_flag  = betaC_flag;
    }

    _results->beta  = beta; // betaQ2
    _results->alpha = alpha;

    // intermediate variables calculated by calcECorrection() needed for debug printout inside process()
    if ( UNLIKELY(msgLevel( MSG::DEBUG)) ){
      _results->Asx   = Asx; // Asx1
      _results->Asy   = Asy; // Asy1
      _results->aG    = aG;
      _results->aE    = aE;
      _results->aB    = aB;
      _results->aX    = aX;
      _results->aY    = aY;
      _results->gC    = gC;
      _results->gT    = gT;
    }
  }

  return eCor;
}
