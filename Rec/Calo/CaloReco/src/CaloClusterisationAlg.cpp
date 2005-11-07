// $Id: CaloClusterisationAlg.cpp,v 1.3 2005-11-07 12:12:41 odescham Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/02/17 12:08:06  ibelyaev
//  update for new CaloKernel and CaloInterfaces
//
// Revision 1.1.1.1  2002/11/13 20:46:40  ibelyaev
// new package 
//
// Revision 1.10  2002/05/12 13:45:03  ibelyaev
//  add correct 'inversion' of comparisons
//
// ============================================================================
#define  CALOALGS_CALOCLUSTERISATIONALG_CPP 1 
// ============================================================================
/// Include files
/// STD and STL 
#include <algorithm>
#include <vector>
#include <functional>
/// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
/// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartRef.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/Stat.h" 
// DetDesc
#include "DetDesc/IGeometryInfo.h"
/// CaloDet 
#include "CaloDet/DeCalorimeter.h"
/// CaloEvent
#include "Event/CaloCluster.h"
#include "Event/CaloDataFunctor.h"
// CaloALgs 
#include "CaloUtils/ClusterFunctors.h"
// local
#include "CaloClusterisationAlg.h"
#include "ClusterisationFunctors.h"

// ============================================================================
/** @file CaloClusterisationAlg.cpp
 *  
 * Implementation file for class : CaloClusterisationAlg
 *
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * @date  06/07/2001 
 */
// ============================================================================

/// Declaration of the Algorithm Factory
static const  AlgFactory<CaloClusterisationAlg>          s_Factory ;
const        IAlgFactory& CaloClusterisationAlgFactory = s_Factory ; 

// ============================================================================
/**  standard constructor
 *    @param  name    name of this concrete algorithm instance 
 *   @param  svc     pointer to Service Locator 
 */
// ============================================================================
CaloClusterisationAlg::CaloClusterisationAlg( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : CaloAlgorithm   ( name , pSvcLocator ) 
  , m_useSeedECut    ( false  )
  , m_useSeedEtCut   ( false  )
  , m_threshold      ( -1*TeV )
{
  declareProperty( "UseSeedECut"   , m_useSeedECut  ) ;
  declareProperty( "UseSeedEtCut"  , m_useSeedEtCut ) ;
  declareProperty( "Threshold"     , m_threshold    ) ;
};

// ============================================================================
/// destructor 
// ============================================================================
CaloClusterisationAlg::~CaloClusterisationAlg() {}; 

// ============================================================================
// Initialisation. Check parameters
// ============================================================================
StatusCode CaloClusterisationAlg::initialize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  
  /// initialize base class   
  StatusCode sc = CaloAlgorithm::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class CaloAlgorithm!",sc); }  

  /// check for parameters
  if( inputData ().empty() ) { return Error("No input data specified!"   ); }
  if( outputData().empty() ) { return Error("No output data specified!"  ); }
  if( detData   ().empty() ) { return Error("No detector data specified!"); }

  return StatusCode::SUCCESS;
};

// ============================================================================
//  Finalize
// ============================================================================
StatusCode CaloClusterisationAlg::finalize() 
{

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return CaloAlgorithm::finalize();
};

// ============================================================================
// Main execution
// ============================================================================
StatusCode CaloClusterisationAlg::execute() 
{
  
  // avoid long names 
  using namespace CaloDataFunctor        ;
  using namespace ClusterisationFunctors ;
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  typedef CaloDigits                 Digits      ;
  typedef CaloClusters               Clusters    ;
  typedef const DeCalorimeter        Detector    ;
  
  /// locate the input data
  Digits*    digits   =    get<Digits>   ( inputData () );
  if( 0 == digits   ) { return Error("'Input'    data are not available!");}
  
  /// locate the detector 
  Detector*  detector = getDet<Detector> ( detData   () );
  if( 0 == detector ) { return Error("'Detector' data are not available!");}
  
  /// create output container of clusters
  Clusters*  clusters = new Clusters();
  {
    // register clusters in Gaudi Transient Store 
    StatusCode sc = put( clusters , outputData() ) ;
    if( sc.isFailure() )
    { return Error("could not register the output='"+outputData()+"'",sc) ;}
  }
  
  // functor to get z-position of cluster 
  ClusterFunctors::ZPosition zPosition( detector );
  
  // select interesting digits ('pre-seeds')
  Digits::iterator selected = digits->begin() ;
  if      ( m_useSeedECut )
    {
      // sort container according to energy
      Less_by_Energy<const CaloDigit*>                     Less;
      std::stable_sort ( digits -> begin () , 
                         digits -> end   () , 
                         inverse ( Less )   ) ;
      // find the first digit "under the threshold"
      Over_E_Threshold<const CaloDigit*> 
        selector (            m_threshold );
      selected = std::find_if ( digits -> begin()     , 
                                digits -> end  ()     , 
                                std::not1( selector ) );
    }
  else if ( m_useSeedEtCut ) 
    {
      // sort container according transverse energy 
      Less_by_TransverseEnergy<const CaloDigit*,Detector*> Less( detector );
      std::stable_sort ( digits -> begin () , 
                         digits -> end   () , 
                         inverse( Less )    ) ;
      // find the first digit "under the threshold"
      Over_Et_Threshold<const CaloDigit*,Detector*> 
        selector ( detector , m_threshold );
      selected = std::find_if ( digits -> begin()     , 
                                digits -> end  ()     , 
                                std::not1( selector ) );
    }
  else 
    {
      // sort container according to energy
      Less_by_Energy<const CaloDigit*>    Less;
      std::stable_sort ( digits -> begin () , 
                         digits -> end   () , 
                         inverse( Less )    ) ;      
      // "select" all digits as potential seeds  
      selected = digits->end() ;
    }
  
  // create seed finder 
  SeedFinder<Detector,Digits> seedFinder( detector , *digits );
  
  // create clusters from seeds
  for( Digits::const_iterator digit = 
         digits->begin() ; selected != digit ; ++digit )
    {
      // create cluster, if it is SEED! 
      CaloCluster* cluster = seedFinder( *digit );
      if( 0 == cluster ) { continue ;}             ///< CONTINUE 
      // calculate e, x , y for cluster  
      double e = 0 ;
      double x = 0 ;
      double y = 0 ;
      StatusCode sc = 
        ClusterFunctors::calculateEXY( cluster->entries().begin() ,
                                       cluster->entries().end  () ,
                                       detector , e, x, y        );
      if( sc.isFailure() ) { continue ; }
      cluster->position().parameters()( CaloPosition::E ) = e ;
      cluster->position().parameters()( CaloPosition::X ) = x ;
      cluster->position().parameters()( CaloPosition::Y ) = y ;
      cluster->position().setZ( zPosition( cluster ) );
      // add a new cluster into container 
      clusters->insert( cluster );
    }
  
  // sort clusters   
  Less_by_Energy<const CaloCluster*> Cmp;
  std::stable_sort( clusters -> begin () , 
                    clusters -> end   () , 
                    std::not2( Cmp )     ) ;
  
  // output statistics 
  Stat nclu  ( chronoSvc(), name()+" #clus", (double) clusters->size()  , 1.);
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
// The End 
// ============================================================================
