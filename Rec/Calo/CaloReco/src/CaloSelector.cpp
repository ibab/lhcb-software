// $Id: CaloSelector.cpp,v 1.5 2006-06-27 16:36:54 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2006/05/30 09:42:05  odescham
// first release of the CaloReco migration
//
// Revision 1.3  2005/11/07 12:12:43  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.2  2004/02/17 12:08:09  ibelyaev
//  update for new CaloKernel and CaloInterfaces
//
// Revision 1.1.1.1  2002/11/13 20:46:42  ibelyaev
// new package 
//
// Revision 1.2  2002/04/26 13:36:40  ibelyaev
//  update for change in ICaloLikelihood interface
//
// Revision 1.1  2002/04/07 18:15:01  ibelyaev
//  preliminary version ('omega'-release)
// 
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
// CaloInterfaces 
#include "CaloInterfaces/ICaloLikelihood.h"
// local
#include "CaloSelector.h"

// ============================================================================
/** @file CaloSelector.cpp 
 *
 *  Implementation file for class : CaloSelector
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date 31/03/2002 
 */
// ============================================================================

// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
static const  ToolFactory<CaloSelector>         s_factory ;
const        IToolFactory&CaloSelectorFactory = s_factory ;

// ============================================================================
/** Standard constructor
 *  @param    type   tool type (?)
 *  @param    name   tool name 
 *  @param    paretn tool parent 
 */
// ============================================================================
CaloSelector::CaloSelector( const std::string&  type   ,
                            const std::string&  name   ,
                            const IInterface*   parent )
  : GaudiTool ( type, name , parent ) 
  , m_lhType     (        ) 
  , m_lhName     (        ) 
  , m_likelihood ( 0      )
  , m_cut        ( 1.e+50 ) 
{  
  // interfaces  
  declareInterface<ICaloClusterSelector> (this);
  // properties 
  declareProperty( "LikelihoodType" , m_lhType ) ;
  declareProperty( "LikelihoodName" , m_lhType ) ;
  declareProperty( "LikelihoodCut"  , m_cut    ) ;
};

// ============================================================================
/// destructor (virtual and protected)
// ============================================================================
CaloSelector::~CaloSelector()
{
};

// ============================================================================
/** standard initialization of the tool 
 *  @see IAlgTool 
 *  @see AlgTool 
 *  @see GaudiTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSelector::initialize() 
{
  // initialialize the base class 
  StatusCode sc = GaudiTool::initialize() ;
  if( sc.isFailure() ) 
  { return Error("Could not initialize the base class GaudiTool!",sc);}
  /// locate the tool 
  m_likelihood = m_lhName.empty() ?
    tool<ICaloLikelihood>( m_lhType            ) :
    tool<ICaloLikelihood>( m_lhType , m_lhName ) ;
  if( 0 == m_likelihood ) { return StatusCode::FAILURE ; }
  //
  return StatusCode::SUCCESS ;
};

// ============================================================================
/** standard finalization  of the tool 
 *  @see IAlgTool 
 *  @see AlgTool 
 *  @see GaudiTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSelector::finalize() 
{
  ///finalize the base class 
  return GaudiTool::finalize() ;
};

// ============================================================================
/** "select"/"preselect" method (functor interface)
 *  @see ICaloClusterSelector 
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool CaloSelector::operator() ( const LHCb::CaloCluster* cluster ) const
{
  if( 0 == cluster ) {  return false ; }
  return  m_cut <= (*m_likelihood) (cluster) ;
};

// ============================================================================
/** "select"/"preselect" method 
 *  @see ICaloClusterSelector 
 *  @param  cluster pointer to calo cluster object to be selected 
 *  @return true if cluster is selected
 */
// ============================================================================
bool CaloSelector::select     ( const LHCb::CaloCluster* cluster ) const 
{ return (*this) ( cluster ); }

// ============================================================================
// The End 
// ============================================================================
