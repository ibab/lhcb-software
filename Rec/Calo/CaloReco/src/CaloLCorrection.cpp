// $Id: CaloLCorrection.cpp,v 1.7 2006-12-01 14:06:36 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// Event 
#include "Event/CaloHypo.h"
// Kernel
#include "GaudiKernel/SystemOfUnits.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// local
#include "CaloLCorrection.h"

/** @file 
 *  Implementation file for class : CaloLCorrection
 *  
 *  @date 2003-03-10 
 *  @author Xxxx XXXXX xxx@xxx.com 
 */

DECLARE_TOOL_FACTORY( CaloLCorrection );

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
  : GaudiTool( type , name , parent ) 
  , m_hypos  () 
  , m_hypos_ () 
  /// 
  , Par_Al1 ()
  , Par_Al2 ()
  , Par_Al3 ()
  , Par_Be1 ()
  , Par_Be2 ()
  , Par_Be3 ()
  , Par_z0  ()
  ///
  , m_area     ()
  , m_calo       ( DeCalorimeterLocation::Ecal )
  , m_spd        ( DeCalorimeterLocation::Spd  )
  , m_prs        ( DeCalorimeterLocation::Prs  )
  , m_detData    ( DeCalorimeterLocation::Ecal )
{
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
  declareProperty    ( "Par_Al1" , Par_Al1 ) ;
  declareProperty    ( "Par_Al2" , Par_Al2 ) ;
  declareProperty    ( "Par_Al3" , Par_Al3 ) ;
  declareProperty    ( "Par_Be1" , Par_Be1 ) ;
  declareProperty    ( "Par_Be2" , Par_Be2 ) ;
  declareProperty    ( "Par_Be3" , Par_Be3 ) ;
  declareProperty    ( "Par_z0"  , Par_z0  ) ;

  /// interafces 
  declareInterface<ICaloHypoTool> ( this ) ;  
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
CaloLCorrection::~CaloLCorrection () {} ;
// ============================================================================

// ============================================================================
/** finalization of the tool 
 *  @see  GaudiTool 
 *  @see   AlgTool 
 *  @see  IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloLCorrection::finalize   () 
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
StatusCode CaloLCorrection::initialize () 
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
  m_calo.setCalo( m_detData );
  
  // check vectors of paramters 
  if( 3 != Par_Al1.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 3 != Par_Al2.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 3 != Par_Al3.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 3 != Par_Be1.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 3 != Par_Be2.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 3 != Par_Be3.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 1 != Par_z0.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }

  if( m_hypos.empty() ) 
    { return Error("Empty vector of allowed Calorimeter Hypotheses!" ) ; }
  
  // debug printout of all allowed hypos 
  debug() << " List of allowed hypotheses : " << endreq;
  for( Hypotheses::const_iterator it = m_hypos.begin() ; 
       m_hypos.end() != it ; ++it ) {
    debug() << "--> " << *it << endreq ;
  };

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
StatusCode CaloLCorrection::operator() ( LHCb::CaloHypo* hypo  ) const 
{ return process( hypo ); };
// ============================================================================

// ============================================================================
/** The main processing method
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloLCorrection::process    ( LHCb::CaloHypo* hypo  ) const 
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
  debug() << " -- Global Cluster E = " << (*iclu)->position().e() << endreq; 

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


  const LHCb::CaloPosition& position = MainCluster->position();
  const double eEcal = position. e () ;


  const LHCb::CaloCluster::Entries& entries = MainCluster->entries();
  LHCb::CaloCluster::Entries::const_iterator iseed =locateDigit ( entries.begin () , 
                                                                  entries.end   () , 
                                                                  LHCb::CaloDigitStatus::SeedCell );
  if( entries.end() == iseed ){ return Error ( "The seed cell is not found !" ) ; }

  // get the "area" of the cluster (where seed is) 
  const unsigned int area = m_area( MainCluster );
  const LHCb::CaloDigit*  seed    = iseed->digit();
  if( 0 == seed ) { return Error ( "Seed digit points to NULL!" ) ; }

  // Cell ID for seed digit 
  LHCb::CaloCellID cellID = seed->cellID() ;

  
  /** here all information is available 
   *     
   *  (1) Ecal energy in 3x3     :    eEcal 
   *  (2) Prs and Spd energies   :    ePrs, eSpd 
   *  (3) weighted barycenter    :    xBar, yBar 
   *  (4) Zone/Area in Ecal      :    area   
   *  (5) SEED digit             :    seed   
   *  (6) CellID of seed digit   :    cellID
   *  (7) Position of seed cell  :    seedPos 
   */

  //  double CellSize =  m_det->cellSize( cellID  );

  //  Gaudi::XYZPoint seedPos = m_det->cellCenter( cellID  );  
  //  double z0 = seedPos.z();

  double z0 = Par_z0[0] ;// Parameter tuned wrt to z0  12566 mm !!
  double Zvtx = 0.   ;// Origin vertex (z) set to 0 !!
  const LHCb::CaloPosition* pos = hypo->position() ;
  double  xg = pos->x();
  double  yg = pos->y();
  double  zg = z0 - Zvtx;
    

  // Uncorrected angle
  double tth   = sqrt ( pow(xg,2) + pow(yg,2) ) / zg ;
  double cth   = cos  ( atan( tth ) ) ;

  // Corrected angle
  int zon = 2 - area;
  double alpha = Par_Al1[zon] - exp( Par_Al2[zon] - Par_Al3[zon] * ePrs/Gaudi::Units::MeV );
  double beta  = Par_Be1[zon] + exp( Par_Be2[zon] - Par_Be3[zon] * ePrs/Gaudi::Units::MeV  );
  double tgfps = alpha * log(eEcal/Gaudi::Units::GeV) +beta ;
  tth = tth / ( 1. + tgfps * cth / zg ) ;
  cth= cos( atan( tth ) ) ;
  double dzfps = cth * tgfps ;
// Recompute Z position and fill CaloPosition
  double zCor = z0 + dzfps;

  debug() << "Hypothesis :" << hypo->hypothesis() << endreq;
  debug()     << " ENE  " << hypo->position ()->e() <<  " "
              << "xg "   << xg <<  " "<< "yg "   << yg <<  endreq;
  debug()     << "zg "   << pos->z() << " " 
              << "z0 "   << z0 <<  " "
              << "DeltaZ "   << dzfps <<  " "
              << "zCor "   << zCor 
              << endreq ;
  

  /** At the end: 
   */
  
  hypo -> position() -> setZ( zCor ) ;

  debug() << " ENE  " << hypo->position ()->e() <<  " "
          << "New zg "   << pos->z() <<  " "
          << endreq ;

  
  return StatusCode::SUCCESS ;

};
// ============================================================================


