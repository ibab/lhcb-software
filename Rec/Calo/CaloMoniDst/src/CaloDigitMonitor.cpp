// Include files
// ============================================================================
// Event
// ============================================================================
#include  "Event/CaloDigit.h"
// ============================================================================
// CaloDet
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
// ============================================================================
// local
// ============================================================================
#include  "CaloMoniAlg.h"
// ============================================================================

/** @class CaloDigitMonitor CaloDigitMonitor.cpp
 *
 *  The algorithm for trivial monitoring of "CaloDigit" containers.
 *  The algorithm produces the following histograms:
 *   1. CaloDigit multiplicity
 *   2. CaloDigit ocupancy 2D plot per area
 *   3. CaloDigit energy 2D plot per area
 *  The same set of histograms, but with cut on Et (or E), is produced if specified
 *
 *  Histograms reside in the directory @p /stat/"Name" , where
 *  @p "Name" is the name of the algorithm
 *
 *  @see   CaloMoniAlg
 *  @see GaudiHistoAlg
 *  @see GaudiAlgorithm
 *  @see      Algorithm
 *  @see     IAlgorithm
 *
 *  @author Konstantin Belous Konstantin.Beloous@itep.ru
 *  @date   21/06/2007
 */

class CaloDigitMonitor : public CaloMoniAlg
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloDigitMonitor>;
public:
  /// standard algorithm initialization 
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  CaloDigitMonitor( const std::string &name, ISvcLocator *pSvcLocator )
    : CaloMoniAlg( name, pSvcLocator ){
    if(detData()     == "Ecal" ){setInputData( LHCb::CaloDigitLocation::Ecal );}
    else if(detData()== "Hcal" ){setInputData( LHCb::CaloDigitLocation::Hcal );}    
    else if(detData()== "Prs"  ){setInputData( LHCb::CaloDigitLocation::Prs  );}
    else if(detData()== "Spd"  ){setInputData( LHCb::CaloDigitLocation::Spd  );}
  }
  /// destructor (virtual and protected)
  virtual ~CaloDigitMonitor() {}
private:
  /// default  construstor  is  private
  CaloDigitMonitor();
  /// copy     construstor  is  private
  CaloDigitMonitor( const CaloDigitMonitor& );
  /// assignement operator  is  private
  CaloDigitMonitor &operator=( const CaloDigitMonitor& );
private:
  DeCalorimeter *m_calo;
//
};

DECLARE_ALGORITHM_FACTORY( CaloDigitMonitor );

// ============================================================================
// standard initialize method
// ============================================================================
StatusCode CaloDigitMonitor::initialize(){ 
  StatusCode sc = CaloMoniAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm

  if      ( "Ecal" == detData() ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );  }  
  else if ( "Hcal" == detData() ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );  }
  else if ( "Prs"  == detData() ){
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs  );  }  
  else if ( "Spd"  == detData() ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Spd  );
  }  
  else{
    return Error( "Unknown detector name "+detData() );
  }
  hBook1( "1", detData() + " : # of Digits"   , m_multMin   ,   m_multMax,  m_multBin   );
  if( detData() != "Spd" ) hBook1( "2", detData() + " : digits energy" , m_energyMin ,   m_energyMax,  m_energyBin );
  if( detData() != "Spd" && detData() != "Prs")
    hBook1( "3", detData() + " : digits Et"     , m_etMin     ,   m_etMax    ,  m_etBin     );

  info() << detData() << " digits from " << inputData() << endreq;
  
  return StatusCode::SUCCESS;
}
// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloDigitMonitor::execute(){
  typedef const LHCb::CaloDigit::Container Digits;
  debug() << name() << " execute " << endreq;
 
// produce histos ?
  debug() << " Producing histo " << produceHistos() << endreq;
  if ( !produceHistos() ) return StatusCode::SUCCESS;
  
  // get input data
  if( !exist<Digits>(inputData())){
    debug() << "no digit container found at " << inputData() << endreq;
    return StatusCode::SUCCESS;
  }
  

  Digits* digits = get<Digits> ( inputData() );
  if ( 0 == digits ){
    debug() << "no digit found in " << inputData() <<endreq;
    return StatusCode::SUCCESS;
  }
  
  long count = 0;
  for( Digits::const_iterator digit = digits->begin();
       digits->end() != digit ; ++digit ){
    if ( 0 == *digit ) continue;
    const LHCb::CaloCellID  id     = (*digit)->cellID();
    const double            e      = (*digit)->e();
    const double            et     = e * m_calo->cellSine( id );
    if( e  < m_eFilter)continue;
    if( et < m_etFilter)continue;    
    count++;
    hFill1( "2", e );
    if( detData() != "Spd" && detData() != "Prs")hFill1( "3", et ); 
    //    double x = m_calo->cellCenter(id).X();
    //    double y = m_calo->cellCenter(id).Y();
    fillCalo2D("4", id , 1. , detData() + " digits position 2D view");
    if( detData() != "Spd" )fillCalo2D("5", id , e  , detData() + " digits energy 2D view");
  }
  hFill1( "1", count );
  
  return StatusCode::SUCCESS;
}



StatusCode CaloDigitMonitor::finalize() {
  debug() << "==> Finalize" << endmsg;
  return CaloMoniAlg::finalize();
}
