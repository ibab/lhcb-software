// $Id: NeutralPPsFromCPsAlg.cpp,v 1.1.1.1 2005-05-31 13:03:31 ibelyaev Exp $
// ============================================================================
// CVS Tag $Name: not supported by cvs2svn $ , version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2005/05/12 09:55:56  ibelyaev
//  remoev calo-associators
//
// ============================================================================
// Include files
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IAssociatorWeighted.h"
// ============================================================================
// from STL
// ============================================================================
#include <string>
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
// ============================================================================
// Kernel 
// ============================================================================
#include "Kernel/CaloHypoPrint.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloParticle.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/ProtoParticle.h"
#include "Event/TrStoredTrack.h"
// ============================================================================
// from CaloInterfaces
// ============================================================================
#include "CaloInterfaces/ICaloHypoLikelihood.h"
// ============================================================================
// from CaloKernel
// ============================================================================
#include "CaloKernel/CaloAlgorithm.h"
// ============================================================================
// CaloUtils
// ============================================================================
#include "CaloUtils/ClusterFunctors.h"
// ============================================================================


/** @class NeutralPPsFromCPsAlg1 NeutralPPsFromCPsAlg1.h
 *  
 *  Creator of the neutral ProtoParticles from CaloParticles
 *
 *  The current version fill following estimators for ProtoParticle
 *  
 *  <li>  <c>CaloTrMatch</c>     as <b>minimal</b> of this estimator for all 
 *        linked <c>CaloHypo</c> objects. The value is extracted from 
 *        the relation table/associator as a relation weigth between 
 *        <c>CaloCluster</c> and <c>TrStoredTrack</c> objects </li>
 *  <li>  <c>CaloDepositID</c>   as <b>maximal</b> of this estimator for all 
 *        linked <c>CaloHypo</c> objects using Spd/Prs estimator tool 
 *        written by Frederic Machefert </li>
 *  <li>  <c>CaloShowerShape</c> as <b>maximal</b> of the estimator for 
 *        all linked <c>CaloHypo</c> objects. Estimator is equal to the 
 *        sum of diagonal elements of cluster spread matrix (2nd order 
 *        moments of the cluster) </li>
 *  <li>  <c>ClusterMass</c>     as <b>maximal</b> of the estimator of 
 *        cluster mass using smart algorithm by Olivier Deschamp </li>
 *  <li>  <c>PhotonID</c>        as the estimator of PhotonID
 *        using nice identifiaction tool 
 *        CaloPhotonEstimatorTool by Frederic Machefert * 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-20
 */

