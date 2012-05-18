//Created by Murilo Rangel

// ============================================================================
// LoKi
// ============================================================================
#include <iostream>
#include "LoKi/LoKi.h"
#include "LoKi/Algo.h"
#include "LoKi/Vertices.h"
#include "LoKi/VertexCast.h"
#include "LoKi/Particles.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/VertexCuts.h"
#include "LoKi/ParticleContextCuts.h"
#include "Kernel/IParticle2MCAssociator.h"

using namespace LoKi        ;
using namespace LoKi::Types ;
using namespace LoKi::Cuts  ;
using namespace LoKi::Particles  ;
using namespace LoKi::Vertices  ;
using namespace LHCb;

bool ptorder (const LHCb::Particle* i, const LHCb::Particle* j) { return (i->pt()>j->pt()); }


LOKI_ALGORITHM(ZplusJets)
//LOKI_MCALGORITHM(ZplusJets)
{

  bool isMC = false;
  // get *ALL* MC particles.
//  MCRange MCAll = mcselect( "ALL" , MCALL ) ; 
//  info() << "# of MC particles = " << MCAll.size() << endreq;
  Tuple tuple = nTuple("Ztuple","Ztuple");

  LHCb::Particle::Range muons = get<LHCb::Particle::Range>("/Event/Phys/StdLooseMuons/Particles");
  LHCb::Particle::Range nopidpions = get<LHCb::Particle::Range>("/Event/Phys/StdNoPIDsPions/Particles") ;
  LHCb::Particle::Range neutral = get<LHCb::Particle::Range>("/Event/Phys/StdLooseAllPhotons/Particles") ;
  LHCb::Particle::Range charged = get<LHCb::Particle::Range>("/Event/Phys/StdNoPIDsPions/Particles") ;

  //muons.sort(ptorder);
  //std::sort(muons.begin(),muons.end(),ptorder);

  if(muons.size()<2) return StatusCode::SUCCESS;

  std::vector<const LHCb::Particle*> muonlist;
  std::vector<LorentzVector> muonvectors;
  for(Range::iterator ip = muons.begin(); muons.end()!=ip; ++ip) {
    const LHCb::Particle* part = *ip; 
    if(part->pt()<20000) continue;
    muonlist.push_back(part);
    LorentzVector g = part->momentum();
    muonvectors.push_back(g);
//    info() << g.Pt() << endreq;
  }
  if(muonlist.size()!=2) return StatusCode::SUCCESS;
  if((muonlist[0]->momentum()+muonlist[1]->momentum()).mass()<40000) return StatusCode::SUCCESS;
//  std::sort(muonlist.begin(), muonlist.end(), ptorder);

//  info() << "Event with #Muons = " << muons.size() << endreq;
//  info() << "#Neutrals = " << neutral.size() << endreq;
//  info() << "#Charged = " << charged.size() << endreq;
  int Nneutral = neutral.size();
  int Ncharged = charged.size();

  tuple->column("m",(muonlist[0]->momentum()+muonlist[1]->momentum()).mass());
  tuple->column("muon1_Px",muonlist[0]->momentum().Px());
  tuple->column("muon1_Py",muonlist[0]->momentum().Py());
  tuple->column("muon1_Pz",muonlist[0]->momentum().Pz());
  tuple->column("muon1_E",muonlist[0]->momentum().E());
  tuple->column("muon2_Px",muonlist[1]->momentum().Px());
  tuple->column("muon2_Py",muonlist[1]->momentum().Py());
  tuple->column("muon2_Pz",muonlist[1]->momentum().Pz());
  tuple->column("muon2_E",muonlist[1]->momentum().E());
//  tuple->column("Nneutral",Nneutral);
//  tuple->column("Ncharged",Ncharged);
  std::vector<float> neutral_px;
  std::vector<float> neutral_py;
  std::vector<float> neutral_pz;
  std::vector<float> neutral_e;
  std::vector<float> charged_px;
  std::vector<float> charged_py;
  std::vector<float> charged_pz;
  std::vector<float> charged_e;
  const std::vector<std::vector<double> > neutral_matrix;

  for(Range::iterator ipn = neutral.begin(); neutral.end()!=ipn; ++ipn) {
    const LHCb::Particle* partn = *ipn;
    LorentzVector lv = partn->momentum();
    neutral_px.push_back(lv.Px());
    neutral_py.push_back(lv.Py());
    neutral_pz.push_back(lv.Pz());
    neutral_e.push_back(lv.E());
  }
  tuple->farray("neutral_px",neutral_px,"Nneutral",10000);
  tuple->farray("neutral_py",neutral_py,"Nneutral",10000);
  tuple->farray("neutral_pz",neutral_pz,"Nneutral",10000);
  tuple->farray("neutral_e",neutral_e,"Nneutral",10000);
  for(Range::iterator ipc = charged.begin(); charged.end()!=ipc; ++ipc) {
    const LHCb::Particle* partc = *ipc;
    LorentzVector lv = partc->momentum();
    charged_px.push_back(lv.Px());
    charged_py.push_back(lv.Py());
    charged_pz.push_back(lv.Pz());
    charged_e.push_back(lv.E());
  }
  tuple->farray("charged_px",charged_px,"Ncharged",10000);
  tuple->farray("charged_py",charged_py,"Ncharged",10000);
  tuple->farray("charged_pz",charged_pz,"Ncharged",10000);
  tuple->farray("charged_e",charged_e,"Ncharged",10000);
  tuple->write();
  return StatusCode::SUCCESS ;
}
