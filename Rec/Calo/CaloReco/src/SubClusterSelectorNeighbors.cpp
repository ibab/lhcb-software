// $Id: SubClusterSelectorNeighbors.cpp,v 1.1.1.1 2002-11-13 20:46:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/04/07 18:15:04  ibelyaev
//  preliminary version ('omega'-release)
//
// Revision 1.1  2001/11/12 21:16:14  ibelyaev
//  new concrete sub-cluster selector is added
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
#include "Event/CaloDataFunctor.h"
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
  // initliaze the base class 
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
SubClusterSelectorNeighbors::tag ( CaloCluster* cluster ) const 
{
  // check the arguments 
  if( 0 == cluster   ) { return StatusCode ( 225 ) ; }   ///< RETURN
  // get digits from the cluster 
  CaloCluster::Entries& entries = cluster->entries();
  // empty container? 
  if( entries.empty() ) { return StatusCode ( 226 )  ; }   ///< RETURN 
  // find seed digit 
  CaloCluster::Entries::iterator iSeed = 
    CaloDataFunctor::clusterLocateDigit( entries.begin ()          , 
                                         entries.end   ()          ,
                                         CaloDigitStatus::SeedCell );
  // the seed is not found !
  if( entries.end() == iSeed ) { return StatusCode ( 227 )  ; }
  // seed is valid? 
  const CaloDigit*     seed = iSeed->digit() ;
  if( 0 == seed             ) { return StatusCode ( 228 )  ; }
  ///
  typedef CaloNeighbors::const_iterator Neighbor;
  const CaloNeighbors& neighbors = 
    det()->neighborCells( seed->cellID() );
  // loop over all digits and select Neighbors submatrix 
  for( CaloCluster::Entries::iterator entry = entries.begin() ;
       entries.end() != entry ; ++entry )
    {
      // reset the existing status
      entry->removeStatus ( CaloDigitStatus::UseForEnergy     ) ;
      entry->removeStatus ( CaloDigitStatus::UseForPosition   ) ;
      entry->removeStatus ( CaloDigitStatus::UseForCovariance ) ;          
      ///
      // skip invalid digits 
      const CaloDigit* digit  = entry->digit()   ;
      if( 0 == digit      )             { continue;  }  ///< CONTINUE 
      // seed cell or neighbour cell
      if( seed == digit ||
          ( std::find( neighbors.begin () , 
                       neighbors.end   () , 
                       digit->cellID   () ) != neighbors.end () ) )
        {
          entry->addStatus ( CaloDigitStatus::UseForEnergy     ) ;
          entry->addStatus ( CaloDigitStatus::UseForPosition   ) ;
          entry->addStatus ( CaloDigitStatus::UseForCovariance ) ;        
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
StatusCode 
SubClusterSelectorNeighbors::untag ( CaloCluster* cluster ) const 
{
  // check the arguments 
  if( 0 == cluster   ) { return StatusCode ( 225 ) ; }   ///< RETURN
  // get digits from the cluster 
  CaloCluster::Entries& entries = cluster->entries();
  // empty container? 
  if( entries.empty() ) { return StatusCode ( 226 )  ; }   ///< RETURN 
  /// loop 
  for( CaloCluster::Entries::iterator entry = entries.begin() ;
       entries.end() != entry ; ++entry )
    {
      // reset the existing status
      entry->addStatus ( CaloDigitStatus::UseForEnergy     ) ;
      entry->addStatus ( CaloDigitStatus::UseForPosition   ) ;
      entry->addStatus ( CaloDigitStatus::UseForCovariance ) ;          
      ///
    }
  ///
  return StatusCode::SUCCESS ;
};

// ============================================================================
// The End 
// ============================================================================
