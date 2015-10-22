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
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  CaloDigitMonitor( const std::string &name, ISvcLocator *pSvcLocator )
    : CaloMoniAlg( name, pSvcLocator )
    , m_calo(NULL)
  { 
    declareProperty( "Spectrum", m_spectrum = false); // activate spectrum per channel histogramming

    setInputData( LHCb::CaloAlgUtils::CaloDigitLocation( name ) );
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
  bool m_spectrum;
//
};

DECLARE_ALGORITHM_FACTORY( CaloDigitMonitor )

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
  if( detData() != "Spd" ) hBook1( "2", detData() + "  digits energy" , m_energyMin ,   m_energyMax,  m_energyBin );
  if( detData() != "Spd" && detData() != "Prs")hBook1( "3", detData() + "  digits Et"     , m_etMin ,   m_etMax  , m_etBin);
  hBook1(  "4", "Hypo X        " + inputData(),  m_xMin      ,    m_xMax      , m_xBin    );
  hBook1(  "5", "Hypo Y        " + inputData(),  m_yMin      ,    m_yMax      , m_yBin );
  hBook2(  "6", "Digit position x vs y   " + inputData(),  m_xMin, m_xMax, m_xBin, m_yMin, m_yMax, m_yBin);
  hBook2(  "7", "Energy-weighted digit position x vs y " + inputData(),m_xMin, m_xMax, m_xBin, m_yMin, m_yMax, m_yBin);

  info() << detData() << " digits from " << inputData() << endmsg;




  
  return StatusCode::SUCCESS;
}
// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloDigitMonitor::execute(){
  typedef const LHCb::CaloDigit::Container Digits;
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << name() << " execute " << endmsg;
  
  // produce histos ?
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << " Producing histo " << produceHistos() << endmsg;
  if ( !produceHistos() ) return StatusCode::SUCCESS;
  
  // get input data
  if( !exist<Digits>(inputData())){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "no digit container found at " << inputData() << endmsg;
    return StatusCode::SUCCESS;
  }
  Digits* digits = get<Digits> ( inputData() );
  if ( digits -> empty() ){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "No digit found in " << inputData() << endmsg;
    return StatusCode::SUCCESS;
  }  
  
  initCounters();
  for( Digits::const_iterator digit = digits->begin(); digits->end() != digit ; ++digit ){
    if ( 0 == *digit ) continue;
    const LHCb::CaloCellID  id     = (*digit)->cellID();
    const double            e      = (*digit)->e();
    const double            et     = e * m_calo->cellSine( id );
    if( e  < m_eFilter)continue;
    if( et < m_etFilter)continue;    
    count( id );
    hFill1(id , "2", e  );
    if( detData() != "Spd" && detData() != "Prs")hFill1(id, "3", et ); 
    double x = m_calo->cellCenter(id).X();
    double y = m_calo->cellCenter(id).Y();
    hFill1(id, "4", x  );
    hFill1(id, "5", y  );
    hFill2(id, "6", x, y );
    hFill2(id, "7", x, y, e );
    if(doHisto("8"))fillCalo2D("8", id , 1. , detData() + " digits position 2D view");
    if( detData() != "Spd" && doHisto("9") )fillCalo2D("9", id , e  , detData() + " digits energy 2D view");

    
    
    if (m_spectrum) {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << "Filling cell by cell histograms" << endmsg;
      int col = id.col();
      int row = id.row();
      std::ostringstream tit;
      tit << detData() << " channel : " << id;
      std::string unit = detData() + "Cells/" + id.areaName() + "/" 
        + Gaudi::Utils::toString(row) + ";" + Gaudi::Utils::toString(col);
      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) 
        verbose() << " et  " << et << " cell " << unit << endmsg;
      if( detData() == "Prs") 
        plot1D(e , unit, tit.str() , m_energyMin, m_energyMax, m_energyBin);
      else
        plot1D(et , unit, tit.str() , m_etMin, m_etMax, m_etBin);      
    }
  }
  fillCounters("1");  
  return StatusCode::SUCCESS;
}
