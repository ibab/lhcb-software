// $Id: CaloNewECorrection.cpp,v 1.4 2004-02-17 12:08:09 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2003/12/11 16:33:40  cattanem
// Fixes for Gaudi v13
//
// Revision 1.2  2003/05/16 08:19:11  cattanem
// remove unused variables
//
// Revision 1.1  2003/04/11 09:33:35  ibelyaev
//  add new E-,S- and L-corrections from Olivier Deschamps
//
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
// Event 
#include "Event/CaloHypo.h"
// Kernel
#include "Kernel/CaloHypoPrint.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// local
#include "CaloNewECorrection.h"

/** @file 
 *  Implementation file for class : CaloNewECorrection
 *  
 *  @date 2003-03-10 
 *  @author Xxxx XXXXX xxx@xxx.com 
 */


// ============================================================================
/*
 *   Declaration of the Tool Factory, needed for instantiation
 */
// ============================================================================
static const  ToolFactory<CaloNewECorrection>         s_Factory ;
const        IToolFactory&CaloNewECorrectionFactory = s_Factory ; 
// ============================================================================
// ============================================================================
/** Standard constructor
 *  @see CaloTool 
 *  @see  AlgTool 
 *  @param type tool type (?)
 *  @param name tool name 
 *  @param parent  tool parent 
 */