class NeutralPPsFromCPsAlg1 : public CaloAlgorithm 
{
  // friend factory for instantiation
  friend class AlgFactory<NeutralPPsFromCPsAlg1>;
protected:
  typedef const IRelationWeighted<CaloCluster,TrStoredTrack,float> Table  ;
  typedef Table::Range                                             Range  ;
public:
  /** standard initialization method 
   *  @see CaloAlgorithm
   *  @return statsu code 
   */
  virtual StatusCode initialize () ;
  /** standard execution  method 
   *  @return statsu code 
   */
  virtual StatusCode execute    () ;
protected:
  /** Standard constructor
   *  @param name algorithm instance name 
   *  @param pSvc service locator 
   */
  NeutralPPsFromCPsAlg1
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloAlgorithm ( name , pSvc ) 
    , m_hyposLong   ()
    , m_hypos       ()
    ///
    , m_matchName    ("Rec/Calo/PhotonMatch")
    ///
    , m_spdprsType   ("CaloSingleGammaTool")
    , m_spdprsName   ("SpdPrsID"   )
    , m_spdprs       ( 0           )
    ///
    , m_photonIDType ( "CaloPhotonEstimatorTool")
    , m_photonIDName ( "PhotonPID"  )
    , m_photonID     ( 0            )
    ///
    , m_caloTrMatch_bad   (  1.e+6 )
    , m_caloDepositID_bad ( -1.e+6 )
    , m_showerShape_bad   ( -1.e+6 )
    , m_clusterMass_bad   ( -1.e+6 )
    , m_photonID_bad      ( -1.e+6 )
    ///
    , m_calo              ( DeCalorimeterLocation:: Ecal )
  {
    m_hyposLong.push_back( (long) CaloHypotheses::Photon              ) ;
    // m_hyposLong.push_back( (long) CaloHypotheses::Pi0Merged           ) ;
    // m_hyposLong.push_back( (long) CaloHypotheses::PhotonFromMergedPi0 ) ;
    // declare the properties 
    declareProperty( "Hypos"        , m_hyposLong    ) ;
    declareProperty( "MatchingName" , m_matchName    ) ;
    declareProperty( "SpdPrsIDType" , m_spdprsType   ) ;
    declareProperty( "SpdPrsIDName" , m_spdprsName   ) ;
    declareProperty( "PhotonIDType" , m_photonIDType ) ;
    declareProperty( "PhotonIDName" , m_photonIDName ) ;
    
    // set the appropriate default for input data
    setInputData  ( CaloParticleLocation::  Default  ) ;
    // set the appropriate default for output data
    setOutputData ( ProtoParticleLocation:: Neutrals ) ;
    // set appropriate defaut for detector data 
    setDetData    ( DeCalorimeterLocation:: Ecal    ) ;
  };
  /// destructor 
  virtual ~NeutralPPsFromCPsAlg1( ){};
private :  
  /** helpful technical method to evaluate 
   *  the ProtoParticle::CaloTrMatch estimator
   *  @see ProtoParticle
   *  @param hypo  pointer to the hypo 
   *  @param table pointer to matching relation table 
   *  @return the value of the estimator 
   */
  double caloTrMatch   ( const  CaloHypo* hypo  , 
                         const Table*     table )  const ;
  /** helpful technical method to evaluate 
   *  the ProtoParticle::ClusterMass estimator
   *  @see ProtoParticle
   *  @param hypo  pointer to the hypo 
   *  @return the value of the estimator 
   */
  double clusterMass   ( const CaloHypo*  hypo  )  const ;
  /** helpful technical method to evaluate 
   *  the ProtoParticle::ShowerShape estimator
   *  @see ProtoParticle
   *  @param hypo  pointer to the hypo 
   *  @return the value of the estimator 
   */
  double showerShape   ( const CaloHypo*  hypo  )  const ;
  /** helpful technical method to evaluate 
   *  the ProtoParticle::CaloDepositID estimator
   *  @see ProtoParticle
   *  @param hypo  pointer to the hypo 
   *  @return the value of the estimator 
   */
  double caloDepositID ( const CaloHypo*  hypo  )  const ;
  /** helpful technical method to evaluate 
   *  the ProtoParticle::PhotonID estimator
   *  @see ProtoParticle
   *  @param hypo  pointer to the hypo 
   *  @return the value of the estimator 
   */
  double photonID      ( const CaloHypo*  hypo  )  const ;
private:
  // default constructor  is private 
  NeutralPPsFromCPsAlg1();
  // copy    constructor  is private 
  NeutralPPsFromCPsAlg1           ( const NeutralPPsFromCPsAlg1& );
  // assignement operator is private 
  NeutralPPsFromCPsAlg1& operator=( const NeutralPPsFromCPsAlg1& );
private:
  // hypotheses to be processes
  std::vector<long>                        m_hyposLong         ;
  std::vector<CaloHypotheses::Hypothesis>  m_hypos             ;
  //
  std::string                              m_matchName         ;
  //
  std::string                              m_spdprsType        ;
  std::string                              m_spdprsName        ;
  ICaloHypoLikelihood*                     m_spdprs            ;
  //
  std::string                              m_photonIDType      ;
  std::string                              m_photonIDName      ;
  ICaloHypoLikelihood*                     m_photonID          ;
  //
  double                                   m_caloTrMatch_bad   ;
  double                                   m_caloDepositID_bad ;
  double                                   m_showerShape_bad   ;
  double                                   m_clusterMass_bad   ;
  double                                   m_photonID_bad      ;
  //
  ClusterFunctors::ClusterFromCalo         m_calo              ;  
};
// ============================================================================

