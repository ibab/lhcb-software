// $Id: CaloHypoMonitor.cpp,v 1.4 2005-11-22 22:43:01 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// Include files
// ============================================================================
// from Gaudi
// ============================================================================
#include  "GaudiKernel/AlgFactory.h"
#include  "GaudiKernel/MsgStream.h" 
#include  "GaudiKernel/IHistogramSvc.h"
// ============================================================================
// AIDA 
// ============================================================================
#include  "AIDA/IHistogram1D.h"
// ============================================================================
// CaloDet 
// ============================================================================
#include  "CaloDet/DeCalorimeter.h"
// ============================================================================
// Event 
// ============================================================================
#include  "Event/CaloHypo.h"
#include  "Event/CaloDataFunctor.h"
// ============================================================================
// CaloUtils 
// ============================================================================
#include  "CaloUtils/dirHbookName.h"
// ============================================================================
// local
// ============================================================================
#include  "CaloMoniAlg.h"
// ============================================================================

/** @class CaloHypoMonitor CaloHypoMonitor.cpp
 *  
 *  The algorithm for trivial monitoring of "CaloHypo" container
 *  The algorithm produces 10 histograms: 
 *  <ol> 
 *  <li> @p CaloHypo multiplicity                           </li>
 *  <li> @p CaloHypo energy distribution                    </li>
 *  <li> @p CaloHypo transverse momentum distribution       </li>
 *  <li> @p CaloHypo mass distribution                      </li>
 *  <li> @p CaloHypo e distribution                         </li>
 *  <li> @p CaloHypo x distribution                         </li>
 *  <li> @p CaloHypo y distribution                         </li>
 *  <li> multiplicity of     @p CaloCluster per @p CaloHypo </li>
 *  <li> multiplicity of Spd @p CaloDigit   per @p CaloHypo </li>
 *  <li> multiplicity of Prs @p CaloDigit   per @p CaloHypo </li>
 *  </ol>
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
class CaloHypoMonitor : public CaloMoniAlg 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloHypoMonitor>;
public:
  /// standard algorithm initialization
  virtual StatusCode initialize();
  /// standard algorithm execution 
  virtual StatusCode execute   ();
protected:
  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloHypoMonitor
  ( const std::string& name   , 
    ISvcLocator*       svcloc )
    : CaloMoniAlg ( name , svcloc ) 
    , m_multMax      ( 100        )
    , m_energyMax    ( 200. * GeV )
    , m_ptMax        (  10. * GeV )
    , m_massMin      (   0. * GeV )
    , m_massMax      (   1. * GeV )
  {
    declareProperty ( "MultiplicityMax" , m_multMax   ) ;
    declareProperty ( "EnergyMax"       , m_energyMax ) ;
    declareProperty ( "PtMax"           , m_ptMax     ) ;
    declareProperty ( "MassMin"         , m_massMin   ) ;
    declareProperty ( "MassMax"         , m_massMax   ) ;
  };
  /// destructor (virtual and protected)
  virtual ~CaloHypoMonitor(){};
private:
  /// default  construstor  is  private 
  CaloHypoMonitor(); 
  /// copy     construstor  is  private 
  CaloHypoMonitor
  ( const CaloHypoMonitor& );
  /// assignement operator  is  private 
  CaloHypoMonitor& operator=
  ( const CaloHypoMonitor& );
private:
  // hypo multiplicity 
  double          m_multMax      ;
  // hypo energy  distribution 
  double          m_energyMax    ;  
  // hypo transverse momentum distribution  
  double          m_ptMax        ;
  // hypo mass distribution  
  double          m_massMin      ;
  double          m_massMax      ;
};
// ============================================================================

// ============================================================================
/** @var CaloHypoMonitorFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloHypoMonitor>         s_Factory ;
const        IAlgFactory&CaloHypoMonitorFactory = s_Factory ;

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
  StatusCode sc = CaloMoniAlg::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  
  // cluster multiplicity
  book ( 1  , "# of Hypos  "    + inputData()  , 
         0  , m_multMax , 200                  ) ;  
  
  // hypo energy
  book ( 2  , "Hypo Energy "    + inputData()  ,    
         0. , m_energyMax                      ) ;  
  
  // hypo transverse momentum
  book ( 3  , "Hypo Pt     "    + inputData()  ,
         0. , m_ptMax                          ) ; 
  
  // hypo mass
  book ( 4  , "Hypo Mass   "    + inputData()  ,
         m_massMin , m_massMax                 ) ; 
  
  // hypo energy
  book ( 5  , "Hypo E      "    + inputData()  ,   
         0. , m_energyMax                      ) ; 
  
  book ( 6  , "Hypo X      "    + inputData()  ,  
         -4 * meter , 4 * meter                ) ; 
  
  book ( 7  , "Hypo Y      "    + inputData()  ,  
         -3 * meter , 3 * meter                ) ; 
  
  book ( 8  ,  "Clusters/Hypo "  + inputData() ,
         0  ,  10                              ) ;
  
  book ( 9  , "Spd/Hypo      "  + inputData()  ,
         0  ,  20                              ) ; 
  
  book ( 10 , "Prs/Hypo      "  + inputData()  ,  
         0  ,  20                              ) ; 
  
  return StatusCode::SUCCESS;
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
  
  // produce histos ?
  if ( !produceHistos() ) { return StatusCode::SUCCESS ; }
  
  // get input data 
  Hypos* hypos = get<Hypos> ( inputData() ) ;
  if ( 0 == hypos       ) { return StatusCode::FAILURE ; }

  // fill multiplicity histogram 
  AIDA::IHistogram1D*  h1 = histo (  1 ) ;
  hFill ( h1 , hypos->size() ) ;
  
  if ( hypos -> empty() ) { return StatusCode::SUCCESS ; }
  
  CaloDataFunctor::DigitFromCalo spd ( DeCalorimeterLocation::Spd );
  CaloDataFunctor::DigitFromCalo prs ( DeCalorimeterLocation::Prs );
  
  AIDA::IHistogram1D*  h2 = histo (  2 ) ;
  AIDA::IHistogram1D*  h3 = histo (  3 ) ;
  AIDA::IHistogram1D*  h4 = histo (  4 ) ;
  AIDA::IHistogram1D*  h5 = histo (  5 ) ;
  AIDA::IHistogram1D*  h6 = histo (  6 ) ;
  AIDA::IHistogram1D*  h7 = histo (  7 ) ;
  AIDA::IHistogram1D*  h8 = histo (  8 ) ;
  AIDA::IHistogram1D*  h9 = histo (  9 ) ;
  AIDA::IHistogram1D* h10 = histo ( 10 ) ;
  
  
  for ( Hypos::const_iterator hypo = hypos -> begin() ;
        hypos -> end () != hypo ; ++hypo ) 
  {
    // skip NULLs 
    if ( 0 == *hypo    ) { continue ; }
    
    const CaloMomentum* momentum = (*hypo)->momentum() ;
    if ( 0 != momentum ) 
    { 
      hFill ( h2 , momentum -> momentum () . e    () ) ;
      hFill ( h3 , momentum -> momentum () . perp () ) ;
      hFill ( h4 , momentum -> momentum () . m    () ) ;
    }
    
    const CaloPosition* position = (*hypo)->position() ;
    if ( 0 != position ) 
    { 
      hFill ( h5 , position -> e () ) ;
      hFill ( h6 , position -> x () ) ;
      hFill ( h7 , position -> y () ) ;
    }
    
    hFill ( h8  , (*hypo)->clusters().size() ) ;

    const CaloHypo::Digits& digits = (*hypo)->digits();
    hFill ( h9  , std::count_if ( digits.begin() , digits.end  () , spd ) ) ;
    hFill ( h10 , std::count_if ( digits.begin() , digits.end  () , prs ) ) ; 
  }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
