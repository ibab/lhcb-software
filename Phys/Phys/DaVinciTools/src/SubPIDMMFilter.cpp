// $Id$
// ============================================================================
// Include files
// ============================================================================
#include "GaudiKernel/IIncidentSvc.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/select.h"
#include "boost/foreach.hpp"
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"
#include "FilterDesktop.h"
// ============================================================================
/** @class SubPIDMMFilter
 *  Simple algorithm to substitute PID and filter on resulting MM.
 *
 *  @code
 *
 *  from Configurables import SubPIDMMFilter
 *
 *  alg = SubPIDMMFilter('D2HH', Code="ALL", MinMM=1764, MaxMM=1964,
 *                       PIDs = [['K+','pi+']])
 *
 *  @endcode
 *
 *  Changes PIDs to K+ pi+ and passes if 1764 < MM < 1964.
 *
 *  @author Mike Williams
 *  @date 2011-07-22
 *
 *                    $Revision$
 *  Last modification $Date$
 *                by  $Author$
 */
class SubPIDMMFilter : public FilterDesktop
{

  friend class AlgFactory<SubPIDMMFilter>;

public:
  virtual StatusCode initialize();
  virtual StatusCode _saveInTES();
  virtual void writeEmptyTESContainers();

protected:
  SubPIDMMFilter(const std::string& name,ISvcLocator* pSvc);
  virtual ~SubPIDMMFilter(){}
  IIncidentSvc* incSvc() const
  {
    if ( !m_incSvc ) { m_incSvc = svc<IIncidentSvc>("IncidentSvc",true); }
    return m_incSvc ;
  }

public:
  virtual StatusCode filter(const LHCb::Particle::ConstVector& input,
                            LHCb::Particle::ConstVector& filtered);

  bool substitute(LHCb::Particle* p,int which);

private:
  SubPIDMMFilter();
  SubPIDMMFilter(const SubPIDMMFilter&);
  SubPIDMMFilter& operator=(const SubPIDMMFilter&);

  std::vector<std::vector<std::string> > m_names;
  std::vector<std::vector<LHCb::ParticleID> > m_pids;
  std::vector<std::vector<double> > m_masses;
  double m_mmMin;
  double m_mmMax;
  unsigned int m_maxParticles;
  std::string m_stopIncidentType;
  mutable IIncidentSvc* m_incSvc; ///< the incident service
};

StatusCode SubPIDMMFilter::initialize() {
  const StatusCode sc = FilterDesktop::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  const int size1 = m_names.size();
  m_masses.resize(size1); m_pids.resize(size1);
  for(int i = 0; i < size1; ++i){
    const int size2 = m_names[i].size();
    m_masses[i].resize(size2); m_pids[i].resize(size2);
    for(int j = 0; j < size2; ++j){
      m_masses[i][j] = LoKi::Particles::massFromName(m_names[i][j]);
      m_pids[i][j]   = LoKi::Particles::pidFromName(m_names[i][j]);
    }
  }
  return sc;
}

SubPIDMMFilter::SubPIDMMFilter(const std::string& name,ISvcLocator* pSvc)
  : FilterDesktop(name,pSvc),
    m_mmMin(0),
    m_mmMax(0),
    m_maxParticles(boost::numeric::bounds<unsigned int>::highest()),
    m_stopIncidentType(),
    m_incSvc(NULL)
{
  declareProperty("MinMM", m_mmMin, "min MM value to filter on");
  declareProperty("MaxMM", m_mmMax, "max MM value to filter on");
  declareProperty("PIDs", m_names, "list of list of PIDs (names) to use");
  declareProperty("MaxParticles", m_maxParticles,
                  "max allowed particles to store");
  declareProperty("StopIncidentType", m_stopIncidentType, "incident type");
}

StatusCode SubPIDMMFilter::filter(const LHCb::Particle::ConstVector& input,
                                  LHCb::Particle::ConstVector& output)
{
  LHCb::Particle::ConstVector filtered ;
  filtered.reserve(input.size());
  LoKi::select(input.begin(),input.end(),
               std::back_inserter(filtered),predicate());
  int size = m_pids.size();
  bool reachedMax = false;
  for(LHCb::Particle::ConstVector::const_iterator ip = filtered.begin();
      filtered.end() != ip; ++ip) 
  {
    const LHCb::Particle* p = *ip ;
    if ( NULL == p ) { continue ; }
    for(int i = 0; i < size; ++i)
    {
      if ( i_markedParticles().size() > m_maxParticles )
      {
        reachedMax = true;
        Warning("Maximum number of allowed particles reached",
                StatusCode::SUCCESS).ignore();
        if ( !m_stopIncidentType.empty() )
        {
          incSvc()->fireIncident(Incident(name(),m_stopIncidentType));
        }
        break;
      }
      LHCb::DecayTree tree(*p);
      if(!substitute(tree.head(),i)) return StatusCode::FAILURE;
      const double mm = tree.head()->measuredMass();
      if ( mm > m_mmMin && mm < m_mmMax ) 
      {
        markNewTree(tree.head()); // mark & store new decay tree
        output.push_back( tree.release() );
      }
    }
    if ( reachedMax ) break;
  }
  return StatusCode::SUCCESS;
}

bool SubPIDMMFilter::substitute(LHCb::Particle* p,int which)
{
  if ( NULL == p ) { return false; }
  const SmartRefVector<LHCb::Particle>& daughters = p->daughters();
  if ( !daughters.empty() )
  {
    double energySum = 0.0, pxSum = 0.0, pySum = 0.0, pzSum = 0.0;
    int index = 0;
    BOOST_FOREACH( const LHCb::Particle* daughter, daughters )
    {
      LHCb::Particle *d = const_cast<LHCb::Particle*>(daughter);
      if ( d->particleID() != m_pids[which][index] )
      {
        d->setParticleID(m_pids[which][index]);
        const double newMass = m_masses[which][index];
        const Gaudi::LorentzVector& oldMom = d->momentum();
        d->setMomentum( Gaudi::LorentzVector(oldMom.Px(),oldMom.Py(),oldMom.Pz(),
                                             std::sqrt(oldMom.P2()+std::pow(newMass,2))) );
        d->setMeasuredMass(newMass);
      }
      energySum += d->momentum().E() ;
      pxSum     += d->momentum().Px();
      pySum     += d->momentum().Py();
      pzSum     += d->momentum().Pz();
      ++index;
    }
    p->setMomentum( Gaudi::LorentzVector(pxSum,pySum,pzSum,energySum) );
    p->setMeasuredMass( p->momentum().M() );
  }
  return true;
}

StatusCode SubPIDMMFilter::_saveInTES ()
{
  return FilterDesktop::BaseClass::_saveInTES() ;
}

void SubPIDMMFilter::writeEmptyTESContainers()
{
  FilterDesktop::BaseClass::writeEmptyTESContainers() ;
}

// ============================================================================
DECLARE_ALGORITHM_FACTORY(SubPIDMMFilter)
// ============================================================================
