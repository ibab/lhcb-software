// $Id: SubClusterSelectorNeighbors.cpp,v 1.1 2001-11-12 21:16:14 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
// CaloEvent 
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloDigit.h"
#include "CaloEvent/CaloDataFunctor.h"
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
// local
#include "SubClusterSelectorNeighbors.h"

// ============================================================================
/** @file SubClusterSelectorNeighbors.cpp
 *  
 *  Implementation file for class : SubClusterSelectorNeighbors
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 07/11/2001 
 */
// ============================================================================

// ============================================================================
/** Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<SubClusterSelectorNeighbors>         s_Factory ;
const        IToolFactory&SubClusterSelectorNeighborsFactory = s_Factory ; 

// ============================================================================
/** Standard Tool Constructor
 *  @param Type type of the tool (useless ? )
 *  @param Name name of the tool 
 *  @param Parent the tool parent 
 */
// ============================================================================
SubClusterSelectorNeighbors::
SubClusterSelectorNeighbors( const std::string& Type,
                             const std::string& Name,
                             const IInterface*  Parent )
  : SubClusterSelectorBase ( Type, Name , Parent ) 
{};

// ============================================================================
/** destructor 
 */
// ============================================================================
SubClusterSelectorNeighbors::~SubClusterSelectorNeighbors() {};

// ============================================================================
/** standard initiliazation 
 *  @return status code 
 */
// ============================================================================
StatusCode SubClusterSelectorNeighbors::initialize()
{
  /// initliaze the base class 
  StatusCode sc = SubClusterSelectorBase::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!",sc); }
  ///
  if( 0 == det ()    )
    { return Error("DeCalorimeter* ponts to NULL!"); }
  ///
  return StatusCode::SUCCESS ;
};

// ============================================================================
/** The main processing method ( functor interface ) 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode 
SubClusterSelectorNeighbors::operator() ( CaloCluster* cluster ) const 
{
  /// check the arguments 
  if( 0 == cluster   ) { return StatusCode ( 225 ) ; }   ///< RETURN
  /// get digits from the cluster 
  CaloCluster::Digits& digits = cluster->digits();
  /// empty container? 
  if( digits.empty() ) { return StatusCode ( 226 )  ; }   ///< RETURN 
  /// find seed digit 
  CaloCluster::Digits::iterator iSeed = 
    CaloDataFunctor::clusterLocateDigit( digits.begin ()       , 
                                         digits.end   ()       ,
                                         DigitStatus::SeedCell );
  /// the seed is not found !
  if( digits.end() == iSeed ) { return StatusCode ( 227 )  ; }
  /// seed is valid? 
  const CaloDigit*     seed = iSeed->first ;
  if( 0 == seed             ) { return StatusCode ( 228 )  ; }
  ///
  typedef CaloNeighbors::const_iterator Neighbor;
  const CaloNeighbors& neighbors = 
    det()->neighborCells( seed->cellID() );
  /// loop over all digits and select Neighbors submatrix 
  for( CaloCluster::Digits::iterator iDigit = digits.begin() ;
       digits.end() != iDigit ; ++iDigit )
    {
      CaloDigitStatus& status = iDigit->second ;
      /// reset the existing status
      status.removeStatus ( DigitStatus::UseForEnergy     ) ;
      status.removeStatus ( DigitStatus::UseForPosition   ) ;
      status.removeStatus ( DigitStatus::UseForCovariance ) ;          
      ///
      /// skip invalid digits 
      const CaloDigit* digit  = iDigit->first   ;
      if( 0 == digit      )             { continue;  }  ///< CONTINUE 
      /// seed cell or neighbour cell
      if( seed == digit ||
          ( std::find( neighbors.begin() , neighbors.end    () , 
                       digit->cellID  () ) != neighbors.end () ) )
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
// The End 
// ============================================================================
