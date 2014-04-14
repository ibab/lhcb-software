// $Id$
// =============================================================================
// Include files 
// =============================================================================
// GaudiKernel
// =============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/HashMap.h"
// ============================================================================
// LHcCbKernel
// ============================================================================
#include "Kernel/CaloCellIDHash.h"
// ============================================================================
// CaloUtils 
// ============================================================================
#include "CaloUtils/CaloCellIDAsProperty.h"
// =============================================================================
// GaudiAlg
// =============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// =============================================================================
// Event 
// =============================================================================
#include "Event/CaloDigit.h"
// =============================================================================
// CaloDet 
// =============================================================================
#include "CaloDet/DeCalorimeter.h"
// =============================================================================
/** @namespace Kali
 *  helper namespace to keep classes and functions for Calorimeter 
 *  Calibration
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-07-31
 */ 
namespace Kali 
{
  // ===========================================================================
  /** @class MisCalibrateCalo
   *  Helper class for (mis)calibration of Digits 
   *  It is very useful for varius (mis)calibration purposes.
   *
   *  (Mis)Calibration coefficients are defined througgh map-like property:
   * 
   *  @code
   * 
   *  from Configurables import Kali__MisCalibrateCalo
   *
   *  kali = Kali__MisCalibrateCalo( 'MisCalibrateEcal' ) 
   *  kali.Coefficients = { 
   *      1451  : 1.2 ,
   *      1452  : 1.1 ,
   *      ..
   *      4551  : 1.2 ,
   *      1451  : 1.2 
   *  }
   *  @endcode
   *
   *  Where map-index is either the full integer  
   *  representation of CaloCellID, or its 'index' part only
   *
   *  @see LHCb::CaloCellID
   *  
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-07-31
   */
  class MisCalibrateCalo : public GaudiAlgorithm
  {
    // =========================================================================
    /// the friend factory for instantiation
    friend class AlgFactory<Kali::MisCalibrateCalo> ;
    // =========================================================================
  public:
    // =========================================================================
    /// standard initialization 
    virtual StatusCode initialize () ;                 // standard initialization 
    /// execution 
    virtual StatusCode execute    () ;                 //               execution 
    /// standard finalization 
    virtual StatusCode finalize   () ;                 //   standard finalization
    // =========================================================================
  protected:
    // =========================================================================
    /** standard constructor
     *  @param name algorithm instance name 
     *  @param pSvc service locator
     */
    MisCalibrateCalo 
    ( const std::string& name ,                       // algorithm instance name 
      ISvcLocator*       pSvc ) ;                     //         service locator
    /// virtual destructor 
    virtual ~MisCalibrateCalo() {} 
    // =========================================================================
  private:
    // =========================================================================
    /// the default constructor is disabled 
    MisCalibrateCalo () ;                 // the default constructor is disabled 
    /// copy constructor is disabled 
    MisCalibrateCalo ( const MisCalibrateCalo& ) ;        // no copy constructor
    /// assignement is disabled 
    MisCalibrateCalo& operator=( const MisCalibrateCalo& ) ;   // no assignement
    // =========================================================================
  public:
    // =========================================================================
    /// property type 
    // typedef std::map<int,double>   PMap ;                    // property type 
    typedef std::map<LHCb::CaloCellID,double>   PMap ;          // property type 
    /// the actual table of calibration coefficents 
    typedef GaudiUtils::HashMap<LHCb::CaloCellID,double> Table ;
    // =========================================================================
  protected:
    // =========================================================================
    void handleMap ( Property& /* p */ ) ;
    // =========================================================================
    /// update the table of calibration coefficients 
    StatusCode updateTable ( const PMap& pmap )  ;
    // =========================================================================
  private:
    // =========================================================================
    /// TES location of digits 
    std::string m_location ;                           // TES location of digits 
    /// the name of calorimeter 
    std::string m_caloName ;                          // the name of calorimeter 
    /// property: { int : double } 
    PMap        m_pmap     ;                       // property: { int : double } 
    // =========================================================================
  private:
    // =========================================================================
    /// actual table { cell : double } 
    Table       m_table    ;                   // actual table { cell : double } 
    // =========================================================================
  };
  // ===========================================================================
} //                                                       end of namespace Kali 
// =============================================================================
/*  standard constructor
 *  @param name algorithm instance name 
 *  @param pSvc service locator
 */