// ============================================================================
/// anonymous namespace to hyde concrete algorithm factory
// ============================================================================
namespace 
{
  // =========================================================================
  /** @var s_Factory
   *  concrete local algorithm factory for caretion of
   *  NeutralPPsFromCPsAlg1 
   */
  // =========================================================================
  const  AlgFactory<NeutralPPsFromCPsAlg1>         s_Factory ;
};
// ===========================================================================
/** @var  NeutralPPsFromCPsAlg1Factory
 *  Declaration of the Algorithm Factory
 */
// ==========================================================================
const   IAlgFactory&NeutralPPsFromCPsAlg1Factory = s_Factory ; 
// ============================================================================

// ============================================================================
/** standard initialization method 
 *  @see CaloAlgorithm
 *  @return statsu code 
 */
// ============================================================================
StatusCode NeutralPPsFromCPsAlg1::initialize() 
{
  // initialize the base class 
  StatusCode sc = CaloAlgorithm::initialize() ;
  if( sc.isFailure() )
  { return Error( "CaloAlgorithm base class  is not initialized" , sc ) ; }
  
  for ( std::vector<long>::const_iterator value = m_hyposLong.begin() ; 
        m_hyposLong.end() != value ; ++value ) 
  {
    if( *value < (long) CaloHypotheses::Undefined || 
        *value > (long) CaloHypotheses::Other      ) 
    { return Error("Unknown calorimeter hypothesis!"); }
    CaloHypotheses::Hypothesis hypo =  (CaloHypotheses::Hypothesis) *value ;
    m_hypos.push_back( hypo );
    info() << " Hyposesis accepted: " ;
    CaloHypoPrint( info() , hypo ) ;
    info() << endreq ;
  };
  
  // locate spdprs
  ///m_spdprs   = tool< ICaloHypoLikelihood>( m_spdprsType , m_spdprsName ) ;
  ///if( 0 == m_spdprs ) { return StatusCode::FAILURE ; }
  
  // locate photon ID 
  ///m_photonID = tool< ICaloHypoLikelihood>( m_photonIDType , m_photonIDName ) ;
  ///if( 0 == m_spdprs ) { return StatusCode::FAILURE ; }
  
  // redefine the  detector 
  m_calo.setCalo ( detData() );
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard execution  method 
 *  @return statsu code 
 */
// ============================================================================
StatusCode NeutralPPsFromCPsAlg1::execute() 
{
  /// avoid long names 
  typedef const CaloParticles                                      CPs    ;
  typedef const CaloParticle::Hypos                                Hypos  ;
  
  /// create and register the output container 
  ProtoParticles* pprtcls = new ProtoParticles();
  StatusCode sc = put( pprtcls , outputData() ) ;
  if ( sc.isFailure() ) { return StatusCode::FAILURE ; }           // RETURN 
  
  /// get input container of CaloParticles
  const CPs* cprtcls = get< CPs >( inputData());
  if ( 0 == cprtcls   ) { return StatusCode::FAILURE ; }           // RETURN 
  
  /// get the relation table 
  //const Table* table = get<Table> ( m_matchName ) ;
  //if ( 0 == table     ) { return Error("Table* points to NULL!");} // RETURN 
  
  /// loop over all caloparticles and create the protoparticles  
  for ( CaloParticles::const_iterator icp = cprtcls->begin() ;
        cprtcls->end() != icp  ; ++icp )
  {
    const CaloParticle* cp = *icp ;
    // skip nulls 
    if( 0 == cp             ) { continue ; }                  // CONTINUE
    // set some "initial values" 
    ProtoParticle* pp             =  0                   ;
    double         caloTrMatch_   =  m_caloTrMatch_bad   ;
    double         caloDepositID_ =  m_caloDepositID_bad ;
    double         showerShape_   =  m_showerShape_bad   ;
    double         clusterMass_   =  m_clusterMass_bad   ;
    double         photonID_      =  m_photonID_bad      ;
    
    // get all hypos, and loop over all "allowed" hypos  
    const Hypos& hypos = cp->hypos();
    for ( Hypos::const_iterator ihypo = hypos.begin() ; 
          hypos.end() != ihypo ; ++ihypo )
    {
      const CaloHypo* hypo = *ihypo ;
      // skip invalid hypos 
      if( 0 == hypo     ) { continue ; }                    // CONTINUE 
      // valid hypothesis ? 
      if( m_hypos.end() == 
          std::find( m_hypos.begin    () , 
                     m_hypos.end      () , 
                     hypo->hypothesis () ) ) { continue ; } // CONTINUE 
      //
      // add it into the "current" ProtoParticle!
      if ( 0 == pp ) { pp = new ProtoParticle() ; }
      pp->addToCalo( hypo );
      
//       { // process with Chi2Match estimators
//         const double chi2           = caloTrMatch( hypo , table );
//         // select MINIMAL chi2 matching estimator  
//         if( chi2 < caloTrMatch_   ) { caloTrMatch_ = chi2 ; }
//       } // end of Chi2Match estimators
      
//       { // process with spdPrsID estimator 
//         const double dep            = caloDepositID( hypo ) ;
//         // select MAXIMAL Spd/Prs estimator 
//         if( caloDepositID_ < dep  ) { caloDepositID_ = dep ; }
//       } // end of spdPrsID processing 
      
//       { // process with ShowerShape estimator 
//         const double shape          = showerShape( hypo );
//         // select MAXIMAL shower shape estimator 
//         if(  showerShape_ < shape ) { showerShape_ = shape ; }            
//       } // end of ShowerShape  processing 
      
//       { // process with ClusterMass estimator 
//         const double mass           = clusterMass( hypo );
//         // select MAXIMAL cluster mass estimator 
//         if( clusterMass_  < mass  ) { clusterMass_ = mass ; }
//       } // end of ClusterMass processing
      
//       { // process with PhotonID estimator 
//         const double phID          = photonID  ( hypo );
//         // select MAXIMAL photon ID estimator 
//         if( photonID_ < phID  ) { photonID_ = phID ; }
//         // end of Phtoon ID  processing
//       }             
          
    } // end of loop over all hypos for given CaloParticle
    
      // ProtoParticle was not created ? 
    if ( 0 == pp ) { continue ; }                        // CONTINUE 
    // insert new protoparticle into container 
    pprtcls->insert( pp ) ;
    // fill protoparticle 
//     pp-> pIDDetectors().
//       push_back( std::make_pair( ProtoParticle::CaloTrMatch   , 
//                                  caloTrMatch_                 ) );
//     pp-> pIDDetectors().
//       push_back( std::make_pair( ProtoParticle::CaloDepositID , 
//                                  caloDepositID_               ) );
//     pp-> pIDDetectors().
//       push_back( std::make_pair( ProtoParticle::ShowerShape   , 
//                                  showerShape_                 ) );
//     pp-> pIDDetectors().
//       push_back( std::make_pair( ProtoParticle::ClusterMass   , 
//                                  clusterMass_                 ) );
//     pp-> pIDDetectors().
//       push_back( std::make_pair( ProtoParticle::PhotonID      , 
//                                  photonID_                    ) );
  }
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** helpful technical method to evaluate 
 *  the ProtoParticle::CaloTrMatch etimator
 *  @see ProtoParticle
 *  @return the value of the estimator 
 */
// ============================================================================
double NeutralPPsFromCPsAlg1::caloTrMatch
( const CaloHypo*                    hypo  , 
  const NeutralPPsFromCPsAlg1::Table* table )  const 
{
  // reset the value 
  double chi2 = m_caloTrMatch_bad ;
  // check arguments
  if      ( 0 == hypo ) 
  {
    Error ("caloTrMatch(): CaloHypo* points to NULL!");
    return chi2;                                              // RETURN !!
  }
  else if ( 0 == table ) 
  {
    Error ("caloTrMatch(): Table* points to NULL!");
    return chi2;                                              // RETURN !!
  }
  // select the first Ecal cluster 
  const CaloHypo::Clusters& clusters = hypo->clusters();
  CaloHypo::Clusters::const_iterator cluster =
    std::find_if( clusters.begin() , clusters.end() , m_calo );
  // no Ecal clusters is found , probably OK 
  if ( clusters.end() == cluster ) { return chi2 ; }            // RETURN !!
  
  // get to all related tracks
  const Range range = table->relations( *cluster ) ;
  if ( range.empty()            )  { return chi2 ; }            // RETURN !!
  
  // get minimal value 
  const double match = range.front().weight();
  
  return match ;
};
// ============================================================================

// ============================================================================
/** helpful technical method to evaluate 
 *  the ProtoParticle::ClusterMass etimator
 *  @see ProtoParticle
 *  @param hypo  pointer to the hypo 
 *  @return the value of the estimator 
 */
// ============================================================================
double NeutralPPsFromCPsAlg1::clusterMass ( const CaloHypo*  hypo  )  const 
{
  // reset the value 
  double mass = m_clusterMass_bad ;
  // check arguments
  if     ( 0 == hypo ) 
  {
    Error("clusterMass(): CaloHypo* points to NULL!");
    return mass;                                              // RETURN !!
  }
  
  if( CaloHypotheses::Pi0Merged == hypo->hypothesis() ) 
  {
    const CaloMomentum* momentum = hypo->momentum ();
    if( 0 == momentum ) 
    { Error("clusterMass():CaloMomentum* points to NULL for 'Pi0Merged'"); }
    else 
    { mass = momentum -> momentum().m(); }
  }  
  else { mass = 0 ; }
  
  return mass ;
};
// ============================================================================


// ============================================================================
/** helpful technical method to evaluate 
 *  the ProtoParticle::ShowerShape etimator
 *  @see ProtoParticle
 *  @param hypo  pointer to the hypo 
 *  @return the value of the estimator 
 */
// ============================================================================
double NeutralPPsFromCPsAlg1::showerShape ( const CaloHypo*  hypo  )  const 
{
  // reset the value 
  double shape = m_showerShape_bad ;
  // check arguments
  if     ( 0 == hypo ) 
  {
    Error("showerShape(): CaloHypo* points to NULL!");
    return shape;                                              // RETURN !!
  }
  
  // get the position 
  const CaloPosition* position = hypo->position();
  if( 0 == position ) 
  {
    if( hypo->hypothesis() != CaloHypotheses::Pi0Merged ) 
    { Warning("showerShape(): CaloPosition* points to NULL!"); }
    return shape;                                              // RETURN !!
  }
  
  // get the "shape"
  shape = position->spread()( 1 , 1 )+ position->spread()( 2 , 2 ) ;
  
  return shape ;
};
// ============================================================================


// ============================================================================
/** helpful technical method to evaluate 
 *  the ProtoParticle::CaloDepositID estimator
 *  @see ProtoParticle
 *  @param hypo  pointer to the hypo 
 *  @return the value of the estimator 
 */
// ============================================================================
double  NeutralPPsFromCPsAlg1::caloDepositID ( const CaloHypo*  hypo  )  const 
{
  // reset the value 
  double dep = m_caloDepositID_bad ;
  // check arguments
  if     ( 0 == hypo ) 
  {
    Error("caloDepositID(): CaloHypo* points to NULL!");
    return dep;                                              // RETURN !!
  }
  
  if( hypo->hypothesis() != CaloHypotheses::Pi0Merged ) 
  { dep = m_spdprs -> likelihood( hypo ) ; }
  
  return dep ;
};
// ============================================================================

// ============================================================================
/** helpful technical method to evaluate 
 *  the ProtoParticle::PhotonID estimator
 *  @see ProtoParticle
 *  @param hypo  pointer to the hypo 
 *  @return the value of the estimator 
 */
// ============================================================================
double NeutralPPsFromCPsAlg1::photonID      
( const CaloHypo*  hypo  )  const 
{
  double phID = m_photonID_bad ;
  if ( 0 == hypo ) 
  {
    Error ( "photonID(): CaloHypo* points to NULL!" ) ;
    return phID ;                                              // RETURN !!
  }
  
  if( hypo->hypothesis() == CaloHypotheses::Photon               || 
      hypo->hypothesis() == CaloHypotheses::PhotonFromMergedPi0  || 
      hypo->hypothesis() == CaloHypotheses::BremmstrahlungPhoton || 
      hypo->hypothesis() == CaloHypotheses::BremmstrahlungPhoton  ) 
  { phID = m_photonID -> likelihood( hypo ) ; }
  
  return phID  ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

