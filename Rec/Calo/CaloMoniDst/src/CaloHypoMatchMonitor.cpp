// $Id: CaloHypoMatchMonitor.cpp,v 1.2 2005-05-08 09:58:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
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
// ============================================================================
// Event 
// ============================================================================
#include   "Event/TrStoredTrack.h"
// ============================================================================
// local
// ============================================================================
#include   "CaloMoniAlg.h"
// ============================================================================

/** @class CaloHypoMatchMonitor CaloHypoMatchMonitor.cpp
 *  
 *  The algorithm for trivial monitoring of matching of 
 *  "CaloClusters" with TrStored Tracks.
 *  It produces 5 histograms:
 *
 *  <ol> 
 *  <li> @p log10(#Relations+1)  distribution               </li>
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

class CaloHypoMatchMonitor : public CaloMoniAlg
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloHypoMatchMonitor>;
public:
  /// standard algorithm initialization 
  virtual StatusCode initialize()
  {
    StatusCode sc = CaloMoniAlg::initialize();
    if ( sc.isFailure() ) { return sc ; }
    // book the histograms
    book ( 1 , "log10(#Links+1)  : '" + inputData() + "' " + name() , 
           0 ,  4 , 100 ) ;
    // number of relations per hypo
    book ( 2 , "Rels per Hypo    : '" + inputData() + "' " + name() , 
           0 ,   25. ,  50 ) ;
    // minimal weight 
    book ( 3 , "Min weight       : '" + inputData() + "' " + name() , 
           0 ,  100. , 200 ) ;
    // maximal weight 
    book ( 4 , "Max weight       : '" + inputData() + "' " + name() , 
           0 , 1000. , 200 ) ;
    // weights 
    book ( 5 , "Weights          : '" + inputData() + "' " + name() , 
           0 , 1000. , 500 ) ;
    return StatusCode::SUCCESS;
  };
  /// standard algorithm execution 
  virtual StatusCode execute   ();
protected:
  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloHypoMatchMonitor
  ( const std::string& name   , 
    ISvcLocator*       svcloc )
    : CaloMoniAlg( name , svcloc ) {} ;
  /// destructor (virtual and protected)
  virtual ~CaloHypoMatchMonitor() {};
private:
  /// default  construstor  is  private 
  CaloHypoMatchMonitor(); 
  /// copy     construstor  is  private 
  CaloHypoMatchMonitor
  ( const CaloHypoMatchMonitor& );
  /// assignement operator  is  private 
  CaloHypoMatchMonitor& operator=
  ( const CaloHypoMatchMonitor& );
};
// ============================================================================

// ============================================================================
/** @var CaloHypoMatchMonitorFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloHypoMatchMonitor>         s_Factory ;
const        IAlgFactory&CaloHypoMatchMonitorFactory = s_Factory ;

// ============================================================================
/** standard algorithm execution 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloHypoMatchMonitor::execute() 
{
  // avoid long names 
  typedef const CaloHypos                                       Hypos ;
  typedef const IRelationWeighted<CaloHypo,TrStoredTrack,float> Table ;
  typedef Table::Range              Range    ;
  typedef Table::iterator           iterator ;
  
  if ( !produceHistos() ) { return StatusCode::SUCCESS ; }
  
  // check relations 
  const Table* table = get<Table>( inputData() ) ;
  if( 0 == table ) { return StatusCode::FAILURE ; }

  AIDA::IHistogram1D* h1 = histo ( 1 ) ;
  AIDA::IHistogram1D* h2 = histo ( 2 ) ;
  AIDA::IHistogram1D* h3 = histo ( 3 ) ;
  AIDA::IHistogram1D* h4 = histo ( 4 ) ;
  AIDA::IHistogram1D* h5 = histo ( 5 ) ;
  
  // logarithm of ( total number of links + 1 ) 
  hFill ( h1 , log10( table->relations().size() + 1 ) ) ;
  
  if ( inputs().empty() ) 
  { return Error ( "No input data are specified" ) ; }
  
  // loop over all containers 
  for ( Inputs::const_iterator input = inputs().begin() ; 
        inputs().end() != input ; ++input ) 
  {
    // get hypos 
    const Hypos* hypos = get<Hypos>( *input );
    if ( 0 == hypos ) { return StatusCode::FAILURE ; }
    // loop over all hypos 
    for ( Hypos::const_iterator hypo = hypos -> begin() ; 
          hypos -> end() != hypo ; ++hypo ) 
    {
      const Range range = table->relations( *hypo );
      // number of related tracks 
      hFill(  h2 , range.size() ) ;
      if( range.empty() ) { continue ; }
      // minimal weight 
      hFill ( h3 , range.front ().weight() ) ;
      // maximal weight 
      hFill ( h4 , range.back  ().weight() ) ;
      // all weights  
      for( iterator relation = range.begin() ; 
           range.end() != relation ; ++relation )
      { hFill ( h5 , relation->weight() ) ; }    
      
    } ; // end of loop over hypos 
  }; // end of loop over containers 
  
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
