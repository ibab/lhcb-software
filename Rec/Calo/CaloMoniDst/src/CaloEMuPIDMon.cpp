// ============================================================================
// Include files
// ============================================================================
// AIDA
// ============================================================================
#include "AIDA/IAxis.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Track.h"
#include "Event/ProtoParticle.h"
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelation.h"
#include "Relations/IRelationWeighted.h"
#include "CaloUtils/Calo2Track.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/lexical_cast.hpp"
#include "boost/assign/list_of.hpp"
// ============================================================================
// Local
// ============================================================================
#include "CaloMoniAlg.h"
// ============================================================================

/** @class CaloEMuPIDMon CaloEMuPIDMon.cpp
 *
 *  Class for monitoring some CaloPID quantities
 *
 *  @author Dmitry Golubkov
 *  @date   2010-03-25
 */
class CaloEMuPIDMon : public CaloMoniAlg
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloEMuPIDMon>;
protected:
public:
  /// standard algorithm initialization
  virtual StatusCode initialize();
  /// standard algorithm execution
  virtual StatusCode execute();
  /// standard algorithm finalization
  virtual StatusCode finalize();
  virtual StatusCode check();
protected:
  /// Standard constructor
  CaloEMuPIDMon( const std::string &name, ISvcLocator *isvc );
  /// virtual destructor
  virtual ~CaloEMuPIDMon() {}
private:
  bool  m_uncut;
  float m_minPt;
  float m_maxPt;
  float m_RichDLLe;
  float m_maxEHcalE;
  float m_minPrsE;
  unsigned long int m_nEventMin;
  bool  m_muonLoose;

private:
};

DECLARE_ALGORITHM_FACTORY( CaloEMuPIDMon )

// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloEMuPIDMon::execute()
{
  if ( !produceHistos() ) return StatusCode::SUCCESS;

  ++ counter("nEvents");

  if ( ! exist<LHCb::ProtoParticle::Container>( LHCb::ProtoParticleLocation::Charged ) ){
    warning() << "event skept: " << LHCb::ProtoParticleLocation::Charged << " doesn't exist !!" << endmsg;
    return StatusCode::SUCCESS;
  }

  const LHCb::ProtoParticle::Container* particles =
    get<LHCb::ProtoParticle::Container>( LHCb::ProtoParticleLocation::Charged ) ;
  if ( particles == 0 ) return StatusCode::SUCCESS;


  // -------------------------------------------------------------------
  // Track loop
  // -------------------------------------------------------------------
  for ( LHCb::ProtoParticle::Container::const_iterator iproto = particles->begin() ;
        particles->end() != iproto ; ++iproto )
  {
    const LHCb::ProtoParticle* proto = *iproto ;
    if ( 0 == proto ) continue ;

    const LHCb::Track* track = proto->track() ;
    if ( 0 == track ) continue ;

    if ( track->type() < LHCb::Track::Long ) continue;
//  if ( track->type() != LHCb::Track::Downstream &&
//       track->type() != LHCb::Track::Long          ) continue;

    if ( ( track->pt() < m_minPt || track->pt() > m_maxPt ) && !m_uncut ) continue;


    bool  inprs = (proto->info( LHCb::ProtoParticle::InAccPrs,  double(false) )!=0);
    bool inecal = (proto->info( LHCb::ProtoParticle::InAccEcal, double(false) )!=0);
    bool inhcal = (proto->info( LHCb::ProtoParticle::InAccHcal, double(false) )!=0);

    float prse  = (float) proto->info(LHCb::ProtoParticle::CaloPrsE,  -1 * Gaudi::Units::GeV);
    float ecale = (float) proto->info(LHCb::ProtoParticle::CaloEcalE, -1 * Gaudi::Units::GeV);
    float hcale = (float) proto->info(LHCb::ProtoParticle::CaloHcalE, -1 * Gaudi::Units::GeV);


    // -----------------------------------------------------------------
    // electron histograms
    // e.g.:  "pt>0.5&&inecal==1&&(hcale<=0||hcale>0&&inhcal==1&&hcale<1000.)&&rdlle>4"
    //   or:  "pt>250&&inecal==1&&prse>15"
    // -----------------------------------------------------------------

    do {
      if ( !inecal )                                                   break; // ----

      float rdlle = (float) proto->info( LHCb::ProtoParticle::RichDLLe, -9999.);
      if ( rdlle < m_RichDLLe && !m_uncut )                            break; // ----
      if ( inhcal && hcale > m_maxEHcalE && !m_uncut)                  break; // ----

      if ( prse < m_minPrsE && !m_uncut )                              break; // ----

      const SmartRefVector < LHCb::CaloHypo > & hypos      = proto->calo();
      const LHCb::CaloHypo                    * m_electron = NULL;

      for (SmartRefVector<LHCb::CaloHypo>::const_iterator ihypo =  hypos.begin();
           ihypo != hypos.end() ; ++ihypo){
        const LHCb::CaloHypo* hypo = *ihypo;
        if ( NULL == hypo ) continue;
        if ( LHCb::CaloHypo::EmCharged == hypo->hypothesis() ) m_electron = hypo;
      }

      // E/p histogram
      if ( m_electron ){
        double eoverp = m_electron->position()->e() / track->p() ;

        if (eoverp > 0.){
          const LHCb::CaloCluster * clust = *(m_electron->clusters().begin());
          if ( clust ){
            LHCb::CaloCellID cell = clust->seed();
            hFill1(cell, "eop",  eoverp);
          }
        }
      }
    } while( false );


    // -----------------------------------------------------------------
    // muon histograms
    // -----------------------------------------------------------------

    bool ismuon = (0!= 
                   ( proto->muonPID()
                     ? ( m_muonLoose ? proto->muonPID()->IsMuonLoose() : proto->muonPID()->IsMuon() )
                     : false)
                   );

    if ( ismuon || m_uncut ){
      if ( inprs) hFill1( "prsem",  prse);
      if (inecal) hFill1("ecalem", ecale);
      if (inhcal) hFill1("hcalem", hcale);
    }
  }

  return StatusCode::SUCCESS;
}
/** Standard constructor
 *  @param name name of the algorithm instance
 *  @param isvc pointer to the Service Locator
 */
