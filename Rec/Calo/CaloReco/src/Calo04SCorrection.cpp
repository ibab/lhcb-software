// $Id: Calo04SCorrection.cpp,v 1.2 2004-03-19 13:15:39 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2004/03/17 16:32:21  ibelyaev
//  add new (04) Photon calibrations from Olivier Deschamps
//
// Revision 1.4  2004/02/09 odescham
//  add new E-,S- and L-corrections tuned for DC04
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
#include "Calo04SCorrection.h"

/** @file 
 *  Implementation file for class : Calo04SCorrection
 *  
 *  @date 2004-02-09
 *  @author Olivier Deschamps
 */


// ============================================================================
/** @var Calo04SCorrection
 *  Declaration of the Tool Factory, needed for instantiation
 */
// ============================================================================
static const  ToolFactory<Calo04SCorrection>         s_Factory ;
const        IToolFactory&Calo04SCorrectionFactory = s_Factory ; 
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
Calo04SCorrection::Calo04SCorrection 
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent ) 
  : CaloTool( type , name , parent ) 
  , m_hypos  () 
  , m_hypos_ () 
  /// 
  , Par_Asinh_01  ()
  , Par_Asinh_00  ()
  , Par_ResOut_0 ()
  , Par_ResMid_0 ()
  , Par_ResInn_0 ()
  , Par_AsOut_0 ()
  , Par_AsMid_0 ()
  , Par_AsInn_0 ()
  , Par_Asinh_11  ()
  , Par_ResOut_1 ()
  , Par_ResMid_1 ()
  , Par_ResInn_1 ()
  , Par_AsOut_1 ()
  , Par_AsMid_1 ()
  , Par_AsInn_1 ()
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
  declareProperty    ( "Par_Asinh_00" , Par_Asinh_00  ) ;
  declareProperty    ( "Par_ResOut_0", Par_ResOut_0 ) ;
  declareProperty    ( "Par_ResMid_0", Par_ResMid_0 ) ;
  declareProperty    ( "Par_ResInn_0", Par_ResInn_0 ) ;
  declareProperty    ( "Par_AsOut_0" , Par_AsOut_0  ) ;
  declareProperty    ( "Par_AsMid_0" , Par_AsMid_0  ) ;
  declareProperty    ( "Par_AsInn_0" , Par_AsInn_0  ) ;
  declareProperty    ( "Par_Asinh_11" , Par_Asinh_11  ) ;
  declareProperty    ( "Par_Asinh_01" , Par_Asinh_01  ) ;
  declareProperty    ( "Par_ResOut_1", Par_ResOut_1 ) ;
  declareProperty    ( "Par_ResMid_1", Par_ResMid_1 ) ;
  declareProperty    ( "Par_ResInn_1", Par_ResInn_1 ) ;
  declareProperty    ( "Par_AsOut_1" , Par_AsOut_1  ) ;
  declareProperty    ( "Par_AsMid_1" , Par_AsMid_1  ) ;
  declareProperty    ( "Par_AsInn_1" , Par_AsInn_1  ) ;
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
Calo04SCorrection::~Calo04SCorrection () {} ;
// ============================================================================

