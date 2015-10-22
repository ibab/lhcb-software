// ============================================================================
// Include files
// ============================================================================
#include "Relations/IRelationWeighted.h"
// ============================================================================
// from PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/CaloHypo.h"
#include "Event/MCParticle.h"
// ============================================================================
// CaloDet
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
// ============================================================================
// CaloUtils
// ============================================================================
#include "CaloUtils/CaloMomentum.h"
#include "CaloUtils/ClusterFunctors.h"
// ============================================================================
// local
// ============================================================================
#include "CaloMoniAlg.h"
// ============================================================================

/** @class CaloPi0Checker CaloPi0Checker.cpp
 *
 *  Simple MC pi0 monitoring algorithm
 *  It produces 2 histograms
 *  <ol>
 *  <li> "Raw" mass distribution of 2 photons </li>
 *  <li> Mass distribution of 2 photons after Pt cut for each photon </li>
 *  <li> Mass distribution of 2 photons after Pt cut for combination </li>
 *  </ol>
 *
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */

class CaloPi0Checker : public CaloMoniAlg{
  /// friend factory for instantiation
  friend class AlgFactory<CaloPi0Checker>;

public:
  /// standard algorithm initialization
  virtual StatusCode initialize()
  { StatusCode sc = CaloMoniAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm

// re-initialize the Ecal cluster selector
    m_calo.setCalo( detData() );

//  locate particle eproperty service
    LHCb::IParticlePropertySvc *ppS =
      svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc", true );
    if ( 0 == ppS ) return StatusCode::FAILURE;

    const LHCb::ParticleProperty *pp = ppS->find( m_pi0Name );
    if ( 0 == pp ) return Error( "Could not locate particle '"+m_pi0Name+"'" );
    m_pi0ID = pp->pid();

    hBook1( "1", "Gamma-Gamma mass       " , 0, 1, 500 );
    hBook1( "2", "Gamma-Gamma mass (MCpi0 match)" , 0, 1, 500 );
    if( m_split ){
      Warning( "No area spliting allowed for CaloPi0Checker").ignore();
      m_split = false;
    }
    return StatusCode::SUCCESS;
  }
  
  /// standard algorithm execution
  virtual StatusCode execute();

protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  CaloPi0Checker( const std::string &name, ISvcLocator *pSvcLocator )
    : CaloMoniAlg( name, pSvcLocator )
    , m_cut( float(50 * Gaudi::Units::perCent) )
    , m_calo( DeCalorimeterLocation::Ecal )
    , m_pi0Name( "pi0" )
    , m_pi0ID( 0 ){ 
    declareProperty( "Ancestor", m_pi0Name );
    declareProperty( "Cut",      m_cut );
    // set the appropriate default value for input data
    setInputData(  "Relations/"+LHCb::CaloClusterLocation::Default );
    // set the appropriate defalts value for input data
    addToInputs( LHCb::CaloHypoLocation::Photons );
    //    addToInputs( LHCb::CaloHypoLocation::SplitPhotons );
    // set the appropriate defualt value for detector data
    setDetData( DeCalorimeterLocation::Ecal );
  }
  /// destructor (virtual and protected)
  virtual ~CaloPi0Checker() {}
private:
  /// default  construstor  is  private
  CaloPi0Checker();
  /// copy     construstor  is  private
  CaloPi0Checker( const CaloPi0Checker& );
  /// assignement operator  is  private
  CaloPi0Checker &operator=( const CaloPi0Checker& );
private:
  // photon "purity" cut
  float                                  m_cut;
  LHCb::ClusterFunctors::ClusterFromCalo m_calo;
std::string                            m_pi0Name;
  LHCb::ParticleID                       m_pi0ID;
};


DECLARE_ALGORITHM_FACTORY( CaloPi0Checker )

// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloPi0Checker::execute(){ 
  typedef const LHCb::CaloHypo            Photon;
  typedef const LHCb::CaloHypo::Container Hypos;
  typedef std::vector<const Photon*>      Photons;
  typedef LHCb::CaloHypo::Clusters        Clusters;
// iterators
  typedef Photons::const_iterator         photon;
  typedef Clusters::const_iterator        cluster;
// relation table
  typedef const IRelationWeighted<LHCb::CaloCluster,LHCb::MCParticle,float>
                                          Table;
  typedef Table::Range                    Range;

  using namespace LHCb::ClusterFunctors;

  if ( inputs().empty() ) return Error( "No input data are specified" );
  if ( !exist<Table>( inputData() ))return StatusCode::SUCCESS;
  const Table *table = get<Table>( inputData() );
  Photons photons;
  for( std::vector<std::string>::const_iterator input = inputs().begin();inputs().end() != input; ++input ){    
    if ( !exist<Hypos> ( *input ) )continue;
    const Hypos *hypos = get<Hypos> ( *input );
    photons.insert( photons.end(), hypos->begin(), hypos->end() );
  }
  
  
  // loop over the first photon
  for( photon g1 = photons.begin(); photons.end() != g1; ++g1 ){
    Photon *photon1 = *g1;
    if ( 0 == photon1 ) continue;
    LHCb::CaloMomentum momentum1( photon1 );
    
    // get Ecal cluster
    const Clusters &clusters1 = photon1->clusters();
    if ( clusters1.empty() ) continue;
    cluster cluster1 = ( 1 == clusters1.size() ) ? clusters1.begin() :
      std::find_if( clusters1.begin(), clusters1.end(), m_calo );
    if ( clusters1.end() == cluster1 ) continue;
    // get all MCtruth information for this cluster
    const float cut1 = (float) ((*cluster1)->e() * m_cut);
    const Range range1 = table->relations( *cluster1, cut1, true );
    
    // loop over the second photon
    for( photon g2 = g1 + 1; photons.end() != g2; ++g2 )
    { Photon *photon2 = *g2;
      if ( 0 == photon2 ) continue;
      LHCb::CaloMomentum momentum2( photon2 );
      
      // get Ecal cluster
      const Clusters &clusters2 = photon2->clusters();
      if ( clusters2.empty() ) continue;
      cluster cluster2 = ( 1 == clusters2.size() ) ? clusters2.begin() :
        std::find_if( clusters2.begin(), clusters2.end(), m_calo );
      if ( clusters2.end() == cluster2 ) continue;
      // get all MCtruth information for this cluster
      const float cut2 = (float)((*cluster2)->e() * m_cut);
      const Range range2 = table->relations( *cluster2, cut2, true );

      // double loop for search the common ancestor
      LHCb::MCParticle *pi0 = 0;
      for( Range::iterator mc1 = range1.begin();( 0 == pi0 ) && ( range1.end() != mc1 ); ++mc1 ){
        LHCb::MCParticle *mcp1 = mc1->to();
        if ( 0 == mcp1 ) continue;
        for ( Range::iterator mc2 = range2.begin(); ( 0 == pi0 ) && ( range2.end() != mc2 ); ++mc2 ){
          if ( mc1->to() != mc2->to() ) continue; // common ancestor?
          if ( m_pi0ID == mc1->to()->particleID() ) pi0 = mc1->to();
        } // end of second MC loop
      } // end of first MC loop
      
      const double mass = (momentum1.momentum()+momentum2.momentum()).mass();
      hFill1( "1", mass/Gaudi::Units::GeV );
      if ( 0 == pi0 ) continue;
      hFill1( "2", mass/Gaudi::Units::GeV );
    } // end of loop over second photon
  } // end of loop over first photon
  return StatusCode::SUCCESS;
}
