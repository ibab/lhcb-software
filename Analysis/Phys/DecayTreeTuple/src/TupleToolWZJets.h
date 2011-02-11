#ifndef TUPLETOOLJETS_H
#define TUPLETOOLJETS_H 1
#include "LoKi/LoKi.h"
#include "TupleToolBase.h"
#include "Kernel/IParticleTupleTool.h"
#include "Kernel/DVAlgorithm.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/FilterParticlesBase.h"
class IJetMaker;
//autor: Albert Bursche
class TupleToolWZJets : public TupleToolBase, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolWZJets( const std::string& type,
		 const std::string& name,
		 const IInterface* parent);

  virtual ~TupleToolWZJets(){}; ///< Destructor
 
  virtual StatusCode fill( const LHCb::Particle*
			   , const LHCb::Particle*
			   , const std::string&
			   , Tuples::Tuple& );
  virtual StatusCode initialize();
 private:
  std::vector<LHCb::Particle*> m_decayParticles;

  /// Check if your track belongs to your decay or not
  bool isParticleInDecay(const LHCb::Particle* part);

  /// Save all particles in your decay descriptor in a vector
  void SaveDecayChainParticles( const LHCb::Particle *top);

  std::string m_BaseName;
  //std::string m_BosonContainer;
  std::vector<std::string> m_PartContainer;
  unsigned int m_MaxJets;
  bool m_reqOverlap;
  bool m_remOverlap;
  bool m_IsoJetAbsID;
  Tuples::Tuple* m_tuple;
  const IJetMaker* m_AdditionalJetMaker;
  const IJetMaker* m_IsoJetMaker;
  const IParticleFilter* m_LokiAddJetFilter;
  const IParticleFilter* m_LokiIsoJetFilter;
  const LHCb::IParticlePropertySvc* m_ppSvc;
  //  virtual bool AdditionalFilter()//slot that can be overwritten in derrived classes
  //  {return true;}
  unsigned int m_magic;
  //  LoKi::Cuts::SUMTREE* charge; 
  LoKi::Types::Fun charge;
  LoKi::Types::Fun positiveParticles; 
  LoKi::Types::Fun negativeParticles; 
  LoKi::Types::Fun neutralParticles; 
  LoKi::Types::Fun maxPT;
  LoKi::Types::Fun m_M;
  LoKi::Types::Fun m_MM;
  LoKi::Types::Fun m_DPHI;
  LoKi::Types::Fun m_DETA;
  LoKi::Types::Fun m_DR2;
  bool WriteJetToTuple(const LHCb::Particle*,std::string prefix);
  double MaxSumNPart(const LHCb::Particle* jet,unsigned int n,const LoKi::Types::Fun& fun = LoKi::Cuts::PT);
  
  template <class T1,class T2>
    class Comperator
  {
    const LoKi::Types::Fun & m_fun;
  public:
    Comperator(const LoKi::Types::Fun &fun)
      :m_fun(fun)
      {}
    bool operator()(T1 t1,T2 t2)
    {return m_fun(t1)>m_fun(t2);}
  };
    
};

#endif // TUPLETOOLJETS_H
