// $Id: CaloSCorrection.cpp,v 1.6 2008-06-30 15:36:33 odescham Exp $
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
#include "CaloSCorrection.h"

/** @file 
 *  Implementation file for class : CaloSCorrection
 *  
 *  @date 2003-03-10 
 *  @author Xxxx XXXXX xxx@xxx.com 
 */

DECLARE_TOOL_FACTORY( CaloSCorrection );

// ============================================================================
/** Standard constructor
 *  @see GaudiTool 
 *  @see  AlgTool 
 *  @param type tool type (?)
 *  @param name tool name 
 *  @param parent  tool parent 
 */
// ============================================================================
CaloSCorrection::CaloSCorrection 
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent ) 
  : GaudiTool( type , name , parent ) 
  , m_hypos  () 
  , m_hypos_ () 
  /// 
  , Par_Asinh  ()
  , Par_ResOut ()
  , Par_ResMid ()
  , Par_ResInn ()
  , Par_AsOut ()
  , Par_AsMid ()
  , Par_AsInn ()
  , Level()
  ///
  , m_area     ()
  , m_calo       ( DeCalorimeterLocation::Ecal )
  , m_spd        ( DeCalorimeterLocation::Spd  )
  , m_prs        ( DeCalorimeterLocation::Prs  )
  , m_detData    ( DeCalorimeterLocation::Ecal  )
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
  declareProperty    ( "Par_Asinh" , Par_Asinh ) ;
  declareProperty    ( "Par_ResOut", Par_ResOut ) ;
  declareProperty    ( "Par_ResMid", Par_ResMid ) ;
  declareProperty    ( "Par_ResInn", Par_ResInn ) ;
  declareProperty    ( "Par_AsOut", Par_AsOut ) ;
  declareProperty    ( "Par_AsMid", Par_AsMid ) ;
  declareProperty    ( "Par_AsInn", Par_AsInn ) ;
  declareProperty    ( "CorrectionLevel", Level ) ;
  Level.push_back ( true ); 
  Level.push_back ( true );
  Level.push_back ( true ); 
  /// interafces 
  declareInterface<ICaloHypoTool> ( this ) ;  
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
CaloSCorrection::~CaloSCorrection () {} ;
// ============================================================================

