// $Id: CaloClusterMatchMonitor.cpp,v 1.9 2009-03-05 15:52:51 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2009/02/20 18:03:24  odescham
// prepare for DQ
//
// Revision 1.7  2008/09/09 15:37:23  odescham
// review
//
// Revision 1.6  2007/07/25 19:49:12  odescham
// major release : see doc
//
// Revision 1.5  2005/11/07 12:16:38  odescham
// v2r0 - adapt to the new Track Event Model
//
// Revision 1.4  2005/05/13 12:58:35  cattanem
// fixes for windows
//
// Revision 1.3  2005/05/08 09:58:25  ibelyaev
//  remove associators, update options
//
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
  virtual StatusCode initialize()
  { StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
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
  virtual StatusCode finalize();
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  CaloClusterMatchMonitor( const std::string &name, ISvcLocator *pSvcLocator )
    : CaloMoniAlg( name, pSvcLocator )
  { setInputData( "Rec/Calo/ClusterMatch" );
    addToInputs( LHCb::CaloClusterLocation::Ecal );
    addToInputs( LHCb::CaloClusterLocation::EcalSplit );
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
 
DECLARE_ALGORITHM_FACTORY( CaloClusterMatchMonitor );

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
    debug() << "No Table container found at " << inputData() << endreq;
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
  } // end of loop over conainers
  return StatusCode::SUCCESS;
}


StatusCode CaloClusterMatchMonitor::finalize() {
  debug() << "==> Finalize" << endmsg;
  return CaloMoniAlg::finalize();
}
