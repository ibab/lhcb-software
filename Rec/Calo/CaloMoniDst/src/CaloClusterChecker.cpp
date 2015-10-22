// ============================================================================
// Include files
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/IRelationWeighted.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloCluster.h"
#include "Event/MCParticle.h"
// ============================================================================
// Local
// ============================================================================
#include "CaloMoniAlg.h"

/** @class CaloClusterChecker CaloClusterChecker.cpp
 *  
 *  The algorithm for trivial monitoring of MCTruthing of 
 *  "CaloClusters" with Tracks.
 *  It produces 5 histograms:
 *
 *  <ol> 
 *  <li> Total Link              distribution               </li>
 *  <li> Link multiplicity       distribution               </li>
 *  <li> Minimal Weight          distribution               </li>
 *  <li> Maximal Weight          distribution               </li>
 *  <li>         Weight          distribution               </li>
 *  </ol>
 *
 *  Histograms reside in the directory @p /stat/"Name" , where 
 *  @ "Name" is the name of the algorithm
 *
 *  @see   CaloMoniAlg
 *  @see GaudiHistoAlg
 *  @see GaudiAlgorithm
 *  @see      Algorithm
 *  @see     IAlgorithm
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */

class CaloClusterChecker : public CaloMoniAlg
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloClusterChecker>;
public:
  /// standard algorithm initialization
  virtual StatusCode initialize()
  { StatusCode sc = CaloMoniAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm
    hBook1( "1", "log10(#Links+1)  '" + inputData() + "'", 0,  5 );
    hBook1( "2", "Rels/Cluster     '" + inputData() + "'", 0, 50 );
    hBook1( "3", "Minimal weight/e '" + inputData() + "'", 0,  2.0 );
    hBook1( "4", "Maximal weight/e '" + inputData() + "'", 0,  2.0 );
    hBook1( "5", "        Weight/e '" + inputData() + "'", 0,  2.0 );
    if( m_split ){
      Warning( "No area spliting allowed for CaloClusterChecker").ignore();
      m_split = false;
    }    
    return StatusCode::SUCCESS;
  }
  /// standard algorithm execution
  virtual StatusCode execute();
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  CaloClusterChecker( const std::string &name,
                             ISvcLocator       *pSvcLocator )
    : CaloMoniAlg( name, pSvcLocator )
  { setInputData( "Relations/"+LHCb::CaloClusterLocation::Default );
    addToInputs( LHCb::CaloClusterLocation::Ecal );
    addToInputs( LHCb::CaloClusterLocation::EcalSplit );
  }
  /// destructor (virtual and protected)
  virtual ~CaloClusterChecker() {}
private:
  /// default  construstor is private
  CaloClusterChecker();
  /// copy     construstor is private
  CaloClusterChecker( const CaloClusterChecker& );
  /// assignement operator is private
  CaloClusterChecker &operator=( const CaloClusterChecker& );
};


 
DECLARE_ALGORITHM_FACTORY( CaloClusterChecker )

// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloClusterChecker::execute()
{ typedef const LHCb::CaloCluster::Container                    Clusters;
  typedef const IRelationWeighted<LHCb::CaloCluster,LHCb::MCParticle,float> Table;
  typedef Table::Range                                          Range;
  typedef Table::iterator                                       iterator;

// produce histos ?
  if ( !produceHistos() ) return StatusCode::SUCCESS;

// get the relation table
  if( !exist<Table> ( inputData() ))return StatusCode::SUCCESS;
  Table *table = get<Table> ( inputData() );
  
  if ( inputs().empty() ) return Error( "No input data are specified" );
  // total number of links
  hFill1( "1", log10( table->relations().size() + 1. ) );
  // loop over all input constainers 
  for( std::vector<std::string>::const_iterator input = inputs().begin();
        inputs().end() != input; ++input )
  { Clusters* clusters = get<Clusters>( *input );
    if ( 0 == clusters ) return StatusCode::FAILURE;
    // loop over clusters
    for( Clusters::const_iterator cluster = clusters->begin();
          clusters->end() != cluster ; ++cluster )
    { const Range range = table->relations( *cluster );
    // number of relations per cluster
      hFill1( "2", range.size() );
      if ( range.empty() ) continue;
// cluster energy
      const double e = (*cluster)->e();
// minimal weight
      hFill1( "3", range.front().weight() / e );
// maximal weight
      hFill1( "4", range.back().weight() / e );
// all weights
      for( iterator relation=range.begin(); range.end()!=relation; ++relation )
      { hFill1( "5", relation->weight() / e );
      }
    } // end of loop over clusters
  } // end of loop over containers
  return StatusCode::SUCCESS;
}
