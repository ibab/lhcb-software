// $Id: Calo04LCorrection.cpp,v 1.1 2004-03-17 16:32:21 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/02/09 odescham
//  add new E-,S- and L-corrections tuned for DC04
//
// Revision 1.2  2003/05/16 08:19:11  cattanem
// remove unused variables
//
// Revision 1.1  2003/04/11 09:33:37  ibelyaev
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
#include "Calo04LCorrection.h"

/** @file 
 *  Implementation file for class : Calo04LCorrection
 *  
 *  @date 2004-02-09
 *  @author Olivier Deschamps 
 */


// ============================================================================
/** @var Calo04LCorrection
 *  Declaration of the Tool Factory, needed for instantiation
 */
// ============================================================================
static const  ToolFactory<Calo04LCorrection>         s_Factory ;
const        IToolFactory&Calo04LCorrectionFactory = s_Factory ; 
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
Calo04LCorrection::Calo04LCorrection 
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent ) 
  : CaloTool( type , name , parent ) 
  , m_hypos  () 
  , m_hypos_ () 
  /// 
  , Par_Al1 ()
  , Par_Al2 ()
  , Par_Al3 ()
  , Par_Be0 ()
  , Par_Be1 ()
  , Par_Be2 ()
  , Par_Be3 ()
  , Par_z0  ()
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
  declareProperty    ( "Par_Al1" , Par_Al1 ) ;
  declareProperty    ( "Par_Al2" , Par_Al2 ) ;
  declareProperty    ( "Par_Al3" , Par_Al3 ) ;
  declareProperty    ( "Par_Be0" , Par_Be0 ) ;
  declareProperty    ( "Par_Be1" , Par_Be1 ) ;
  declareProperty    ( "Par_Be2" , Par_Be2 ) ;
  declareProperty    ( "Par_Be3" , Par_Be3 ) ;
  declareProperty    ( "Par_z0"  , Par_z0  ) ;

  /// det the default detector 
  setDetName         ( DeCalorimeterLocation::Ecal ) ;
  /// interafces 
  declareInterface<ICaloHypoTool> ( this ) ;  
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
Calo04LCorrection::~Calo04LCorrection () {} ;
// ============================================================================

// ============================================================================
/** finalization of the tool 
 *  @see  CaloTool 
 *  @see   AlgTool 
 *  @see  IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode Calo04LCorrection::finalize   () 
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
StatusCode Calo04LCorrection::initialize () 
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
  if( 3 != Par_Al1.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 3 != Par_Al2.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 3 != Par_Al3.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  if( 3 != Par_Be0.size() ) 
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
StatusCode Calo04LCorrection::operator() ( CaloHypo* hypo  ) const 
{ return process( hypo ); };
// ============================================================================

// ============================================================================
/** The main processing method
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode Calo04LCorrection::process    ( CaloHypo* hypo  ) const 
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
   const double eEcal = position. e () ;

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
   *  (2) Prs and Spd energies   :    ePrs, eSpd 
   *                       (seed digit) & ePrsSum, eSpdSum (cluster Sum)
   *  (3) weighted barycenter    :    xBar, yBar 
   *  (4) Zone/Area in Ecal      :    area   
   *  (5) SEED digit             :    seed   (NOT FOR SPLITPHOTONS !!)
   *  (6) CellID of seed digit   :    cellID
   *  (7) Position of seed cell  :    seedPos 
   */

  //  double CellSize =  det()->cellSize( cellID  );
  
  //  double z0 = seedPos.z();
  double z0 = Par_z0[0] ;// Parameter tuned wrt to z0=12566 mm !!
  double Zvtx = 0.   ;// Origin vertex (z) set to 0 !!
  const CaloPosition* pos = hypo->position() ;
  double  xg = pos->x();
  double  yg = pos->y();
  double  zg = z0 - Zvtx;
    

  // Uncorrected angle
  double tth   = sqrt ( pow(xg,2) + pow(yg,2) ) / zg ;
  double cth   = cos  ( atan( tth ) ) ;

  // Corrected angle
  double alpha = Par_Al1[area] + Par_Al2[area]*ePrs + Par_Al3[area] * ePrs*ePrs;
  double beta  = Par_Be1[area] + Par_Be2[area]*ePrs + Par_Be3[area] * ePrs*ePrs;
  if(0 >= ePrs ){beta = Par_Be0[area]; }
  double tgfps = alpha * log(eEcal/GeV) + beta ;
  tth = tth / ( 1. + tgfps * cth / zg ) ;
  cth= cos( atan( tth ) ) ;
  double dzfps = cth * tgfps ;
// Recompute Z position and fill CaloPosition
  double zCor = z0 + dzfps;

   msg<< MSG::DEBUG
     << " ENE  " << hypo->position ()->e()/GeV <<  " "
      << "ePrs "   << ePrs <<  " "
      << "eEcal "   << eEcal <<  " "
      << "xg "   << xg <<  " "
      << "yg "   << yg <<  " "
      << "zg "   << pos->z() <<  " "
      << "z0 "   << z0 <<  " "
      << "DeltaZ "   << dzfps <<  " "
      << "zCor "   << zCor <<  " "
      << "alpha "  << alpha <<  " "
      << "beta "   << beta <<  " "
      << "tgfps "   << tgfps <<  " "
      << "tth "   << tth <<  " "
      << "cth "   << cth <<  " "
      << endreq ;
  

  /** At the end: 
   */
  
   //   if( 0 != hypo -> position () ) 
   //   { delete hypo->position () ; hypo->setPosition( (CaloPosition*) 0 ); }
   //if( 0 != hypo -> momentum () ) 
   //  { delete hypo->momentum () ; hypo->setMomentum( (CaloMomentum*) 0 ); }
   //  hypo->setPosition( position.clone() );

   // CaloPosition::Parameters& parameters = hypo ->position() ->parameters () ;
   // CaloPosition::Covariance& covariance = hypo ->position() ->covariance () ;

  /** The following quantities should be updated 
   */

  hypo -> position() -> setZ( zCor ) ;

   msg<< MSG::DEBUG
      << " ENE  " << hypo->position ()->e() <<  " "
      << "New zg "   << pos->z() <<  " "
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
