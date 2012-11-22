#include "TupleToolJetsBase.h"
#include "LoKi/AParticleCuts.h"

TupleToolJetsBase::TupleToolJetsBase( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
  : TupleToolBase ( type, name , parent ),
    charge(LoKi::Cuts::ONE),
    positiveParticles(LoKi::Cuts::ONE ),
    negativeParticles(LoKi::Cuts::ONE ),
    neutralParticles(LoKi::Cuts::ONE ),
    maxPT(LoKi::Cuts::ONE ),
    m_M(LoKi::Cuts::ONE ),
    m_MM(LoKi::Cuts::ONE )
{}
#define SAVEPOINT(POINT,FUN)                    \
  (POINT?POINT->FUN:-1.0)
bool TupleToolJetsBase::WriteJetToTuple(const LHCb::Particle*jet,std::string prefix)
{
  //filter plus and minus signs out to the prefix
  std::map<std::string,std::string> replacements ;
  replacements["+"]="plus";
  replacements["-"]="minus";
  for (std::map<std::string,std::string>::iterator it = replacements.begin();it != replacements.end();++it)
  {
    size_t point = 0;
    while((point = prefix.find(it->first))!=std::string::npos)
      prefix = prefix.replace(point,it->second.size(),it->second);
  }

  if ( msgLevel(MSG::DEBUG) ) debug() << (jet?"Filling jet object ":"Filling dummy jet object ") << jet << " " << prefix << " in tuple at address "<<m_tuple<< endmsg;

  bool result = true;
  result &= (*m_tuple)->column( prefix+"_PX",  SAVEPOINT(jet,momentum().Px()));
  result &= (*m_tuple)->column( prefix+"_PY",  SAVEPOINT(jet,momentum().Py()));
  result &= (*m_tuple)->column( prefix+"_PZ",  SAVEPOINT(jet,momentum().Pz()));
  result &= (*m_tuple)->column( prefix+"_PE",  SAVEPOINT(jet,momentum().e()));
  result &= (*m_tuple)->column( prefix+"_PT",  SAVEPOINT(jet,momentum().Pt()));
  result &= (*m_tuple)->column( prefix+"_P",   SAVEPOINT(jet,momentum().P()));
  result &= (*m_tuple)->column( prefix+"_Eta", SAVEPOINT(jet,momentum().Eta()));
  result &= (*m_tuple)->column( prefix+"_Phi", SAVEPOINT(jet,momentum().Phi()));
  result &= (*m_tuple)->column( prefix+"_M", SAVEPOINT(jet,momentum().mass()));
  result &= (*m_tuple)->column( prefix+"_MLoKi",jet?m_M(jet):-99.0);
  result &= (*m_tuple)->column( prefix+"_MMLoKi", jet?m_MM(jet):-99.0);
  result &= (*m_tuple)->column( prefix+"_NTrk", jet->info(9001,-1000));
  result &= (*m_tuple)->column( prefix+"_N90", jet->info(9002,-1000));
  result &= (*m_tuple)->column( prefix+"_MTF", jet->info(9003,-1000));
  result &= (*m_tuple)->column( prefix+"_NSatCells", jet->info(9004,-1000));
  result &= (*m_tuple)->column( prefix+"_NHasPVInfo", jet->info(9005,-1000));
  result &= (*m_tuple)->column( prefix+"_JEC_Cor", jet->info(9100,-1000));
  result &= (*m_tuple)->column( prefix+"_JEC_PV", jet->info(9101,-1000));
  if (m_verbose)
  {
    result &= (*m_tuple)->column( prefix+"_ParticleMultiplicity",  SAVEPOINT(jet,daughters().size()));
    result &= (*m_tuple)->column( prefix+"_Charge", jet?charge(jet):-1.0);
    result &= (*m_tuple)->column( prefix+"_positiveParticleMultiplicity", jet?positiveParticles(jet):-1.0);
    result &= (*m_tuple)->column( prefix+"_negativeParticleMultiplicity", jet?negativeParticles(jet):-1.0);
    result &= (*m_tuple)->column( prefix+"_neutralParticleMultiplicity", jet?neutralParticles(jet):-1.0);
    result &= (*m_tuple)->column( prefix+"_chargedParticleMultiplicity", jet?positiveParticles(jet)+negativeParticles(jet):-1.0);
    result &= (*m_tuple)->column( prefix+"_maxPT", jet?maxPT(jet):-1.0);
    SmartRefVector< LHCb::Particle > SortedDaughters;
    //buffer to store intermediate result to speed things up a bit
    result &= (*m_tuple)->column( prefix+"_PT1",  jet?MaxSumNPart(jet,1, LoKi::Cuts::PT,&SortedDaughters):-1.0);
    result &= (*m_tuple)->column( prefix+"_PT2",  jet?MaxSumNPart(jet,2, LoKi::Cuts::PT,&SortedDaughters):-1.0);
    result &= (*m_tuple)->column( prefix+"_PT3",  jet?MaxSumNPart(jet,3, LoKi::Cuts::PT,&SortedDaughters):-1.0);
    result &= (*m_tuple)->column( prefix+"_PT4",  jet?MaxSumNPart(jet,4, LoKi::Cuts::PT,&SortedDaughters):-1.0);
    result &= (*m_tuple)->column( prefix+"_PT5",  jet?MaxSumNPart(jet,5, LoKi::Cuts::PT,&SortedDaughters):-1.0);
    result &= (*m_tuple)->column( prefix+"_PT10", jet?MaxSumNPart(jet,10,LoKi::Cuts::PT,&SortedDaughters):-1.0);
    result &= (*m_tuple)->column( prefix+"_PT15", jet?MaxSumNPart(jet,15,LoKi::Cuts::PT,&SortedDaughters):-1.0);
    result &= (*m_tuple)->column( prefix+"_PT25", jet?MaxSumNPart(jet,25,LoKi::Cuts::PT,&SortedDaughters):-1.0);
  }
  return result;
}

StatusCode TupleToolJetsBase::initialize()
{
  const StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;
  //get LoKi objects
  charge = LoKi::Cuts::SUMTREE (LoKi::Cuts::Q, LoKi::Cuts::ALL, 0.0 ) ;
  positiveParticles = LoKi::Cuts::NINTREE (  LoKi::Cuts::Q > 0 );
  negativeParticles = LoKi::Cuts::NINTREE (  LoKi::Cuts::Q < 0 );
  neutralParticles  = LoKi::Cuts::NINTREE (  LoKi::Cuts::Q == 0 );
  maxPT = LoKi::Cuts::MAXTREE(LoKi::Cuts::PT,LoKi::Cuts::BASIC,-1);
  m_M = LoKi::Particles::Mass();
  m_MM = LoKi::Particles::MeasuredMass();
  return sc ;
}

double TupleToolJetsBase::MaxSumNPart(const LHCb::Particle* jet,
                                      unsigned int n,
                                      const LoKi::Types::Fun& fun,
                                      SmartRefVector< LHCb::Particle >* SortedDaughters)
{
  //  fun == LoKi::Cuts::ONE;
  if (jet->daughters().size() <= n)
    return LoKi::Cuts::SUMTREE ( LoKi::Cuts::BASIC, fun)(jet);
  if (SortedDaughters&&SortedDaughters->size()) // Vector given and filled
    return LoKi::Cuts::ASUM (fun)(LHCb::Particle::ConstVector(SortedDaughters->begin(),SortedDaughters->begin()+n));
  SmartRefVector< LHCb::Particle > daughters; //empty vector to take address of in case SortedDaughters == NULL
  if (!SortedDaughters)
    SortedDaughters = &daughters;
  SortedDaughters->assign(jet->daughters().begin(),jet->daughters().end());
  sort(SortedDaughters->begin(),SortedDaughters->end(),Comperator<const LHCb::Particle*,const LHCb::Particle*>(fun));
  return LoKi::Cuts::ASUM (fun)(LHCb::Particle::ConstVector(SortedDaughters->begin(),SortedDaughters->begin()+n));
}
