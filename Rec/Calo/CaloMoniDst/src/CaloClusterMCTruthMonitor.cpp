// $Id: CaloClusterMCTruthMonitor.cpp,v 1.3 2005-05-08 09:58:24 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <math.h>
// ============================================================================
// Relations 
// ============================================================================
#include   "Relations/IRelationWeighted.h"
// ============================================================================
// from Gaudi
// ============================================================================
#include   "GaudiKernel/AlgFactory.h"
#include   "GaudiKernel/MsgStream.h" 
#include   "GaudiKernel/IHistogramSvc.h"
// ============================================================================
// AIDA 
// ============================================================================
#include   "AIDA/IHistogram1D.h"
// ============================================================================
// CaloDet 
// ============================================================================
#include   "CaloDet/DeCalorimeter.h"
// ============================================================================
// Event 
// ============================================================================
#include   "Event/CaloHypo.h"
#include   "Event/MCParticle.h"
// ============================================================================
// CaloUtils 
// ============================================================================
#include   "CaloUtils/dirHbookName.h"
// ============================================================================
// local
// ============================================================================
#include   "CaloMoniAlg.h"
// ============================================================================

/** @class CaloClusterMCTruthMonitor CaloClusterMCTruthMonitor.cpp
 *  
 *  The algorithm for trivial monitoring of MCTruthing of 
 *  "CaloClusters" with TrStored Tracks.
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
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */

class CaloClusterMCTruthMonitor : public CaloMoniAlg 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloClusterMCTruthMonitor>;
public:
  /// standard algorithm initialization 
  virtual StatusCode initialize ()
  {
    StatusCode sc = CaloMoniAlg::initialize() ;
    if ( sc.isFailure() ) { return sc ;}
    book ( 1 , "log10(#Links+1)       '" + inputData() + "'" , 
           0 ,   5  , 100 ) ;
    book ( 2 , "Rels/Cluster          '" + inputData() + "'" , 
           0 ,   50       ) ;
    book ( 3 , "Minimal Weight/e      '" + inputData() + "'" , 
           0 ,   2.0      ) ;
    book ( 4 , "Maximal Weight/e      '" + inputData() + "'" , 
           0 ,   2.0      ) ;
    book ( 5 , "        Weight/e      '" + inputData() + "'" , 
           0 ,   2.0      ) ;
    return StatusCode::SUCCESS ;  
  };
  /// standard algorithm execution 
  virtual StatusCode execute    ();  
protected:
  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloClusterMCTruthMonitor
  ( const std::string& name   , 
    ISvcLocator*       svcloc )
    : CaloMoniAlg      ( name , svcloc ) 
  {
    setInputData ( "Rec/Relations/CaloClusters2MCParticles" ) ;
    addToInputs  ( CaloClusterLocation :: Ecal              ) ;
    addToInputs  ( CaloClusterLocation :: EcalSplit         ) ;
  };  
  /// destructor (virtual and protected)
  virtual ~CaloClusterMCTruthMonitor(){} ;
private:
  /// default  construstor  is  private 
  CaloClusterMCTruthMonitor(); 
  /// copy     construstor  is  private 
  CaloClusterMCTruthMonitor
  ( const CaloClusterMCTruthMonitor& );
  /// assignement operator  is  private 
  CaloClusterMCTruthMonitor& operator=
  ( const CaloClusterMCTruthMonitor& );
};
// ============================================================================

// ============================================================================
namespace 
{  
  // ==========================================================================
  /** @var CaloClusterMCTruthMonitorFactory
   *  Declaration of the Algorithm Factory
   */
  // ==========================================================================
  const AlgFactory<CaloClusterMCTruthMonitor>         s_Factory ;
}
const  IAlgFactory&CaloClusterMCTruthMonitorFactory = s_Factory ;
// ============================================================================

// ============================================================================
/** standard algorithm execution 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloClusterMCTruthMonitor::execute() 
{
  // avoid long names 
  typedef const   CaloClusters                                  Clusters ;
  typedef const IRelationWeighted<CaloCluster,MCParticle,float> Table    ;
  typedef Table::Range                                          Range    ;
  typedef Table::iterator                                       iterator ;
  
  // produce histos ?
  if ( !produceHistos() ) { return StatusCode::SUCCESS ; }
  
  // get the relation table 
  const Table* table = get<Table> ( inputData() ) ;
  if (    0 == table  ) { return StatusCode::FAILURE ; }

  AIDA::IHistogram1D* h1 = histo ( 1 ) ;
  AIDA::IHistogram1D* h2 = histo ( 2 ) ;
  AIDA::IHistogram1D* h3 = histo ( 3 ) ;
  AIDA::IHistogram1D* h4 = histo ( 4 ) ;
  AIDA::IHistogram1D* h5 = histo ( 5 ) ;
  
  // total number of links 
  hFill ( h1 , log10( table->relations().size() + 1 ) ) ;
  if ( inputs().empty() ) 
  { return Error ( "Containers of Clusters are not specified " ) ; }
  // make a loop over containers of clusters 
  for ( Inputs::const_iterator input = inputs().begin() ; 
        inputs().end() != input ; ++input ) 
  {
    // get clusters 
    Clusters* clusters = get<Clusters>( *input );
    if ( 0 == clusters ) { return StatusCode::FAILURE ; }
    // loop over clusters 
    for ( Clusters::const_iterator cluster = clusters -> begin() ; 
          clusters -> end() != cluster ; ++cluster ) 
    {
      // get all MC relation for the cluster 
      const Range range = table->relations( *cluster );      
      // number of relations per cluster
      hFill ( h2 , range.size() ) ;
      if ( range.empty() ) { continue ; }
      // cluster energy 
      const double e = (*cluster) -> e() ;
      // minimal weight 
      hFill ( h3 , range.front().weight() / e  ) ;
      // maximal weight 
      hFill ( h4 , range.back ().weight() / e  ) ;      
      // all weights  
      for ( iterator relation = range.begin() ; 
            range.end() != relation ; ++relation )
      { hFill ( h5 , relation->weight() / e ) ; } 
      
    } // end of loop over clusters 
  } // end of loop over containers 
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
