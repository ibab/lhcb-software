// $Id: SubClusterSelectorAll.cpp,v 1.2 2001-11-08 20:07:05 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2001/11/08 10:58:35  ibelyaev
//  new tools are added for selection of subclusters within the cluster
//
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
// CaloEvent 
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloDigit.h"
// local
#include "SubClusterSelectorAll.h"

// ============================================================================
/** @file SubClusterSelectorAll.cpp
 *  
 *  Implementation file for class : SubClusterSelectorAll
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 07/11/2001 
 */
// ============================================================================


// ============================================================================
/** Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<SubClusterSelectorAll>         s_Factory ;
const        IToolFactory&SubClusterSelectorAllFactory = s_Factory ; 

// ============================================================================
/** Standard Tool Constructor
 *  @param type type of the tool (useless ? )
 *  @param name name of the tool 
 *  @param parent the tool parent 
 */
// ============================================================================

SubClusterSelectorAll::SubClusterSelectorAll( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : SubClusterSelectorBase ( type, name , parent ) 
{};

// ============================================================================
/** destructor 
 */
// ============================================================================
SubClusterSelectorAll::~SubClusterSelectorAll() {};

// ============================================================================
/** The main processing method ( functor interface ) 
 *
 *  Error Codes:
 *     - 225  - cluster points to NULL
 *     - 226  - cell/digit container is empty 
 * 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode SubClusterSelectorAll::operator() ( CaloCluster* cluster ) const 
{
  /// check the arguments 
  if( 0 == cluster              ) { return StatusCode ( 225 ) ; }///< RETURN
  if( cluster->digits().empty() ) { return StatusCode ( 226 ) ; }///< RETURN
  /// loop over digits and "select the whole cluster" as a subcluster
  for( CaloCluster::Digits::iterator iDigit = cluster->digits().begin() ;
       cluster->digits().end() != iDigit ; ++iDigit ) 
    {
      CaloDigitStatus& status = iDigit->second ;
      if( 0 != iDigit->first ) 
        { 
          status.setStatus    ( DigitStatus::UseForEnergy     ) ;
          status.setStatus    ( DigitStatus::UseForPosition   ) ;
          status.setStatus    ( DigitStatus::UseForCovariance ) ;
        }
      else 
        {    
          status.removeStatus ( DigitStatus::UseForEnergy     ) ;
          status.removeStatus ( DigitStatus::UseForPosition   ) ;
          status.removeStatus ( DigitStatus::UseForCovariance ) ;
        }
    }
  ///
  return StatusCode::SUCCESS ;
};

// ============================================================================
// The End 
// ============================================================================
