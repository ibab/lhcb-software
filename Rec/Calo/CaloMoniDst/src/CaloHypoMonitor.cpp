// $Id: CaloHypoMonitor.cpp,v 1.1.1.1 2004-10-25 09:00:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2004/02/17 12:00:58  ibelyaev
//  add new algorithm and update for new base
//
// Revision 1.6  2002/11/13 20:49:11  ibelyaev
//  small update of monitoring algorithms
//
// ============================================================================
// Include files
// from Gaudi
#include  "GaudiKernel/AlgFactory.h"
#include  "GaudiKernel/MsgStream.h" 
#include  "GaudiKernel/IHistogramSvc.h"
// AIDA 
#include  "AIDA/IHistogram1D.h"
// CaloDet 
#include  "CaloDet/DeCalorimeter.h"
// Event 
#include  "Event/CaloHypo.h"
// CaloUtils 
#include  "CaloUtils/dirHbookName.h"
// local
#include  "CaloHypoMonitor.h"

// ============================================================================
/** @file CaloHypoMonitor.cpp
 * 
 *  Template implementation file for class : CaloHypoMonitor
 *  @see CaloHypoMonitor
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/11/2001
 */
// ============================================================================

// ============================================================================
/** @var CaloHypoMonitorFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloHypoMonitor>         s_Factory ;
const        IAlgFactory&CaloHypoMonitorFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloHypoMonitor::CaloHypoMonitor
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : CaloAlgorithm  ( name , svcloc ) 
  , m_multiplicity (   0        )
  , m_multMax      ( 100        )
  , m_energy       (   0        ) 
  , m_energyMax    ( 200. * GeV )
  , m_pt           (   0        )
  , m_ptMax        (  10. * GeV )
  , m_mass         (   0        )
  , m_massMin      (   0. * GeV )
  , m_massMax      (   1. * GeV )
  , m_e            (   0        )
  , m_x            (   0        )
  , m_y            (   0        )
  , m_clusters     (   0        )
  , m_spd          (   0        )
  , m_prs          (   0        )
{
  declareProperty( "MultiplicityMax" , m_multMax   ) ;
  declareProperty( "EnergyMax"       , m_energyMax ) ;
  declareProperty( "PtMax"           , m_ptMax     ) ;
  declareProperty( "MassMin"         , m_massMin   ) ;
  declareProperty( "MassMax"         , m_massMax   ) ;
};
// ============================================================================

// ============================================================================
/** destructor
 */
// ============================================================================
CaloHypoMonitor::~CaloHypoMonitor() {};
// ============================================================================

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloHypoMonitor::initialize() 
{  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  
  StatusCode sc = CaloAlgorithm::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class CaloAlgorithm",sc);}
  
  // PAWC/RZ directory
  const std::string directory( dirHbookName( "Calo/" + name() ) );
  
  // book the histograms
  // cluster multiplicity
  m_multiplicity   =
    histoSvc () -> book ( directory                        ,  ///< directory
                          1                                ,  ///< ID
                          "# of Hypos  "    + inputData()  ,  ///< title
                          200                              ,  ///< #bins
                          0                                ,  ///< low edge
                          m_multMax                        ); ///< upper edge
  if( 0 == m_multiplicity )
    { return Error("Could not book 'Hypo Multiplicity' histogram" ) ; }
  
  // hypo energy
  m_energy         =
    histoSvc () -> book ( directory                        ,  ///< directory
                          2                                ,  ///< ID
                          "Hypo Energy "    + inputData()  ,  ///< title
                          400                              ,  ///< #bins
                          0.                               ,  ///< low edge
                          m_energyMax                      ); ///< upper edge
  if( 0 == m_energy       )
    { return Error("Could not book 'Hypo Energy' histogram"       ) ; }
  
  // hypo transverse momentum
  m_pt             =
    histoSvc () -> book ( directory                        ,  ///< directory
                          3                                ,  ///< ID
                          "Hypo Pt     "    + inputData()  ,  ///< title
                          300                              ,  ///< #bins
                          0.                               ,  ///< low edge
                          m_ptMax                          ); ///< upper edge
  if( 0 == m_pt           )
    { return Error("Could not book 'Hypo Et'     histogram"       ) ; }

  // hypo mass 
  m_mass           =
    histoSvc () -> book ( directory                        ,  ///< directory
                          4                                ,  ///< ID
                          "Hypo Mass   "    + inputData()  ,  ///< title
                          500                              ,  ///< #bins
                          m_massMin                        ,  ///< low edge
                          m_massMax                        ); ///< upper edge
  if( 0 == m_mass         )
    { return Error("Could not book 'Hypo Mass'   histogram"       ) ; }
  
  // hypo energy
  m_e              =
    histoSvc () -> book ( directory                        ,  ///< directory
                          5                                ,  ///< ID
                          "Hypo E      "    + inputData()  ,  ///< title
                          400                              ,  ///< #bins
                          0.                               ,  ///< low edge 
                          m_energyMax                      ); ///< upper edge
  if( 0 == m_e       )
    { return Error("Could not book 'Hypo E' histogram"       ) ; }

  // hypo x 
  m_x              =
    histoSvc () -> book ( directory                        ,  ///< directory
                          6                                ,  ///< ID
                          "Hypo X      "    + inputData()  ,  ///< title
                          400                              ,  ///< #bins
                          -4 * meter                       ,  ///< low edge
                          4 * meter                        ); ///< upper edge
  if( 0 == m_x         )
    { return Error("Could not book 'Hypo X'   histogram"       ) ; }

  // hypo y 
  m_y              =
    histoSvc () -> book ( directory                        ,  ///< directory
                          7                                ,  ///< ID
                          "Hypo Y      "    + inputData()  ,  ///< title
                          300                              ,  ///< #bins
                          -3 * meter                       ,  ///< low edge
                          3 * meter                        ); ///< upper edge
  if( 0 == m_y         )
    { return Error("Could not book 'Hypo Y'   histogram"       ) ; }
  
  // # clusters 
  m_clusters       =
    histoSvc () -> book ( directory                        ,  ///< directory
                          8                                ,  ///< ID
                          "Clusters/Hypo "  + inputData()  ,  ///< title
                          20                               ,  ///< #bins
                          0                                ,  ///< low edge
                          10                               ); ///< upper edge
  if( 0 == m_clusters     )
    { return Error("Could not book 'Clusters/Hypo' histogram"       ) ; }

  // # spd hits 
  m_spd            =
    histoSvc () -> book ( directory                        ,  ///< directory
                          9                                ,  ///< ID
                          "Spd/Hypo      "  + inputData()  ,  ///< title
                          40                               ,  ///< #bins
                          0                                ,  ///< low edge
                          20                               ); ///< upper edge
  if( 0 == m_spd          )
    { return Error("Could not book 'Spd/Hypo'      histogram"       ) ; }
  
  // # spd hits 
  m_prs            =
    histoSvc () -> book ( directory                        ,  ///< directory
                          10                               ,  ///< ID
                          "Prs/Hypo      "  + inputData()  ,  ///< title
                          40                               ,  ///< #bins
                          0                                ,  ///< low edge
                          20                               ); ///< upper edge
  if( 0 == m_prs          )
    { return Error("Could not book 'prs/Hypo'      histogram"       ) ; }

  return StatusCode::SUCCESS;
};
// ============================================================================

