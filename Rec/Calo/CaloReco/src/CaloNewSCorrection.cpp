// $Id: CaloNewSCorrection.cpp,v 1.6 2005-11-07 12:12:43 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2004/02/17 12:08:09  ibelyaev
//  update for new CaloKernel and CaloInterfaces
//
// Revision 1.4  2003/12/11 16:33:40  cattanem
// Fixes for Gaudi v13
//
// Revision 1.3  2003/05/16 08:19:11  cattanem
// remove unused variables
//
// Revision 1.2  2003/04/17 07:06:48  cattanem
// fix for Windows
//
// Revision 1.1  2003/04/11 09:33:40  ibelyaev
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
#include "CaloNewSCorrection.h"

/** @file 
 *  Implementation file for class : CaloNewSCorrection
 *  
 *  @date 2003-03-10 
 *  @author Xxxx XXXXX xxx@xxx.com 
 */


// ============================================================================
/*
 *  Declaration of the Tool Factory, needed for instantiation
 */
// ============================================================================
static const  ToolFactory<CaloNewSCorrection>         s_Factory ;
const        IToolFactory&CaloNewSCorrectionFactory = s_Factory ; 
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
CaloNewSCorrection::CaloNewSCorrection 
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent ) 
  : CaloTool( type , name , parent ) 
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
{
  /// properties
  /// acceptable hypotheses 
  m_hypos_.push_back ( (int) CaloHypotheses::Photon               ) ;
  m_hypos_.push_back ( (int) CaloHypotheses::PhotonFromMergedPi0  ) ;
  m_hypos_.push_back ( (int) CaloHypotheses::BremmstrahlungPhoton ) ;
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
  /// det the default detector 
  setDetName         ( DeCalorimeterLocation::Ecal ) ;
  /// interafces 
  declareInterface<ICaloHypoTool> ( this ) ;  
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
CaloNewSCorrection::~CaloNewSCorrection () {} ;
// ============================================================================

// ============================================================================
/** finalization of the tool 
 *  @see  CaloTool 
 *  @see   AlgTool 
 *  @see  IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloNewSCorrection::finalize   () 
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
StatusCode CaloNewSCorrection::initialize () 
{
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
  MsgStream log( msgSvc() , name() ) ;
  log << MSG::DEBUG 
      << " List of allowed hypotheses : " << endreq;
  for( Hypotheses::const_iterator it = m_hypos.begin() ; 
       m_hypos.end() != it ; ++it ) 
    {      log << " \t \t" ; CaloHypoPrint( log , *it ) ; log << endreq ;
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
StatusCode CaloNewSCorrection::operator() ( CaloHypo* hypo  ) const 
{ return process( hypo ); };
// ============================================================================

// ============================================================================
/** The main processing method
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloNewSCorrection::process    ( CaloHypo* hypo  ) const 
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
   //  const double eEcal = position. e () ;
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
   *  (5) SEED digit             :    seed   (NOT FOR SPLITPHOTONS !!)
   *  (6) CellID of seed digit   :    cellID
   *  (7) Position of seed cell  :    seedPos 
   */

  double CellSize =  det()->cellSize( cellID  );
  double Asx   = - ( xBar - seedPos.x() ) / CellSize ;
  double Asy   = - ( yBar - seedPos.y() ) / CellSize ;
  double Delta = 0.5;


  // Deconvolute Asx/Asy  from previous corrections for Split (temporarly)
  if( CaloHypotheses::PhotonFromMergedPi0 == hypo->hypothesis() && Level[0] ){
    double bold[3]  = {  0.1093 ,  0.1326 ,  0.1462 }; 
  Asx = Delta * sinh ( Asx / bold[area] ) / sinh ( Delta / bold[area] );
  Asy = Delta * sinh ( Asy / bold[area] ) / sinh ( Delta / bold[area] );
  }
  msg<< MSG::DEBUG
     << " ENE  " << hypo->position ()->e()/GeV    <<  " "
     << "Asx/Asy" << Asx << "/" << Asy
     << endreq;
    
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

  const CaloPosition* pos = hypo->position() ;

  msg<< MSG::DEBUG
     << " ENE  " << hypo->position ()->e()/GeV    <<  " "
     << "xBar/yBar "   << xBar  <<  "/" << yBar   <<  " "
     << "xg/yg  "      << pos->x() << "/" << pos->y() <<  " "
     << "xNew/yNew "   << xCor <<  "/" << yCor    <<  " "
     << "xcel/ycel "   << seedPos.x() <<  "/" << seedPos.y() <<  " "
     << endreq ;
  
  
  /** At the end: 
   */
  
  //   if( 0 != hypo -> position () ) 
  //     { delete hypo->position () ; hypo->setPosition( (CaloPosition*) 0 ); }
  //   if( 0 != hypo -> momentum () ) 
  //     { delete hypo->momentum () ; hypo->setMomentum( (CaloMomentum*) 0 ); }
  // hypo->setPosition( position.clone() );
  CaloPosition::Parameters& parameters = hypo ->position() ->parameters () ;
  //  CaloPosition::Covariance& covariance = hypo ->position() ->covariance () ;

  /** The following quantities should be updated 
   */

  parameters ( CaloPosition::X ) = xCor ;
  parameters ( CaloPosition::Y ) = yCor ;

  msg<< MSG::DEBUG
     << "NEW xg/yg  "   << pos->x() << "/" << pos->y() <<  " "
     << endreq ;

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
