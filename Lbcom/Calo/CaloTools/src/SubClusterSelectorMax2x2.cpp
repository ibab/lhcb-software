// $Id: SubClusterSelectorMax2x2.cpp,v 1.2 2002-04-07 18:15:03 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2001/11/08 20:07:06  ibelyaev
//  new tools are added into  the package
//
// Revision 1.1  2001/11/08 10:58:34  ibelyaev
//  new tools are added for selection of subclusters within the cluster
//
// ============================================================================
// Include files
// STD & STL 
#include <algorithm>
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
#include "SubClusterSelectorMax2x2.h"

// ============================================================================
/** @file SubClusterSelectorMax2x2.cpp
 *  
 *  Implementation file for class : SubClusterSelectorMax2x2
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 07/11/2001 
 */
// ============================================================================


// ============================================================================
/** Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<SubClusterSelectorMax2x2>         s_Factory ;
const        IToolFactory&SubClusterSelectorMax2x2Factory = s_Factory ; 

// ============================================================================
/** Standard Tool Constructor
 *  @param type type of the tool (useless ? )
 *  @param name name of the tool 
 *  @param parent the tool parent 
 */
// ============================================================================

SubClusterSelectorMax2x2::SubClusterSelectorMax2x2( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : SubClusterSelectorBase ( type, name , parent ) 
  , m_matrix ()
{};

// ============================================================================
/** destructor 
 */
// ============================================================================
SubClusterSelectorMax2x2::~SubClusterSelectorMax2x2() {};

// ============================================================================
/** standard initiliazation 
 *  @return status code 
 */
// ============================================================================
StatusCode SubClusterSelectorMax2x2::initialize()
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
/** tag the certain submatrix 
 *  @param  cluster pointer to cluster 
 *  @@param Type    submatrix type 
 *  @return status code 
 */
// ============================================================================
StatusCode 
SubClusterSelectorMax2x2::
tag2x2( CaloCluster* cluster                    ,
        const CellMatrix2x2::SubMatrixType Type ) const 
{
  m_matrix.setType( Type ) ;
  StatusCode sc = 
    ClusterFunctors::
    tagTheSubCluster( cluster  , 
                      m_matrix ,
                      modify() , 
                      CaloDigitStatus::ModifiedByMax2x2Tagger );
  if( sc.isFailure() )
    { return Error("Error from 'tagTheSubCluster()'",sc); }
  ///
  return StatusCode::SUCCESS ;
};

// ============================================================================
/** tag the certain submatrix 
 *  @param  cluster pointer to cluster 
 *  @@param Type    submatrix type 
 *  @return status code 
 */
// ============================================================================
StatusCode 
SubClusterSelectorMax2x2::
untag2x2( CaloCluster* cluster                    ,
          const CellMatrix2x2::SubMatrixType Type ) const 
{
  m_matrix.setType( Type ) ;
  StatusCode sc = 
    ClusterFunctors::
    untagTheSubCluster( cluster  , 
                        m_matrix ,
                        CaloDigitStatus::ModifiedByMax2x2Tagger );
  if( sc.isFailure() )
    { return Error("Error from 'untagTheSubCluster()'",sc); }
  ///
  return StatusCode::SUCCESS ;
};

// ============================================================================
/** The main processing method ( functor interface ) 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode SubClusterSelectorMax2x2::tag ( CaloCluster* cluster ) const 
{
  // check the  cluster 
  if( 0 == cluster   ) { return StatusCode ( 225 ) ; }   ///< RETURN
  ///  
  std::vector<double> eSubMatrix( CellMatrix2x2::Max , 0 );
  for( unsigned int iType =  0 ; 
       iType < CellMatrix2x2::Max ; ++iType )
    {
      // tag 
      tag2x2   ( cluster, (CellMatrix2x2::SubMatrixType) iType ); 
      // collect the energy 
      eSubMatrix[ iType ] = 
        ClusterFunctors::
        energy( cluster->entries().begin () ,
                cluster->entries().end   () );      
      // untag 
      untag2x2 ( cluster, (CellMatrix2x2::SubMatrixType) iType ); 
    }
  /// find maximum subcluster 
  std::vector<double>::const_iterator iE =
    std::max_element( eSubMatrix.begin() , eSubMatrix.end() );
  /// 
  CellMatrix2x2::SubMatrixType Type = 
    (CellMatrix2x2::SubMatrixType) ( iE - eSubMatrix.begin() );
  // tag maximum subclus
  return tag2x2( cluster , Type );
};

// ============================================================================
/** The main processing method ( functor interface ) 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode SubClusterSelectorMax2x2::untag ( CaloCluster* cluster ) const 
{
  ///
  for( unsigned int iType =  0 ; 
       iType < CellMatrix2x2::Max ; ++iType )
    { untag2x2 ( cluster, (CellMatrix2x2::SubMatrixType) iType ); }
  ///
  return StatusCode::SUCCESS ;
};

// ============================================================================
// The End 
// ============================================================================
