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
 *  @ "Name" is the name of the algorithm
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
  /// standard algorithm execution
  virtual StatusCode execute();
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  CaloDigitMonitor( const std::string &name, ISvcLocator *pSvcLocator )
    : CaloMoniAlg( name, pSvcLocator )
  {    declareProperty( "ECuts",  m_eCuts );
    declareProperty( "EtCuts", m_etCuts );
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
  std::vector<double> m_eCuts;
  std::vector<double> m_etCuts;
};

DECLARE_ALGORITHM_FACTORY( CaloDigitMonitor );

// ============================================================================
// standard initialize method
// ============================================================================
StatusCode CaloDigitMonitor::initialize()
{ StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm
  if      ( "Ecal" == detData() ) m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  else if ( "Hcal" == detData() ) m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
  else if ( "Prs"  == detData() ) m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
  else if ( "Spd"  == detData() ) m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Spd );
  else 
  { return Error( "Unknown detector name "+detData() );
  }
  hBook1( "1", "# of Digits",          0,   6000,  600 );
  hBook2( "2", "Digits (col,row) Area = 0  " , 0., 63. , 64, 0. ,63., 64);
  hBook2( "3", "Digits (col,row) Area = 1  " , 0., 63. , 64, 0. ,63., 64);
  if("Hcal" != detData() )hBook2( "4", "Digits (col,row) Area = 2  " , 0., 63. , 64, 0. ,63., 64);
  hBook2( "5", "Energy(col,row) Area = 0  " , 0., 63. , 64, 0. ,63., 64);
  hBook2( "6", "Energy(col,row) Area = 1  " , 0., 63. , 64, 0. ,63., 64);
  if("Hcal" != detData() )hBook2( "7", "Digit(col,row) Area = 2  " , 0., 63. , 64, 0. ,63., 64);



  unsigned int i;
  char         hname[128];
  for( i=0; i<m_eCuts.size(); i++ )  {
    std::stringstream ecut("");
    ecut << "eCut_" << m_eCuts[i]/Gaudi::Units::MeV ;
    
    if ( m_eCuts[i] > 0 ){
      sprintf( hname, "# of Digits (e>%.0f MeV)", m_eCuts[i]/Gaudi::Units::MeV );
      hBook1( ecut.str() + "/1", hname, 0,     200,  200 );

      sprintf( hname, "Digits (col,row) Area = 0  (e>%.0f MeV)", m_eCuts[i]/Gaudi::Units::MeV );
      hBook2( ecut.str() + "/2" , hname , 0., 63. , 64, 0. ,63., 64);

      sprintf( hname, "Digits (col,row) Area = 1  (e>%.0f MeV)", m_eCuts[i]/Gaudi::Units::MeV );
      hBook2( ecut.str() + "/3" , hname , 0., 63. , 64, 0. ,63., 64);

      if("Hcal" != detData() ){
        sprintf( hname, "Digits (col,row) Area = 2  (e>%.0f MeV)", m_eCuts[i]/Gaudi::Units::MeV );
        hBook2( ecut.str() + "/4" , hname , 0., 63. , 64, 0. ,63., 64);
      }  

      sprintf( hname, "Energy (col,row) Area = 0  (e>%.0f MeV)", m_eCuts[i]/Gaudi::Units::MeV );
      hBook2( ecut.str() + "/5" , hname , 0., 63. , 64, 0. ,63., 64);

      sprintf( hname, "Energy (col,row) Area = 1  (e>%.0f MeV)", m_eCuts[i]/Gaudi::Units::MeV );
      hBook2( ecut.str() + "/6" , hname , 0., 63. , 64, 0. ,63., 64);

      if("Hcal" != detData() ){
        sprintf( hname, "Energy (col,row) Area = 2  (e>%.0f MeV)", m_eCuts[i]/Gaudi::Units::MeV );
        hBook2( ecut.str() + "/7" , hname , 0., 63. , 64, 0. ,63., 64);
      }
    } 
  }
  for( i=0; i<m_etCuts.size(); i++ ){ 
    std::stringstream etcut("");
    etcut << "etCut_" << m_etCuts[i]/Gaudi::Units::MeV ;

    if ( m_etCuts[i] > 0 ){ 
      sprintf( hname, "# of Digits (et>%.0f MeV)", m_etCuts[i]/Gaudi::Units::MeV );
      hBook1( etcut.str() + "/1", hname, 0,     200,  200 );

      sprintf( hname, "Digits (col,row) Area = 0  (et>%.0f MeV)", m_etCuts[i]/Gaudi::Units::MeV );
      hBook2( etcut.str() + "/2" , hname , 0., 63. , 64, 0. ,63., 64);

      sprintf( hname, "Digits (col,row) Area = 1  (et>%.0f MeV)", m_etCuts[i]/Gaudi::Units::MeV );
      hBook2( etcut.str() + "/3" , hname , 0., 63. , 64, 0. ,63., 64);

      if("Hcal" != detData() ){
        sprintf( hname, "Digits (col,row) Area = 2  (et>%.0f MeV)", m_etCuts[i]/Gaudi::Units::MeV );
        hBook2( etcut.str() + "/4" , hname , 0., 63. , 64, 0. ,63., 64);
      }  

      sprintf( hname, "Energy (col,row) Area = 0  (et>%.0f MeV)", m_etCuts[i]/Gaudi::Units::MeV );
      hBook2( etcut.str() + "/5" , hname , 0., 63. , 64, 0. ,63., 64);

      sprintf( hname, "Energy (col,row) Area = 1  (et>%.0f MeV)", m_etCuts[i]/Gaudi::Units::MeV );
      hBook2( etcut.str() + "/6" , hname , 0., 63. , 64, 0. ,63., 64);

      if("Hcal" != detData() ){
        sprintf( hname, "Energy (col,row) Area = 2  (et>%.0f MeV)", m_etCuts[i]/Gaudi::Units::MeV );
        hBook2( etcut.str() + "/7" , hname , 0., 63. , 64, 0. ,63., 64);
      }
    }
  }
  return StatusCode::SUCCESS;
}
// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloDigitMonitor::execute()
{ typedef const LHCb::CaloDigit::Container Digits;
  unsigned int hid, i;

// produce histos ?
  if ( !produceHistos() ) return StatusCode::SUCCESS;

// get input data
  Digits* digits = get<Digits> ( inputData() );
  if ( 0 == digits ) return StatusCode::FAILURE;

// fill multiplicity histogram
  hFill1( "1", digits->size() );
  std::vector<int> nDigits( m_eCuts.size()+m_etCuts.size() );

  for( Digits::const_iterator digit = digits->begin();
       digits->end() != digit ; ++digit )
  { if ( 0 == *digit ) continue;

    const LHCb::CaloCellID &cellID = (*digit)->cellID();
    const unsigned int  col = cellID.col();
    const unsigned int  row = cellID.row();    
    const unsigned int  area = cellID.area();    
    const double            e      = (*digit)->e();
    const double            et     = e * m_calo->cellSine( cellID );

    if( 0 == area ) hFill2( "2", row, col );
    if( 1 == area ) hFill2( "3", row, col );
    if( 2 == area ) hFill2( "4", row, col );
    if( 0 == area ) hFill2( "5", row, col ,e );
    if( 1 == area ) hFill2( "6", row, col ,e );
    if( 2 == area ) hFill2( "7", row, col ,e );

//
    for( i=0; i<m_eCuts.size(); i++ ){ 
      std::stringstream ecut("");
      ecut << "eCut_" << m_eCuts[i]/Gaudi::Units::MeV ;
      if (( m_eCuts[i] > 0 ) && ( e > m_eCuts[i] )){ 
        nDigits[i]++;
        if( 0 == area ) hFill2( ecut.str() +"/2", row, col );
        if( 1 == area ) hFill2( ecut.str() +"/3", row, col );
        if( 2 == area ) hFill2( ecut.str() +"/4", row, col );
        if( 0 == area ) hFill2( ecut.str() +"/5", row, col ,e );
        if( 1 == area ) hFill2( ecut.str() +"/6", row, col ,e );
        if( 2 == area ) hFill2( ecut.str() +"/7", row, col ,e );
      }
    }
    for( i=0; i<m_etCuts.size(); i++ ){ 
      std::stringstream etcut("");
      etcut << "etCut_" << m_etCuts[i]/Gaudi::Units::MeV ;
      if (( m_etCuts[i] > 0 ) && ( et > m_etCuts[i] )){
        nDigits[m_eCuts.size()+i]++;
        if( 0 == area ) hFill2( etcut.str() +"/2", row, col );
        if( 1 == area ) hFill2( etcut.str() +"/3", row, col );
        if( 2 == area ) hFill2( etcut.str() +"/4", row, col );
        if( 0 == area ) hFill2( etcut.str() +"/5", row, col ,e );
        if( 1 == area ) hFill2( etcut.str() +"/6", row, col ,e );
        if( 2 == area ) hFill2( etcut.str() +"/7", row, col ,e );
      }
    }
  }
  hid = 5;
  for( i=0; i<m_eCuts.size(); i++ ){
    std::stringstream ecut("");
    ecut << "eCut_" << m_eCuts[i]/Gaudi::Units::MeV ;
    if ( m_eCuts[i] > 0 )hFill1( ecut.str() +"/1", nDigits[i] );
  }
  for( i=0; i<m_etCuts.size(); i++ ){
    std::stringstream etcut("");
    etcut << "etCut_" << m_etCuts[i]/Gaudi::Units::MeV ;
    if ( m_etCuts[i] > 0 )hFill1( etcut.str() +"/1", nDigits[m_eCuts.size()+i] );
  }

  return StatusCode::SUCCESS;
}
