// ============================================================================
// Include files
// Event 
#include "Event/CaloCluster.h"
#include "Event/CaloDigit.h"
// CaloUtils 
#include "CaloUtils/ClusterFunctors.h"
#include "CaloUtils/CaloDataFunctor.h"
#include "CaloUtils/CellMatrix2x2.h"
// local
#include "SubClusterSelector2x2.h"

// ============================================================================
/** @file SubClusterSelector2x2.cpp
 *  
 *  Implementation file for class : SubClusterSelector2x2
 * 
 *  @author F. Machefert
 *  @date 06/14/2014
 */
// ============================================================================

DECLARE_TOOL_FACTORY( SubClusterSelector2x2 )

// ============================================================================
/** Standard Tool Constructor
 *  @param type type of the tool (useless ? )
 *  @param name name of the tool 
 *  @param parent the tool parent 
 */
// ============================================================================

SubClusterSelector2x2::SubClusterSelector2x2( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : SubClusterSelectorBase ( type, name , parent ) 
  , m_matrix ()
{}

// ============================================================================
/** destructor 
 */
// ============================================================================
SubClusterSelector2x2::~SubClusterSelector2x2() {}

// ============================================================================
/** standard initiliazation 
 *  @return status code 
 */
// ============================================================================
StatusCode SubClusterSelector2x2::initialize()
{
  /// initliaze the base class 
  StatusCode sc = SubClusterSelectorBase::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!",sc); }
  if( 0 != det() )
    {
      m_det=det();
      m_matrix.setDet( m_det ) ;
    }
  else 
    { return Error("DeCalorimeter* ponts to NULL!"); }
  ///
  return StatusCode::SUCCESS ;
}

// ============================================================================
/** The main processing method  
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode SubClusterSelector2x2::tag ( LHCb::CaloCluster* cluster ) const{
  double energy;
  CellMatrix2x2::SubMatrixType type;
  //
  StatusCode sc = choice2x2( cluster , type, energy );
  if( sc.isFailure() )return Error( "Error from 'choice2x2()'" , sc ); 
  //
  m_matrix.setType( type );
  sc =LHCb::ClusterFunctors::
      tagTheSubCluster( cluster  ,
                        m_matrix ,
                        modify() ,
                        mask(),
                        LHCb::CaloDigitStatus::ModifiedByMax2x2Tagger ) ;

  //
  if( sc.isFailure() ) 
    { return Error( "Error from 'tagTheSubCluster()'" , sc ); }
  //
  return StatusCode::SUCCESS ;  }

// ============================================================================
/** The main processing method (untag) 
 *  @param cluster pointer to CaloCluster object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode SubClusterSelector2x2::untag ( LHCb::CaloCluster* cluster ) const
{
  double energy;
  CellMatrix2x2::SubMatrixType type;
  //
  StatusCode sc = choice2x2( cluster , type, energy );
  if( sc.isFailure() ) 
    { return Error( "Error from 'choice2x2()'" , sc ); }
  //
  m_matrix.setType( type );
  sc = LHCb::ClusterFunctors::
          untagTheSubCluster( cluster  , 
                              m_matrix , 
                              LHCb::CaloDigitStatus::ModifiedByMax2x2Tagger );
  //
  if( sc.isFailure() ) 
    { return Error( "Error from 'untagTheSubCluster()'" , sc ); }
  //
  return StatusCode::SUCCESS ;
}

// ============================================================================
/** The method that selects the 2x2 cluster out of the full cluster
 *  @param cluster pointer to CaloCluster object to be processed
 *  @param type the selected type of cluster
 *  @param energy the energy of the selected cluster
 *  @return status code
 */
// ============================================================================
StatusCode SubClusterSelector2x2::choice2x2 (
    LHCb::CaloCluster* cluster ,
    CellMatrix2x2::SubMatrixType &type,
    double &energy) const
{
   // check the arguments
   if( 0 == cluster               ) {
       return Error ( "cluster points to NULL in 'choice 2x2()'" );
   }
   // get all entries
   LHCb::CaloCluster::Entries& entries = cluster->entries() ;
   // find seed digit
   LHCb::CaloCluster::Entries::iterator seedEntry =
     LHCb::ClusterFunctors::locateDigit( entries.begin ()          ,
                  entries.end   ()          ,
                  LHCb::CaloDigitStatus::SeedCell );
   // check the seed
   if( entries.end() == seedEntry ) {
       return Error ( "could not find the seed of the cluster in 'choice2x2()'." );
   }
   const LHCb::CaloDigit* seed = seedEntry->digit() ;
   if( 0             == seed      ) {
       return Error ( "cluster seed points to NULL in 'choice2x2()'." );
   }

   LHCb::CaloCellID seedCell = seed->cellID();

   // loop over all entried
   CellMatrix2x2 matrix( m_det );

   double e[4];
   CellMatrix2x2::SubMatrixType mode[4];
   
   mode[0] = CellMatrix2x2::UpperLeft;
   mode[1] = CellMatrix2x2::UpperRight;
   mode[2] = CellMatrix2x2::LowerLeft;
   mode[3] = CellMatrix2x2::LowerRight;

//   std::cout << " * Running on a cluster e=" 
//             << cluster->e() 
//             << std::endl;
   
    for ( int j=0; j<4; ++j ){
      e[j]=0.;
      matrix.setType ( mode[j] );
      //      std::cout << " Mode "<< mode[j] << " -> ";
      for( LHCb::CaloCluster::Entries::iterator entry = entries.begin() ;
	   entries.end() != entry ; ++entry )
        {
	  const LHCb::CaloDigit* digit = entry->digit() ;
	  if( 0 == digit    ){ continue ; } // CONTINUE
	  double tmp             = digit->e();
	  LHCb::CaloCellID  cell = digit->cellID();
	  double frac = matrix ( seedCell, cell ) ;
	  e[j] +=  frac * tmp;
	  //	  std::cout << " - " << frac << " " << tmp << "(" << e[j] <<")" ;
        }
      //      std::cout << " -> " << e[j] << std::endl;
    }
    // select the max energy case
    type= mode[0]; energy = e[0];
    for (int i=1; i<4; ++i) {
      if (e[i]>energy) { energy = e[i] ; type = mode[i]; }
    }
    // std::cout   << " Summary 2x2 clusterisation: "
    //             << e[0] << " " << e[1] << " " << e[2] << " " << e[3]
    //             << " --> " << energy << "[" << type << "]" << std::endl;
    return StatusCode::SUCCESS;
}


// ============================================================================
// The End 
// ============================================================================
