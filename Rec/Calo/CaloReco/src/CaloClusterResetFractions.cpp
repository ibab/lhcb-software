// $Id: CaloClusterResetFractions.cpp,v 1.1.1.1 2002-11-13 20:46:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2002/04/07 18:14:59  ibelyaev
//  preliminary version ('omega'-release)
//
// Revision 1.2  2001/12/09 14:33:08  ibelyaev
//  update for newer version of Gaudi
//
// Revision 1.1  2001/11/08 20:07:04  ibelyaev
//  new tools are added into  the package
// 
// ============================================================================
// include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
// CaloEvent/Event 
#include "Event/CaloCluster.h"
// local
#include "CaloClusterResetFractions.h"

// ============================================================================
/** @file  CaloClusterResetFractions.cpp
 *  
 *  Implementation file for class : CaloClusterResteFractions
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 07/11/2001 
 */
// ============================================================================

// ============================================================================
/** Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloClusterResetFractions>         s_Factory ;
const        IToolFactory&CaloClusterResetFractionsFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param type   tool type 
 *  @param name   tool name 
 *  @param parent tool parent 
 */
// ============================================================================
CaloClusterResetFractions::
CaloClusterResetFractions ( const std::string& type   ,
                            const std::string& name   ,
                            const IInterface*  parent )
  : CaloTool( type , name , parent )
{
  /// declare the available interfaces
  declareInterface<ICaloClusterTool>(this);
};

// ============================================================================
/** destructor, protected and virtual
 */
// ============================================================================
CaloClusterResetFractions::~CaloClusterResetFractions() {};

// ============================================================================
/** standard initialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode    
CaloClusterResetFractions::initialize ()
{ return CaloTool::initialize() ; }

// ============================================================================
/** standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloClusterResetFractions::finalize   ()
{ return CaloTool::initialize() ; }

// ============================================================================
/** The main processing method 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloClusterResetFractions::process ( CaloCluster* cluster ) const
{ return (*this) (cluster) ; };

// ============================================================================
/** The main processing method (functor interface) 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */
// ============================================================================
StatusCode 
CaloClusterResetFractions::operator() ( CaloCluster* cluster ) const
{
  /// check the arguments 
  if( 0 == cluster ) { return Error("CaloCluster* points to NULL!"); }
  /// loop over all digits and reset the fractions 
  for( CaloCluster::Entries::iterator entry = cluster->entries().begin() ;
       cluster->entries().end() != entry ; ++entry )
    { entry->setFraction( 1 ) ; }
  ///
  return StatusCode::SUCCESS ;
};

// ============================================================================
// The End 
// ============================================================================
