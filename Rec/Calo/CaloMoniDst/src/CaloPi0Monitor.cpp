// ============================================================================
// Include files
// ============================================================================
#include <vector>
#include "Event/CaloHypo.h"
#include "CaloUtils/CaloMomentum.h"
#include "CaloDet/DeCalorimeter.h"
#include "CaloMoniAlg.h"
#include "Event/Particle.h"
#include "CaloUtils/CaloParticle.h"
#include "CaloInterfaces/ICaloHypo2Calo.h"
// ============================================================================

/** @class CaloPi0Monitor CaloPi0Monitor.cpp
 *
 *  Simple pi0 monitoring algorithm
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
  { StatusCode sc = CaloMoniAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm
    hBook1( "1", "(gg) multiplicity " + inputData() , m_multMin  , m_multMax , m_multBin );
    hBook1( "2", "(gg) energy " + inputData()       , m_energyMin  , m_energyMax , m_energyBin );
    hBook1( "3", "(gg) et     " + inputData()       , m_etMin  , m_etMax , m_etBin );
    hBook1( "4", "(gg) mass   " + inputData()       , m_massMin  , m_massMax , m_massBin );
    hBook1( "5", "(gg) combinatorial background" + inputData()       , m_massMin  , m_massMax , m_massBin );
    hBook1( "6", "bkg-substracted (gg) mass   " + inputData()       , m_massMin  , m_massMax , m_massBin );
    hBook1( "7", "(gg) mass  for |y|-gamma > " + Gaudi::Utils::toString(m_yCut) + " "  
            + inputData()  , m_massMin  , m_massMax , m_massBin );
    m_calo = getDet<DeCalorimeter>(DeCalorimeterLocation::Ecal);
    // get tool
    m_toSpd = tool<ICaloHypo2Calo> ( "CaloHypo2Calo", "CaloHypo2Spd" , this );
    m_toSpd->setCalos( "Ecal" ,"Spd");
    m_toPrs = tool<ICaloHypo2Calo> ( "CaloHypo2Calo", "CaloHypo2Prs" , this );
    m_toPrs->setCalos( "Ecal" ,"Prs");

    // set mass window to histo range
    m_massFilterMin = m_massMin;
    m_massFilterMax = m_massMax;


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
    : CaloMoniAlg( name, pSvcLocator )
    , m_calo(NULL)
    , m_toSpd(NULL)
    , m_toPrs(NULL)
  { 
    declareProperty( "PhotonPtFilter"   , m_ptPhoton = 250 * Gaudi::Units::MeV );
    declareProperty( "IsolationFilter"  , m_isol = 4 );
    declareProperty( "AllowConverted"   , m_conv = false);
    declareProperty( "RejectedYBand"    , m_yCut = 300);
    declareProperty( "PhotonPrsFilterMin", m_prsPhoton = 10*Gaudi::Units::MeV);
    declareProperty( "PhotonMaxPtFilter", m_ptMaxPhoton= 0);

    m_multMax = 150;
    addToInputs( LHCb::CaloAlgUtils::CaloHypoLocation("Photons",context() ) );
    setInputData( LHCb::CaloAlgUtils::CaloHypoLocation("Photons",context() ) );
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
  double m_ptMaxPhoton;
  double m_isol;
  double m_prsPhoton;
  DeCalorimeter* m_calo;
  ICaloHypo2Calo* m_toSpd ;   
  ICaloHypo2Calo* m_toPrs ;   
  bool m_conv;
  double m_yCut;
};


DECLARE_ALGORITHM_FACTORY( CaloPi0Monitor )

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

    if( !exist<Hypos> ( *input ) ){
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << "no hypo container found at " << *input << endmsg;
      continue;
    };

    Hypos* hypos = get<Hypos>( *input );
    if ( 0 == hypos ) return StatusCode::SUCCESS;
    photons.insert( photons.end(), hypos->begin(), hypos->end() );
  }
  if ( photons.empty() )return StatusCode::SUCCESS;  





// loop over the first photon
  initCounters();
  for( photon g1 = photons.begin(); photons.end() != g1; ++g1 ){
    if ( 0 == *g1 ) continue;
    if( !m_conv && m_toSpd->multiplicity ( *(*g1) , "Spd"  ) > 0 )continue;
    if( m_toPrs->energy ( *(*g1) , "Prs"  ) < m_prsPhoton )continue;
    LHCb::CaloMomentum momentum1( *g1 );
    if(momentum1.pt() < m_ptPhoton)continue;
    Gaudi::LorentzVector v1( momentum1.momentum() );
    Gaudi::XYZPoint p1( (*g1)->position()->x() , (*g1)->position()->y() , (*g1)->position()->z() );


// loop over the second photon
    for( photon g2 = g1 + 1; photons.end() != g2; ++g2 ){
      if ( 0 == *g2 ) continue;
      LHCb::CaloMomentum momentum2( *g2 );
      if(momentum2.pt() < m_ptPhoton)continue;
      if( std::max(momentum1.pt(),momentum2.pt()) < m_ptMaxPhoton)continue;
      if( !m_conv && m_toSpd->multiplicity ( *(*g2) , "Spd"  ) > 0 )continue;
      if( m_toPrs->energy ( *(*g1) , "Prs"  ) < m_prsPhoton )continue;
      Gaudi::LorentzVector v2( momentum2.momentum() );
      Gaudi::LorentzVector pi0( v1 + v2 );
      Gaudi::XYZPoint p2( (*g2)->position()->x() , (*g2)->position()->y() , (*g2)->position()->z() );
      // background shape from (x,y)->(-x,-y) symmetrized g2
      Gaudi::LorentzVector v2Sym( v2 );
      v2Sym.SetPx( -v2.Px() );
      v2Sym.SetPy( -v2.Py() );
      Gaudi::LorentzVector bkg( v1 + v2Sym );
      Gaudi::XYZPoint p2Sym( -(*g2)->position()->x() , -(*g2)->position()->y() , (*g2)->position()->z() );
      bool isBkg = false;
      if( bkg.e() > m_eFilter &&
          bkg.pt() > m_etFilter &&
          bkg.mass() > m_massFilterMin &&
          bkg.mass() < m_massFilterMax ) isBkg = true;
      bool isPi0 = false;
      if( pi0.e() > m_eFilter &&
          pi0.pt() > m_etFilter &&
          pi0.mass() > m_massFilterMin &&
          pi0.mass() < m_massFilterMax ) isPi0 = true;
      
      //      info() << "pi0 E/M" << pi0.e() << " / " << pi0.mass()
      //       << "  " << v1.E() << " " << v2.E() 
      //       << endmsg;
      
      
      if( !isPi0 && !isBkg)continue;

      // Get cellIDs
      LHCb::CaloCellID id1 = LHCb::CaloCellID();
      LHCb::CaloCellID id2 = LHCb::CaloCellID();
      if ( (*g1)->clusters().size() > 0 ){
        SmartRef<LHCb::CaloCluster> cluster= *((*g1)->clusters().begin());
        if( cluster != 0 )id1 = (*cluster).seed();      
      }
      if ( (*g2)->clusters().size() > 0 ){
        SmartRef<LHCb::CaloCluster> cluster= *((*g2)->clusters().begin());
        if( cluster != 0)id2 = (*cluster).seed();      
      }

      // define pi0 area
      LHCb::CaloCellID id = (id1.area() == id2.area() ) ? id1 : LHCb::CaloCellID();

      // isolation criteria

      Gaudi::XYZVector vec    = p2 - p1;
      Gaudi::XYZVector vecSym = p2Sym - p1;
      double cSize = m_calo->cellSize( id1 );
      if( m_calo->cellSize( id2 ) >  cSize ) cSize = m_calo->cellSize( id2 );
      double isol = (cSize > 0) ?  vec.Rho() / cSize : 0;
      double isolSym = (cSize > 0) ?  vecSym.Rho() / cSize : 0;


      double y1 = m_calo->cellCenter(id1).Y();
      double y2 = m_calo->cellCenter(id2).Y();
      

      if( isPi0 && isol > m_isol){
        count(id);
        hFill1(id, "2", pi0.e()   );
        hFill1(id, "3", pi0.pt()  );
        hFill1(id, "4", pi0.mass());      
        hFill1(id, "6", pi0.mass(), 1.);      
        if( fabs(y1) > m_yCut && fabs(y2) > m_yCut )hFill1("7",pi0.mass(), 1.);
      }
      if( isBkg && isolSym > m_isol){ 
        hFill1(id, "5", bkg.mass());      
        hFill1(id, "6", bkg.mass(), -1.);      
      }
    }
  }  
  fillCounters("1");
  return StatusCode::SUCCESS;
}




StatusCode CaloPi0Monitor::finalize() {
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;
  return CaloMoniAlg::finalize();
}