// =============================================================================
Kali::MisCalibrateCalo::MisCalibrateCalo 
( const std::string& name ,                           // algorithm instance name 
  ISvcLocator*       pSvc )                           //         service locator
  : GaudiAlgorithm ( name , pSvc ) 
  , m_location     ( LHCb::CaloDigitLocation::Ecal )
  , m_caloName     ( DeCalorimeterLocation::Ecal ) 
  , m_pmap  () 
  , m_table () 
{
  // 
  if      ( std::string::npos != name.find ( "Ecal" ) ) 
  {
    m_location = LHCb::CaloDigitLocation ::Ecal ;
    m_caloName = DeCalorimeterLocation   ::Ecal ;  
  }
  else if ( std::string::npos != name.find ( "Hcal" ) ) 
  {
    m_location = LHCb::CaloDigitLocation ::Hcal ;
    m_caloName = DeCalorimeterLocation   ::Hcal ;  
  }
  else if ( std::string::npos != name.find ( "Spd"  ) ) 
  {
    m_location = LHCb::CaloDigitLocation ::Spd  ;
    m_caloName = DeCalorimeterLocation   ::Spd  ;  
  }
  else if ( std::string::npos != name.find ( "Prs"  ) ) 
  {
    m_location = LHCb::CaloDigitLocation ::Prs  ;
    m_caloName = DeCalorimeterLocation   ::Prs  ;  
  }
  //
  declareProperty 
    ( "Digits"        , m_location  ,
      "TES address of digits to be (mis)cailbrated" );
  declareProperty 
    ( "Calorimeter"   , m_caloName  ,
      "Calorimeter to be used"      ) ;
  declareProperty 
    ( "Coefficients"  , m_pmap ,
      "Map of (mis)calibration coefficients" ) 
    -> declareUpdateHandler(&Kali::MisCalibrateCalo::handleMap , this ) ;
}
// =============================================================================
// standard initialization 
// =============================================================================
StatusCode Kali::MisCalibrateCalo::initialize ()      // standard initialization 
{
  StatusCode sc = GaudiAlgorithm::initialize () ;
  if ( sc.isFailure() ) { return sc ; }                                // RETURN 
  //
  m_table.clear() ;
  updateTable ( m_pmap ) ;
  //
  return StatusCode::SUCCESS ;
}
// =============================================================================
// standard finalization 
// =============================================================================
StatusCode Kali::MisCalibrateCalo::finalize  ()       //   standard finalization
{ return GaudiAlgorithm::finalize () ; }
// =============================================================================
// execution
// =============================================================================
StatusCode Kali::MisCalibrateCalo::execute   ()                     // execution
{
  if ( m_table.empty() ) { return StatusCode::SUCCESS ; }              // RETURN 
  //
  // get digits 
  LHCb::CaloDigit::Container* digits = 
    get<LHCb::CaloDigit::Container>( m_location ) ;
  //  
  StatEntity* cnt = 0 ;
  if ( msgLevel ( MSG::DEBUG ) ) { cnt = &counter("#scaled") ; }
  // loop over container
  for ( LHCb::CaloDigit::Container::iterator idigit = digits->begin() ; 
        digits->end() != idigit ; ++idigit ) 
  {
    LHCb::CaloDigit* digit = *idigit ;
    if ( 0 == digit              ) { continue ; }                    // CONTINUE 
    Table::const_iterator ifind = m_table.find ( digit->cellID() ) ;
    if (  ifind == m_table.end() ) { continue ; }                    // CONTINUE 
    //
    const double scale = ifind->second ;
    //
    digit->setE ( digit->e() * scale ) ;
    //
    if ( 0 != cnt ) { (*cnt) += ( scale - 1 ) ; }
  }
  //
  return StatusCode::SUCCESS ;
}
// =============================================================================
// update the table of cofficients 
// =============================================================================
StatusCode Kali::MisCalibrateCalo::updateTable
( const Kali::MisCalibrateCalo::PMap& pmap ) 
{
  // get calorimeter 
  const DeCalorimeter* det = getDet<DeCalorimeter>( m_caloName ) ;
  //
  const int calo = CaloCellCode::CaloNumFromName ( m_caloName ) ;
  Assert  (  0  <= calo , "Invalid Calo index" ) ; 
  const unsigned int icalo = calo ;
  //
  for ( PMap::const_iterator item = pmap.begin() ; 
        pmap.end() != item ; ++item ) 
  {
    // get ID 
    LHCb::CaloCellID cellID ( item->first ) ;
    // redefine calo (if needed)
    if ( icalo != cellID.calo() ) 
    { 
      ++counter("suspicios cell") ;
      cellID.setCalo ( icalo ) ; 
    }
    //
    // check the calo
    if ( !det->valid ( cellID ) )
    {
      ++counter ("invalid cell") ;
      continue ; 
    }
    //
    if ( cellID.isPin() ) 
    {
      ++counter ("pin cell") ;
      continue ; 
    }
    // 
    const double scale = item->second ;
    //
    m_table[ cellID ] = scale ;
    counter ( "cells" ) += scale;
  }
  //
  info () << "Calibration coefficients are defined" << m_table.size() << endmsg ;
  //
  if ( m_table.empty() ) 
  { return Warning ( "No calibration coefficients are defined" ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// Handler for update property 
// ============================================================================
void Kali::MisCalibrateCalo::handleMap ( Property& /* p */ ) 
{
  // no action if not yet initialized 
  if ( FSMState() < Gaudi::StateMachine::INITIALIZED ) { return ; }
  // 
  m_table.clear() ;
  updateTable ( m_pmap ) ;
}
// =============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Kali,MisCalibrateCalo)
// =============================================================================
// The END 
// =============================================================================
