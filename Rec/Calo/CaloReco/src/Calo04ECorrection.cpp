// $Id: Calo04ECorrection.cpp,v 1.3 2004-06-03 06:04:23 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2004/05/06 16:32:21  odescham
// Minor changes (to be consistent with v1r2 revision of options file)
//
// Revision 1.1  2004/03/17 16:32:21  ibelyaev
//  add new (04) Photon calibrations from Olivier Deschamps
//
// Revision 1.4   2004/02/06 odescham
// New E-corrections for DC04
//
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
#include "Calo04ECorrection.h"

/** @file 
 *  Implementation file for class : Calo04ECorrection
 *  
 *  @date 2004-02-06
 *  @author Olivier Deschamps
 */


// ============================================================================
/*
 *   Declaration of the Tool Factory, needed for instantiation
 */
// ============================================================================
static const  ToolFactory<Calo04ECorrection>         s_Factory ;
const        IToolFactory&Calo04ECorrectionFactory = s_Factory ; 
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
Calo04ECorrection::Calo04ECorrection 
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent ) 
  : CaloTool( type , name , parent ) 
  , m_hypos  () 
  , m_hypos_ () 
  /// 
  , A0 ()
  , A1_a_nospd ()
  , A1_b_nospd ()
  , A1_c_nospd ()
  , A2_a_nospd ()
  , A2_b_nospd ()
  , A2_c_nospd ()
  , A2_d_nospd ()
  , A3x_Inn0_nospd ()
  , A3x_Inn1_nospd ()
  , A3x_Inn2_nospd ()
  , A3y_Inn0_nospd ()
  , A3y_Inn1_nospd ()
  , A3y_Inn2_nospd ()
  , A3x_Mid0_nospd ()
  , A3x_Mid1_nospd ()
  , A3y_Mid0_nospd ()
  , A3y_Mid1_nospd ()
  , A3x_Out0_nospd ()
  , A3y_Out0_nospd ()
  , A1_a_spd ()
  , A1_b_spd ()
  , A1_c_spd ()
  , A2_a_spd ()
  , A2_b_spd ()
  , A2_c_spd ()
  , A2_d_spd ()
  , A3x_Inn0_spd ()
  , A3x_Inn1_spd ()
  , A3x_Inn2_spd ()
  , A3y_Inn0_spd ()
  , A3y_Inn1_spd ()
  , A3y_Inn2_spd ()
  , A3x_Mid0_spd ()
  , A3x_Mid1_spd ()
  , A3y_Mid0_spd ()
  , A3y_Mid1_spd ()
  , A3x_Out0_spd ()
  , A3y_Out0_spd ()
  , BetaPrs_a_nospd()
  , BetaPrs_b_nospd()
  , BetaPrs_a_spd()
  , BetaPrs_b_spd()
  , SplitFactor ()
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
  declareProperty    ( "GlobalFactor" , A0 ) ;

  declareProperty    ( "Alpha1_NoSpdHit_a" , A1_a_nospd ) ;
  declareProperty    ( "Alpha1_NoSpdHit_b" , A1_b_nospd ) ;
  declareProperty    ( "Alpha1_NoSpdHit_c" , A1_c_nospd ) ;

  declareProperty    ( "Alpha2_NoSpdHit_a" , A2_a_nospd ) ;
  declareProperty    ( "Alpha2_NoSpdHit_b" , A2_b_nospd ) ;
  declareProperty    ( "Alpha2_NoSpdHit_c" , A2_c_nospd ) ;
  declareProperty    ( "Alpha2_NoSpdHit_d" , A2_d_nospd ) ;

  declareProperty    ( "Alpha3X_Inner_NoSpdHit_0" , A3x_Inn0_nospd ) ;
  declareProperty    ( "Alpha3X_Inner_NoSpdHit_1" , A3x_Inn1_nospd ) ;
  declareProperty    ( "Alpha3X_Inner_NoSpdHit_2" , A3x_Inn2_nospd ) ;
  declareProperty    ( "Alpha3Y_Inner_NoSpdHit_0" , A3y_Inn0_nospd ) ;
  declareProperty    ( "Alpha3Y_Inner_NoSpdHit_1" , A3y_Inn1_nospd ) ;
  declareProperty    ( "Alpha3y_Inner_NoSpdHit_2" , A3y_Inn2_nospd ) ;
  declareProperty    ( "Alpha3X_Middle_NoSpdHit_0" , A3x_Mid0_nospd ) ;
  declareProperty    ( "Alpha3X_Middle_NoSpdHit_1" , A3x_Mid1_nospd ) ;
  declareProperty    ( "Alpha3Y_Middle_NoSpdHit_0" , A3y_Mid0_nospd ) ;
  declareProperty    ( "Alpha3Y_Middle_NoSpdHit_1" , A3y_Mid1_nospd ) ;
  declareProperty    ( "Alpha3X_Outer_NoSpdHit_0" , A3x_Out0_nospd ) ;
  declareProperty    ( "Alpha3Y_Outer_NoSpdHit_0" , A3y_Out0_nospd ) ;

  declareProperty    ( "Alpha1_SpdHit_a" , A1_a_spd ) ;
  declareProperty    ( "Alpha1_SpdHit_b" , A1_b_spd ) ;
  declareProperty    ( "Alpha1_SpdHit_c" , A1_c_spd ) ;
  declareProperty    ( "Alpha2_SpdHit_a" , A2_a_spd ) ;
  declareProperty    ( "Alpha2_SpdHit_b" , A2_b_spd ) ;
  declareProperty    ( "Alpha2_SpdHit_c" , A2_c_spd ) ;
  declareProperty    ( "Alpha2_SpdHit_d" , A2_d_spd ) ;
  declareProperty    ( "Alpha3X_Inner_SpdHit_0" , A3x_Inn0_spd ) ;
  declareProperty    ( "Alpha3X_Inner_SpdHit_1" , A3x_Inn1_spd ) ;
  declareProperty    ( "Alpha3X_Inner_SpdHit_2" , A3x_Inn2_spd ) ;
  declareProperty    ( "Alpha3Y_Inner_SpdHit_0" , A3y_Inn0_spd ) ;
  declareProperty    ( "Alpha3Y_Inner_SpdHit_1" , A3y_Inn1_spd ) ;
  declareProperty    ( "Alpha3y_Inner_SpdHit_2" , A3y_Inn2_spd ) ;
  declareProperty    ( "Alpha3X_Middle_SpdHit_0" , A3x_Mid0_spd ) ;
  declareProperty    ( "Alpha3X_Middle_SpdHit_1" , A3x_Mid1_spd ) ;
  declareProperty    ( "Alpha3Y_Middle_SpdHit_0" , A3y_Mid0_spd ) ;
  declareProperty    ( "Alpha3Y_Middle_SpdHit_1" , A3y_Mid1_spd ) ;
  declareProperty    ( "Alpha3X_Outer_SpdHit_0" , A3x_Out0_spd ) ;
  declareProperty    ( "Alpha3Y_Outer_SpdHit_0" , A3y_Out0_spd ) ;
  declareProperty    ( "BetaPrs_NoSpdHit_a" , BetaPrs_a_nospd ) ;
  declareProperty    ( "BetaPrs_NoSpdHit_b" , BetaPrs_b_nospd ) ;
  declareProperty    ( "BetaPrs_SpdHit_a" , BetaPrs_a_spd ) ;
  declareProperty    ( "BetaPrs_SpdHit_b" , BetaPrs_b_spd ) ;
  declareProperty    ( "SplitPhotonFactor" , SplitFactor ) ;

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
Calo04ECorrection::~Calo04ECorrection () {} ;
// ============================================================================

