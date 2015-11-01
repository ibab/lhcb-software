// $Id$
// ============================================================================
// Include files
// ============================================================================
#include <sstream>
#include "GaudiKernel/IIncidentSvc.h"
#include "LoKi/ParticleProperties.h"
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

private:

  bool substitute(LHCb::Particle* p,const unsigned int which);

private:

  SubPIDMMFilter();
  SubPIDMMFilter(const SubPIDMMFilter&);
  SubPIDMMFilter& operator=(const SubPIDMMFilter&);

private:

  std::vector<std::vector<std::string> > m_names;
  std::vector<std::vector<LHCb::ParticleID> > m_pids;
  std::vector<std::vector<double> > m_masses;
  double m_mmMin;
  double m_mmMax;
  unsigned int m_maxParticles;
  std::string m_stopIncidentType;
  mutable IIncidentSvc* m_incSvc; ///< the incident service

};

StatusCode SubPIDMMFilter::initialize()
{
  const StatusCode sc = FilterDesktop::initialize () ;
  if ( sc.isFailure() ) { return sc ; }

  const int size1 = m_names.size();
  m_masses.resize(size1); m_pids.resize(size1);
  for(int i = 0; i < size1; ++i)
  {
    const int size2 = m_names[i].size();
    m_masses[i].resize(size2); m_pids[i].resize(size2);
    for(int j = 0; j < size2; ++j)
    {
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

StatusCode SubPIDMMFilter::filter( const LHCb::Particle::ConstVector& input,
                                   LHCb::Particle::ConstVector& output )
{
  LHCb::Particle::ConstVector filtered ;
  filtered.reserve(input.size());
  std::copy_if(input.begin(),input.end(),
               std::back_inserter(filtered),std::cref(predicate()));
  bool reachedMax = false;
  for ( auto ip = filtered.begin(); filtered.end() != ip; ++ip )
  {
    const LHCb::Particle* p = *ip ;
    if ( !p ) { continue ; }
    for ( unsigned int i = 0; i < m_pids.size(); ++i )
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
      if ( !substitute(tree.head(),i) ) return StatusCode::FAILURE;
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

bool SubPIDMMFilter::substitute( LHCb::Particle* p,const unsigned int which )
{
  if ( !p ) { return false; }

  // Check which index (should never fail...)
  if ( which >= m_pids.size() )
  {
    Warning( "'which' index " + std::to_string(which) 
           + " exceeds bound " + std::to_string( m_pids.size()-1)).ignore();
    return false;
  }

  // loop over daugthers and start changing those PIDs
  const SmartRefVector<LHCb::Particle>& daughters = p->daughters();
  if ( !daughters.empty() )
  {
    double energySum = 0.0, pxSum = 0.0, pySum = 0.0, pzSum = 0.0;
    unsigned int index = 0;
    for( const LHCb::Particle* daughter: daughters )
    {
      if ( index >= (m_pids[which]).size() )
      {
        Warning( "'index' index " + std::to_string(index) 
               + " exceeds bound " + std::to_string((m_pids[which]).size()-1)
               ).ignore();
      }
      else
      {
        const LHCb::ParticleID& newPID = (m_pids[which])[index];
        if ( daughter->particleID() != newPID )
        {
          if ( daughter->particleID().threeCharge() != newPID.threeCharge() )
          {
            std::ostringstream mess;
            mess << "Substitution will change Particle charge! "
                 << daughter->particleID() << " -> " << newPID;
            Warning( mess.str(), StatusCode::SUCCESS ).ignore();
          }
          { // const cast to set things
            LHCb::Particle * d = const_cast<LHCb::Particle*>(daughter);
            d->setParticleID(newPID);
            const double& newMass = (m_masses[which])[index];
            const Gaudi::LorentzVector& oldMom = d->momentum();
            d->setMomentum( Gaudi::LorentzVector(oldMom.Px(),oldMom.Py(),oldMom.Pz(),
                                                 std::sqrt(oldMom.P2()+std::pow(newMass,2))) );
            d->setMeasuredMass(newMass);
          }
        }
        energySum += daughter->momentum().E() ;
        pxSum     += daughter->momentum().Px();
        pySum     += daughter->momentum().Py();
        pzSum     += daughter->momentum().Pz();
      }
      ++index;
    }
    p->setMomentum( Gaudi::LorentzVector(pxSum,pySum,pzSum,energySum) );
    p->setMeasuredMass( p->momentum().M() );
  }

  return true;
}

StatusCode SubPIDMMFilter::_saveInTES()
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