// ============================================================================
CaloNewECorrection::CaloNewECorrection 
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent ) 
  : CaloTool( type , name , parent ) 
    , m_hypos  () 
    , m_hypos_ () 
  /// 
    , A1_a ()
    , A1_b ()
    , A2_a ()
    , A2_b ()
    , SlopeX()
    , SlopeY()
    , Beta()
    , GlobalFactor()
    , Level ()   
  ///
    , m_area   ()
    , m_calo       ( DeCalorimeterLocation::Ecal )
    , m_spd        ( DeCalorimeterLocation::Spd  )
    , m_prs        ( DeCalorimeterLocation::Prs  )
{
  /// properties
  /// acceptable hypotheses 
  m_hypos_.push_back ( (int) CaloHypotheses::Photon               ) ;
  m_hypos_.push_back ( (int) CaloHypotheses::PhotonFromMergedPi0  ) ;
  m_hypos_.push_back ( (int) CaloHypotheses::BremmstrahlungPhoton ) ;
  declareProperty    ( "Hypotheses"   , m_hypos_   ) ;
  /// vectors of external parameters 
  declareProperty    ( "Corr1_constant" , A1_a ) ;
  declareProperty    ( "Corr1_slope"    , A1_b ) ;
  declareProperty    ( "Corr2_constant" , A2_a ) ;
  declareProperty    ( "Corr2_slope"    , A2_b ) ;
  declareProperty    ( "Corr3_slopeX"   , SlopeX ) ;
  declareProperty    ( "Corr3_slopeY"   , SlopeY ) ;
  declareProperty    ( "CorrPrs"        , Beta   ) ;
  declareProperty    ( "GlobalFactor"   , GlobalFactor ) ;
  declareProperty    ( "CorrectionLevel"   , Level ) ;
  Level.push_back ( true);
  Level.push_back ( true);
  Level.push_back ( true);
  Level.push_back ( true);
  Level.push_back ( true);
  Level.push_back ( true);

  /// det the default detector 
  setDetName         ( DeCalorimeterLocation::Ecal ) ;
  /// interafces 
  declareInterface<ICaloHypoTool> ( this ) ;  
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
CaloNewECorrection::~CaloNewECorrection () {} ;
// ============================================================================

// ============================================================================
/** finalization of the tool 
 *  @see  CaloTool 
 *  @see   AlgTool 
 *  @see  IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloNewECorrection::finalize   () 
{
  m_hypos.clear();
  /// finalize the base class 
  return CaloTool::finalize () ;
};
// ============================================================================

// ============================================================================
/** initialization of the tool 
 *  @see  CaloTool 
 *  @see   AlgTool 
 *  @see  IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloNewECorrection::initialize () 
{
  MsgStream log( msgSvc() , name() ) ;
  /// first initialize the base class 
  StatusCode sc = CaloTool::initialize();
  if( sc.isFailure() ) 
  { return Error ( "Unable initialize the base class CaloTool!" , sc ) ; }
  
  // transform vector of accepted hypos
  m_hypos.clear () ;
  for( Hypotheses_::const_iterator ci = m_hypos_.begin() ; 
       m_hypos_.end() != ci ; ++ci ) 
  {
    const int hypo = *ci ;
    if( hypo <= (int) CaloHypotheses::Undefined || 
        hypo >= (int) CaloHypotheses::Other      ) 
    { return Error("Invalid/Unknown  Calorimeter hypothesis object!" ) ; }
    m_hypos.push_back( (CaloHypotheses::Hypothesis) hypo );
  }
  
  // locate and set and configure the Detector 
  const DeCalorimeter* ecal = getDet<DeCalorimeter>( detName () ) ;
  if( 0 == ecal ) { return StatusCode::FAILURE ; }
  setDet( ecal ) ;
  m_calo.setCalo( detName() );
  
  // check vectors of paramters 
  if( 3 != A1_a.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 3 != A1_b.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 3 != A2_a.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 3 != A2_b.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 3 != SlopeX.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 3 != SlopeY.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 3 != GlobalFactor.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 7 != Level.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }


  
  if( m_hypos.empty() ) 
    { return Error("Empty vector of allowed Calorimeter Hypotheses!" ) ; }
  
  // debug printout of all allowed hypos 
  debug() << " List of allowed hypotheses : " << endreq;
  for( Hypotheses::const_iterator it = m_hypos.begin() ; 
       m_hypos.end() != it ; ++it ) 
  { debug ()  << " \t \t" ; CaloHypoPrint( log , *it ) ; log << endreq ; };
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** The main processing method (functor interface)
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloNewECorrection::operator() ( CaloHypo* hypo  ) const 
{ return process( hypo ); };
// ============================================================================

// ============================================================================
/** The main processing method
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloNewECorrection::process    ( CaloHypo* hypo  ) const 
{
  MsgStream  msg( msgSvc(), name() );
  // avoid long names 
  typedef const CaloHypo::Digits   Digits   ;
  typedef const CaloHypo::Clusters Clusters ;
  
  using namespace ClusterFunctors ;
  using namespace CaloDataFunctor ;
  
  // check arguments 
  if( 0 == hypo ) { return Error ( " CaloHypo* points to NULL!" ) ; }

  // check the Hypo
  Hypotheses::const_iterator h = 
    std::find( m_hypos.begin() , m_hypos.end() , hypo->hypothesis() ) ;
  if( m_hypos.end() == h ) { return Error ( "Invalid hypothesis!" ) ; }

  // get all clusters from the hypo 
  const Clusters& clusters = hypo->clusters() ;
  // find the first cluster from Ecal (Global cluster)
  Clusters::const_iterator iclu =
    std::find_if( clusters.begin () , clusters.end () , m_calo );
  if( clusters.end() == iclu ) 
    { return Error("No clusters from '"+detName()+"' is found!"); }
  // For Split photons pi0 find the split cluster
  Clusters::const_iterator icl  = iclu;
  if(  CaloHypotheses::PhotonFromMergedPi0 == hypo->hypothesis() 
       &&  2 == clusters.size() ){icl = iclu+1;}

  /*
    Cluster information (e/x/y  and Prs/Spd digit)
  */
  const CaloCluster* cluster = *icl ;
  if( 0 == cluster ) { return Error ( "CaloCLuster* points to NULL!" ) ; }
  double ePrs = 0 ;
  double eSpd = 0 ;
  const Digits& digits = hypo->digits();
  for( Digits::const_iterator d = digits.begin() ; digits.end() != d ; ++d ) 
    { 
      if     ( *d == 0     ) { continue           ; }
      else if( m_prs( *d ) ) { ePrs  += (*d)->e() ; } 
      else if( m_spd( *d ) ) { eSpd  += (*d)->e() ; } 
    }
  const CaloPosition& position = cluster->position();
  const double eEcal = position. e () ;
  const double xBar  = position. x () ;
  const double yBar  = position. y () ;



  /*
     Informations from seed Digit Seed ID & position
     (Not directly AVAILABLE FOR SPLITCLUSTERS !!!)
  */
  const CaloCluster* Maincluster = *iclu ;
  const CaloCluster::Entries& entries = Maincluster->entries();
  CaloCluster::Entries::const_iterator iseed = 
    locateDigit ( entries.begin () , 
                  entries.end   () , CaloDigitStatus::SeedCell );
  if( entries.end() == iseed )
    { return Error ( "The seed cell is not found !" ) ; }
  // get the "area" of the cluster (where seed is placed) 
  const unsigned int area = m_area( Maincluster );
  const CaloDigit*  seed    = iseed->digit();
  if( 0 == seed ) { return Error ( "Seed digit points to NULL!" ) ; }
  // Cell ID for seed digit 
  CaloCellID cellID = seed->cellID() ;
  // position of the SEED 
  HepPoint3D seedPos = det()->cellCenter( cellID  );
  // USE TRICK FOR SPLITCLUSTER (local seed digit not available for the moment)
  if(  CaloHypotheses::PhotonFromMergedPi0 == hypo->hypothesis() 
       &&  2 == clusters.size() ){
    const CaloPosition* pos = hypo->position() ;
    double  x = pos->x();
    double  y = pos->y();
    double  z = seedPos.z();
    const HepPoint3D   point   ( x , y , z ) ;
    cellID  =  det()->Cell( point );
    if( CaloCellID() == cellID ){ return Error ( "Cell does not exist !") ; }
    seedPos =  det()->cellCenter( cellID );
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

  const int ShiftCol[3] = {  0 ,  0 ,  8 };
  const int ShiftRow[3] = {  6 , 12 , 14 };
   


  // Digit not available for 2nd cluster (avoid double Eprs counting)
  CaloCellID MainID = seed->cellID() ;
  HepPoint3D MainPos = det()->cellCenter( MainID );
  double WhereX =seedPos.x()-MainPos.x();
  double WhereY =seedPos.y()-MainPos.y();
  if(  CaloHypotheses::PhotonFromMergedPi0 == hypo->hypothesis() 
       &&        ( 0 != WhereX ||  0 != WhereY) 
       ){
    ePrs = 0 ;
    eSpd = 0 ;
    // if(0 != WhereX ||  0 != WhereY){ eSpd = 0. ;}
  }
  /////////////////////////////////////////////////////////

  
  double CellSize =  det()->cellSize( cellID  );
  double Asx   = ( xBar - seedPos.x() ) / CellSize ;
  double Asy   = ( yBar - seedPos.y() ) / CellSize ;
  unsigned int    Col   = cellID.col()  - ShiftCol[area] +1;
  unsigned int    Row   = cellID.row()  - ShiftRow[area] +1;


  // Deconvolute Asx/Asy  from previous corrections for SPLIT (temporarly)
  if(  CaloHypotheses::PhotonFromMergedPi0 == hypo->hypothesis()  ){
    double bold[3]  = {  0.1093 ,  0.1326 ,  0.1462 }; 
    double Delta =0.5;
    Asx = Delta * sinh ( Asx / bold[area] ) / sinh ( Delta / bold[area] );
    Asy = Delta * sinh ( Asy / bold[area] ) / sinh ( Delta / bold[area] );
  }
  double bDist = sqrt( Asx*Asx + Asy*Asy) * sqrt ( 2. ) ;
  
  double A1=1;
  double A2=1;
  double A3=1;
  
  // Longitudinal leakage
  if( Level[0] ){ A1 = 1. / (A1_a[area] + A1_b[area] * eEcal / GeV) ;}
  // Lateral leakage
  if( Level[1] ){A2 = 1. / (A2_a[area] + A2_b[area] * bDist) ;}

  // leakage induced by Ecal module frame 
  double signX = 0;
  if( 1 == Col % (area+1) )  {signX  = +1.;}
  if( 0 == Col % (area+1) )  {signX  = -1.;}
  double signY = 0;
  if( 1 == Row % (area+1) )  {signY  = +1.;}
  if( 0 == Row % (area+1) )  {signY  = -1.;}
  double CorrX = signX * SlopeX[area] / 100. ;
  double CorrY = signY * SlopeY[area] / 100. ;
  double A3x = 1. + CorrX * Asx ;
  double A3y = 1. + CorrY * Asy ;
  if( Level[2] ){A3  = 1. / A3x / A3y ;}

  // Apply Ecal leakage corrections
  double Alpha = A1 * A2 * A3;
  double Ecor  = Alpha * eEcal ;

  // Apply Preshower corrections
  if( Level[3] ){Ecor  += Beta[area] * ePrs ;}
 
 // Apply global rescaling for No Spd case or Spd
  if( Level[4] ){Ecor = Ecor / GlobalFactor[0] ;} 
  if( 0 <  eSpd && Level[5] ){Ecor = Ecor / GlobalFactor[1]; }
  if( Level[6] && CaloHypotheses::PhotonFromMergedPi0 == hypo->hypothesis()  ){
    Ecor = Ecor / GlobalFactor[2]; }

  msg<< MSG::INFO
     << " Main Cluster " << Maincluster->position().e()/GeV    <<  " "
     << "area "  << area  <<  " "
     << "Row "  << Row <<  " "
     << "Col "  << Col <<  " "
     << "Asx "  << Asx <<  " "
     << "Asy "  << Asy <<  " "
     << "eSpd "  << eSpd <<  " "
     << "ePrs "  << ePrs  <<  " "
     << "eEcal " << eEcal/GeV <<  " "
     << "Ecor "  <<  Ecor/GeV <<  " "
     << endreq;
  

  /** At the end: 
   */
  
  //   if( 0 != hypo -> position () ) 
  //     { delete hypo->position () ; hypo->setPosition( (CaloPosition*) 0 ); }
  //   if( 0 != hypo -> momentum () ) 
  //     { delete hypo->momentum () ; hypo->setMomentum( (CaloMomentum*) 0 ); }
  //  hypo->setPosition( position.clone() );
  CaloPosition::Parameters& parameters = hypo ->position() ->parameters () ;
  //  CaloPosition::Covariance& covariance = hypo ->position() ->covariance () ;

  /** The following quantities should be updated 
   */
  msg<< MSG::INFO
     << " ENE 1 " << hypo->position ()->e() <<  " " << endreq;
  
  parameters ( CaloPosition::E ) = Ecor ;

  msg<< MSG::INFO
     << " ENE 2 " << hypo->position ()->e() <<  " " << endreq;
  // parameters ( CaloPosition::X ) = ... ;
  // parameters ( CaloPosition::Y ) = ... ;

  // Covariance to be DONE
  // covariance ( CaloPosition::X , CaloPosition::X ) = .. ;
  // covariance ( CaloPosition::Y , CaloPosition::X ) = .. ;
  // covariance ( CaloPosition::E , CaloPosition::X ) = .. ;
  // covariance ( CaloPosition::Y , CaloPosition::Y ) = .. ;
  // covariance ( CaloPosition::E , CaloPosition::Y ) = .. ;
  // covariance ( CaloPosition::E , CaloPosition::E ) = .. ;
  
  return StatusCode::SUCCESS ;

};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
