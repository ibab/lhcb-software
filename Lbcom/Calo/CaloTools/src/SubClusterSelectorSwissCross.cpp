// $Id: SubClusterSelectorSwissCross.cpp,v 1.2 2001-11-08 20:07:06 ibelyaev Exp $
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
#include "CaloEvent/CaloDataFunctor.h"
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
/** The main processing method ( functor interface ) 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode 
SubClusterSelectorSwissCross::operator() ( CaloCluster* cluster ) const 
{
  /// check the arguments 
  if( 0 == cluster   ) { return StatusCode ( 225 )  ; }   ///< RETURN
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
  if( 0 == seed             ) { return StatusCode ( 228 ) ; }
  /// loop over all digits and select SwissCross submatrix 
  for( CaloCluster::Digits::iterator iDigit = digits.begin() ;
       digits.end() != iDigit ; ++iDigit )
    {
      CaloDigitStatus& status = iDigit->second ;
      /// reset the existing status 
      status.removeStatus ( DigitStatus::UseForEnergy     ) ;
      status.removeStatus ( DigitStatus::UseForPosition   ) ;
      status.removeStatus ( DigitStatus::UseForCovariance ) ;          
      /// skip invalid digits 
      const CaloDigit* digit  = iDigit->first   ;
      if( 0 == digit      ) { continue;  }  ///< CONTINUE 
      /// check for valid digits and select SwissCross submatrix  
      if( 0 <  m_matrix( seed->cellID() , digit->cellID() ) )
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