// ============================================================================
/** finalization of the tool 
 *  @see  CaloTool 
 *  @see   AlgTool 
 *  @see  IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode Calo04ECorrection::finalize   () 
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
StatusCode Calo04ECorrection::initialize () 
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
  if( 6 != Level.size() ) 
    { return Error ( "Invalid number of parameters" ) ; }
  
  if( m_hypos.empty() ) 
    { return Error("Empty vector of allowed Calorimeter Hypotheses!" ) ; }
  
  // debug printout of all allowed hypos 
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
StatusCode Calo04ECorrection::operator() ( CaloHypo* hypo  ) const 
{ return process( hypo ); };
// ============================================================================

// ============================================================================
/** The main processing method
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode Calo04ECorrection::process    ( CaloHypo* hypo  ) const 
{
  MsgStream  msg( msgSvc(), name() );
  // avoid long names 

  msg << MSG::DEBUG << " Process Ecorrection : " << endreq;

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
      if ( *d == 0 ){ continue ; }
      else if( m_prs( *d ) ) { ePrsSum  += (*d)->e() ; } 
      else if( m_spd( *d ) ) { eSpdSum  += (*d)->e() ; } 
    }
  const CaloPosition& position = cluster->position();
  double eEcal = position. e () ;
  const double xBar  = position. x () ;
  const double yBar  = position. y () ;
  /*
     Informations from seed Digit Seed ID & position
  */
  

  const CaloCluster::Entries& entries = cluster->entries();
  CaloCluster::Entries::const_iterator iseed = 
    locateDigit ( entries.begin () , 
                  entries.end   () , CaloDigitStatus::SeedCell );
  
  // eEcal from 3x3 ...
  //  eEcal = 0;
  for(CaloCluster::Entries::const_iterator ient = entries.begin();
      ient != entries.end() ; ient++){
    const CaloDigit*  ent = ient->digit();
    CaloCellID     cellID = ent->cellID() ;
    //const CaloDigit*  seed = iseed->digit();
    // CaloCellID  seedID = seed->cellID() ;
    // unsigned int    seedCol   = seedID.col();
    // unsigned int    seedRow   = seedID.row();
    //unsigned int    seedArea  = seedID.area();
    unsigned int    Col   = cellID.col();
    unsigned int    Row   = cellID.row();
    unsigned int    Area  = cellID.area();
    //if(Area == seedArea && abs(Col-seedCol) <=1 && abs(Row-seedRow) <=1){
    //eEcal += ent->e();
    msg<< MSG::INFO
       << " ENE entries " << ent->e() << " "
       << "fraction " << ient->fraction()  << " "
       << "Id = "<<  Area  << "  " <<  Row << " " << Col
       << " status " << ient->status()
       << "For energy " << CaloDigitStatus::UseForEnergy 
       << endreq;
    
  }
  //  }


  if(  CaloHypotheses::PhotonFromMergedPi0 == hypo->hypothesis()){
    msg<< MSG::INFO << "Photon From Merged "<< endreq; }
  msg<< MSG::INFO << "ENTRIES " << entries.size() << endreq;
  msg<< MSG::INFO << "iseed-entries.end() " << entries.end()-iseed << endreq;
  for(CaloCluster::Entries::const_iterator iseed = entries.begin ();
      iseed != entries.end   () ; ++iseed){
  }

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
   *  (1) Ecal energy in 3x3     :    eEcal (MeV)
   *  (2) Prs and Spd energies   :    ePrs, eSpd (seed digit) & 
   *                                  ePrsSum, eSpdSum (cluster Sum)
   *  (3) weighted barycenter    :    xBar, yBar 
   *  (4) Zone/Area in Ecal      :    area   
   *  (5) SEED digit             :    seed    
   *  (6) CellID of seed digit   :    cellID  
   *  (7) Position of seed cell  :    seedPos 
   *
   */

  const int ShiftCol[3] = {  0 ,  0 ,  8 };
  const int ShiftRow[3] = {  6 , 12 , 14 };
   

  CaloCellID MainID = seed->cellID() ;
  HepPoint3D MainPos = det()->cellCenter( MainID );
  double CellSize =  det()->cellSize( cellID  );
  double Asx   = ( xBar - seedPos.x() ) / CellSize ;
  double Asy   = ( yBar - seedPos.y() ) / CellSize ;
  unsigned int    Col   = cellID.col()  - ShiftCol[area] +1;
  unsigned int    Row   = cellID.row()  - ShiftRow[area] +1;


  double bDist = sqrt( Asx*Asx + Asy*Asy) * sqrt ( 2. ) ;
  
  double A1=1;
  double A2=1;
  double A3=1;
  double A3x=1;
  double A3y=1;
  
  Parameters A1_a=A1_a_spd;
  Parameters A1_b=A1_b_spd;
  Parameters A1_c=A1_c_spd;
  Parameters A2_a=A2_a_spd;
  Parameters A2_b=A2_b_spd;
  Parameters A2_c=A2_c_spd;
  Parameters A2_d=A2_d_spd;
  if( 0 == eSpd ){
    A1_a=A1_a_nospd;
    A1_b=A1_b_nospd;
    A1_c=A1_c_nospd;
    A2_a=A2_a_nospd;
    A2_b=A2_b_nospd;
    A2_c=A2_c_nospd;
    A2_d=A2_d_nospd;
  }
  Parameters A3x_p;
  Parameters A3y_p;
  if( 2 == area && 0 == Col % (area+1) && 0 == eSpd ){A3x_p = A3x_Inn0_nospd;}
  if( 2 == area && 1 == Col % (area+1) && 0 == eSpd ){A3x_p = A3x_Inn1_nospd;}
  if( 2 == area && 2 == Col % (area+1) && 0 == eSpd ){A3x_p = A3x_Inn2_nospd;}
  if( 2 == area && 0 == Row % (area+1) && 0 == eSpd ){A3y_p = A3y_Inn0_nospd;}
  if( 2 == area && 1 == Row % (area+1) && 0 == eSpd ){A3y_p = A3y_Inn1_nospd;}
  if( 2 == area && 2 == Row % (area+1) && 0 == eSpd ){A3y_p = A3y_Inn2_nospd;}
  if( 1 == area && 0 == Col % (area+1) && 0 == eSpd ){A3x_p = A3x_Mid0_nospd;}
  if( 1 == area && 1 == Col % (area+1) && 0 == eSpd ){A3x_p = A3x_Mid1_nospd;}
  if( 1 == area && 0 == Row % (area+1) && 0 == eSpd ){A3y_p = A3y_Mid0_nospd;}
  if( 1 == area && 1 == Row % (area+1) && 0 == eSpd ){A3y_p = A3y_Mid1_nospd;}
  if( 0 == area && 0 == Col % (area+1) && 0 == eSpd ){A3x_p = A3x_Out0_nospd;}
  if( 0 == area && 0 == Row % (area+1) && 0 == eSpd ){A3y_p = A3y_Out0_nospd;}
  if( 2 == area && 0 == Col % (area+1) && 0 != eSpd ){A3x_p = A3x_Inn0_spd;}
  if( 2 == area && 1 == Col % (area+1) && 0 != eSpd ){A3x_p = A3x_Inn1_spd;}
  if( 2 == area && 2 == Col % (area+1) && 0 != eSpd ){A3x_p = A3x_Inn2_spd;}
  if( 2 == area && 0 == Row % (area+1) && 0 != eSpd ){A3y_p = A3y_Inn0_spd;}
  if( 2 == area && 1 == Row % (area+1) && 0 != eSpd ){A3y_p = A3y_Inn1_spd;}
  if( 2 == area && 2 == Row % (area+1) && 0 != eSpd ){A3y_p = A3y_Inn2_spd;}
  if( 1 == area && 0 == Col % (area+1) && 0 != eSpd ){A3x_p = A3x_Mid0_spd;}
  if( 1 == area && 1 == Col % (area+1) && 0 != eSpd ){A3x_p = A3x_Mid1_spd;}
  if( 1 == area && 0 == Row % (area+1) && 0 != eSpd ){A3y_p = A3y_Mid0_spd;}
  if( 1 == area && 1 == Row % (area+1) && 0 != eSpd ){A3y_p = A3y_Mid1_spd;}
  if( 0 == area && 0 == Col % (area+1) && 0 != eSpd ){A3x_p = A3x_Out0_spd;}
  if( 0 == area && 0 == Row % (area+1) && 0 != eSpd ){A3y_p = A3y_Out0_spd;}



  

  double elog=0;
  if(0 < eEcal)elog =log(eEcal/GeV);
  //  if( Level[0] && 0 < ePrsSum  ){ // Modif Mai 2004 : no Prs requirement
  if( Level[0] ){
    A1 = 1./(A1_a[area]+A1_b[area]*elog+A1_c[area]*elog*elog); 
  }
  // Lateral correction
  if( Level[1] )
  {A2 = 1./(A2_a[area]+A2_b[area]*bDist
            + A2_c[area]*bDist*bDist+A2_d[area]*bDist*bDist*bDist);}
  // Module Frame correction
  if( Level[2] ){
  A3x = 1./ (A3x_p[0] + A3x_p[1] * Asx + A3x_p[2] * Asx*Asx);
  A3y = 1./ (A3y_p[0] + A3y_p[1] * Asy + A3y_p[2] * Asy*Asy);
  A3  = A3x * A3y ;
  }
  // Apply all Ecal corrections
  double Alpha = A1 * A2 * A3 ;
  double Ecor  = Alpha * eEcal ;

  // Preshower Leakage 
  Parameters Beta_a = BetaPrs_a_spd;
  Parameters Beta_b = BetaPrs_b_spd;
  if( 0 == eSpd ){
    Parameters Beta_a=BetaPrs_a_nospd;
    Parameters Beta_b=BetaPrs_b_nospd;
  }
  double Eps = ePrsSum;
  // Avoid double counting (should be corrected in CaloMergedPi0Alg)
  if( CaloHypotheses::PhotonFromMergedPi0 == hypo->hypothesis()  ){Eps = ePrs;}
  double Beta = Beta_a[area] + Beta_b[area] * elog; // Modif Mai 2004 (elog <-> Ecor/GeV)
  if( Level[3] ){Ecor  += Beta * Eps ;}
  

  // Split Photon rescaling
  if( Level[5] && CaloHypotheses::PhotonFromMergedPi0 == hypo->hypothesis()  ){
    Ecor = Ecor / SplitFactor[area]; }


  if( Level[4] ){ Ecor = Ecor / A0[area] ;  } 


  if(  CaloHypotheses::PhotonFromMergedPi0 == hypo->hypothesis()){
    msg<< MSG::INFO << "Photon From Merged "<< endreq;}
    msg<< MSG::INFO
     << " Main Cluster " << cluster->position().e()/GeV    <<  " "
     << "area "  << area  <<  " "
     << "Row "  << Row <<  " "
     << "Col "  << Col <<  " "
     << "Asx "  << Asx <<  " "
     << "Asy "  << Asy <<  " "
     << "eSpd "  << eSpd <<  " "
     << "ePrs "  << ePrs  <<  " "
     << "eSpdSum "  << eSpdSum <<  " "
     << "ePrsSum "  << ePrsSum  <<  " "
     << "eEcal " << eEcal/GeV <<  " "
     << "Ecor "  <<  Ecor/GeV <<  " "
     << "A0 "  <<  A0[area] <<  " "
     << "A1 "  <<  A1 <<  " "
     << "A2 "  <<  A2 <<  " "
     << "A3 "  <<  A3 <<  " "
     << "A3x "  <<  A3x <<  " "
     << "A3y "  <<  A3y <<  " "
     << "bDist "  <<  bDist <<  " "
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