CaloEMuPIDMon::CaloEMuPIDMon( const std::string &name, ISvcLocator *isvc )
  : CaloMoniAlg( name, isvc )
//
  , m_uncut     ( true )
  , m_minPt     (    0 )
  , m_maxPt     (    0 )
  , m_RichDLLe  (    0 )
  , m_maxEHcalE (    0 )
  , m_minPrsE   (    0 )
  , m_nEventMin (    0 )
  , m_muonLoose ( true )
//
{
  declareProperty( "uncut",           m_uncut     = false, "true = do not apply the cuts");
  declareProperty( "pTmin",           m_minPt     = 250.,  "minimal pT of the track [MeV/c]");
  declareProperty( "pTmax",           m_maxPt     = 1.e10, "maximal pT of the track [MeV/c]");
  declareProperty( "RichDLLe",        m_RichDLLe  =-1.e10, "minimal ProtoParticle::RichDLLe for electron id");
  declareProperty( "maxEHcalE",       m_maxEHcalE = 1.e10, "maximal ProtoParticle::CaloHcalE for electron id [MeV/c]");
  declareProperty( "minPrsE",         m_minPrsE   = 15.,   "minimal ProtoParticle::CaloPrsE for electron id [MeV/c]");
  declareProperty( "nEventMin",       m_nEventMin = 200,   "minimal number of events to check");
  declareProperty( "useIsMuonLoose",  m_muonLoose = true,  "use IsMuonLoose instead of IsMuon for muon selection");

  // DG: on SLC5 I get 'warning: type qualifiers ignired on function return type'
  // if I combine the two follwing lines in one call (?)
  std::vector<std::string> histoList = boost::assign::list_of<std::string>("All");
  setProperty( "histoList",           histoList);
  setProperty( "removeFromHistoList", std::vector<std::string>() );
  setProperty( "SaturationBin1D",     false);
  setProperty( "SaturationBin2D",     false);
}
/** Finalize - calculate mean, rms, relative overflow, etc. and
 *  compare with the reference parameters, generate warnins or
 *  error messages if the difference exceeds NSigmaThreshold's  */
StatusCode CaloEMuPIDMon::finalize()
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;

// dummy method -- commented out for the time being
//if (counter("nEvents").nEntries() >= m_nEventMin)
//  this->check().ignore();

  return CaloMoniAlg::finalize();
}
/** Check - calculate a few monitored parameters and compare them
 *  with the references, generate warnins or error messages
 */
StatusCode CaloEMuPIDMon::check()
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "check not implemented yet" << endmsg;
  return StatusCode::SUCCESS;
}
/**
 * initialize the algorithm, define the histograms
 */
StatusCode CaloEMuPIDMon::initialize()
{
  StatusCode sc = CaloMoniAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  hBook1(    "eop",    "E/p", 0.05,  2.55, 50);

  // for the time being let's not split MIP plots into A and C sides
  bool old_m_splitSides = m_splitSides;
  m_splitSides = false;

  hBook1(  "prsem",  "E Prs",  0.5,  25.5, 25);
  hBook1( "ecalem", "E Ecal",  40., 2040., 50);
  hBook1( "hcalem", "E Hcal", 100., 5100., 50);

  m_splitSides = old_m_splitSides;

  return StatusCode::SUCCESS;
}
