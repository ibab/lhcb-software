// $Id: CaloPi0Monitor.cpp,v 1.9 2009-08-05 17:40:24 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.9 $
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
// ============================================================================
// Event
// ============================================================================
#include "Event/CaloHypo.h"
// ============================================================================
// CaloUtils
// ============================================================================
#include "CaloUtils/CaloMomentum.h"
// ============================================================================
// local
// ============================================================================
#include "CaloMoniAlg.h"
// ============================================================================

/** @class CaloPi0Monitor CaloPi0Monitor.cpp
 *
 *  Simple pi0 monitoring algorithm
 *  It produces 7 histograms:
 *  <ol>
 *  <li> "Raw" mass distribution of 2 photons </li>
 *  <li> Mass distribution of 2 photons after     Pt cut for each photon </li>
 *  <li> Mass distribution of 2 photons after 0.5*Pt cut for each photon </li>
 *  <li> Mass distribution of 2 photons after 2.0*Pt cut for each photon </li>
 *  <li> Mass distribution of 2 photons after     Pt cut for combination </li>
 *  <li> Mass distribution of 2 photons after 0.5*Pt cut for combination </li>
 *  <li> Mass distribution of 2 photons after 2.0*Pt cut for combination </li>
 *  </ol>
 *
 *  @see   CaloMoniAlg
 *  @see GaudiHistoAlg
 *  @see GaudiAlgorithm
 *  @see      Algorithm
 *  @see     IAlgorithm
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */
class CaloPi0Monitor : public CaloMoniAlg
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloPi0Monitor>;
public:
  /// standard algorithm initialization
  virtual StatusCode initialize()
  { StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm
    hBook1( "1", "pi0 multiplicity " + inputData() , m_multMin  , m_multMax , m_multBin );
    hBook1( "2", "pi0 energy " + inputData()       , m_energyMin  , m_energyMax , m_energyBin );
    hBook1( "3", "pi0 et     " + inputData()       , m_etMin  , m_etMax , m_etBin );
    hBook1( "4", "pi0 Mass   " + inputData()       , m_massMin  , m_massMax , m_massBin );
    return StatusCode::SUCCESS;
  }
  virtual StatusCode execute();
  virtual StatusCode finalize();
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  CaloPi0Monitor( const std::string &name, ISvcLocator *pSvcLocator )
    : CaloMoniAlg( name, pSvcLocator ){ 
    declareProperty( "PhotonPtFilter", m_ptPhoton = 250 * Gaudi::Units::MeV );
    m_multMax = 150;
    addToInputs( LHCb::CaloHypoLocation::Photons );

  }
  /// destructor (virtual and protected)
  virtual ~CaloPi0Monitor() {}
private:
  /// default  construstor is private
  CaloPi0Monitor();
  /// copy     construstor is private
  CaloPi0Monitor( const CaloPi0Monitor& );
  /// assignement operator is private
  CaloPi0Monitor &operator=( const CaloPi0Monitor& );
private:
  double m_ptPhoton;
};

DECLARE_ALGORITHM_FACTORY( CaloPi0Monitor );

// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloPi0Monitor::execute()
{ typedef const LHCb::CaloHypo::Container    Hypos;
  typedef const LHCb::CaloHypo               Photon;
  typedef std::vector<const LHCb::CaloHypo*> Photons;
  typedef Photons::const_iterator            photon;

  if ( !produceHistos() ) return StatusCode::SUCCESS;

  if ( inputs().empty() ) return Error( "No input data are specified" );

  Photons photons;
  for( std::vector<std::string>::const_iterator input = inputs().begin();
       inputs().end() != input; ++input ){
    Hypos* hypos = get<Hypos>( *input );
    if ( 0 == hypos ) return StatusCode::SUCCESS;
    photons.insert( photons.end(), hypos->begin(), hypos->end() );
  }
  if ( photons.empty() )return StatusCode::SUCCESS;  

// loop over the first photon
  initCounters();
  for( photon g1 = photons.begin(); photons.end() != g1; ++g1 ){
    if ( 0 == *g1 ) continue;
    LHCb::CaloMomentum momentum1( *g1 );
    if(momentum1.pt() < m_ptPhoton)continue;
    Gaudi::LorentzVector v1( momentum1.momentum() );
// loop over the second photon
    for( photon g2 = g1 + 1; photons.end() != g2; ++g2 ){
      if ( 0 == *g2 ) continue;
      LHCb::CaloMomentum momentum2( *g2 );
      if(momentum2.pt() < m_ptPhoton)continue;
      Gaudi::LorentzVector v2( momentum2.momentum() );
      Gaudi::LorentzVector pi0( v1 + v2 );
      const double mass = pi0.mass() ;
      const double e = pi0.e();
      const double et = pi0.pt();
      if( e < m_eFilter)continue;
      if( et< m_etFilter)continue;
      if( mass<m_massFilterMin || mass>m_massFilterMax)continue;
      LHCb::CaloCellID id1 = LHCb::CaloCellID();
      LHCb::CaloCellID id2 = LHCb::CaloCellID();
      if ( (*g1)->clusters().size() > 0 ){
        SmartRef<LHCb::CaloCluster> cluster= *((*g1)->clusters().begin());
        id1 = (*cluster).seed();      
      }
      if ( (*g2)->clusters().size() > 0 ){
        SmartRef<LHCb::CaloCluster> cluster= *((*g2)->clusters().begin());
        id2 = (*cluster).seed();      
      }
      LHCb::CaloCellID id = (id1.area() == id2.area() ) ? id1 : LHCb::CaloCellID();
      count(id);
      hFill1(id, "2", e   );
      hFill1(id, "3", et  );
      hFill1(id, "4", mass);      
    }
  }
  fillCounters("1");
  return StatusCode::SUCCESS;
}




StatusCode CaloPi0Monitor::finalize() {
  debug() << "==> Finalize" << endmsg;
  return CaloMoniAlg::finalize();
}