namespace Local
{
  // ==========================================================================
  /** @class DigitFromCalo 
   *  simple utility to count digits from certain calorimeter 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date 31/03/2002 
   */
  class DigitFromCalo 
    : public std::unary_function<const CaloDigit*,bool>
  { 
  public: 
    /** constructor
     *  @parameter calo  calorimeter name 
     */
    explicit DigitFromCalo( const std::string& calo )
      : m_calo( CaloCellCode::CaloNumFromName( calo ) ) {} ;
    
    /** constructor
     *  @parameter calo  calorimeter index 
     */
    explicit DigitFromCalo( const int  calo )
      : m_calo(                                calo   ) {} ;
    
    /** the only essential method
     *  @param digit pointer to CaloDigit object 
     *  @return true if digit belongs to the predefined calorimeter 
     */
    inline bool operator() ( const CaloDigit* digit ) const 
    {
      if( 0 == digit ) { return false ; }
      return (int) digit->cellID().calo() == m_calo ;
    };
    
  private:
    
    /// default constructor is private 
    DigitFromCalo();
    
  private:
    
    int m_calo ;
    
  };
};
// ============================================================================


// ============================================================================
/** standard algorithm execution 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloHypoMonitor::execute() 
{
  // avoid long names 
  typedef const   CaloHypos          Hypos ;
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  // get input data 
  Hypos* hypos = get<Hypos> ( inputData() ) ;
  if( 0 == hypos   )        { return StatusCode::FAILURE ; }
  
  Local::DigitFromCalo spd( DeCalorimeterLocation::Spd );
  Local::DigitFromCalo prs( DeCalorimeterLocation::Prs );
  
  // fill multiplicity histogram 
  m_multiplicity -> fill ( hypos->size() );
  
  for( Hypos::const_iterator hypo = hypos -> begin() ;
       hypos -> end () != hypo ; ++hypo ) 
    {
      // skip NULLs 
      if( 0 == *hypo    ) { continue ; }
      
      
      m_clusters -> fill ( (*hypo)->clusters().size() );
      
      const CaloHypo::Digits& digits = (*hypo)->digits();
      
      m_spd -> fill ( std::count_if ( digits.begin() , 
                                      digits.end  () , spd ) ) ;
      m_prs -> fill ( std::count_if ( digits.begin() , 
                                      digits.end  () , prs ) ) ;
      
      const CaloMomentum* momentum = (*hypo)->momentum() ;
      
      if( 0 != momentum ) 
        { 
          const double e    = momentum->momentum().e    () ;
          const double pt   = momentum->momentum().perp () ;
          const double mass = momentum->momentum().m    () ;
          
          m_energy -> fill( e    ) ;
          m_pt     -> fill( pt   ) ;
          m_mass   -> fill( mass ) ;
        }

      const CaloPosition* position = (*hypo)->position() ;
      
      if( 0 != position ) 
        { 
          const double e = position->e () ;
          const double x = position->x () ;
          const double y = position->y () ;
          
          m_e -> fill( e ) ;
          m_x -> fill( x ) ;
          m_y -> fill( y ) ;
        }

    }
 
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
