// $Id: SubClusterSelectorMax2x2.cpp,v 1.1 2001-11-08 20:07:06 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
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
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloDigit.h"
#include "CaloEvent/CaloDataFunctor.h"
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
  /// initliaze the base class 
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
SubClusterSelectorMax2x2::tag( CaloCluster* cluster                    ,
                               const CaloCellID&                  seed ,
                               const CellMatrix2x2::SubMatrixType Type ) const 
{
  m_matrix.setType( Type ) ;
  /// loop over all digits and select Max2x2 submatrix 
  for( CaloCluster::Digits::iterator iDigit = cluster->digits().begin() ;
       cluster->digits().end() != iDigit ; ++iDigit )
    {
      CaloDigitStatus& status = iDigit->second ;
      /// reset the existing statuses 
      status.removeStatus ( DigitStatus::UseForEnergy     ) ;
      status.removeStatus ( DigitStatus::UseForPosition   ) ;
      status.removeStatus ( DigitStatus::UseForCovariance ) ;          
      /// skip invalid digits 
      const CaloDigit* digit  = iDigit->first   ;
      if( 0 == digit      ) { continue;  }  ///< CONTINUE 
      /// check for valid digits and select Max2x2 submatrix  
      if( 0 <  m_matrix( seed , digit->cellID() ) )
        {
          status.setStatus    ( DigitStatus::UseForEnergy     ) ;
          status.setStatus    ( DigitStatus::UseForPosition   ) ;
          status.setStatus    ( DigitStatus::UseForCovariance ) ;        
        }
      ///
    }
  ///
  return StatusCode::SUCCESS ;
};

// ============================================================================
/** The main processing method ( functor interface ) 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode SubClusterSelectorMax2x2::operator() ( CaloCluster* cluster ) const 
{
  /// avoid long names
  using namespace CaloDataFunctor;
  /// check the arguments 
  if( 0 == cluster   ) { return StatusCode ( 225 ) ; }   ///< RETURN
  /// get digits from the cluster 
  CaloCluster::Digits& digits = cluster->digits();
  /// empty container? 
  if( digits.empty() ) { return StatusCode ( 226 )  ; }   ///< RETURN 
  /// find seed digit 
  CaloCluster::Digits::iterator iSeed = 
    clusterLocateDigit( digits.begin ()       , 
                        digits.end   ()       ,
                        DigitStatus::SeedCell );
  /// the seed is not found !
  if( digits.end() == iSeed ) { return StatusCode ( 227 )  ; }
  /// seed is valid? 
  const CaloDigit*     seed = iSeed->first ;
  if( 0 == seed             ) { return StatusCode ( 228 )  ; }
  ///
  std::vector<double> eSubMatrix( CellMatrix2x2::Max );
  for( unsigned int iType =  0 ; 
       iType < CellMatrix2x2::Max ; ++iType )
    {
      tag( cluster, seed->cellID() ,(CellMatrix2x2::SubMatrixType) iType ); 
      /// collect the energy 
      eSubMatrix[ iType ] = 
        clusterEnergy( cluster->digits().begin () ,
                       cluster->digits().end   () );
    }
  /// find maximum subcluster 
  std::vector<double>::const_iterator iE =
    std::max_element( eSubMatrix.begin() , eSubMatrix.end() );
  /// 
  CellMatrix2x2::SubMatrixType Type = 
    (CellMatrix2x2::SubMatrixType) ( iE - eSubMatrix.begin() );
  /// tag maximum subclus
  return tag( cluster , seed->cellID() , Type );
};

// ============================================================================
// The End 
// ============================================================================
