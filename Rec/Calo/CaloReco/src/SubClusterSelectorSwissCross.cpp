// $Id: SubClusterSelectorSwissCross.cpp,v 1.1.1.1 2002-11-13 20:46:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2002/04/07 18:15:04  ibelyaev
//  preliminary version ('omega'-release)
//
// Revision 1.2  2001/11/08 20:07:06  ibelyaev
//  new tools are added into  the package
//
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
#include "Event/CaloCluster.h"
#include "Event/CaloDigit.h"
// CaloUtils 
#include "CaloUtils/ClusterFunctors.h"
// local
#include "SubClusterSelectorSwissCross.h"

// ============================================================================
/** @file SubClusterSelectorSwissCross.cpp
 *  
 *  Implementation file for class : SubClusterSelectorSwissCross
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 07/11/2001 
 */
// ============================================================================


// ============================================================================
/** Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<SubClusterSelectorSwissCross>         s_Factory ;
const        IToolFactory&SubClusterSelectorSwissCrossFactory = s_Factory ; 

// ============================================================================
/** Standard Tool Constructor
 *  @param type type of the tool (useless ? )
 *  @param name name of the tool 
 *  @param parent the tool parent 
 */
// ============================================================================
SubClusterSelectorSwissCross::
SubClusterSelectorSwissCross( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : SubClusterSelectorBase ( type, name , parent ) 
  , m_matrix ()
{};

// ============================================================================
/** destructor 
 */
// ============================================================================
SubClusterSelectorSwissCross::~SubClusterSelectorSwissCross() {};

// ============================================================================
/** standard initiliazation 
 *  @return status code 
 */
// ============================================================================
StatusCode SubClusterSelectorSwissCross::initialize()
{
  // initliaze the base class 
  StatusCode sc = SubClusterSelectorBase::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!",sc); }
  if( 0 != det ()    )
    { m_matrix.setDet( det() ) ; }
  else 
    { return Error("DeCalorimeter* ponts to NULL!"); }
  ///
  return StatusCode::SUCCESS ;
};

// ============================================================================
/** The main processing method
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode 
SubClusterSelectorSwissCross::tag ( CaloCluster* cluster ) const 
{
  StatusCode sc = 
    ClusterFunctors::
    tagTheSubCluster( cluster  , 
                      m_matrix , 
                      modify() , 
                      CaloDigitStatus::ModifiedBySwissCrossTagger ) ;
  //
  if( sc.isFailure() ) 
    { return Error( "Error from 'tagTheSubCluster()'" , sc ); }
  //
  return StatusCode::SUCCESS ;  
};

// ============================================================================
/** The main processing method
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode 
SubClusterSelectorSwissCross::untag ( CaloCluster* cluster ) const
{
  StatusCode sc = 
    ClusterFunctors::
    untagTheSubCluster( cluster  , 
                        m_matrix , 
                        CaloDigitStatus::ModifiedBySwissCrossTagger );
  //
  if( sc.isFailure() ) 
    { return Error( "Error from 'untagTheSubCluster()'" , sc ); }
  //
  return StatusCode::SUCCESS ;  
};

// ============================================================================
// The End 
// ============================================================================
