#include "GaudiKernel/IIncidentSvc.h"
//#include "Kernel/Particle2Vertex.h"
#include "Kernel/IDVAlgorithm.h"
#include "Kernel/IDistanceCalculator.h"
#include "LoKi/select.h"
#include "FilterDesktop.h"

/** @class ConeJetProxyFilter
 *
 *  Vector sum of PT from all charged and neutral particles in a cone.
 *
 *  @author Konstantinos A. Petridis
 *  @date   2013-09-02
 */

class ConeJetProxyFilter : public FilterDesktop
{

public:

  ConeJetProxyFilter(const std::string &name,ISvcLocator *pSvc);

  virtual StatusCode initialize();

public:

  virtual StatusCode filter(const LHCb::Particle::ConstVector& input,
                            LHCb::Particle::ConstVector& output);

private:

  double conePT ( const LHCb::Particle* p,
                  const LHCb::Particles * charged,
                  const LHCb::Particles * neutral, 
                  const LHCb::VertexBase *bpv );

private:

  double m_dR;
  double m_minPT;
  double m_maxTkChiSqPerDOF;
  double m_maxTkGhostProb;
  double m_maxTkIPChiSq;
  double m_maxTkDOCA;
  std::string m_chargedLoc;
  std::string m_neutralLoc;
  const IDistanceCalculator* m_dist;

};

StatusCode ConeJetProxyFilter::initialize()
{
  const StatusCode sc = FilterDesktop::initialize();
  if ( sc.isFailure() ) { return sc ; }

  // get tools
  m_dist = distanceCalculator();

  return sc;
}

ConeJetProxyFilter::ConeJetProxyFilter(const std::string& name,ISvcLocator* pSvc)
  : FilterDesktop(name,pSvc),
    m_dR(1.0),
    m_minPT(0),
    m_chargedLoc(""),
    m_neutralLoc(""),
    m_dist(NULL)
{
  declareProperty("DeltaR", m_dR, "Cone size (defaults to 1.0)");
  declareProperty("MinPT", m_minPT, "Filter on cone PT > this");
  declareProperty("MaxTrackChiSqPerDOF", m_maxTkChiSqPerDOF = 3.0,
                  "Max Track chi^2 / D.O.F." );
  declareProperty("MaxTrackGhostProb", m_maxTkGhostProb = 0.4,
                  "Max Track ghost probability" );
  declareProperty("MaxTrackIPChiSq", m_maxTkIPChiSq = 16.0,
                  "Max Track I.P. chi^2" );
  declareProperty("MaxTrackDOCA", m_maxTkDOCA = 1.0,
                  "Max Track DOCA" );
  declareProperty("ChargedParticles", m_chargedLoc, "location of charged particles");
  declareProperty("NeutralParticles", m_neutralLoc, "location of neutral particles");
}

StatusCode ConeJetProxyFilter::filter(const LHCb::Particle::ConstVector& input,
                                      LHCb::Particle::ConstVector& output)
{
  LHCb::Particles *charged = getIfExists<LHCb::Particles>(m_chargedLoc+"/Particles");
  LHCb::Particles *neutral = getIfExists<LHCb::Particles>(m_neutralLoc+"/Particles");
  if ( !charged || charged->empty() )
  {
    return Warning( "Failed to get particles from " + m_chargedLoc );
  }
  if ( !neutral || neutral->empty() )
  {
    return Warning( "Failed to get particles from " + m_neutralLoc );
  }

  LHCb::Particle::ConstVector filtered;
  filtered.reserve(input.size());
  LoKi::select(input.begin(),input.end(),
               std::back_inserter(filtered),predicate());

  output.reserve(filtered.size());
  for ( LHCb::Particle::ConstVector::const_iterator ip = filtered.begin();
        filtered.end() != ip; ++ip ) 
  {
    const LHCb::Particle *p = *ip;
    if ( !p ) { continue ; }
    const LHCb::VertexBase* bpv = bestVertex(p);
    const double pt = conePT(p,charged,neutral,bpv);
    if ( pt < 0 ) return StatusCode::FAILURE;
    if ( pt > m_minPT ) output.push_back(p);
  }
  markParticles(output);
  return StatusCode::SUCCESS;
}

double ConeJetProxyFilter::conePT(const LHCb::Particle *p,
                                  const LHCb::Particles *charged,
                                  const LHCb::Particles *neutral,
                                  const LHCb::VertexBase *pv)
{
  Gaudi::LorentzVector p4cone(0,0,0,0);
  const Gaudi::LorentzVector& p4 = p->momentum();
  // charged
  for ( LHCb::Particles::const_iterator ip = charged->begin(); 
        ip != charged->end(); ++ip ) 
  {
    // clean up
    if ( (*ip)->proto()->track()->chi2PerDoF()     > m_maxTkChiSqPerDOF ) continue;
    if ( (*ip)->proto()->track()->ghostProbability() > m_maxTkGhostProb ) continue;
    // prompt from same pv or "close" to topo
    bool use = false;
    double imp(-1),chi2(-1),doca(-1);
    m_dist->distance((*ip),pv,imp,chi2);
    if ( chi2 < m_maxTkIPChiSq )
    {
      use = true; 
    }
    else
    {
      m_dist->distance(p,(*ip),doca);
      if ( doca < m_maxTkDOCA ) { use = true; }
    }
    if(!use) continue;
    const Gaudi::LorentzVector& p4part = (*ip)->momentum();
    double deltaPhi = fabs( p4.Phi() - p4part.Phi() );
    if(deltaPhi > M_PI) deltaPhi = 2*M_PI - deltaPhi;
    const double deltaEta = p4.Eta() - p4part.Eta();
    const double dR   = std::sqrt(deltaPhi*deltaPhi + deltaEta*deltaEta);
    if ( dR < m_dR ) { p4cone += p4part; }
  }
  // neutral
  for(LHCb::Particles::const_iterator ip = neutral->begin(); 
      ip != neutral->end(); ++ip ) 
  {
    const Gaudi::LorentzVector& p4part = (*ip)->momentum();
    double deltaPhi = fabs( p4.Phi() - p4part.Phi() );
    if(deltaPhi > M_PI) deltaPhi = 2*M_PI - deltaPhi;
    const double deltaEta = p4.Eta() - p4part.Eta();
    const double dR   = std::sqrt(deltaPhi*deltaPhi + deltaEta*deltaEta);
    if ( dR < m_dR ) { p4cone += p4part; }
  }
  return p4cone.Pt();
}

// ============================================================================
DECLARE_ALGORITHM_FACTORY(ConeJetProxyFilter)
// ============================================================================
