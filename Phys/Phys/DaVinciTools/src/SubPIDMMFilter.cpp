// $Id$
// ============================================================================
// Include files
// ============================================================================
#include "LoKi/ParticleProperties.h"
#include "LoKi/select.h"
#include <boost/foreach.hpp>
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
class SubPIDMMFilter : public FilterDesktop {

  friend class AlgFactory<SubPIDMMFilter>;

public:
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode _saveInTES();
  virtual void writeEmptyTESContainers();

protected:
  SubPIDMMFilter(const std::string& name,ISvcLocator* pSvc);
  virtual ~SubPIDMMFilter();

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
};

SubPIDMMFilter::~SubPIDMMFilter(){}

StatusCode SubPIDMMFilter::initialize() {
  StatusCode sc = FilterDesktop::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  int size1 = m_names.size();
  m_masses.resize(size1); m_pids.resize(size1);
  for(int i = 0; i < size1; i++){
    int size2 = m_names[i].size();
    m_masses[i].resize(size2); m_pids[i].resize(size2);
    for(int j = 0; j < size2; j++){
      m_masses[i][j] = LoKi::Particles::massFromName(m_names[i][j]);
      m_pids[i][j] = LoKi::Particles::pidFromName(m_names[i][j]);
    }
  }
  return StatusCode::SUCCESS ;
}

StatusCode SubPIDMMFilter::finalize() {return FilterDesktop::finalize();}

SubPIDMMFilter::SubPIDMMFilter(const std::string& name,ISvcLocator* pSvc):
  FilterDesktop(name,pSvc), m_mmMin(0), m_mmMax(0){
  declareProperty("MinMM", m_mmMin, "min MM value to filter on");
  declareProperty("MaxMM", m_mmMax, "max MM value to filter on");
  declareProperty("PIDs", m_names, "list of list of PIDs (names) to use");
}

StatusCode SubPIDMMFilter::filter(const LHCb::Particle::ConstVector& input,
                                  LHCb::Particle::ConstVector& output){
  LHCb::Particle::ConstVector filtered ;
  filtered.reserve(input.size());
  LoKi::select(input.begin(),input.end(),
               std::back_inserter(filtered),predicate());
  int size = m_pids.size();
  for(LHCb::Particle::ConstVector::const_iterator ip = filtered.begin();
      filtered.end() != ip; ++ip) {
    const LHCb::Particle* p = *ip ;
    if ( 0 == p ) { continue ; }
    for(int i = 0; i < size; i++){
      LHCb::DecayTree tree(*p);
      if(!substitute(tree.head(),i)) return StatusCode::FAILURE;
      double mm = tree.head()->measuredMass();
      if(mm > m_mmMin && mm < m_mmMax){
        markNewTree(tree.head()); // mark & store new decay tree
        output.push_back  ( tree.release () ) ;
      }
    }
  }
  return StatusCode::SUCCESS ;
}

bool SubPIDMMFilter::substitute(LHCb::Particle* p,int which){
  if ( 0 == p ) { return false; }
  const SmartRefVector<LHCb::Particle> daughters = p->daughters();
  Gaudi::LorentzVector newMom,oldMom;
  if(!daughters.empty()) {
    double energySum = 0.0, pxSum = 0.0, pySum = 0.0, pzSum = 0.0;
    int index = 0;
    BOOST_FOREACH(const LHCb::Particle* daughter, daughters ) {
      LHCb::Particle *d = const_cast<LHCb::Particle*>(daughter);
      if(d->particleID() != m_pids[which][index]){
	d->setParticleID(m_pids[which][index]);
	double newMass = m_masses[which][index];
	oldMom = d->momentum();
	newMom = Gaudi::LorentzVector();
	newMom.SetXYZT(oldMom.Px(),oldMom.Py(),oldMom.Pz(),
		       ::sqrt(oldMom.P2() + newMass*newMass)) ;
	d->setMomentum(newMom) ;
	d->setMeasuredMass(newMass);
      }
      energySum += d->momentum().E() ;
      pxSum += d->momentum().Px();
      pySum += d->momentum().Py();
      pzSum += d->momentum().Pz();
      index++;
    }
    newMom = Gaudi::LorentzVector();
    newMom.SetXYZT(pxSum, pySum, pzSum, energySum);
    p->setMomentum(newMom);
    p->setMeasuredMass(newMom.M());
  }
  return true;
}

StatusCode SubPIDMMFilter::_saveInTES () { return DVAlgorithm::_saveInTES() ; }

void SubPIDMMFilter::writeEmptyTESContainers() {
  DVAlgorithm::writeEmptyTESContainers() ;
}

// ============================================================================
DECLARE_ALGORITHM_FACTORY(SubPIDMMFilter)
// ============================================================================
