// $Id: CaloClusterSOMSelector.cpp,v 1.1 2002-06-14 11:38:08 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
// CaloEvent 
#include "Event/CaloCluster.h"
#include "Event/CaloDataFunctor.h"
// local
#include "CaloClusterSOMSelector.h"

// ============================================================================
/** @file 
 *
 *  Implementation file for class : CaloClusterSOMSelector
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/05/2002
 */
// ============================================================================


// ============================================================================
/** @var CaloClusterSOMSelectorFactory
 *  Mandatory declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloClusterSOMSelector>         s_factory ;
const        IToolFactory&CaloClusterSOMSelectorFactory = s_factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @see   CaloTool
 *  @see    AlgTool 
 *  @param type    type of the tool  (?)
 *  @param name    name of the concrete instance 
 *  @param parent  pointer to parent object (algorithm, service or tool)
 */
// ============================================================================
CaloClusterSOMSelector::CaloClusterSOMSelector
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : CaloTool ( type, name , parent ) 
  , m_e0   (   ) 
  , m_e1   (   ) 
  , m_e2   (   ) 
  , m_s0   (   ) 
  , m_s1   (   ) 
  , m_s2   (   ) 
  , m_aTol ( 0 )
  , m_bTol ( 1 )
{
  declareInterface<ICaloClusterSelector> (this);
  declareProperty( "Es0"        , m_e0   ) ;
  declareProperty( "Es1"        , m_e1   ) ;
  declareProperty( "Es2"        , m_e2   ) ;
  declareProperty( "SOMs0"      , m_s0   ) ;
  declareProperty( "SOMs1"      , m_s1   ) ;
  declareProperty( "SOMs2"      , m_s2   ) ;
  declareProperty( "aTolerance" , m_aTol ) ;
  declareProperty( "bTolerance" , m_bTol ) ;
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
CaloClusterSOMSelector::~CaloClusterSOMSelector() {}
// ============================================================================

// ============================================================================
/** standard finalization of the Tool 
 *  @see CaloTool
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return statsu code 
 */
// ============================================================================
StatusCode CaloClusterSOMSelector::finalize   () 
{  
  m_e0.clear () ;
  m_e1.clear () ;
  m_e2.clear () ;
  
  m_s0.clear () ;
  m_s1.clear () ;
  m_s2.clear () ;
  
  // finalize the base class 
  return CaloTool::finalize();
};
// ============================================================================

// ============================================================================
/** standard initialization of the Tool 
 *  @see CaloTool
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return statsu code 
 */
// ============================================================================
StatusCode CaloClusterSOMSelector::initialize () 
{
  // initialize the base class 
  StatusCode sc = CaloTool::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize teh base class CaloTool" , sc ) ; }
  // check the dimensions of the parameters 
  if( m_e0.size() != m_e1.size () || 
      m_e1.size() != m_e2.size ()  )
    { return Error("Mismatch of container sizes E  !"); }
  if( m_s0.size() != m_s1.size () || 
      m_s1.size() != m_s2.size ()  )
    { return Error("Mismatch of container sizes S  !"); }
  if( m_e0.size() != m_s0.size ()  )
    { return Error("Mismatch of container sizes ES !"); }
  if( m_e0.empty () )
    { return Error("Container is empty!"); }
  //
  return StatusCode::SUCCESS ;
};

// ============================================================================

// ============================================================================
/** Main "select"/"preselect" method 
 *  @see ICaloClusterSelector 
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool CaloClusterSOMSelector::select ( const CaloCluster* cluster ) const 
{ return (*this) ( cluster ); };
// ============================================================================

// ============================================================================
/** Main "select"/"preselect" method (functor interface)
 *  @see ICaloClusterSelector 
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool CaloClusterSOMSelector::operator() ( const CaloCluster* cluster ) const 
{
  // avoid long names 
  using namespace CaloDataFunctor                       ;
  typedef CaloCluster::Entries                 Entries  ;
  typedef CaloCluster::Entries::const_iterator iterator ;
  
  // check arguments;
  Assert ( 0 != cluster , "CaloCluster* points to NULL ");
  
  // get cluster seed cell
  iterator seed = clusterLocateDigit( cluster->entries().begin () , 
                                      cluster->entries().end   () , 
                                      CaloDigitStatus::SeedCell  );
  Assert( cluster->entries().end() != seed , "CaloCluster has no SeedCell!");
  
  const CaloDigit* digit = seed->digit() ;  
  Assert( 0 != digit , "SeedCell is invalid 1 !"  ) ;
  
  // get calorimeter area 
  const unsigned int area = digit->cellID().area();  
  
  // cluster energy 
  const double energy  = cluster->e();
  
  // evalute the limit 
  const double lim     = limit( energy , area ) ;
  
  // cluster SecondOrderMoment (with z-normalization) 
  const double som    =
    ( cluster->position().spread()(1,1) +
      cluster->position().spread()(2,2) ) 
    / cluster->position().z() 
    / cluster->position().z() ;

  // apply the tolerance 
  return som < ( lim + m_aTol ) * m_bTol ? true : false ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

