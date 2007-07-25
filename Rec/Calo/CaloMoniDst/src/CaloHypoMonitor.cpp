// $Id: CaloHypoMonitor.cpp,v 1.5 2007-07-25 19:49:13 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $
// ============================================================================
// Include files
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
#include  "CaloUtils/CaloMomentum.h"
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
 *  <li> @p CaloHypo x distribution                         </li>
 *  <li> @p CaloHypo y distribution                         </li>
 *  <li> multiplicity of     @p CaloCluster per @p CaloHypo </li>
 *  <li> multiplicity of Spd @p CaloDigit   per @p CaloHypo </li>
 *  <li> multiplicity of Prs @p CaloDigit   per @p CaloHypo </li>
 *  <li> CaloHypo x vs y distribution                       </li>
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
  virtual StatusCode initialize()
  { StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm
    hBook1( "1", "# of Hypos    " + inputData(),  0,    m_multMax, m_multMax );
    hBook1( "2", "Hypo Energy   " + inputData(),  0,    m_energyMax );
    hBook1( "3", "Hypo Pt       " + inputData(),  0,    m_ptMax );
    hBook1( "4", "Hypo Mass     " + inputData(),  m_massMin, m_massMax );
    hBook1(  "5", "Hypo X        " + inputData(), -m_xMax, m_xMax );
    hBook1(  "6", "Hypo Y        " + inputData(), -m_yMax, m_yMax );
    hBook1(  "7", "Clusters/Hypo " + inputData(),  0,    10, 10 );
    hBook1(  "8", "Spd/Hypo      " + inputData(),  0,    20, 20 );
    hBook1(  "9", "Prs/Hypo      " + inputData(),  0,    20, 20 );
    hBook2( "10", "Hypo x vs y   " + inputData(),  -m_xMax, m_xMax, 100, -m_yMax, m_yMax, 100);
    return StatusCode::SUCCESS;
  }
  /// standard algorithm execution
  virtual StatusCode execute();
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  CaloHypoMonitor( const std::string &name, ISvcLocator *pSvcLocator )
    : CaloMoniAlg( name, pSvcLocator )
    , m_multMax( 100 )
    , m_energyMax( 200. * Gaudi::Units::GeV )
    , m_ptMax( 15. * Gaudi::Units::GeV )
    , m_massMin( 0. * Gaudi::Units::GeV )
    , m_massMax( 1. * Gaudi::Units::GeV )
  { declareProperty( "MultiplicityMax", m_multMax );
    declareProperty( "EnergyMax",       m_energyMax );
    declareProperty( "PtMax",           m_ptMax );
    declareProperty( "MassMin",         m_massMin );
    declareProperty( "MassMax",         m_massMax );
    declareProperty( "xMax",          m_xMax = 4 * Gaudi::Units::meter);
    declareProperty( "yMax",          m_yMax = 4 * Gaudi::Units::meter);
  }
  /// destructor (virtual and protected)
  virtual ~CaloHypoMonitor() {}
private:
  /// default  construstor  is  private
  CaloHypoMonitor();
  /// copy     construstor  is  private
  CaloHypoMonitor( const CaloHypoMonitor& );
  /// assignement operator  is  private
  CaloHypoMonitor &operator=( const CaloHypoMonitor& );
private:
  // hypo multiplicity
  double m_multMax;
  // hypo energy  distribution
  double m_energyMax;
  // hypo transverse momentum distribution
  double m_ptMax;
  // hypo mass distribution
  double m_massMin;
  double m_massMax;
  double m_xMax;
  double m_yMax;
};
DECLARE_ALGORITHM_FACTORY( CaloHypoMonitor );

// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloHypoMonitor::execute()
{ typedef const LHCb::CaloHypo::Container Hypos;

  // produce histos ?
  if ( !produceHistos() ) return StatusCode::SUCCESS;

  // get input data
  Hypos *hypos = get<Hypos> ( inputData() );
  if ( 0 == hypos ) return StatusCode::FAILURE;

  // fill multiplicity histogram
  hFill1( "1", hypos->size() );

  if ( hypos -> empty() ) return StatusCode::SUCCESS;

  LHCb::CaloDataFunctor::DigitFromCalo spd( DeCalorimeterLocation::Spd );
  LHCb::CaloDataFunctor::DigitFromCalo prs( DeCalorimeterLocation::Prs );

  for( Hypos::const_iterator hypo = hypos->begin();
       hypos->end () != hypo ; ++hypo )
  { if ( 0 == *hypo ) continue;

    { LHCb::CaloMomentum momentum( *hypo );
      hFill1( "2", momentum.momentum().e() );
      hFill1( "3", momentum.momentum().pt() );
      hFill1( "4", momentum.momentum().mass() );
    }

    const LHCb::CaloHypo::Position *position = (*hypo)->position();
    if ( 0 != position )
    { hFill1( "5", position->x() );
      hFill1( "6", position->y() );
    }

    hFill1( "7", (*hypo)->clusters().size() );

    const LHCb::CaloHypo::Digits& digits = (*hypo)->digits();
    hFill1( "8",  std::count_if( digits.begin(), digits.end(), spd ) );
    hFill1( "9", std::count_if( digits.begin(), digits.end(), prs ) );
    if( NULL != position)hFill2( "10", position->x(),position->y());
  }
  return StatusCode::SUCCESS;
}
