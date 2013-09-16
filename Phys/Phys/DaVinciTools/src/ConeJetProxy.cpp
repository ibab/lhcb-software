#include "GaudiKernel/IIncidentSvc.h"
//#include "Kernel/Particle2Vertex.h"
#include "Kernel/IDVAlgorithm.h"
#include "Kernel/IDistanceCalculator.h"
#include "LoKi/select.h"
#include "FilterDesktop.h"

/** @class ConeJetProxyFilter
 *
 * \brief Vector sum of PT from all charged and neutral particles in a cone.
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

  IIncidentSvc* incSvc() const
  {
    if ( !m_incSvc ) { m_incSvc = svc<IIncidentSvc>("IncidentSvc",true); }
    return m_incSvc ;
  }

private:

  double m_dR;
  double m_minPT;
  std::string m_chargedLoc;
  std::string m_neutralLoc;
  std::string m_stopIncidentType;
  mutable IIncidentSvc* m_incSvc; ///< the incident service
  const IDistanceCalculator* m_dist;

};


StatusCode ConeJetProxyFilter::initialize()
{
  const StatusCode sc = FilterDesktop::initialize () ;
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
    m_stopIncidentType(),
    m_incSvc(NULL),
    m_dist(NULL)
{
  declareProperty("DeltaR", m_dR, "Cone size (defaults to 1.0)");
  declareProperty("MinPT", m_minPT, "Filter on cone PT > this");
  declareProperty("ChargedParticles", m_chargedLoc, "location of charged particles");
  declareProperty("NeutralParticles", m_neutralLoc, "location of neutral particles");
  declareProperty("StopIncidentType", m_stopIncidentType, "incident type");
}

StatusCode ConeJetProxyFilter::filter(const LHCb::Particle::ConstVector& input,
                                      LHCb::Particle::ConstVector& output)
{
  LHCb::Particle::ConstVector filtered;
  filtered.reserve(input.size());
  LoKi::select(input.begin(),input.end(),
               std::back_inserter(filtered),predicate());
  LHCb::Particles *charged = get<LHCb::Particles>(m_chargedLoc+"/Particles");
  LHCb::Particles *neutral = get<LHCb::Particles>(m_neutralLoc+"/Particles");
  if ( charged->empty() )
  {
    return Warning( "Failed to get particles from " + m_chargedLoc );
  }
  if ( neutral->empty() )
  {
    return Warning( "Failed to get particles from " + m_neutralLoc );
  }

  for ( LHCb::Particle::ConstVector::const_iterator ip = filtered.begin();
        filtered.end() != ip; ++ip ) 
  {
    const LHCb::Particle *p = *ip;
    if ( 0 == p ) { continue ; }
    const LHCb::VertexBase* bpv = bestVertex(p);
    const double pt = conePT(p,charged,neutral,bpv);
    if ( pt < 0 ) return StatusCode::FAILURE;
    if ( pt > m_minPT) output.push_back(p);
  }
  markParticles(output);
  return StatusCode::SUCCESS ;
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
    if ( (*ip)->proto()->track()->chi2PerDoF() > 3         ) continue;
    if ( (*ip)->proto()->track()->ghostProbability() > 0.4 ) continue;
    // prompt from same pv or "close" to topo
    bool use = false;
    double imp(-1),chi2(-1),doca(-1);
    m_dist->distance((*ip),pv,imp,chi2);
    if ( chi2 < 16 )
    {
      use = true; 
    }
    else
    {
      m_dist->distance(p,(*ip),doca);
      if(doca < 1.0) use = true;
    }
    //if(!use) std::cout << "fail!" << std::endl;
    if(!use) continue;
    const Gaudi::LorentzVector& p4part = (*ip)->momentum();
    double deltaPhi = fabs(p4.Phi() - p4part.Phi() );
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
    double deltaPhi = fabs(p4.Phi() - p4part.Phi() );
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
