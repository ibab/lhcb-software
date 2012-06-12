#ifndef TUPLETOOLSUBMASS_H
#define TUPLETOOLSUBMASS_H 1

// Include files
// from Gaudi
#include "TupleToolBase.h"
#include "Kernel/IParticleTupleTool.h"            // Interface
#include "Event/Particle.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"


/** @class TupleToolSubMass TupleToolSubMass.h jborel/TupleToolSubMass.h
 *
 * \brief Return all n-bodies mass combinations from the decay tree
 *
 * 
 * \verbatim
from configurables import TupleToolSubMass
Tuple.addTool(TupleToolSubMass)
Tuple.TupleToolSubMass.SetMax = 3  # look to 2- and 3-bodies  (defaut all possible n-bodies)
\endverbatim
 * 
 * - will create Sum_{p=2,max) C(N,p)  new entries in the nTuple called [head]_M[ij...]
 *
 * - Particle daughters are sorted by PID at each branch of the tree (cc-independant)
 *
 * **** Substitution property
 * 
 * - usage : 
 *  TupleTool.Substitution["pi+"]="K+"
 *  TupleTool.Substitution["K+"]="pi+"
 *
 * - produce alternative mass with substituted PID pi<->K (cc is assumed)
 *
 * -change only one pion (K) at once in case of several pion (K) in the decay tree (producing separate output par pion (K) ) 
 *
 * **** DoubleSubstitution property
 * 
 * - usage : 
 *  TupleTool.DoubleSubstitution["K+/pi-"]="pi+/K-"
 *  TupleTool.DoubleSubstitution["K+/K-"]="pi+/pi-"
 *
 * - change all [K+pi-]cc ([K+K-]cc) pairs to (pi+K-)cc ([pi+pi-]cc)
 * - change only one pair at once in case of several pairs in the decay tree (producing separate output per pair) 
 * - "/" separator is not mandatory : K+pi- syntax is allowed (just a bit slower to parse)
 *
 *  @author Olivier Deschamps
 *  @date   2011-06-01
 */

class SortDaughtersByPID {
  public:
  SortDaughtersByPID(const LHCb::Particle* P){
    m_sign=1;
    if ( P )m_sign = ( P->particleID().pid() > 0) ? +1 : -1;
  }
  bool operator()(const LHCb::Particle* c1, const LHCb::Particle* c2) {
    return m_sign*c1->particleID().pid() > m_sign*c2->particleID().pid();
  }
  int m_sign;
};
  

class TupleToolSubMass : public TupleToolBase, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolSubMass( const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

  virtual ~TupleToolSubMass(){}; ///< Destructor

  virtual StatusCode initialize(); ///< initialisation  

  virtual StatusCode fill( const LHCb::Particle*
			   , const LHCb::Particle*
			   , const std::string&
			   , Tuples::Tuple& );

private :
  std::vector<std::vector<int> >     combine(std::vector<std::vector<int> >  vec, unsigned int level, int nfs);
  std::vector<const LHCb::Particle*> getTree(const LHCb::Particle* P );  
  std::pair<std::string,double>      getMass(std::vector<const LHCb::Particle*> tree , const std::vector<int> map,
                                             int sPos=-1, double sMass=0.,int sPos2=-1,double sMass2=0.);
  std::string                        getFlag(std::string from,std::string to,int pos,
                                             std::string from2="",std::string to2="",int pos2=-1);
  Gaudi::LorentzVector               sMomentum(const LHCb::Particle* part , double sMass);
  std::pair<std::string,std::string> parseID(std::string PID);

  unsigned int m_max;
  std::map<std::string,std::string> m_subst;
  std::map<std::string,std::string> m_subst2;
  LHCb::IParticlePropertySvc* m_ppsvc;
  const LHCb::ParticleProperty* property(std::string name){return (m_ppsvc) ? m_ppsvc->find( name ) : NULL;};    
  const LHCb::ParticleProperty* property(const LHCb::ParticleID pid){return (m_ppsvc) ? m_ppsvc->find( pid ) : NULL;};    
  const LHCb::Particle* m_ancestor;
};

#endif // TUPLETOOLSUBMASS_H
