// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// CaloDet
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
// ============================================================================
// Event 
// ============================================================================
#include  "Event/MCCaloDigit.h"
// ============================================================================
/** @class MCCaloLED 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-03-02
 */
class MCCaloLED : public GaudiAlgorithm
{
  friend class AlgFactory<MCCaloLED> ;
public:
  /// initialize the algorithm 
  virtual StatusCode initialize () ;
  /// execute    the algorithm 
  virtual StatusCode execute    () ;
protected:
  MCCaloLED
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : GaudiAlgorithm ( name , pSvc ) 
    , m_output   ( LHCb::MCCaloHitLocation::Ecal )
    , m_caloName ( DeCalorimeterLocation::Ecal   )
    , m_mean     ( 5.00 * Gaudi::Units::GeV      ) // add 5 Gev of transverse energy
    , m_sigma    ( 0.25 * Gaudi::Units::perCent  ) // sigma is aroun 5 channel per mean 2000 channels
    , m_fraction ( 1.0 / 16.0                    ) // fraction of cells to be fired
    , m_calo(NULL)
    , m_rndmSvc(NULL) 
  {
    declareProperty ( "Output"      , m_output   ) ;
    declareProperty ( "Calorimeter" , m_caloName ) ;
    declareProperty ( "MeanET"      , m_mean     ) ;
    declareProperty ( "Sigma"       , m_sigma    ) ;
    declareProperty ( "Fraction"    , m_fraction ) ;
    //
    setProperty     ( "PropertiesPrint" , "True" ) ;
  } ;
  virtual ~MCCaloLED(){} ;
private:
  // the default constructor is disabled 
  MCCaloLED() ;
  // copy constructor is disabled 
  MCCaloLED            ( const  MCCaloLED& ) ;
  // assignement operator is disabled 
  MCCaloLED& operator= ( const  MCCaloLED& ) ;
private:
  typedef std::vector<LHCb::CaloCellID>  Cells ;
private:
  // location of output hits 
  std::string                m_output   ; ///< location of output hits 
  // calorimeter name
  std::string                m_caloName ; ///< calorimeter name 
  // mean value fo the fired trasnverse energy 
  double                     m_mean     ; ///< mean value fo the fired trasnverse energy 
  // relative sigma on the fired energy
  double                     m_sigma    ; ///< relative sigma on the fired energy
  // fraction of fired hits 
  double                     m_fraction ; ///<fraction of fired hits 
  // list of all Calorimeter cells 
  Cells                      m_cells    ; ///< list of all Calorimeter cells 
  // calorimeter itself 
  DeCalorimeter*             m_calo     ; ///< calorimeter itself 
  // random numbers service 
  IRndmGenSvc*               m_rndmSvc  ; ///< random numbers service 
} ;
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY(MCCaloLED );
// ============================================================================
// Execution of the algorithm
// ============================================================================
StatusCode MCCaloLED::execute    () 
{
  // just for convinience 
  const StatusCode SUCCESS = StatusCode::SUCCESS ;
  
  // create MCHits
  LHCb::MCCaloHit::Container* hits = new LHCb::MCCaloHit::Container() ;
  // register them in TES 
  put ( hits , m_output ) ;
  
  // prepare 2 helper objest to geenrate random numbers 
  Rndm::Numbers gauss ( m_rndmSvc , Rndm::Gauss  ( 0.0 , 1. ) ) ;
  Rndm::Numbers flat  ( m_rndmSvc , Rndm::Flat   ( 0.0 , 1. ) ) ;
  
  // loop over the cells which needs to be fired:
  for ( Cells::const_iterator icell = m_cells.begin() ; 
        m_cells.end() != icell ; ++icell ) 
  {
    // should the cell be fired?
    if ( flat() > m_fraction  ) { continue ; }  // CONTINUE 
    /// get cellID 
    const LHCb::CaloCellID id = *icell ;
    if ( !m_calo->valid( id ) ) { continue ; }  // CONTINUE 
    
    /// generate the energy 
    const double mean   = m_mean / m_calo->cellSine ( id ) / m_calo->activeToTotal() ;
    const double energy = mean * ( 1.0 + m_sigma * gauss() ) ;
  
    // create new MC hit
    LHCb::MCCaloHit*        mchit = new LHCb::MCCaloHit() ;                 
    // fill it 
    mchit->setCellID   ( id     ) ;
    mchit->setTime     ( 0      ) ;
    mchit->setActiveE  ( energy ) ;
    // add hit to the container 
    hits->add( mchit );
  }
  
  // some statistics 
  counter("#hits") += hits->size() ;
  
  return SUCCESS ;
} ;
// ============================================================================
/// initialize the algorithm 
// ============================================================================
StatusCode MCCaloLED::initialize() 
{
  // initialize the base 
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  // get the random service 
  m_rndmSvc = svc<IRndmGenSvc>( "RndmGenSvc" , true) ;
  // get the calorimeter 
  m_calo = getDet<DeCalorimeter>( m_caloName )  ;
  // fill container of cells ;
  m_cells.clear() ;
  m_cells.reserve ( m_calo->numberOfCells() ) ;
  int iCalo = CaloCellCode::CaloNumFromName ( m_caloName ) ;
  if ( 0 > iCalo ) { return Error("Invaild Calo Code!") ; }
  
  for ( int iArea = 0 ; iArea < 3 ; ++iArea ) 
  {
    for ( int iRow = 0 ; iRow < 64 ; ++iRow ) 
    {
      for ( int iCol = 0 ; iCol < 64 ; ++iCol ) 
      {
        // create CellID 
        const LHCb::CaloCellID id ( iCalo , iArea , iRow , iCol ) ;
        // check it ! 
        if ( !m_calo->valid ( id ) ) { continue ; }
        // add it into list of good cells
        m_cells.push_back( id ) ;
      }
    } 
  }
  {
    // remove duplicates (if any) 
    std::sort ( m_cells.begin() , m_cells.end() ) ;
    Cells::iterator iend = std::unique( m_cells.begin() , m_cells.end() ) ;
    m_cells.erase ( iend , m_cells.end() ) ;
  }
  //
  if ( propsPrint() || msgLevel( MSG::DEBUG ) ) 
  { info() << "Number of valid cells: " << m_cells.size() << endmsg ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// The END 
// ============================================================================
