// $Id: CaloECorrection.cpp,v 1.5 2006-12-01 14:06:36 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
//
#include "GaudiKernel/SystemOfUnits.h"
// Event 
#include "Event/CaloHypo.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// local
#include "CaloECorrection.h"

/** @file 
 *  Implementation file for class : CaloECorrection
 *  
 *  @date 2003-03-10 
 *  @author Deschamps Olivier
 */

DECLARE_TOOL_FACTORY( CaloECorrection );

// ============================================================================
/** Standard constructor
 *  @see GaudiTool 
 *  @see  AlgTool 
 *  @param type tool type (?)
 *  @param name tool name 
 *  @param parent  tool parent 
 */
// ============================================================================
CaloECorrection::CaloECorrection 
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent ) 
  : GaudiTool( type , name , parent ) 
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
  , m_calo       (DeCalorimeterLocation::Ecal )
  , m_spd        (DeCalorimeterLocation::Spd  )
  , m_prs        (DeCalorimeterLocation::Prs  )
  , m_detData    (DeCalorimeterLocation::Ecal ){
  

  /// properties
  /// acceptable hypotheses 
  m_hypos_.push_back ( (int) LHCb::CaloHypo::Photon               ) ;
  m_hypos_.push_back ( (int) LHCb::CaloHypo::PhotonFromMergedPi0  ) ;
  m_hypos_.push_back ( (int) LHCb::CaloHypo::BremmstrahlungPhoton ) ;
  m_hypos_.push_back ( (int) LHCb::CaloHypo::EmCharged ) ;
  m_hypos_.push_back ( (int) LHCb::CaloHypo::Electron ) ;
  m_hypos_.push_back ( (int) LHCb::CaloHypo::Positron ) ;

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
  declareProperty    ( "CorrectionLevel", Level ) ;
  Level.push_back ( true);
  Level.push_back ( true);
  Level.push_back ( true);
  Level.push_back ( true);
  Level.push_back ( true);
  Level.push_back ( true);

  /// interafces 
  declareInterface<ICaloHypoTool> ( this ) ;  
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
CaloECorrection::~CaloECorrection () {} ;
// ============================================================================

// ============================================================================
/** finalization of the tool 
 *  @see  GaudiTool 
 *  @see   AlgTool 
 *  @see  IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloECorrection::finalize   () 
{
  m_hypos.clear();
  /// finalize the base class 
  return GaudiTool::finalize () ;
};
// ============================================================================

// ============================================================================
/** initialization of the tool 
 *  @see  GaudiTool 
 *  @see   AlgTool 
 *  @see  IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloECorrection::initialize () 
{
  /// first initialize the base class 
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) 
  { return Error ( "Unable initialize the base class GaudiTool!" , sc ) ; }
  
  // transform vector of accepted hypos
  m_hypos.clear () ;
  for( Hypotheses_::const_iterator ci = m_hypos_.begin() ; 
       m_hypos_.end() != ci ; ++ci ) 
  {
    const int hypo = *ci ;
    if( hypo <= (int) LHCb::CaloHypo::Undefined || 
        hypo >= (int) LHCb::CaloHypo::Other      ) 
    { return Error("Invalid/Unknown  Calorimeter hypothesis object!" ) ; }
    m_hypos.push_back( (LHCb::CaloHypo::Hypothesis) hypo );
  }
  
  // locate and set and configure the Detector 
  m_det = getDet<DeCalorimeter>( m_detData ) ;
  if( 0 == m_det ) { return StatusCode::FAILURE ; }
  m_calo.setCalo( m_detData);
  
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
  if( 4 != GlobalFactor.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 7 != Level.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }


  
  if( m_hypos.empty() ) 
    { return Error("Empty vector of allowed Calorimeter Hypotheses!" ) ; }
  
  // debug printout of all allowed hypos 
  debug() << " List of allowed hypotheses : " << endreq;
  for( Hypotheses::const_iterator it = m_hypos.begin() ; 
       m_hypos.end() != it ; ++it ) 
  { debug ()  <<  " -->" << *it  << endreq ; };
  
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
StatusCode CaloECorrection::operator() ( LHCb::CaloHypo* hypo  ) const 
{ return process( hypo ); };
// ============================================================================

// ============================================================================
/** The main processing method
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloECorrection::process    ( LHCb::CaloHypo* hypo  ) const 
{
  // avoid long names 
  typedef const LHCb::CaloHypo::Digits   Digits   ;
  typedef const LHCb::CaloHypo::Clusters Clusters ;
  
  using namespace LHCb::ClusterFunctors ;
  using namespace LHCb::CaloDataFunctor ;
  
  
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
  { return Error("No clusters from '"+m_detData+"' is found!"); }

  const LHCb::CaloCluster* GlobalCluster = *iclu ;
  debug() << " -- Global Cluster E = " << (*iclu)->position().e() << endreq;;

  // Look for the splitCluster when PhotonFromMerged
  if(  LHCb::CaloHypo::PhotonFromMergedPi0 == hypo->hypothesis()       
       &&  2 == clusters.size() )iclu++;
  
  const LHCb::CaloCluster* MainCluster = *iclu ;
  debug() << " ------ Main  cluster E = " << (*iclu)->position().e() << endreq;
  

  /*
    Cluster information (e/x/y  and Prs/Spd digit)
  */
  if( 0 == MainCluster ) { return Error ( "CaloCLuster* points to NULL!" ) ; }
  double ePrs = 0 ;
  double eSpd = 0 ;
  const Digits& digits = hypo->digits();
  for( Digits::const_iterator d = digits.begin() ; digits.end() != d ; ++d ){ 
      if     ( *d == 0     ) { continue           ; }
      else if( m_prs( *d ) ) { ePrs  += (*d)->e() ; } 
      else if( m_spd( *d ) ) { eSpd  += (*d)->e() ; } 
  }

  // For Split Photon - share the Prs energy
  if(  LHCb::CaloHypo::PhotonFromMergedPi0 == hypo->hypothesis() ){
    ePrs *= MainCluster->position().e()/GlobalCluster->position().e() ;
  }


  // Get position
  const LHCb::CaloPosition& position = MainCluster->position();
  const double eEcal = position. e () ;
  const double xBar  = position. x () ;
  const double yBar  = position. y () ;

  // seed ID & position
  const LHCb::CaloCluster::Entries& entries = MainCluster->entries();
  LHCb::CaloCluster::Entries::const_iterator iseed = locateDigit ( entries.begin () ,
                                                                   entries.end   () , 
                                                                   LHCb::CaloDigitStatus::SeedCell );
  if( entries.end() == iseed ){ return Error ( "The seed cell is not found !" ) ; }

  // get the "area" of the cluster (where seed is) 
  const unsigned int area = m_area( MainCluster );
  const LHCb::CaloDigit*  seed    = iseed->digit();
  if( 0 == seed ) { return Error ( "Seed digit points to NULL!" ) ; }
  // Cell ID for seed digit 
  LHCb::CaloCellID cellID = seed->cellID() ;
  Gaudi::XYZPoint seedPos = m_det->cellCenter( cellID  );

  
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
  
  double A1=1;
  double A2=1;
  double A3=1;
  
  // Longitudinal leakage
  if( Level[0] ){ A1 = 1. / (A1_a[area] + A1_b[area] * eEcal / Gaudi::Units::GeV) ;}
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
 
 // Apply global rescaling for ALL Types (factor should be 1.)
  if( Level[4] ){Ecor = Ecor / GlobalFactor[0] ;} 

 // Apply additional  rescaling for converted photons
  if( 0 <  eSpd && Level[5] && LHCb::CaloHypo::EmCharged !=  hypo->hypothesis()){Ecor = Ecor / GlobalFactor[1]; }

  // Apply additional rescaling for SplitPhotons
  if( Level[6] && LHCb::CaloHypo::PhotonFromMergedPi0 == hypo->hypothesis()  ){
    Ecor = Ecor / GlobalFactor[2]; }

  // Apply additional rescaling for Electron
  if(  LHCb::CaloHypo::EmCharged ==  hypo->hypothesis()){
    Ecor = Ecor / GlobalFactor[3]; }

  debug() << "Hypothesis :" << hypo->hypothesis() << endreq;
  debug() << "area "  << area  <<  " "<< "LocRow "  << Row <<  " "
          << "LocCol "  << Col <<  "cellID" << cellID << endreq;
  debug() << "Asx "  << Asx <<  " " << "Asy "  << Asy  << endreq;
  debug() << "eSpd "  << eSpd <<  " "    << "ePrs "  << ePrs  <<  endreq;
  debug() << "eEcal " << eEcal <<  " "    << "Ecor "  <<  Ecor    << endreq;
  

  LHCb::CaloPosition::Parameters& parameters = hypo ->position() ->parameters () ;
  parameters ( LHCb::CaloPosition::E ) = Ecor ;
  //  CaloPosition::Covariance& covariance = hypo ->position() ->covariance () ;


  
  return StatusCode::SUCCESS ;

};
// ============================================================================


