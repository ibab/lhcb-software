// $Id: CaloSelectNeutralCluster.cpp,v 1.2 2004-02-17 12:08:09 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/13 20:46:40  ibelyaev
// new package 
//
// Revision 1.2  2002/06/13 12:36:13  ibelyaev
//  new algorithms and new options
//
// Revision 1.1.1.1  2002/05/21 14:18:21  ibelyaev
// New package
// 
// ============================================================================
// Relations
#include "Relations/IAssociatorWeighted.h"
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
// GaudiKernel
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"
// local 
#include "CaloSelectNeutralCluster.h"

// ============================================================================
/** @file 
 *
 *  implementation file for class CaloSelectNeutralCluster
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 26 Apr 2002 
 */
// ============================================================================

// ============================================================================
/** @var CaloSelectNeutralClusterFactory 
 *  mandatory instantiation of tool factory
 */
// ============================================================================
static const ToolFactory<CaloSelectNeutralCluster>         s_Factory ;
const       IToolFactory&CaloSelectNeutralClusterFactory = s_Factory ;
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @see  CaloTool
 *  @see   AlgTool
 *  @see  IAlgTool 
 *  @param type tool type (?)
 *  @param name name of the concrete instance 
 *  @param parent pointer to the parent 
 */
// ============================================================================
CaloSelectNeutralCluster::CaloSelectNeutralCluster
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent )
  : CaloTool ( type , name , parent )
  , m_associatorTypeName
("AssociatorWeighted<CaloCluster,TrStoredTrack,double>/PhotonMatch") 
  , m_associator          (    0 ) 
  , m_chi2cut             ( -100 )
{
  declareInterface<ICaloClusterSelector> (this);
  declareProperty ("Associator" , m_associatorTypeName ) ;
  declareProperty ("Cut"        , m_chi2cut            ) ;
};
// ============================================================================

// ============================================================================
/// destructor (virtual and protected)
// ============================================================================
CaloSelectNeutralCluster::~CaloSelectNeutralCluster() {};
// ============================================================================

// ============================================================================
/** standard finalization   method 
 *  @see CaloTool
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code
 */
// ============================================================================
StatusCode 
CaloSelectNeutralCluster::finalize     () 
{
  // finalize the base class 
  return CaloTool::finalize();
};
// ============================================================================

// ============================================================================
/** standard initialization method 
 *  @see CaloTool
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code
 */
// ============================================================================
StatusCode 
CaloSelectNeutralCluster::initialize ()
{  
  // initialize the base class 
  StatusCode sc = CaloTool::initialize () ;
  if( sc.isFailure() )
  {return Error("Could not initialize the base class CaloTool!",sc); }
  // locate the associator 
  m_associator = tool<ITable>( m_associatorTypeName ) ;
  //
  return StatusCode::SUCCESS ;
}; 
// ============================================================================

// ============================================================================
/** @brief "select"  method 
 *
 *  Cluster is considered to be "selected" 
 *  if there are no reconstructed tracks with 
 *  chi2 value for 2D-matching under the threshold value 
 *
 *  @see ICaloClusterSelector
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool CaloSelectNeutralCluster::select     
( const CaloCluster* cluster ) const 
{ return (*this) ( cluster ); };
// ============================================================================

// ============================================================================
/** @brief "select"  method (functor interface)
 *
 *  Cluster is considered to be "selected" 
 *  if there are no reconstructed tracks with 
 *  chi2 value for 2D-matching under the threshold value 
 *
 *  @see ICaloClusterSelector
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool CaloSelectNeutralCluster::operator() 
  ( const CaloCluster* cluster   ) const 
{
  // check the cluster 
  if( 0 == cluster ) { Warning("CaloCluster* points to NULL!"); return false ;}
  Assert( 0 != associator() , " Associator points to NULL! "     );
  // get the reference table 
  const ITable::DirectType* table = associator()->direct() ;
  Assert( 0 != table        , " Relation Table points to NULL! " );
  // get all relations with WEIGHT = 'chi2' under the threshold value 
  const ITable::DirectType::Range range = 
    table->relations( cluster , m_chi2cut , false ) ;
  ///
  return range.empty() ? true : false ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