// ============================================================================
/** finalization of the tool 
 *  @see  CaloTool 
 *  @see   AlgTool 
 *  @see  IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode Calo04SCorrection::finalize   () 
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
StatusCode Calo04SCorrection::initialize () 
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
  
  // Check parameter size
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
StatusCode Calo04SCorrection::operator() ( CaloHypo* hypo  ) const 
{ return process( hypo ); };
// ============================================================================

// ============================================================================
/** The main processing method
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode Calo04SCorrection::process    ( CaloHypo* hypo  ) const 
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
  double ePrsSum = 0 ;
  double eSpdSum = 0 ;
  const Digits& digits = hypo->digits();
  for( Digits::const_iterator d = digits.begin() ; digits.end() != d ; ++d ) 
    { 
      if     ( *d == 0     ) { continue           ; }
      else if( m_prs( *d ) ) { ePrsSum  += (*d)->e() ; } 
      else if( m_spd( *d ) ) { eSpdSum  += (*d)->e() ; } 
    }
   const CaloPosition& position = cluster->position();
   //  const double eEcal = position. e () ;
  const double xBar  = position. x () ;
  const double yBar  = position. y () ;



  /*
     Informations from seed Digit Seed ID & position
  */
  const CaloCluster::Entries& entries = cluster->entries();
  CaloCluster::Entries::const_iterator iseed = 
    locateDigit ( entries.begin () , 
                  entries.end   () , CaloDigitStatus::SeedCell );
  if( entries.end() == iseed )
    { return Error ( "The seed cell is not found !" ) ; }
  // get the "area" of the cluster (where seed is placed) 
  const unsigned int area = m_area( cluster );
  const CaloDigit*  seed    = iseed->digit();
  if( 0 == seed ) { return Error ( "Seed digit points to NULL!" ) ; }
  // Cell ID for seed digit 
  CaloCellID cellID = seed->cellID() ;
  // position of the SEED 
  HepPoint3D seedPos = det()->cellCenter( cellID  );
  // Spd/Prs digit in front of cluster seed
  double ePrs = 0 ;
  double eSpd = 0 ;
  for( Digits::const_iterator d = digits.begin() ; digits.end() != d ; ++d ) 
  { 
    const CaloDigit* dig = *d;
    if ( *d == 0 ){ continue ; }
    else if( m_prs( *d ) 
             && cellID.area() == dig->cellID().area()
             && cellID.row() == dig->cellID().row()
             && cellID.col() == dig->cellID().col() ){ ePrs = (*d)->e() ; } 
    else if( m_spd( *d )
             && cellID.area() == dig->cellID().area()
             && cellID.row() == dig->cellID().row()
             && cellID.col() == dig->cellID().col() ){ eSpd = (*d)->e() ; } 
  }
  
  /** here all informations are available 
   *     
   *  (1) Ecal energy in 3x3     :    eEcal 
   *  (2) Prs and Spd energies   :    ePrs, eSpd (seed digit) & 
   *                                  ePrsSum, eSpdSum (cluster Sum)
   *  (3) weighted barycenter    :    xBar, yBar 
   *  (4) Zone/Area in Ecal      :    area   
   *  (5) SEED digit             :    seed   
   *  (6) CellID of seed digit   :    cellID
   *  (7) Position of seed cell  :    seedPos 
   */

  double CellSize =  det()->cellSize( cellID  );
  double Asx   = - ( xBar - seedPos.x() ) / CellSize ;
  double Asy   = - ( yBar - seedPos.y() ) / CellSize ;
  double Delta = 0.5;

  Parameters Par_Asinh;
  if( 0 == eSpd && 0 >= ePrs ){Par_Asinh = Par_Asinh_00 ; }
  if( 0 == eSpd && 0 <  ePrs ){Par_Asinh = Par_Asinh_01 ; }
  if( 0 != eSpd ){Par_Asinh = Par_Asinh_11 ; }
  
   
  // Analytical barycenter for exponential decay shower profile
  double Argx = Asx / Delta * cosh( Delta / Par_Asinh[area] );
  double Argy = Asy / Delta * cosh( Delta / Par_Asinh[area] );
  if(Level[0]){
    Asx = Par_Asinh[area] * log( Argx + sqrt( Argx*Argx + 1.) );
    Asy = Par_Asinh[area] * log( Argy + sqrt( Argy*Argy + 1.) );
  }
  
  // Residual asymmetries due to single exponential approximation
  Parameters Par_Res;
  if( 0 == eSpd ){
    if( 0 == area){ Par_Res = Par_ResOut_0 ; }
    if( 1 == area){ Par_Res = Par_ResMid_0 ; }
    if( 2 == area){ Par_Res = Par_ResInn_0 ; }
  }
  if( 0 != eSpd ){
    if( 0 == area){ Par_Res = Par_ResOut_1 ; }
    if( 1 == area){ Par_Res = Par_ResMid_1 ; }
    if( 2 == area){ Par_Res = Par_ResInn_1 ; }
  }
  
  double DAsx=0.;
  double DAsy=0.;
  {for (unsigned int i = 0 ; i !=Par_Res.size() ; ++i){
    DAsx += Par_Res[i] * pow(Asx,int(i));
    DAsy += Par_Res[i] * pow(Asy,int(i));}
  }
  
  if(Level[1]){
    Asx -= DAsx ;
    Asy -= DAsy ;
  }
  
  // Geometrical asymmetries due to non symmetrical shower spot 
  // Left/Right for (X) & Up/Down for (Y)
  Parameters Par_As;
  if( 0 == eSpd ){
    if( 0 == area){ Par_As = Par_AsOut_0 ; }
    if( 1 == area){ Par_As = Par_AsMid_0 ; }
    if( 2 == area){ Par_As = Par_AsInn_0 ; }
  }
  if( 0 != eSpd ){
    if( 0 == area){ Par_As = Par_AsOut_1 ; }
    if( 1 == area){ Par_As = Par_AsMid_1 ; }
    if( 2 == area){ Par_As = Par_AsInn_1 ; }
  }
  Parameters Par_AsX = Par_As;
  Parameters Par_AsY = Par_As;
  if ( 0 < xBar ) {
    Par_AsX[0] = - Par_As[0];
    Par_AsX[2] = - Par_As[2];
  }
  if ( 0 < yBar ) {
    Par_AsY[0] = - Par_As[0];
    Par_AsY[2] = - Par_As[2];
  }
  
  double DDAsx = 0.;
  double DDAsy = 0.;
  {for (unsigned int i = 0 ; i !=Par_As.size() ; ++i){
    DDAsx += Par_AsX[i]   * pow(Asx,int(i));
    DDAsy += Par_AsY[i]   * pow(Asy,int(i)); }
  } // Fix VC6 scoping bug
  if(Level[2]){
    Asx += DDAsx;
    Asy += DDAsy;
  }
  
  // Recompute position and fill CaloPosition
  double xCor = seedPos.x() - Asx * CellSize;
  double yCor = seedPos.y() - Asy * CellSize;

  const CaloPosition* pos = hypo->position() ;

  msg << MSG::DEBUG
      << "area "   << area <<  " "
      << "xBar/yBar "   << xBar  <<  "/" << yBar   <<  " "
      << "eSpd/ePrs "   << eSpd << "/" << ePrs <<  " "
      << "Asx/Asy "     << - ( xBar - seedPos.x() ) / CellSize  
      <<  "/" << - ( yBar - seedPos.y() ) / CellSize<<  " "
      << "New Asx/Asy "     << Asx <<  "/" << Asy
      << "DAsx/DAsy "     << DAsx <<  "/" << DAsy
      << "DDAsx/DDAsy "     << DDAsx <<  "/" << DDAsy
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