// ============================================================================
/** finalization of the tool 
 *  @see  GaudiTool 
 *  @see   AlgTool 
 *  @see  IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSCorrection::finalize   () 
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
StatusCode CaloSCorrection::initialize () 
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
  if( 3 != Par_Asinh.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 8 != Par_ResOut.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 8 != Par_ResMid.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 8 != Par_ResInn.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 6 != Par_AsOut.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 6 != Par_AsMid.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 6 != Par_AsInn.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 3 != Level.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }

  if( m_hypos.empty() ) 
    { return Error("Empty vector of allowed Calorimeter Hypotheses!" ) ; }
  
  // debug printout of all allowed hypos 
  debug() << " List of allowed hypotheses : " << endreq;
  for( Hypotheses::const_iterator it = m_hypos.begin() ; 
       m_hypos.end() != it ; ++it ) {
    debug() <<" --> "<< *it << endreq ;
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
StatusCode CaloSCorrection::operator() ( LHCb::CaloHypo* hypo  ) const 
{ return process( hypo ); };
// ============================================================================

// ============================================================================
/** The main processing method
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloSCorrection::process    ( LHCb::CaloHypo* hypo  ) const 
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
  const double xBar  = position. x () ;
  const double yBar  = position. y () ;



  //  Informations from seed Digit Seed ID & position
  const LHCb::CaloCluster::Entries& entries = MainCluster->entries();
  LHCb::CaloCluster::Entries::const_iterator iseed =locateDigit ( entries.begin () , 
                                                                  entries.end   () , 
                                                                  LHCb::CaloDigitStatus::SeedCell );
  if( entries.end() == iseed )
    { return Error ( "The seed cell is not found !" ) ; }

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
   *  (5) SEED digit             :    seed   (NOT FOR SPLITPHOTONS !!)
   *  (6) CellID of seed digit   :    cellID
   *  (7) Position of seed cell  :    seedPos 
   */

  double CellSize =  m_det->cellSize( cellID  );
  double Asx   = - ( xBar - seedPos.x() ) / CellSize ;
  double Asy   = - ( yBar - seedPos.y() ) / CellSize ;
  double Delta = 0.5;


  // Analytical barycenter for exponential decay shower profile
  double Argx = Asx / Delta * cosh( Delta / Par_Asinh[area] );
  double Argy = Asy / Delta * cosh( Delta / Par_Asinh[area] );
  if(Level[0]){
    Asx = Par_Asinh[area] * log( Argx + sqrt( Argx*Argx + 1.) );
    Asy = Par_Asinh[area] * log( Argy + sqrt( Argy*Argy + 1.) );
  }
  
  // Residual asymmetries due to single exponential approximation
  double DAsx=0.;
  double DAsy=0.;
  {for ( int i = 0 ; i !=8 ; ++i){
    if( 0 == area ){ 
      DAsx += Par_ResOut[i] * pow(Asx,i);
      DAsy += Par_ResOut[i] * pow(Asy,i);}
    if( 1 == area ){ 
      DAsx += Par_ResMid[i] * pow(Asx,i);
      DAsy += Par_ResMid[i] * pow(Asy,i);}
    if( 2 == area ){ 
      DAsx += Par_ResInn[i] * pow(Asx,i);
      DAsy += Par_ResInn[i] * pow(Asy,i);}
  }
  } // Fix VC6 scoping bug
  
  if(Level[1]){
    Asx -= DAsx ;
    Asy -= DAsy ;
  }
  
  // Geometrical asymmetries due to non symmetrical shower spot 
  // Left/Right for (X) & Up/Down for (Y)
  double DDAsx = 0.;
  double DDAsy = 0.;
  {for ( int i = 0 ; i !=3 ; ++i){
    if( 0 == area && 0 < xBar ){DDAsx += Par_AsOut[i]   * pow(Asx,i);}
    if( 0 == area && 0 > xBar ){DDAsx += Par_AsOut[i+3] * pow(Asx,i);}
    if( 1 == area && 0 < xBar ){DDAsx += Par_AsMid[i]   * pow(Asx,i);}
    if( 1 == area && 0 > xBar ){DDAsx += Par_AsMid[i+3] * pow(Asx,i);}
    if( 2 == area && 0 < xBar ){DDAsx += Par_AsInn[i]   * pow(Asx,i);}   
    if( 2 == area && 0 > xBar ){DDAsx += Par_AsInn[i+3] * pow(Asx,i);}   
    if( 0 == area && 0 < yBar ){DDAsy += Par_AsOut[i]   * pow(Asy,i);}
    if( 0 == area && 0 > yBar ){DDAsy += Par_AsOut[i+3] * pow(Asy,i);}
    if( 1 == area && 0 < yBar ){DDAsy += Par_AsMid[i]   * pow(Asy,i);}
    if( 1 == area && 0 > yBar ){DDAsy += Par_AsMid[i+3] * pow(Asy,i);}
    if( 2 == area && 0 < yBar ){DDAsy += Par_AsInn[i]   * pow(Asy,i);}   
    if( 2 == area && 0 > yBar ){DDAsy += Par_AsInn[i+3] * pow(Asy,i);}   
  }
  } // Fix VC6 scoping bug
  if(Level[2]){
    Asx += DDAsx;
    Asy += DDAsy;
  }
  
  // Recompute position and fill CaloPosition
  double xCor = seedPos.x() - Asx * CellSize;
  double yCor = seedPos.y() - Asy * CellSize;

  const LHCb::CaloPosition* pos = hypo->position() ;

  debug() << "Hypothesis :" << hypo->hypothesis() << endreq;
  debug() << "Hypo E :  " << hypo->position ()->e()   <<  " "  <<cellID << endreq;
  debug() << "xBar/yBar " << xBar  <<  "/" << yBar   <<  endreq;
  debug() << "xg/yg  "      << pos->x() << "/" << pos->y() <<  endreq;
  debug() << "xNew/yNew "   << xCor <<  "/" << yCor    <<  endreq;
  debug() << "xcel/ycel "   << seedPos.x() <<  "/" << seedPos.y() << endreq ;
  
  
  /** At the end: 
   */
  
  LHCb::CaloPosition::Parameters& parameters = hypo ->position() ->parameters () ;
  //  CaloPosition::Covariance& covariance = hypo ->position() ->covariance () ;
  parameters ( LHCb::CaloPosition::X ) = xCor ;
  parameters ( LHCb::CaloPosition::Y ) = yCor ;

  return StatusCode::SUCCESS ;

};
// ============================================================================


