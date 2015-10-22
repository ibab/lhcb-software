// ============================================================================
// Include files
// ============================================================================
// Relations
// ============================================================================
#include   "Relations/IRelationWeighted.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/CaloCluster.h"
#include "Event/Track.h"
// ============================================================================
// CaloUtils
// ============================================================================
#include "CaloUtils/Calo2Track.h"
// ============================================================================
// local
// ============================================================================
#include   "CaloMoniAlg.h"
// ============================================================================

/** @class CaloClusterMatchMonitor CaloClusterMatchMonitor.cpp
 *
 *  The algorithm for trivial monitoring of matching of
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
 *  @see     Algorithm
 *  @see    IAlgorithm
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */

class CaloClusterMatchMonitor : public CaloMoniAlg
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloClusterMatchMonitor>;
public:
  /// standard algorithm initialization
  virtual StatusCode initialize(){ 
    StatusCode sc = CaloMoniAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm
    hBook1( "1", "log10(#Links+1) '"+inputData()+"'", 0,    4, 100 );
    hBook1( "2", "Tracks per cluster",                0,   25,  25 );
    hBook1( "3", "Minimal weight",                    0,  100, 200 );
    hBook1( "4", "Maximal weight",                    0, 1000, 200 );
    hBook1( "5", "Weights",                           0, 1000, 500 );
    if( m_split ){
      Warning( "No area spliting allowed for CaloClusterMatchMonitor").ignore();
      m_split = false;
    }    
    return StatusCode::SUCCESS;
  }
  virtual StatusCode execute();
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  CaloClusterMatchMonitor( const std::string &name, ISvcLocator *pSvcLocator )
    : CaloMoniAlg( name, pSvcLocator )
  { setInputData( LHCb::CaloAlgUtils::CaloIdLocation("ClusterMatch",context() ) );
    addToInputs( LHCb::CaloAlgUtils::CaloClusterLocation(name,context() ) );
    addToInputs( LHCb::CaloAlgUtils::CaloSplitClusterLocation( context() ) );
  }
  /// destructor (virtual and protected)
  virtual ~CaloClusterMatchMonitor() {};
private:
  /// default  construstor  is  private
  CaloClusterMatchMonitor();
  /// copy     construstor  is  private
  CaloClusterMatchMonitor( const CaloClusterMatchMonitor& );
  /// assignement operator  is  private
  CaloClusterMatchMonitor &operator=( const CaloClusterMatchMonitor& );
};
 
DECLARE_ALGORITHM_FACTORY( CaloClusterMatchMonitor )

// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloClusterMatchMonitor::execute()
{ typedef const LHCb::CaloCluster::Container   Clusters;
  typedef const LHCb::Calo2Track::IClusTrTable Table;
  typedef Table::Range                         Range;
  typedef Table::iterator                      iterator;

// produce histos ?
  if ( !produceHistos() ) return StatusCode::SUCCESS;

// check relations
  if ( inputs().empty() ) return Error( "No input data are specified" );
  if( !exist<Table>( inputData() ) ){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "No Table container found at " << inputData() << endmsg;
    return StatusCode::SUCCESS;
  }  
  Table *table = get<Table>( inputData() );

// total number of links
  hFill1( "1", log10( table->relations().size() + 1. ) );
// loop over all input constainers
  for( std::vector<std::string>::const_iterator input = inputs().begin();inputs().end() != input; ++input ){
    Clusters* clusters = get<Clusters>( *input );
    if ( 0 == clusters ) return StatusCode::SUCCESS;
    // loop over all clusters
    for ( Clusters::const_iterator cluster = clusters->begin();
          clusters->end() != cluster ; ++cluster ){ 
      const Range range = table->relations( *cluster );
      // number of related tracks
      hFill1( "2", range.size() );
      if ( range.empty() ) continue;
      // minimal weight
      hFill1( "3", range.front().weight() );
      // maximal weight
      hFill1( "4", range.back().weight() );
      // all weights
      for( iterator relation=range.begin(); range.end()!=relation; ++relation ){ 
        hFill1( "5", relation->weight() );
      }
    } // end of loop over clusters
    counter("Monitor " + *input) += clusters->size();
  } // end of loop over containers
  counter("Monitor " + inputData() )+= table->relations().size();
  return StatusCode::SUCCESS;
}
