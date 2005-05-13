// $Id: CaloClusterMatchMonitor.cpp,v 1.4 2005-05-13 12:58:35 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2005/05/08 09:58:25  ibelyaev
//  remove associators, update options
//
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
#include   "Event/TrStoredTrack.h"
// ============================================================================
// local
// ============================================================================
#include   "CaloMoniAlg.h"
// ============================================================================

/** @class CaloClusterMatchMonitor CaloClusterMatchMonitor.cpp
 *  
 *  The algorithm for trivial monitoring of matching of 
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

class CaloClusterMatchMonitor : public CaloMoniAlg 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloClusterMatchMonitor>;
public:
  /// standard algorithm initialization 
  virtual StatusCode initialize() 
  {
    StatusCode sc = CaloMoniAlg::initialize() ;
    if ( sc.isFailure() ) { return sc ; }
    book ( 1 , "log10(#Links+1)     '" + inputData() + "'" , 0 , 4    , 100 ) ;
    book ( 2 , "#Tracks per cluster '" + inputData() + "'" , 0 , 25   ,  50 ) ;
    book ( 3 , "Minimal weight      '" + inputData() + "'" , 0 , 100  , 200 ) ;
    book ( 4 , "Maximal weight      '" + inputData() + "'" , 0 , 1000 , 200 ) ;
    book ( 5 , "Weights             '" + inputData() + "'" , 0 , 1000 , 500 ) ;  
    return StatusCode::SUCCESS ;
  };
  /// standard algorithm execution
  virtual StatusCode execute   () ;
protected:
  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloClusterMatchMonitor
  ( const std::string& name   , 
    ISvcLocator*       svcloc )
    : CaloMoniAlg     ( name , svcloc ) 
  {
    setInputData    ( "Rec/Calo/PhotonMatch"          ) ;
    addToInputs     ( CaloClusterLocation:: Ecal      ) ;
    addToInputs     ( CaloClusterLocation:: EcalSplit ) ;
  };
  /// destructor (virtual and protected)
  virtual ~CaloClusterMatchMonitor(){} ;
private:
  /// default  construstor  is  private 
  CaloClusterMatchMonitor(); 
  /// copy     construstor  is  private 
  CaloClusterMatchMonitor
  ( const CaloClusterMatchMonitor& );
  /// assignement operator  is  private 
  CaloClusterMatchMonitor& operator=
  ( const CaloClusterMatchMonitor& );
};
// ============================================================================

// ============================================================================
/** @var CaloClusterMatchMonitorFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloClusterMatchMonitor>         s_Factory ;
const        IAlgFactory&CaloClusterMatchMonitorFactory = s_Factory ;

// ============================================================================
/** standard algorithm execution 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloClusterMatchMonitor::execute() 
{
  // avoid long names 
  typedef const   CaloClusters                                     Clusters ;
  typedef const IRelationWeighted<CaloCluster,TrStoredTrack,float> Table    ;
  typedef Table::Range                                             Range    ;
  typedef Table::iterator                                          iterator ;
  
  // produce histos ?
  if ( !produceHistos() ) { return StatusCode::SUCCESS ; }
  
  // check relations 
  const Table* table = get<Table>( inputData() ) ;
  if ( 0 == table ) { return StatusCode::FAILURE ; }
  
  AIDA::IHistogram1D* h1 = histo ( 1 ) ;
  AIDA::IHistogram1D* h2 = histo ( 2 ) ;
  AIDA::IHistogram1D* h3 = histo ( 3 ) ;
  AIDA::IHistogram1D* h4 = histo ( 4 ) ;
  AIDA::IHistogram1D* h5 = histo ( 5 ) ;
  
  // total number of links 
  hFill ( h1 , log10( table->relations().size() + 1. ) ) ;
  
  if ( inputs().empty() ) 
  { return Error ( "No input data are specified" ) ; }
  
  /// loop over all input constainers 
  for ( Inputs::const_iterator input = inputs().begin() ; 
        inputs().end() != input ; ++input ) 
  {
    // get input data 
    Clusters* clusters = get<Clusters>( *input );
    if ( 0 == clusters ) { return StatusCode::FAILURE ; }
    // loop over all clusters 
    for ( Clusters::const_iterator cluster = clusters -> begin() ; 
          clusters -> end() != cluster ; ++cluster ) 
    {
      const Range range = table->relations( *cluster );
      // number of related tracks 
      hFill ( h2 , range.size() ) ;
      if ( range.empty() ) { continue ; }
      // minimal weight 
      hFill ( h3 ,  range.front().weight() ) ;
      // maximal weight 
      hFill ( h4 ,  range.back ().weight() ) ;
      // all weights  
      for ( iterator relation = range.begin() ; 
            range.end() != relation ; ++relation )
      { hFill ( h5 , relation->weight() ); }
    }; // end of loop over clusters
  }; // end of loop over conainers
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
